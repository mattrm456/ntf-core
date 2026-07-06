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
#include <bsls_byteorder.h>
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

#define NTSU_BPF_EXTENSIONS 0

#define NTSU_PACKETUTIL_LOG 0

#if NTSU_PACKETUTIL_LOG

#define NTSU_PACKETUTIL_LOG_INSTRUCTION(begin, pc, description)               \
    do {                                                                      \
        const bsl::size_t position =                                          \
            static_cast<bsl::size_t>((pc) - (begin));                         \
        BALL_LOG_TRACE << "Packet filter executing [" << (position)           \
                       << "]: " << (description)                              \
                       << " [ k = " << static_cast<bsl::uint32_t>((pc)->k)    \
                       << " jt = " << static_cast<bsl::uint32_t>((pc)->jt)    \
                       << " jf = " << static_cast<bsl::uint32_t>((pc)->jf)    \
                       << " ]" << BALL_LOG_END;                               \
    } while (false)

#define NTSU_PACKETUTIL_LOG_STATE(A, X, scratch)                              \
    do {                                                                      \
        BALL_LOG_TRACE << "State:"                                            \
                       << "\nBPF A = " << (A) << " X = " << (X)               \
                       << "\nLink Header Offset =      " << (scratch)[0]      \
                       << "\nLink Header Length =      " << (scratch)[1]      \
                       << "\nLink Packet Size =        " << (scratch)[2]      \
                       << "\nLink Protocol =           " << (scratch)[3]      \
                       << "\nNetwork Header Offset =   " << (scratch)[4]      \
                       << "\nNetwork Header Length =   " << (scratch)[5]      \
                       << "\nNetwork Packet Size =     " << (scratch)[6]      \
                       << "\nNetwork Protocol =        " << (scratch)[7]      \
                       << "\nTransport Header Offset = " << (scratch)[8]      \
                       << "\nTransport Header Length = " << (scratch)[9]      \
                       << "\nTransport Packet Size   = " << (scratch)[10]     \
                       << "\nTransport Protocol =      " << (scratch)[11]     \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_PACKETUTIL_LOG_INVALID_CODE(pc)                                  \
    do {                                                                      \
        BALL_LOG_ERROR << "Invalid code " << (pc)->code << BALL_LOG_END;      \
    } while (false)

#define NTSU_PACKETUTIL_LOG_INVALID_PC(pc)                                    \
    do {                                                                      \
        BALL_LOG_ERROR << "Invalid PC" << BALL_LOG_END;                       \
    } while (false)

#define NTSU_PACKETUTIL_LOG_INVALID_JUMP_TOO_FAR(pc)                          \
    do {                                                                      \
        BALL_LOG_ERROR << "Link failure: line " << (pc)                       \
                       << " indicates an illegal jump too far"                \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_PACKETUTIL_LOG_INVALID_JUMP_BACKWARDS(pc)                        \
    do {                                                                      \
        BALL_LOG_ERROR << "Link failure: line " << (pc)                       \
                       << " indicates an illegal jump backwards"              \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_PACKETUTIL_LOG_UNDEFINED_LABEL(pc, label)                        \
    do {                                                                      \
        BALL_LOG_ERROR << "Link failure: line " << (pc)                       \
                       << " references undefined label '" << label << "'"     \
                       << BALL_LOG_END;                                       \
    } while (false)

#else

#define NTSU_PACKETUTIL_LOG_INSTRUCTION(begin, pc, description)

#define NTSU_PACKETUTIL_LOG_STATE(A, X, scratch)

#define NTSU_PACKETUTIL_LOG_INVALID_CODE(pc)

#define NTSU_PACKETUTIL_LOG_INVALID_PC(pc)

#define NTSU_PACKETUTIL_LOG_INVALID_JUMP_TOO_FAR(pc)

#define NTSU_PACKETUTIL_LOG_INVALID_JUMP_BACKWARDS(pc)

#define NTSU_PACKETUTIL_LOG_UNDEFINED_LABEL(pc, label)

#endif

#define NTSU_EXTRACT_BE_U_2(p)                                                \
    ((uint16_t)(((uint16_t)(*((const uint8_t*)(p) + 0)) << 8) |               \
                ((uint16_t)(*((const uint8_t*)(p) + 1)) << 0)))

#define NTSU_EXTRACT_BE_U_4(p)                                                \
    ((uint32_t)(((uint32_t)(*((const uint8_t*)(p) + 0)) << 24) |              \
                ((uint32_t)(*((const uint8_t*)(p) + 1)) << 16) |              \
                ((uint32_t)(*((const uint8_t*)(p) + 2)) << 8) |               \
                ((uint32_t)(*((const uint8_t*)(p) + 3)) << 0)))

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
, jt(0U)
, jf(0U)
, k(0U)
{
}

PacketFilter::Instruction::Instruction()
: code(0)
, jt(0)
, jf(0)
, k(0)
{
}

PacketFilter::Command* PacketFilter::Compiler::emit(Script* script)
{
    Command* command = 0;

    if (script->size() > 0 && script->back().label.size() > 0 &&
        script->back().code == 0)
    {
        command = &script->back();
    }
    else {
        script->resize(script->size() + 1);
        command = &script->back();
    }

    return command;
}

void PacketFilter::Compiler::label(Script* script, const bsl::string& label)
{
    Command* command = Compiler::emit(script);

    command->label.push_back(label);
}

void PacketFilter::Compiler::compile(Script*       script,
                                     bsl::uint16_t code,
                                     bsl::uint32_t k)
{
    Command* command = Compiler::emit(script);

    command->code = code;
    command->k    = k;
    command->jt   = 0U;
    command->jf   = 0U;
}

void PacketFilter::Compiler::compile(Script*            script,
                                     bsl::uint16_t      code,
                                     const bsl::string& k)
{
    Command* command = Compiler::emit(script);

    command->code = code;
    command->k    = k;
    command->jt   = 0U;
    command->jf   = 0U;
}

void PacketFilter::Compiler::compile(Script*            script,
                                     bsl::uint16_t      code,
                                     bsl::uint32_t      k,
                                     const bsl::string& jt,
                                     const bsl::string& jf)
{
    Command* command = Compiler::emit(script);

    command->code = code;
    command->k    = k;
    command->jt   = jt;
    command->jf   = jf;
}

void PacketFilter::Compiler::compile(Script*            script,
                                     bsl::uint16_t      code,
                                     bsl::uint32_t      k,
                                     bsl::uint8_t       jt,
                                     const bsl::string& jf)
{
    Command* command = Compiler::emit(script);

    command->code = code;
    command->k    = k;
    command->jt   = jt;
    command->jf   = jf;
}

void PacketFilter::Compiler::compile(Script*            script,
                                     bsl::uint16_t      code,
                                     bsl::uint32_t      k,
                                     const bsl::string& jt,
                                     bsl::uint8_t       jf)
{
    Command* command = Compiler::emit(script);

    command->code = code;
    command->k    = k;
    command->jt   = jt;
    command->jf   = jf;
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

        error = Compiler::resolve(&instruction->k, pc, command.k, labelMap);
        if (error) {
            return error;
        }

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

        for (bsl::size_t i = 0; i < command.label.size(); ++i) {
            (*labelMap)[command.label[i]] = pc;
        }
    }

    return ntsa::Error();
}

ntsa::Error PacketFilter::Compiler::resolve(bsl::uint8_t*   value,
                                            bsl::size_t     pc,
                                            const Symbol&   symbol,
                                            const LabelMap& labelMap)
{
    ntsa::Error error;

    bsl::uint32_t temp;
    error = PacketFilter::Compiler::resolve(&temp, pc, symbol, labelMap);
    if (error) {
        return error;
    }

    if (temp > 255) {
        NTSU_PACKETUTIL_LOG_INVALID_JUMP_TOO_FAR(pc);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *value = static_cast<bsl::uint8_t>(temp);

    return ntsa::Error();
}

ntsa::Error PacketFilter::Compiler::resolve(bsl::uint32_t*  value,
                                            bsl::size_t     pc,
                                            const Symbol&   symbol,
                                            const LabelMap& labelMap)
{
    *value = 0;

    if (symbol.index() == static_cast<bsl::size_t>(k_SYMBOL_TYPE_LITERAL)) {
        *value = bsl::get<bsl::uint32_t>(symbol);
    }
    else if (symbol.index() == static_cast<bsl::size_t>(k_SYMBOL_TYPE_LABEL)) {
        bsl::string label = bsl::get<bsl::string>(symbol);

        if (label.empty()) {
            return ntsa::Error();
        }

        LabelMap::const_iterator it = labelMap.find(label);
        if (it == labelMap.end()) {
            NTSU_PACKETUTIL_LOG_UNDEFINED_LABEL(pc, label);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const bsl::size_t index = static_cast<bsl::uint8_t>(it->second);

        if (index <= pc) {
            NTSU_PACKETUTIL_LOG_INVALID_JUMP_BACKWARDS(pc);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const bsl::size_t offset = index - pc - 1;

        if (offset > bsl::numeric_limits<bsl::uint32_t>::max()) {
            NTSU_PACKETUTIL_LOG_INVALID_JUMP_TOO_FAR(pc);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *value = static_cast<bsl::uint32_t>(offset);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error PacketUtil::compile(PacketFilter::Program*    program,
                                ntsa::DeviceType::Value   deviceType,
                                const ntsa::PacketFilter& filter)
{
    ntsa::Error error;

    typedef ntsu::PacketFilter::Script      PFS;
    typedef ntsu::PacketFilter::Program     PFP;
    typedef ntsu::PacketFilter::Instruction PFI;
    typedef ntsu::PacketFilter::Compiler    PFC;

    // The index into scratch memory where the offset from the beginning of the
    // packet to the beginning of the layer-2 (e.g. Ethernet) header is stored.
    const bsl::uint32_t k_L2_HEADER_OFFSET = 0;

    // The index into scratch memory where the layer-2 (e.g. Ethernet) header
    // length is stored.
    const bsl::uint32_t k_L2_HEADER_LENGTH = 1;

    // The index into scratch memory where the layer-2 (e.g. Ethernet) packet
    // length is stored.
    const bsl::uint32_t k_L2_PACKET_LENGTH = 2;

    // The index into scratch memory where the layer-3 protocol (e.g. IPv4 or
    // IPv6) carried by the layer-2 (e.g. Ethernet) packet is stored.
    const bsl::uint32_t k_L2_PROTOCOL = 3;

    // The index into scratch memory where the offset from the beginning of the
    // packet to the beginning of the layer-3 (e.g. IPv4 or IPv6) header is
    // stored.
    const bsl::uint32_t k_L3_HEADER_OFFSET = 4;

    // The index into scratch memory where the layer-3 (e.g. IPv4 or IPv6)
    // header length is stored.
    const bsl::uint32_t k_L3_HEADER_LENGTH = 5;

    // The index into scratch memory where the layer-3 (e.g. IPv4 or IPv6)
    // packet length is stored.
    const bsl::uint32_t k_L3_PACKET_LENGTH = 6;

    // The index into scratch memory where the layer-4 protocol (e.g. TCP or
    // UDP) carried by the layer-3 (e.g. IPv4 or IPv6) packet is stored.
    const bsl::uint32_t k_L3_PROTOCOL = 7;

    // The index into scratch memory where the offset from the beginning of the
    // packet to the beginning of the layer-4 (e.g. TCP or UDP) header is
    // stored.
    const bsl::uint32_t k_L4_HEADER_OFFSET = 8;

    // The index into scratch memory where the layer-4 (e.g. TCP or UDP)
    // header length is stored.
    const bsl::uint32_t k_L4_HEADER_LENGTH = 9;

    // The index into scratch memory where the layer-4 (e.g. TCP or UDP)
    // packet length is stored.
    const bsl::uint32_t k_L4_PACKET_LENGTH = 10;

    // The index into scratch memory where the layer-5 protocol (e.g. the
    // application protocol) carried by the layer-4 (e.g. TCP or UDP) packet is
    // stored.
    const bsl::uint32_t k_L4_PROTOCOL = 11;

    // Start with a blank program. The BPF interpreter guarantees that the
    // accumulator register, the index register, and the scratch memory are
    // zeroed.

    program->clear();

    // This implementation only supports non-loopback Ethernet devices for now.

    if (deviceType == ntsa::DeviceType::e_LOCAL ||
        deviceType == ntsa::DeviceType::e_LOOPBACK)
    {
        PacketUtil::acceptAll(program);
        return ntsa::Error();
    }

    // This implementation only supports Ethernet link-level packet types.

    if (deviceType != ntsa::DeviceType::e_ETHERNET) {
        BALL_LOG_ERROR << "Failed to compile packet filter: the device type "
                       << deviceType << " is not supported" << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    PFS script;

    if (deviceType == ntsa::DeviceType::e_ETHERNET) {
        // Filter the Ethernet packet.

        PFC::label(&script, "filter-ethernet");

        // Store the Ethernet header length and protocol carried by the
        // Ethernet packet into scratch memory. The length of the Ethernet
        // header and offset of the protocol field depends on whether the
        // Ethernet header has a VLAN tag.

        PFC::label(&script, "store-ethernet-attributes");

        // Load the wire length into the accumulator register.

        PFC::compile(&script, NTSU_BPF_LD + NTSU_BPF_W + NTSU_BPF_LEN, 0);

        // The accumulator register into scratch memory to remember the
        // Ethernet packet length.

        PFC::compile(&script, NTSU_BPF_ST, k_L2_PACKET_LENGTH);

        // Load the 2-byte tag protocol identifier (TPID) field into the
        // accumulator register.

        PFC::compile(&script,
                     NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_ABS,
                     ntsa::EthernetHeader::k_TPID_OFFSET);

        // The Ethernet tag protocol identifier will be set to 0x8100 if the
        // Ethernet frame is 802.1Q tagged, and will indicate the layer-3
        // protocol otherwise.

        PFC::compile(&script,
                     NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                     0x8100,
                     "store-ethernet-attributes-vlan",
                     0);

        PFC::label(&script, "store-ethernet-attributes-standard");

        // Store the accumulator register into scratch memory to remember the
        // protocol carried by the Ethernet packet.

        PFC::compile(&script, NTSU_BPF_ST, k_L2_PROTOCOL);

        // Load the fixed-size length of a standard (non-tagged) Ethernet
        // header into the accumulator register.

        PFC::compile(&script,
                     NTSU_BPF_LD + NTSU_BPF_W + NTSU_BPF_K,
                     ntsa::EthernetHeader::k_MIN_HEADER_LENGTH);

        // Store the accumulator register into scratch memory to remember the
        // Ethernet header length.

        PFC::compile(&script, NTSU_BPF_ST, k_L2_HEADER_LENGTH);

        // Store the accumulator register into scratch memory to remember the
        // layer-3 header offset.

        PFC::compile(&script, NTSU_BPF_ST, k_L3_HEADER_OFFSET);

        // Jump to the end of the "if-802.1Q-tagged-else" branch.

        PFC::compile(&script,
                     NTSU_BPF_JMP + NTSU_BPF_JA,
                     "store-ethernet-attributes-end");

        PFC::label(&script, "store-ethernet-attributes-vlan");

        // Load the 2-byte Ethernet protocol field after the tag into the
        // accumulator register.

        PFC::compile(&script, NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_ABS, 14);

        // Store the accumulator register into scratch memory to remember the
        // protocol carried by the Ethernet packet.

        PFC::compile(&script, NTSU_BPF_ST, k_L2_PROTOCOL);

        // Load the fixed-size length of an 802.1Q tagged Ethernet header
        // into the accumulator register.

        PFC::compile(&script,
                     NTSU_BPF_LD + NTSU_BPF_W + NTSU_BPF_K,
                     ntsa::EthernetHeader::k_MAX_HEADER_LENGTH);

        // Store the accumulator register into scratch memory to remember the
        // Ethernet header length.

        PFC::compile(&script, NTSU_BPF_ST, k_L2_HEADER_LENGTH);

        // Store the accumulator register into scratch memory to remember the
        // layer-3 header offset.

        PFC::compile(&script, NTSU_BPF_ST, k_L3_HEADER_OFFSET);

        PFC::label(&script, "store-ethernet-attributes-end");

        // Reject the packet unless its source Ethernet address is allowed by
        // the filter.

        PFC::label(&script, "filter-ethernet-source-address");

        if (filter.sourceEthernetAddress().size() > 0) {
            for (bsl::size_t i = 0; i < filter.sourceEthernetAddress().size();
                 ++i)
            {
                PFC::label(
                    &script,
                    "filter-ethernet-source-address-" + bsl::to_string(i));

                const ntsa::EthernetAddress& ethernetAddress =
                    filter.sourceEthernetAddress()[i];

                const bsl::uint32_t ethernetAddress0 =
                    (static_cast<bsl::uint32_t>(ethernetAddress[0]) << 24) |
                    (static_cast<bsl::uint32_t>(ethernetAddress[1]) << 16) |
                    (static_cast<bsl::uint32_t>(ethernetAddress[2]) << 8) |
                    (static_cast<bsl::uint32_t>(ethernetAddress[3]));

                const bsl::uint32_t ethernetAddress4 =
                    (static_cast<bsl::uint32_t>(ethernetAddress[4]) << 8) |
                    (static_cast<bsl::uint32_t>(ethernetAddress[5]));

                PFC::compile(&script,
                             NTSU_BPF_LD + NTSU_BPF_W + NTSU_BPF_ABS,
                             ntsa::EthernetHeader::k_SOURCE_ADDRESS_OFFSET);
                PFC::compile(
                    &script,
                    NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                    ethernetAddress0,
                    0,
                    "filter-ethernet-source-address-" + bsl::to_string(i + 1));

                PFC::compile(&script,
                             NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_ABS,
                             ntsa::EthernetHeader::k_SOURCE_ADDRESS_OFFSET +
                                 sizeof(bsl::uint32_t));
                PFC::compile(
                    &script,
                    NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                    ethernetAddress4,
                    "filter-ethernet-source-address-end",
                    "filter-ethernet-source-address-" + bsl::to_string(i + 1));
            }

            PFC::label(
                &script,
                "filter-ethernet-source-address-" +
                    bsl::to_string(filter.sourceEthernetAddress().size()));

            PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
        }

        PFC::label(&script, "filter-ethernet-source-address-end");

        // Reject the packet unless its destination Ethernet address is allowed by
        // the filter.

        PFC::label(&script, "filter-ethernet-destination-address");

        if (filter.destinationEthernetAddress().size() > 0) {
            for (bsl::size_t i = 0;
                 i < filter.destinationEthernetAddress().size();
                 ++i)
            {
                PFC::label(&script,
                           "filter-ethernet-destination-address-" +
                               bsl::to_string(i));

                const ntsa::EthernetAddress& ethernetAddress =
                    filter.destinationEthernetAddress()[i];

                const bsl::uint32_t ethernetAddress0 =
                    (static_cast<bsl::uint32_t>(ethernetAddress[0]) << 24) |
                    (static_cast<bsl::uint32_t>(ethernetAddress[1]) << 16) |
                    (static_cast<bsl::uint32_t>(ethernetAddress[2]) << 8) |
                    (static_cast<bsl::uint32_t>(ethernetAddress[3]));

                const bsl::uint32_t ethernetAddress4 =
                    (static_cast<bsl::uint32_t>(ethernetAddress[4]) << 8) |
                    (static_cast<bsl::uint32_t>(ethernetAddress[5]));

                PFC::compile(
                    &script,
                    NTSU_BPF_LD + NTSU_BPF_W + NTSU_BPF_ABS,
                    ntsa::EthernetHeader::k_DESTINATION_ADDRESS_OFFSET);
                PFC::compile(&script,
                             NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                             ethernetAddress0,
                             0,
                             "filter-ethernet-destination-address-" +
                                 bsl::to_string(i + 1));

                PFC::compile(
                    &script,
                    NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_ABS,
                    ntsa::EthernetHeader::k_DESTINATION_ADDRESS_OFFSET +
                        sizeof(bsl::uint32_t));
                PFC::compile(&script,
                             NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                             ethernetAddress4,
                             "filter-ethernet-destination-address-end",
                             "filter-ethernet-destination-address-" +
                                 bsl::to_string(i + 1));
            }

            PFC::label(&script,
                       "filter-ethernet-destination-address-" +
                           bsl::to_string(
                               filter.destinationEthernetAddress().size()));

            PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
        }

        PFC::label(&script, "filter-ethernet-destination-address-end");

        // Reject the packet unless the Ethernet packet carries a protocol that
        // matches the valid packet types.

        PFC::label(&script, "filter-ethernet-protocol");

        if (filter.packetType().size() > 0) {
            PFC::compile(&script, NTSU_BPF_LD + NTSU_BPF_MEM, k_L2_PROTOCOL);

            for (bsl::size_t i = 0; i < filter.packetType().size(); ++i) {
                const ntsa::PacketType::Value packetType =
                    filter.packetType()[i];

                if (packetType == ntsa::PacketType::e_IPV4) {
                    PFC::compile(&script,
                                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                                 ntsa::EthernetProtocol::e_IPV4,
                                 "filter-ip",
                                 0);
                }
                else if (packetType == ntsa::PacketType::e_IPV6) {
                    PFC::compile(&script,
                                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                                 ntsa::EthernetProtocol::e_IPV6,
                                 "filter-ip",
                                 0);
                }
                else if (packetType == ntsa::PacketType::e_ARP) {
                    PFC::compile(&script,
                                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                                 ntsa::EthernetProtocol::e_ARP,
                                 "filter-arp",
                                 0);
                }
                else if (packetType == ntsa::PacketType::e_RARP) {
                    PFC::compile(&script,
                                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                                 ntsa::EthernetProtocol::e_RARP,
                                 "filter-rarp",
                                 0);
                }
            }

            PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
        }

        PFC::label(&script, "filter-ethernet-protocol-end");

        PFC::label(&script, "filter-ethernet-end");
    }

    PFC::label(&script, "filter-ip");

    // Detect the Internet Protocol version and jump to the appropriate
    // layer-3 filter.

    // Load the layer-3 header offset into the index register.
    //
    // Instruction:
    //     X = ipv4_header_offset
    //
    // State:
    //     A = 0
    //     X = ipv4_header_offset

    PFC::compile(&script, NTSU_BPF_LDX + NTSU_BPF_MEM, k_L3_HEADER_OFFSET);

    // Load the IP header version into the accumulator register. The IP header
    // version is always stored in the lower 4-bit nibble of the first byte
    // of either the IPv4 header or the IPv6 header.

    PFC::compile(&script, NTSU_BPF_LD + NTSU_BPF_B + NTSU_BPF_IND, 0);

    // Mask out everything but the 4-bit version in the upper nibble by bitwise
    // and-ing the accumulator register with 0xF0.

    PFC::compile(&script, NTSU_BPF_ALU + NTSU_BPF_AND + NTSU_BPF_K, 0xF0);
    PFC::compile(&script, NTSU_BPF_ALU + NTSU_BPF_RSH + NTSU_BPF_K, 4);

    // Jump to the start of the IPv4 filter if the Internet Protocol is 4.

    PFC::compile(&script,
                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                 4,
                 "filter-ipv4",
                 0);

    // Otherwise, jump to the start of the IPv6 filter if the Internet Protocol
    // is 6.

    PFC::compile(&script,
                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                 6,
                 "filter-ipv6",
                 0);

    // Otherwise, reject the packet.

    PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");

    PFC::label(&script, "filter-ipv4");

    // Load the IPv4 header version and length into the accumulator register.
    //
    // Instruction:
    //     A = (byte) packet[ethernet_header_length + 0]
    //
    // State:
    //     A = <IPv4 header version and length in 32-bit words>
    //     X = ipv4_header_offset

    PFC::compile(&script, NTSU_BPF_LD + NTSU_BPF_B + NTSU_BPF_IND, 0);

    // Mask out everything but the 4-bit header length in the lower nibble by
    // bitwise and-ing the accumulator register with 0x0F.
    //
    // Instruction:
    //     A = A & 0x0F
    //
    // State:
    //     A = <IPv4 header length in 32-bit words>
    //     X = ipv4_header_offset

    PFC::compile(&script, NTSU_BPF_ALU + NTSU_BPF_AND + NTSU_BPF_K, 0x0F);

    // Multiply the accumulator register by 4 (since the IPv4 header length
    // measures 32-bit words) to convert the length from the number of 32-bit
    // words to the number of bytes.
    //
    // Instruction:
    //     A = A * 4
    //
    // State:
    //     A = <IPv4 header length in bytes>
    //     X = ipv4_header_offset

    PFC::compile(&script, NTSU_BPF_ALU + NTSU_BPF_MUL + NTSU_BPF_K, 4);

    // Store the accumulator register into scratch memory to remember the
    // IPv4 header length.
    //
    // Instruction:
    //     ip_header_length = A
    //
    // State:
    //     A = <IPv4 header length in bytes>
    //     X = ipv4_header_offset

    PFC::compile(&script, NTSU_BPF_ST, k_L3_HEADER_LENGTH);

    // Add the IPv4 header length to the offset to the start of the IPV4 packet
    // to calculate the offset to the start of the transport header.

    PFC::compile(&script, NTSU_BPF_ALU + NTSU_BPF_ADD + NTSU_BPF_X, 0);

    // Store the accumulator register into scratch memory to remember the
    // layer-4 header offset.

    PFC::compile(&script, NTSU_BPF_ST, k_L4_HEADER_OFFSET);

    // Load the IPv4 packet length into the accumulator register.

    PFC::compile(&script,
                 NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_IND,
                 ntsa::Ipv4Header::k_TOTAL_LENGTH_OFFSET);

    // Store the accumulator register into scratch memory to remember the
    // protocol carried by the IPv4 packet.
    //
    // Instruction:
    //     ip_header_protocol = A
    //
    // State:
    //     A = <IPv4 packet length>
    //     X = ipv4_header_offset

    PFC::compile(&script, NTSU_BPF_ST, k_L3_PACKET_LENGTH);

    // Load the protocol carried by the IPv4 packet into the accumulator
    // register.
    //
    // Instruction:
    //     A = (byte) packet[ipv4_header_offset + 9]
    //
    // State:
    //     A = <IPv4 protocol>
    //     X = ipv4_header_offset

    PFC::compile(&script,
                 NTSU_BPF_LD + NTSU_BPF_B + NTSU_BPF_IND,
                 ntsa::Ipv4Header::k_PROTOCOL_OFFSET);

    // Store the accumulator register into scratch memory to remember the
    // protocol carried by the IPv4 packet.
    //
    // Instruction:
    //     ip_header_protocol = A
    //
    // State:
    //     A = <IPv4 protocol>
    //     X = ipv4_header_offset

    PFC::compile(&script, NTSU_BPF_ST, k_L3_PROTOCOL);

    PFC::label(&script, "filter-ipv4-source-address");

    if (filter.sourceIpv4Address().size() > 0) {
        // Load the 32-bit source IPv4 address from its absolute position inside
        // an IPv4 packet inside an Ethernet packet.

        PFC::compile(&script,
                     NTSU_BPF_LD + NTSU_BPF_W + NTSU_BPF_IND,
                     ntsa::Ipv4Header::k_SOURCE_ADDRESS_OFFSET);

        // Compare with each allowed source IPv4 address.

        for (bsl::size_t i = 0; i < filter.sourceIpv4Address().size(); ++i) {
            PFC::compile(&script,
                         NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                         BSLS_BYTEORDER_BE_U32_TO_HOST(
                             filter.sourceIpv4Address()[i].value()),
                         "filter-ipv4-source-address-end",
                         0);
        }

        PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
    }

    PFC::label(&script, "filter-ipv4-source-address-end");

    PFC::label(&script, "filter-ipv4-destination-address");

    if (filter.destinationIpv4Address().size() > 0) {
        // Load the 32-bit destination IPv4 address from its absolute position
        // inside an IPv4 packet inside an Ethernet packet.

        PFC::compile(&script,
                     NTSU_BPF_LD + NTSU_BPF_W + NTSU_BPF_IND,
                     ntsa::Ipv4Header::k_DESTINATION_ADDRESS_OFFSET);

        // Compare with each allowed destination IPv4 address.

        for (bsl::size_t i = 0; i < filter.destinationIpv4Address().size();
             ++i)
        {
            PFC::compile(&script,
                         NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                         BSLS_BYTEORDER_BE_U32_TO_HOST(
                             filter.destinationIpv4Address()[i].value()),
                         "filter-ipv4-destination-address-end",
                         0);
        }

        PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
    }

    PFC::label(&script, "filter-ipv4-destination-address-end");

    /// Load the transport protocol into the accumulator register.

    PFC::compile(&script, NTSU_BPF_LD + NTSU_BPF_MEM, k_L3_PROTOCOL);

    // Jump to the start of the TCP filter if the protocol carried by the IPv4
    // packet is TCP.

    PFC::compile(&script,
                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                 ntsa::Ipv4Header::k_PROTOCOL_TCP,
                 "filter-tcp",
                 0);

    // Jump to the start of the UDP filter if the protocol carried by the IPv4
    // packet is UDP.

    PFC::compile(&script,
                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                 ntsa::Ipv4Header::k_PROTOCOL_UDP,
                 "filter-udp",
                 0);

    // Jump to the start of the ICMP filter if the protocol carried by the IPv4
    // packet is ICMP.

    PFC::compile(&script,
                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                 ntsa::Ipv4Header::k_PROTOCOL_ICMP,
                 "filter-icmp",
                 0);

    // Jump to the start of the IGMP filter if the protocol carried by the IPv4
    // packet is IGMP.

    PFC::compile(&script,
                 NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                 ntsa::Ipv4Header::k_PROTOCOL_ICMP,
                 "filter-igmp",
                 0);

    // Otherwise, accept the packet.

    PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "accept");

    PFC::label(&script, "filter-ipv4-end");

    PFC::label(&script, "filter-ipv6");

    PFC::label(&script, "filter-ipv4-end");

    PFC::label(&script, "filter-tcp");

    const bool wantTcp =
        filter.packetType().empty() ||
        bsl::find(filter.packetType().begin(),
                  filter.packetType().end(),
                  ntsa::PacketType::e_TCP) != filter.packetType().end();

    if (wantTcp) {
        // Load the layer-4 header offset into the index register.
        //
        // Instruction:
        //     X = tcp_header_offset
        //
        // State:
        //     A = tcp_source_port
        //     X = tcp_header_offset

        PFC::compile(&script, NTSU_BPF_LDX + NTSU_BPF_MEM, k_L4_HEADER_OFFSET);

        PFC::label(&script, "filter-tcp-source-port");

        if (filter.sourceTcpPort().size() > 0) {
            // Load the source port into the accumulator register.

            PFC::compile(&script,
                         NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_IND,
                         ntsa::TcpHeader::k_SOURCE_PORT_OFFSET);

            // Compare with each allowed source TCP port.

            for (bsl::size_t i = 0; i < filter.sourceTcpPort().size(); ++i) {
                const ntsa::Port sourceTcpPort = filter.sourceTcpPort()[i];

                PFC::compile(&script,
                             NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                             sourceTcpPort,
                             "filter-tcp-source-port-end",
                             0);
            }

            PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
        }

        PFC::label(&script, "filter-tcp-source-port-end");

        PFC::label(&script, "filter-tcp-destination-port");

        if (filter.destinationTcpPort().size() > 0) {
            // Load the destination port into the accumulator register.

            PFC::compile(&script,
                         NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_IND,
                         ntsa::TcpHeader::k_DESTINATION_PORT_OFFSET);

            // Compare with each allowed destination TCP port.

            for (bsl::size_t i = 0; i < filter.destinationTcpPort().size();
                 ++i)
            {
                const ntsa::Port destinationTcpPort =
                    filter.destinationTcpPort()[i];

                PFC::compile(&script,
                             NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                             destinationTcpPort,
                             "filter-tcp-destination-port-end",
                             0);
            }

            PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
        }

        PFC::label(&script, "filter-tcp-destination-port-end");
    }
    else {
        PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
    }

    PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "accept");

    PFC::label(&script, "filter-tcp-end");

    PFC::label(&script, "filter-udp");

    const bool wantUdp =
        filter.packetType().empty() ||
        bsl::find(filter.packetType().begin(),
                  filter.packetType().end(),
                  ntsa::PacketType::e_UDP) != filter.packetType().end();

    if (wantUdp) {
        // Load the layer-4 header offset into the index register.
        //
        // Instruction:
        //     X = udp_header_offset
        //
        // State:
        //     A = udp_source_port
        //     X = udp_header_offset

        PFC::compile(&script, NTSU_BPF_LDX + NTSU_BPF_MEM, k_L4_HEADER_OFFSET);

        PFC::label(&script, "filter-udp-source-port");

        if (filter.sourceUdpPort().size() > 0) {
            // Load the source port into the accumulator register.

            PFC::compile(&script,
                         NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_IND,
                         ntsa::UdpHeader::k_SOURCE_PORT_OFFSET);

            for (bsl::size_t i = 0; i < filter.sourceUdpPort().size(); ++i) {
                const ntsa::Port sourceUdpPort = filter.sourceUdpPort()[i];

                PFC::compile(&script,
                             NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                             sourceUdpPort,
                             "filter-udp-source-port-end",
                             0);
            }

            PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
        }

        PFC::label(&script, "filter-udp-source-port-end");

        PFC::label(&script, "filter-udp-destination-port");

        if (filter.destinationUdpPort().size() > 0) {
            // Load the destination port into the accumulator register.

            PFC::compile(&script,
                         NTSU_BPF_LD + NTSU_BPF_H + NTSU_BPF_IND,
                         ntsa::UdpHeader::k_DESTINATION_PORT_OFFSET);

            for (bsl::size_t i = 0; i < filter.destinationUdpPort().size();
                 ++i)
            {
                const ntsa::Port destinationUdpPort =
                    filter.destinationUdpPort()[i];

                PFC::compile(&script,
                             NTSU_BPF_JMP + NTSU_BPF_JEQ + NTSU_BPF_K,
                             destinationUdpPort,
                             "filter-udp-destination-port-end",
                             0);
            }

            PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
        }

        PFC::label(&script, "filter-udp-destination-port-end");
    }
    else {
        PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
    }

    PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "accept");

    PFC::label(&script, "filter-udp-end");

    PFC::label(&script, "filter-icmp");

    const bool wantIcmp =
        filter.packetType().empty() ||
        bsl::find(filter.packetType().begin(),
                  filter.packetType().end(),
                  ntsa::PacketType::e_ICMP) != filter.packetType().end();

    if (wantIcmp) {
        // TODO
    }
    else {
        PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
    }

    PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "accept");

    PFC::label(&script, "filter-icmp-end");

    PFC::label(&script, "filter-igmp");

    const bool wantIgmp =
        filter.packetType().empty() ||
        bsl::find(filter.packetType().begin(),
                  filter.packetType().end(),
                  ntsa::PacketType::e_IGMP) != filter.packetType().end();

    if (wantIgmp) {
        // TODO
    }
    else {
        PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
    }

    PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "accept");

    PFC::label(&script, "filter-igmp-end");

    PFC::label(&script, "filter-arp");

    const bool wantArp =
        filter.packetType().empty() ||
        bsl::find(filter.packetType().begin(),
                  filter.packetType().end(),
                  ntsa::PacketType::e_ARP) != filter.packetType().end();

    if (wantArp) {
        // TODO
    }
    else {
        PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
    }

    PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "accept");

    PFC::label(&script, "filter-arp-end");

    PFC::label(&script, "filter-rarp");

    const bool wantRarp =
        filter.packetType().empty() ||
        bsl::find(filter.packetType().begin(),
                  filter.packetType().end(),
                  ntsa::PacketType::e_RARP) != filter.packetType().end();

    if (wantRarp) {
        // TODO
    }
    else {
        PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "reject");
    }

    PFC::compile(&script, NTSU_BPF_JMP + NTSU_BPF_JA, "accept");

    PFC::label(&script, "filter-rarp-end");

    PFC::label(&script, "accept");
    PFC::compile(&script,
                 NTSU_BPF_RET + NTSU_BPF_K,
                 static_cast<bsl::uint32_t>(INT_MAX));

    PFC::label(&script, "reject");
    PFC::compile(&script, NTSU_BPF_RET + NTSU_BPF_K, 0);

    error = PFC::link(program, script);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

void PacketUtil::acceptAll(PacketFilter::Program* program)
{
    PacketFilter::Script script;
    PacketFilter::Compiler::compile(&script,
                                    NTSU_BPF_RET + NTSU_BPF_K,
                                    static_cast<bsl::uint32_t>(INT_MAX));
    PacketFilter::Compiler::link(program, script);
}

void PacketUtil::rejectAll(PacketFilter::Program* program)
{
    PacketFilter::Script script;
    PacketFilter::Compiler::compile(&script, NTSU_BPF_RET + NTSU_BPF_K, 0);
    PacketFilter::Compiler::link(program, script);
}

bool PacketUtil::execute(const PacketFilter::Program& program,
                         const bdlbb::BlobBuffer&     packet)
{
    if (program.empty()) {
        return true;
    }

    const bsl::uint8_t* input = reinterpret_cast<bsl::uint8_t*>(packet.data());
    bsl::uint32_t       inputSize = static_cast<bsl::uint32_t>(packet.size());

    enum { k_SCRATCH_WORDS = 16 };

    bsl::uint32_t scratch[k_SCRATCH_WORDS];
    NTSCFG_MEMORY_ZERO(scratch, sizeof scratch);

    bsl::uint32_t A = 0;
    bsl::uint32_t X = 0;

    bsl::uint32_t k = 0;

    bsl::uint32_t result = 0;

    const PacketFilter::Instruction* pcBegin = &program.front();
    const PacketFilter::Instruction* pcEnd   = pcBegin + program.size();
    const PacketFilter::Instruction* pc      = pcBegin - 1;

    bool done = false;

    while (!done) {
        ++pc;

        if (pc >= pcEnd) {
            NTSU_PACKETUTIL_LOG_INVALID_PC(pc);
            return false;
        }

        switch (pc->code) {
        case NTSU_BPF_RET | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "RET+K");
            result = pc->k;
            done   = true;
            break;

        case NTSU_BPF_RET | NTSU_BPF_A:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "RET+A");
            result = A;
            done   = true;
            break;

        case NTSU_BPF_LD | NTSU_BPF_W | NTSU_BPF_ABS:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LD+W+ABS");
            k = pc->k;
            if (k > inputSize || sizeof(bsl::int32_t) > inputSize - k) {
                return 0;
            }
            A = NTSU_EXTRACT_BE_U_4(&input[k]);
            break;

        case NTSU_BPF_LD | NTSU_BPF_H | NTSU_BPF_ABS:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LD+H+ABS");
            k = pc->k;
            if (k > inputSize || sizeof(bsl::int16_t) > inputSize - k) {
                return 0;
            }
            A = NTSU_EXTRACT_BE_U_2(&input[k]);
            break;

        case NTSU_BPF_LD | NTSU_BPF_B | NTSU_BPF_ABS:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LD+B+ABS");
            k = pc->k;
            if (k >= inputSize) {
                return 0;
            }
            A = input[k];
            break;

        case NTSU_BPF_LD | NTSU_BPF_W | NTSU_BPF_LEN:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LD+W+LEN");
            A = inputSize;
            break;

        case NTSU_BPF_LDX | NTSU_BPF_W | NTSU_BPF_LEN:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LDX+W+LEN");
            X = inputSize;
            break;

        case NTSU_BPF_LD | NTSU_BPF_W | NTSU_BPF_IND:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LD+W+IND");
            k = X + pc->k;
            if (pc->k > inputSize || X > inputSize - pc->k ||
                sizeof(bsl::int32_t) > inputSize - k)
            {
                return 0;
            }
            A = NTSU_EXTRACT_BE_U_4(&input[k]);
            break;

        case NTSU_BPF_LD | NTSU_BPF_H | NTSU_BPF_IND:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LD+H+IND");
            k = X + pc->k;
            if (X > inputSize || pc->k > inputSize - X ||
                sizeof(bsl::int16_t) > inputSize - k)
            {
                return 0;
            }
            A = NTSU_EXTRACT_BE_U_2(&input[k]);
            break;

        case NTSU_BPF_LD | NTSU_BPF_B | NTSU_BPF_IND:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LD+B+IND");
            k = X + pc->k;
            if (pc->k >= inputSize || X >= inputSize - pc->k) {
                return 0;
            }
            A = input[k];
            break;

        case NTSU_BPF_LDX | NTSU_BPF_B | NTSU_BPF_MSH:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LDX+B+MSH");
            k = pc->k;
            if (k >= inputSize) {
                return 0;
            }
            X = (input[pc->k] & 0x0F) << 2;
            break;

        case NTSU_BPF_LD | NTSU_BPF_IMM:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LD+IMM");
            A = pc->k;
            break;

        case NTSU_BPF_LDX | NTSU_BPF_IMM:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LDX+IMM");
            X = pc->k;
            break;

        case NTSU_BPF_LD | NTSU_BPF_MEM:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LD+MEM");
            A = scratch[pc->k];
            break;

        case NTSU_BPF_LDX | NTSU_BPF_MEM:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LDX+MEM");
            X = scratch[pc->k];
            break;

        case NTSU_BPF_ST:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "ST");
            scratch[pc->k] = A;
            break;

        case NTSU_BPF_STX:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "STX");
            scratch[pc->k] = X;
            break;

        case NTSU_BPF_JMP | NTSU_BPF_JA:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "JA");
            pc += pc->k;
            break;

        case NTSU_BPF_JMP | NTSU_BPF_JGT | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "JGT+K");
            pc += (A > pc->k) ? pc->jt : pc->jf;
            break;

        case NTSU_BPF_JMP | NTSU_BPF_JGE | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "JGE+K");
            pc += (A >= pc->k) ? pc->jt : pc->jf;
            break;

        case NTSU_BPF_JMP | NTSU_BPF_JEQ | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "JEQ+K");
            pc += (A == pc->k) ? pc->jt : pc->jf;
            break;

        case NTSU_BPF_JMP | NTSU_BPF_JSET | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "JSET+K");
            pc += ((A & pc->k) != 0) ? pc->jt : pc->jf;
            break;

        case NTSU_BPF_JMP | NTSU_BPF_JGT | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "JGT+X");
            pc += (A > X) ? pc->jt : pc->jf;
            break;

        case NTSU_BPF_JMP | NTSU_BPF_JGE | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "JGE+X");
            pc += (A >= X) ? pc->jt : pc->jf;
            break;

        case NTSU_BPF_JMP | NTSU_BPF_JEQ | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "JEQ+X");
            pc += (A == X) ? pc->jt : pc->jf;
            break;

        case NTSU_BPF_JMP | NTSU_BPF_JSET | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "JSET+X");
            pc += (A & X) ? pc->jt : pc->jf;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_ADD | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "ADD+X");
            A += X;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_SUB | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "SUB+X");
            A -= X;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_MUL | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "MUL+X");
            A *= X;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_DIV | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "DIV+X");
            if (X == 0) {
                return 0;
            }
            A /= X;
            break;

