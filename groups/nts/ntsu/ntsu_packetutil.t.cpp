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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_packetutil_t_cpp, "$Id$ $CSID$")

#include <ntsu_packetutil.h>

#include <ntsa_adapter.h>
#include <ntsa_packet.h>
#include <ntsa_packetfactory.h>
#include <ntsa_packetpool.h>
#include <ntsu_adapterutil.h>
#include <bdlb_print.h>
#include <bsls_platform.h>

#if NTS_BUILD_WITH_PCAP
#include <pcap.h>
#else
#if defined(BSLS_PLATFORM_OS_DARWIN)
#include <net/bpf.h>
#endif
#endif

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsu {

// Provide tests for 'ntsu::PacketUtil'.
class PacketUtilTest
{
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.PACKETUTIL.TEST");

    /// Discover the loopback device and load its adapter into the specified
    /// 'result'. Return true if such a loopback device is found, and false
    /// otherwise.
    static bool discoverLoopback(ntsa::Adapter* result);

    /// Discover the default device and load its adapter into the specified
    /// 'result'. Return true if such a default device is found, and false
    /// otherwise.
    static bool discoverDefault(ntsa::Adapter* result);

    // Return a packet created through the specified 'packetFactory' from the
    // specified 'adapter' to that same 'adapter'.
    static bsl::shared_ptr<ntsa::Packet> createPacket(
        const ntsa::Adapter&                        adapter,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory);

    /// Execute the specified packet filter 'program' on the specified
    /// 'packetData'. Return true if the packet is allowed, and return false if
    /// the packet is rejected.
    static bool execute(const ntsu::PacketFilter::Program& program,
                        const bdlbb::BlobBuffer&           packetData);

  public:
    // TODO
    static void verifyConstants();

    // TODO
    static void verifyFilterLoopback();

    // TODO
    static void verifyFilterDefault();
};

bool PacketUtilTest::discoverLoopback(ntsa::Adapter* result)
{
    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);

    for (bsl::size_t i = 0; i < adapterList.size(); ++i) {
        const ntsa::Adapter& candidateAdapter = adapterList[i];
        if (candidateAdapter.ipv4Address().has_value()) {
            if (candidateAdapter.ipv4Address().value().isLoopback()) {
                *result = candidateAdapter;
                return true;
            }
        }
    }

    return false;
}

bool PacketUtilTest::discoverDefault(ntsa::Adapter* result)
{
    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);

    for (bsl::size_t i = 0; i < adapterList.size(); ++i) {
        const ntsa::Adapter& candidateAdapter = adapterList[i];
        if (candidateAdapter.ipv4Address().has_value()) {
            if (!candidateAdapter.ipv4Address().value().isLoopback()) {
                *result = candidateAdapter;
                return true;
            }
        }
    }

    return false;
}

bsl::shared_ptr<ntsa::Packet> PacketUtilTest::createPacket(
    const ntsa::Adapter&                        adapter,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernet = packet->makeEthernet();

    ntsa::EthernetAddress sourceEthernetAddress;
    ntsa::EthernetAddress destinationEthernetAddress;

    sourceEthernetAddress.parse(adapter.ethernetAddress());
    destinationEthernetAddress.parse(adapter.ethernetAddress());

    ethernet.header().setSource(sourceEthernetAddress);
    ethernet.header().setDestination(destinationEthernetAddress);

    ethernet.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4 = ethernet.payload().makeIpv4();

    ntsa::Ipv4Address sourceIpv4Address      = adapter.ipv4Address().value();
    ntsa::Ipv4Address destinationIpv4Address = adapter.ipv4Address().value();

    ipv4.header().setSourceAddress(sourceIpv4Address);
    ipv4.header().setDestinationAddress(destinationIpv4Address);

    ipv4.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_UDP);
    ipv4.header().setId(1);
    ipv4.header().setPreserve(true);

    ntsa::UdpPacket& udp = ipv4.payload().makeUdp();

    const ntsa::Port sourceUdpPort      = 3001;
    const ntsa::Port destinationUdpPort = 4001;

    udp.header().setSourcePort(sourceUdpPort);
    udp.header().setDestinationPort(destinationUdpPort);

    bdlbb::BlobBuffer payload;
    packetFactory->createOutgoingBlobBuffer(&payload);

    NTSCFG_MEMORY_COPY(payload.data(), "Hello, world!", 13);
    payload.setSize(13);

    udp.setPayload(payload);

    return packet;
}

