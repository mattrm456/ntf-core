// Copyright 2020-2023 Bloomberg Finance L.P.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <ntsu_packetutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_packetutil_cpp, "$Id$ $CSID$")

#include <ntsa_adapter.h>
#include <ntsa_packet.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetdecodercontext.h>
#include <ntsa_packetdecoderoptions.h>
#include <ntsa_packetencoder.h>
#include <ntsa_packetencodercontext.h>
#include <ntsa_packetencoderoptions.h>
#include <ntsa_packetqueue.h>
#include <ntsa_shutdowntype.h>
#include <ntscfg_limits.h>
#include <ntscfg_platform.h>
#include <ntsu_adapterutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

#include <bdlb_string.h>
#include <bdlbb_blob.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlcc_fixedqueue.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorguard.h>
#include <bslma_default.h>
#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_DARWIN)
#include <arpa/inet.h>
#include <net/bpf.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>
#include <netinet/if_ether.h>
#include <sys/sysctl.h>
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <linux/errqueue.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <winerror.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
// clang-format on
#ifdef interface
#undef interface
#endif
#pragma comment(lib, "ws2_32")
#endif

namespace BloombergLP {
namespace ntsu {

bsl::uint16_t PacketFilter::Encoding::getClass(bsl::uint16_t code)
{
    return code & 0x07;
}

bsl::uint16_t PacketFilter::Encoding::getSize(bsl::uint16_t code)
{
    return code & 0x18;
}

bsl::uint16_t PacketFilter::Encoding::getMode(bsl::uint16_t code)
{
    return code & 0xe0;
}

bsl::uint16_t PacketFilter::Encoding::getOperation(bsl::uint16_t code)
{
    return code & 0xf0;
}

bsl::uint16_t PacketFilter::Encoding::getSource(bsl::uint16_t code)
{
    return code & 0x08;
}

bsl::uint16_t PacketFilter::Encoding::getReturnValue(bsl::uint16_t code)
{
    return code & 0x18;
}

bsl::uint16_t PacketFilter::Encoding::getMisc(bsl::uint16_t code)
{
    return code & 0xf8;
}

PacketFilter::Command::Command()
: label(0)
, code(0)
, jt(0)
, jf(0)
, k(0)
{
}

PacketFilter::Instruction::Instruction()
: code(0)
, jt(0)
, jf(0)
, k(0)
{
}

void PacketFilter::Compiler::label(Script* script, const char* label)
{
    Command command;
    command.label = label;

    script->push_back(command);
}

void PacketFilter::Compiler::compile(Script*       script,
                                     bsl::uint16_t code,
                                     bsl::uint32_t k)
{
    PacketFilter::Compiler::compile(script, code, k, 0, 0);
}

void PacketFilter::Compiler::compile(Script*       script,
                                     bsl::uint16_t code,
                                     bsl::uint32_t k,
                                     const char*   jt,
                                     const char*   jf)
{
    Command* command = 0;

    if (script->size() > 0 && script->back().label != 0 &&
        script->back().code == 0)
    {
        command = &script->back();
    }
    else {
        script->resize(script->size() + 1);
        command = &script->back();
    }

    command->code = code;
    command->jt   = jt;
    command->jf   = jf;
    command->k    = k;
}

ntsa::Error PacketFilter::Compiler::link(Program*      program,
                                         const Script& script)
{
    ntsa::Error error;

    program->clear();

    LabelMap labelMap;
    error = Compiler::analyze(&labelMap, script);
    if (error) {
        return error;
    }

    program->resize(script.size());

    for (bsl::size_t pc = 0; pc < script.size(); ++pc) {
        const Command& command = script[pc];

        Instruction* instruction = &((*program)[pc]);

        instruction->code = command.code;
        instruction->k    = command.k;

        error = Compiler::resolve(&instruction->jt, pc, command.jt, labelMap);
        if (error) {
            return error;
        }

        error = Compiler::resolve(&instruction->jf, pc, command.jf, labelMap);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error PacketFilter::Compiler::analyze(LabelMap*     labelMap,
                                            const Script& script)
{
    labelMap->clear();

    for (bsl::size_t pc = 0; pc < script.size(); ++pc) {
        const Command& command = script[pc];

        if (command.label != 0) {
            const bsl::size_t labelSize = bsl::strlen(command.label);
            if (labelSize > 0) {
                bsl::string key(command.label, labelSize);
                BALL_LOG_INFO << "Found label '" << key << "' at position "
                              << pc << BALL_LOG_END;
                (*labelMap)[key] = pc;
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error PacketFilter::Compiler::resolve(bsl::uint8_t*   jump,
                                            bsl::size_t     pc,
                                            const char*     label,
                                            const LabelMap& labelMap)
{
    *jump = 0;

    if (label == 0) {
        BALL_LOG_INFO << "Jump label is null" << BALL_LOG_END;
        return ntsa::Error();
    }

    const char*       keyData = label;
    const bsl::size_t keySize = bsl::strlen(keyData);

    if (keySize == 0) {
        BALL_LOG_INFO << "Jump label is empty" << BALL_LOG_END;
        return ntsa::Error();
    }

    bsl::string key(keyData, keySize);

    LabelMap::const_iterator it = labelMap.find(key);
    if (it == labelMap.end()) {
        BALL_LOG_ERROR << "Link failure: line " << pc
                       << " references undefined label '" << label << "'"
                       << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t index = static_cast<bsl::uint8_t>(it->second);

    if (index <= pc) {
        BALL_LOG_ERROR << "Link failure: line " << pc
                       << " indicates an illegal jump backwards"
                       << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t offset = index - pc - 1;

    if (offset > 255) {
        BALL_LOG_ERROR << "Link failure: line " << pc
                       << " indicates an illegal jump too far" << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    BALL_LOG_INFO << "Resolving jump to label '" << key << "' at position "
                  << index << " to offset " << offset
                  << " from instruction at " << pc << BALL_LOG_END;

    *jump = static_cast<bsl::uint8_t>(offset);

    return ntsa::Error();
}

ntsa::Error PacketUtil::compile(PacketFilter::Program*    program,
                                ntsa::DeviceType::Value   deviceType,
                                const ntsa::Adapter&      adapter,
                                const ntsa::PacketFilter& filter)
{
    ntsa::Error error;

    typedef ntsu::PacketFilter::Script      PFS;
    typedef ntsu::PacketFilter::Program     PFP;
    typedef ntsu::PacketFilter::Instruction PFI;
    typedef ntsu::PacketFilter::Compiler    PFC;

    program->clear();

    if (deviceType == ntsa::DeviceType::e_LOCAL ||
        deviceType == ntsa::DeviceType::e_LOOPBACK)
    {
        return ntsa::Error();  // TODO
    }

    if (deviceType != ntsa::DeviceType::e_ETHERNET) {
        BALL_LOG_ERROR << "Failed to compile packet filter: the device type "
                       << deviceType << " is not supported" << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    ntsa::EthernetAddress ethernetAddress;
    if (!ethernetAddress.parse(adapter.ethernetAddress())) {
        BALL_LOG_ERROR << "Failed to compile packet filter: failed to parse "
                          "ethernet address '"
                       << adapter.ethernetAddress() << "'" << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    PFS script;

    const bsl::uint32_t ethernetAddress0 =
        (static_cast<bsl::uint32_t>(ethernetAddress[0]) << 24) |
        (static_cast<bsl::uint32_t>(ethernetAddress[1]) << 16) |
        (static_cast<bsl::uint32_t>(ethernetAddress[2]) << 8) |
        (static_cast<bsl::uint32_t>(ethernetAddress[3]));

    PFC::compile(&script, NTSU_BPF_LD + NTSU_BPF_W + NTSU_BPF_ABS, 0);
    PFC::compile(&script,
                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                 ethernetAddress0,
                 0,
                 "reject");

    const bsl::uint32_t ethernetAddress4 =
        (static_cast<bsl::uint32_t>(ethernetAddress[4]) << 8) |
        (static_cast<bsl::uint32_t>(ethernetAddress[5]));

    PFC::compile(&script, NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_ABS, 4);
    PFC::compile(&script,
                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                 ethernetAddress4,
                 0,
                 "reject");

    PFC::label(&script, "accept");
    PFC::compile(&script, NTSU_BPF_RET + NTSU_BPF_K, (u_int)(-1));

    PFC::label(&script, "reject");
    PFC::compile(&script, NTSU_BPF_RET + NTSU_BPF_K, 0);

    error = PFC::link(program, script);
    if (error) {
        BALL_LOG_ERROR
            << "Failed to compile packet filter: failed to link program: "
            << error << BALL_LOG_END;
        return error;
    }

    return ntsa::Error();
}

void PacketUtil::acceptAll(PacketFilter::Program* program)
{
    PacketFilter::Script script;
    PacketFilter::Compiler::compile(&script,
                                    NTSU_BPF_RET + NTSU_BPF_K,
                                    (u_int)(-1));
    PacketFilter::Compiler::link(program, script);
}

void PacketUtil::rejectAll(PacketFilter::Program* program)
{
    PacketFilter::Script script;
    PacketFilter::Compiler::compile(&script, NTSU_BPF_RET + NTSU_BPF_K, 0);
    PacketFilter::Compiler::link(program, script);
}

}  // close package namespace
}  // close enterprise namespace