#if NTSU_BPF_EXTENSIONS
        case NTSU_BPF_ALU | NTSU_BPF_MOD | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "MOD+X");
            if (X == 0) {
                return 0;
            }
            A %= X;
            break;
#endif

        case NTSU_BPF_ALU | NTSU_BPF_AND | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "AND+X");
            A &= X;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_OR | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "OR+X");
            A |= X;
            break;

#if NTSU_BPF_EXTENSIONS
        case NTSU_BPF_ALU | NTSU_BPF_XOR | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "XOR+X");
            A ^= X;
            break;
#endif

        case NTSU_BPF_ALU | NTSU_BPF_LSH | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LSH+X");
            if (X < 32) {
                A <<= X;
            }
            else {
                A = 0;
            }
            break;

        case NTSU_BPF_ALU | NTSU_BPF_RSH | NTSU_BPF_X:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "RSH+X");
            if (X < 32) {
                A >>= X;
            }
            else {
                A = 0;
            }
            break;

        case NTSU_BPF_ALU | NTSU_BPF_ADD | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "ADD+K");
            A += pc->k;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_SUB | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "SUB+K");
            A -= pc->k;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_MUL | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "MUL+K");
            A *= pc->k;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_DIV | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "DIV+K");
            A /= pc->k;
            break;