#if NTS_BUILD_WITH_PCAP

bool PacketUtilTest::execute(const ntsu::PacketFilter::Program& program,
                             const bdlbb::BlobBuffer&           packetData)
{
    bool ourResult = ntsu::PacketUtil::execute(program, packetData);

    BALL_LOG_DEBUG << "OurResult = " << ourResult << BALL_LOG_END;


    pcap_t *dead_handle = pcap_open_dead(DLT_EN10MB, 65535);

    struct bpf_program fp;
    NTSCFG_MEMORY_ZERO(&fp, sizeof fp);

    fp.bf_insns = const_cast<struct bpf_insn*>(
        reinterpret_cast<const struct bpf_insn*>(&program.front()));
    fp.bf_len = static_cast<bsl::uint32_t>(program.size());

    struct pcap_pkthdr md;
    NTSCFG_MEMORY_ZERO(&md, sizeof md);

    md.len    = packetData.size();
    md.caplen = packetData.size();

    // Evaluate the packet directly against the compiled BPF program
    // Returns non-zero on a match, or zero if it fails the filter.

    int match = pcap_offline_filter(
        &fp,
        &md,
        reinterpret_cast<const bsl::uint8_t*>(packetData.data()));

    pcap_close(dead_handle);

    if (match) {
        return true;
    }
    else {
        return false;
    }
}

#else

bool PacketUtilTest::execute(const ntsu::PacketFilter::Program& program,
                             const bdlbb::BlobBuffer&           packetData)
{
    NTSCFG_NOT_IMPLEMENTED();
    return false;
}

#endif

#if defined(BSLS_PLATFORM_OS_DARWIN)

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyConstants)
{
    NTSCFG_TEST_EQ(NTSU_BPF_LD, BPF_LD);
    NTSCFG_TEST_EQ(NTSU_BPF_LDX, BPF_LDX);
    NTSCFG_TEST_EQ(NTSU_BPF_ST, BPF_ST);
    NTSCFG_TEST_EQ(NTSU_BPF_STX, BPF_STX);
    NTSCFG_TEST_EQ(NTSU_BPF_ALU, BPF_ALU);
    NTSCFG_TEST_EQ(NTSU_BPF_JMP, BPF_JMP);
    NTSCFG_TEST_EQ(NTSU_BPF_RET, BPF_RET);
    NTSCFG_TEST_EQ(NTSU_BPF_MISC, NTSU_BPF_MISC);

    NTSCFG_TEST_EQ(NTSU_BPF_W, BPF_W);
    NTSCFG_TEST_EQ(NTSU_BPF_H, BPF_H);
    NTSCFG_TEST_EQ(NTSU_BPF_B, BPF_B);

    NTSCFG_TEST_EQ(NTSU_BPF_IMM, BPF_IMM);
    NTSCFG_TEST_EQ(NTSU_BPF_ABS, BPF_ABS);
    NTSCFG_TEST_EQ(NTSU_BPF_IND, BPF_IND);
    NTSCFG_TEST_EQ(NTSU_BPF_MEM, BPF_MEM);
    NTSCFG_TEST_EQ(NTSU_BPF_LEN, BPF_LEN);
    NTSCFG_TEST_EQ(NTSU_BPF_MSH, BPF_MSH);

    NTSCFG_TEST_EQ(NTSU_BPF_ADD, BPF_ADD);
    NTSCFG_TEST_EQ(NTSU_BPF_SUB, BPF_SUB);
    NTSCFG_TEST_EQ(NTSU_BPF_MUL, BPF_MUL);
    NTSCFG_TEST_EQ(NTSU_BPF_DIV, BPF_DIV);
    NTSCFG_TEST_EQ(NTSU_BPF_OR, BPF_OR);
    NTSCFG_TEST_EQ(NTSU_BPF_AND, BPF_AND);
    NTSCFG_TEST_EQ(NTSU_BPF_LSH, BPF_LSH);
    NTSCFG_TEST_EQ(NTSU_BPF_RSH, BPF_RSH);
    NTSCFG_TEST_EQ(NTSU_BPF_NEG, BPF_NEG);
    NTSCFG_TEST_EQ(NTSU_BPF_JA, BPF_JA);
    NTSCFG_TEST_EQ(NTSU_BPF_JEQ, BPF_JEQ);
    NTSCFG_TEST_EQ(NTSU_BPF_JGT, BPF_JGT);
    NTSCFG_TEST_EQ(NTSU_BPF_JGE, BPF_JGE);
    NTSCFG_TEST_EQ(NTSU_BPF_JSET, BPF_JSET);

    NTSCFG_TEST_EQ(NTSU_BPF_K, BPF_K);
    NTSCFG_TEST_EQ(NTSU_BPF_X, BPF_X);

    NTSCFG_TEST_EQ(NTSU_BPF_A, BPF_A);

    NTSCFG_TEST_EQ(NTSU_BPF_TAX, BPF_TAX);
    NTSCFG_TEST_EQ(NTSU_BPF_TXA, BPF_TXA);
}