#if NTSU_BPF_EXTENSIONS
        case NTSU_BPF_ALU | NTSU_BPF_MOD | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "MOD+K");
            A %= pc->k;
            break;
#endif

        case NTSU_BPF_ALU | NTSU_BPF_AND | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "AND+K");
            A &= pc->k;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_OR | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "OR+K");
            A |= pc->k;
            break;

#if NTSU_BPF_EXTENSIONS
        case NTSU_BPF_ALU | NTSU_BPF_XOR | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "XOR+K");
            A ^= pc->k;
            break;
#endif

        case NTSU_BPF_ALU | NTSU_BPF_LSH | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "LSH+K");
            A <<= pc->k;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_RSH | NTSU_BPF_K:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "RSH+K");
            A >>= pc->k;
            break;

        case NTSU_BPF_ALU | NTSU_BPF_NEG:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "NEG");
            A = (0U - A);
            break;

        case NTSU_BPF_MISC | NTSU_BPF_TAX:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "TAX");
            X = A;
            break;

        case NTSU_BPF_MISC | NTSU_BPF_TXA:
            NTSU_PACKETUTIL_LOG_INSTRUCTION(pcBegin, pc, "TXA");
            A = X;
            break;

        default:
            NTSU_PACKETUTIL_LOG_INVALID_CODE(pc);
            return false;
        }

        NTSU_PACKETUTIL_LOG_STATE(A, X, scratch);
    }

    if (result == 0) {
        return false;
    }
    else {
        if (inputSize <= result) {
            return true;
        }
        else {
            return false;
        }
    }
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createTcp(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    ntsa::Port                                  sourceTcpPort,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    ntsa::Port                                  destinationTcpPort)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_TCP);
    ipv4Packet.header().setPreserve(true);

    ntsa::TcpPacket& tcpPacket = ipv4Packet.payload().makeTcp();

    tcpPacket.header().setSourcePort(sourceTcpPort);
    tcpPacket.header().setDestinationPort(destinationTcpPort);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createTcp(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv6Address&                    sourceIpv6Address,
    ntsa::Port                                  sourceTcpPort,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv6Address&                    destinationIpv6Address,
    ntsa::Port                                  destinationTcpPort)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV6);

    ntsa::Ipv6Packet& ipv6Packet = ethernetPacket.payload().makeIpv6();

    ipv6Packet.header().setSourceAddress(sourceIpv6Address);
    ipv6Packet.header().setDestinationAddress(destinationIpv6Address);
    ipv6Packet.header().setNextHeader(ntsa::Ipv6Header::k_PROTOCOL_TCP);

    ntsa::TcpPacket& tcpPacket = ipv6Packet.payload().makeTcp();

    tcpPacket.header().setSourcePort(sourceTcpPort);
    tcpPacket.header().setDestinationPort(destinationTcpPort);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createUdp(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    ntsa::Port                                  sourceUdpPort,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    ntsa::Port                                  destinationUdpPort)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_UDP);
    ipv4Packet.header().setPreserve(true);

    ntsa::UdpPacket& udpPacket = ipv4Packet.payload().makeUdp();

    udpPacket.header().setSourcePort(sourceUdpPort);
    udpPacket.header().setDestinationPort(destinationUdpPort);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createUdp(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv6Address&                    sourceIpv6Address,
    ntsa::Port                                  sourceUdpPort,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv6Address&                    destinationIpv6Address,
    ntsa::Port                                  destinationUdpPort)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV6);

    ntsa::Ipv6Packet& ipv6Packet = ethernetPacket.payload().makeIpv6();

    ipv6Packet.header().setSourceAddress(sourceIpv6Address);
    ipv6Packet.header().setDestinationAddress(destinationIpv6Address);
    ipv6Packet.header().setNextHeader(ntsa::Ipv6Header::k_PROTOCOL_UDP);

    ntsa::UdpPacket& udpPacket = ipv6Packet.payload().makeUdp();

    udpPacket.header().setSourcePort(sourceUdpPort);
    udpPacket.header().setDestinationPort(destinationUdpPort);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIcmpEchoRequest(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IcmpEchoRequest&                payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_ICMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IcmpPacket& icmpPacket = ipv4Packet.payload().makeIcmp();

    icmpPacket.header().setType(ntsa::IcmpType::e_ECHO_REQUEST);
    icmpPacket.header().setCode(0);

    icmpPacket.payload().makeEchoRequest(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIcmpEchoResponse(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IcmpEchoResponse&               payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_ICMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IcmpPacket& icmpPacket = ipv4Packet.payload().makeIcmp();

    icmpPacket.header().setType(ntsa::IcmpType::e_ECHO_RESPONSE);
    icmpPacket.header().setCode(0);

    icmpPacket.payload().makeEchoResponse(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIcmpRouterRequest(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IcmpRouterRequest&              payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_ICMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IcmpPacket& icmpPacket = ipv4Packet.payload().makeIcmp();

    icmpPacket.header().setType(ntsa::IcmpType::e_ROUTER_REQUEST);
    icmpPacket.header().setCode(0);

    icmpPacket.payload().makeRouterRequest(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIcmpRouterResponse(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IcmpRouterResponse&             payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_ICMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IcmpPacket& icmpPacket = ipv4Packet.payload().makeIcmp();

    icmpPacket.header().setType(ntsa::IcmpType::e_ROUTER_RESPONSE);
    icmpPacket.header().setCode(0);

    icmpPacket.payload().makeRouterResponse(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIcmpUnreachable(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IcmpUnreachable&                payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_ICMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IcmpPacket& icmpPacket = ipv4Packet.payload().makeIcmp();

    icmpPacket.header().setType(ntsa::IcmpType::e_UNREACHABLE);
    icmpPacket.header().setCode(0);

    icmpPacket.payload().makeUnreachable(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIcmpTimeout(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IcmpTimeout&                    payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_ICMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IcmpPacket& icmpPacket = ipv4Packet.payload().makeIcmp();

    icmpPacket.header().setType(ntsa::IcmpType::e_TIMEOUT);
    icmpPacket.header().setCode(0);

    icmpPacket.payload().makeTimeout(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIcmpProblem(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IcmpProblem&                    payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_ICMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IcmpPacket& icmpPacket = ipv4Packet.payload().makeIcmp();

    icmpPacket.header().setType(ntsa::IcmpType::e_PROBLEM);
    icmpPacket.header().setCode(0);

    icmpPacket.payload().makeProblem(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIgmpJoin(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IgmpJoin&                       payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_IGMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IgmpPacket& igmpPacket = ipv4Packet.payload().makeIgmp();

    igmpPacket.header().setType(ntsa::IgmpType::e_REPORT_V2);
    igmpPacket.header().setMaxResponseCode(0);

    igmpPacket.payload().makeJoin(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIgmpLeave(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IgmpLeave&                      payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_IGMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IgmpPacket& igmpPacket = ipv4Packet.payload().makeIgmp();

    igmpPacket.header().setType(ntsa::IgmpType::e_LEAVE);
    igmpPacket.header().setMaxResponseCode(0);

    igmpPacket.payload().makeLeave(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIgmpQuery(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IgmpQuery&                      payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_IGMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IgmpPacket& igmpPacket = ipv4Packet.payload().makeIgmp();

    igmpPacket.header().setType(ntsa::IgmpType::e_QUERY);
    igmpPacket.header().setMaxResponseCode(30);

    igmpPacket.payload().makeQuery(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createIgmpReport(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::Ipv4Address&                    sourceIpv4Address,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::Ipv4Address&                    destinationIpv4Address,
    const ntsa::IgmpReport&                     payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4Packet = ethernetPacket.payload().makeIpv4();

    ipv4Packet.header().setSourceAddress(sourceIpv4Address);
    ipv4Packet.header().setDestinationAddress(destinationIpv4Address);
    ipv4Packet.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_IGMP);
    ipv4Packet.header().setPreserve(true);

    ntsa::IgmpPacket& igmpPacket = ipv4Packet.payload().makeIgmp();

    igmpPacket.header().setType(ntsa::IgmpType::e_REPORT_V3);
    igmpPacket.header().setMaxResponseCode(0);

    igmpPacket.payload().makeReport(payload);
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createArpRequest(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::ArpRequest&                     payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_ARP);

    ntsa::ArpPacket& arpPacket = ethernetPacket.payload().makeArp();

    arpPacket.header().setHardwareType(
        ntsa::ArpHeader::k_HARDWARE_TYPE_ETHERNET);
    arpPacket.header().setHardwareAddressLength(sizeof(ntsa::EthernetAddress));
    arpPacket.header().setProtocolType(ntsa::ArpHeader::k_PROTOCOL_TYPE_IPV4);
    arpPacket.header().setProtocolAddressLength(sizeof(ntsa::Ipv4Address));
    arpPacket.header().setOperation(ntsa::ArpType::e_REQUEST);

    arpPacket.payload().makeRequest(payload);

    return packet;
}

bsl::shared_ptr<ntsa::Packet> PacketUtil::createArpResponse(
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::EthernetAddress&                sourceEthernetAddress,
    const ntsa::EthernetAddress&                destinationEthernetAddress,
    const ntsa::ArpResponse&                    payload)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernetPacket = packet->makeEthernet();

    ethernetPacket.header().setSource(sourceEthernetAddress);
    ethernetPacket.header().setDestination(destinationEthernetAddress);
    ethernetPacket.header().setProtocol(ntsa::EthernetProtocol::e_ARP);

    ntsa::ArpPacket& arpPacket = ethernetPacket.payload().makeArp();

    arpPacket.header().setHardwareType(
        ntsa::ArpHeader::k_HARDWARE_TYPE_ETHERNET);
    arpPacket.header().setHardwareAddressLength(sizeof(ntsa::EthernetAddress));
    arpPacket.header().setProtocolType(ntsa::ArpHeader::k_PROTOCOL_TYPE_IPV4);
    arpPacket.header().setProtocolAddressLength(sizeof(ntsa::Ipv4Address));
    arpPacket.header().setOperation(ntsa::ArpType::e_REQUEST);

    arpPacket.payload().makeResponse(payload);

    return packet;
}

}  // close package namespace
}  // close enterprise namespace