#else

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyConstants)
{
}

#endif

#if NTS_BUILD_WITH_PCAP

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyFilterLoopback)
{
    BALL_LOG_DEBUG << "Verifying packet filter with libcap" << BALL_LOG_END;

    ntsa::Adapter adapter;
    if (!PacketUtilTest::discoverLoopback(&adapter)) {
        return;
    }
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyFilterDefault)
{
    BALL_LOG_DEBUG << "Verifying packet filter with libcap" << BALL_LOG_END;

    ntsa::Error error;

    ntsa::Adapter adapter;
    if (!PacketUtilTest::discoverDefault(&adapter)) {
        return;
    }

    bsl::shared_ptr<ntsa::PacketPool> packetPool;
    packetPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                             static_cast<bsl::size_t>(ntsa::PacketPool::k_MTU),
                             static_cast<bsl::size_t>(ntsa::PacketPool::k_MTU),
                             NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntsa::Packet> packet = createPacket(adapter, packetPool);

    ntsa::PacketEncoderContext packetEncoderContext;
    ntsa::PacketEncoderOptions packetEncoderOptions;

    bdlbb::BlobBuffer packetBuffer;
    packetPool->createIncomingBlobBuffer(&packetBuffer);
    NTSCFG_TEST_EQ(packetBuffer.size(),
                   static_cast<bsl::size_t>(ntsa::PacketPool::k_MTU));

    error = packet->encode(&packetEncoderContext,
                           &packetBuffer,
                           packetEncoderOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_DEBUG << "Packet = " << packet << BALL_LOG_END;

    BALL_LOG_DEBUG << "Packet encoded length " << packetBuffer.size()
                   << BALL_LOG_END;

    BALL_LOG_DEBUG << "Packet encoded data:\n"
                   << bdlb::PrintStringHexDumper(packetBuffer.data(),
                                                 packetBuffer.size())
                   << BALL_LOG_END;

#if 0
    {
        ntsu::PacketFilter::Program program;
        ntsu::PacketUtil::acceptAll(&program);

        const bool result = PacketUtilTest::execute(program, packetBuffer);
        BALL_LOG_INFO << "acceptAll = " << result << BALL_LOG_END;
    }

    {
        ntsu::PacketFilter::Program program;
        ntsu::PacketUtil::rejectAll(&program);

        const bool result = PacketUtilTest::execute(program, packetBuffer);
        BALL_LOG_INFO << "rejectAll = " << result << BALL_LOG_END;
    }
#endif

    {
        ntsa::PacketFilter packetFilter;
        packetFilter.addPacketType(ntsa::PacketType::e_IPV4);
        packetFilter.addPacketType(ntsa::PacketType::e_UDP);

        packetFilter.addDestinationEthernetAddress(ntsa::EthernetAddress(adapter.ethernetAddress()));
        packetFilter.addDestinationEthernetAddress(ntsa::EthernetAddress::broadcast());

        ntsu::PacketFilter::Program program;
        error = ntsu::PacketUtil::compile(&program,
                                          ntsa::DeviceType::e_ETHERNET,
                                          adapter,
                                          packetFilter);
        NTSCFG_TEST_OK(error);

        const bool result = PacketUtilTest::execute(program, packetBuffer);
        BALL_LOG_INFO << "filter = " << result << BALL_LOG_END;
    }
}

#else

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyFilterLoopback)
{
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyFilterDefault)
{
}

#endif

}  // close namespace ntsu
}  // close namespace BloombergLP
