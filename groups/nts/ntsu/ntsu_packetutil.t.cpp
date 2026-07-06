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

#if defined(BSLS_PLATFORM_OS_DARWIN)
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <netinet/in.h>
#include <sys/socket.h>
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

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsu {

// Provide tests for 'ntsu::PacketUtil'.
class PacketUtilTest
{
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.PACKETUTIL.TEST");

    // Discover the loopback device and load its adapter into the specified
    // 'result'. Return true if such a loopback device is found, and false
    // otherwise.
    static bool discoverLoopback(ntsa::Adapter* result);

    // Discover the default device and load its adapter into the specified
    // 'result'. Return true if such a default device is found, and false
    // otherwise.
    static bool discoverDefault(ntsa::Adapter* result);

    // Return a packet factory.
    static bsl::shared_ptr<ntsa::PacketFactory> createPacketFactory();

    // Return a packet created through the specified 'packetFactory' from the
    // specified 'adapter' to that same 'adapter'.
    static bsl::shared_ptr<ntsa::Packet> createPacket(
        const ntsa::Adapter&                        adapter,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory);

    // Load into the specified 'result' the encoding of the specified 'packet'
    // created through the specified 'packetFactory' suitable for filtering
    // through a device of the specified 'deviceType'.
    static void encodePacket(
        bdlbb::BlobBuffer*                          result,
        const bsl::shared_ptr<ntsa::Packet>&        packet,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
        ntsa::DeviceType::Value                     deviceType);

    // Execute the specified packet filter 'program' on the specified
    // 'packetData'. Return true if the packet is allowed, and return false if
    // the packet is rejected.
    static bool execute(const ntsu::PacketFilter::Program& program,
                        const bdlbb::BlobBuffer&           packetData);

    // Verify the specified 'packetFilter' for the specified 'adapter' of the
    // specified 'deviceType' returns the specified expected 'result' when run
    // on the specified 'packet' created through the specified 'packetFactory'.
    static void verifyFilter(
        const bsl::shared_ptr<ntsa::Packet>&        packet,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
        const ntsa::PacketFilter&                   packetFilter,
        const ntsa::Adapter&                        adapter,
        ntsa::DeviceType::Value                     deviceType,
        bool                                        result);

  public:
    // Verify constants match the constants defined by the operating system
    // and/or thirdparty libraries.
    static void verifyConstants();

    // Verify a program that accepts all Ethernet packets.
    static void verifyEthernetAcceptAll();

    // Verify a program that rejects all Ethernet packets.
    static void verifyEthernetRejectAll();

    // Verify a program that conditionally accepts or rejects TCP/IPv4 Ethernet
    // packets.
    static void verifyEthernetIpv4Tcp();

    // Verify a program that conditionally accepts or rejects UDP/IPv4 Ethernet
    // packets.
    static void verifyEthernetIpv4Udp();

    // Verify a program that accepts all loopback packets.
    static void verifyLoopbackAcceptAll();

    // Verify a program that rejects all loopback packets.
    static void verifyLoopbackRejectAll();
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

bsl::shared_ptr<ntsa::PacketFactory> PacketUtilTest::createPacketFactory()
{
    bsl::shared_ptr<ntsa::PacketPool> packetPool;
    packetPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                             static_cast<bsl::size_t>(ntsa::PacketPool::k_MTU),
                             static_cast<bsl::size_t>(ntsa::PacketPool::k_MTU),
                             NTSCFG_TEST_ALLOCATOR);

    return packetPool;
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

void PacketUtilTest::encodePacket(
    bdlbb::BlobBuffer*                          result,
    const bsl::shared_ptr<ntsa::Packet>&        packet,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    ntsa::DeviceType::Value                     deviceType)
{
    ntsa::Error error;

    result->reset();

    NTSCFG_TEST_TRUE(packet);
    NTSCFG_TEST_TRUE(packet->isEthernet());

    ntsa::PacketEncoderContext packetEncoderContext;
    ntsa::PacketEncoderOptions packetEncoderOptions;

    bdlbb::BlobBuffer packetBuffer;
    packetFactory->createIncomingBlobBuffer(&packetBuffer);
    NTSCFG_TEST_EQ(packetBuffer.size(),
                   static_cast<bsl::size_t>(ntsa::PacketPool::k_MTU));

    if (deviceType == ntsa::DeviceType::e_ETHERNET) {
        error = packet->encode(&packetEncoderContext,
                               &packetBuffer,
                               packetEncoderOptions);
        NTSCFG_TEST_OK(error);
    }
    else if (deviceType == ntsa::DeviceType::e_LOCAL ||
             deviceType == ntsa::DeviceType::e_LOOPBACK)
    {
        bsl::uint32_t protocol = 0;

        if (packet->ethernet().payload().isIpv4()) {
            protocol = AF_INET;
        }
        else if (packet->ethernet().payload().isIpv6()) {
            protocol = AF_INET6;
        }
        else {
            NTSCFG_TEST_TRUE(false);
        }

        if (deviceType == ntsa::DeviceType::e_LOOPBACK) {
            protocol = BSLS_BYTEORDER_HOST_TO_BE(protocol);
        }

        ntsa::PacketEncoder packetEncoder(&packetBuffer);

        error = packetEncoder.encodeRaw(&protocol, sizeof protocol);
        NTSCFG_TEST_OK(error);

        if (packet->ethernet().payload().isIpv4()) {
            const ntsa::Ipv4Packet& ipv4 = packet->ethernet().payload().ipv4();

            error = ipv4.encode(&packetEncoderContext,
                                &packetEncoder,
                                packetEncoderOptions);
            NTSCFG_TEST_OK(error);
        }
        else if (packet->ethernet().payload().isIpv6()) {
            const ntsa::Ipv6Packet& ipv6 = packet->ethernet().payload().ipv6();

            error = ipv6.encode(&packetEncoderContext,
                                &packetEncoder,
                                packetEncoderOptions);
            NTSCFG_TEST_OK(error);
        }
        else {
            NTSCFG_TEST_TRUE(false);
        }

        error = packetEncoder.flush();
        NTSCFG_TEST_OK(error);

        packetBuffer.setSize(packetEncoder.position());
    }
    else {
        NTSCFG_TEST_TRUE(false);
    }

    *result = packetBuffer;
}

bool PacketUtilTest::execute(const ntsu::PacketFilter::Program& program,
                             const bdlbb::BlobBuffer&           packetData)
{
#if NTS_BUILD_WITH_PCAP

    const bool ourResult = ntsu::PacketUtil::execute(program, packetData);

    struct bpf_program fp;
    NTSCFG_MEMORY_ZERO(&fp, sizeof fp);

    fp.bf_insns = const_cast<struct bpf_insn*>(
        reinterpret_cast<const struct bpf_insn*>(&program.front()));
    fp.bf_len = static_cast<bsl::uint32_t>(program.size());

    struct pcap_pkthdr md;
    NTSCFG_MEMORY_ZERO(&md, sizeof md);

    md.len    = packetData.size();
    md.caplen = packetData.size();

    const int match = pcap_offline_filter(
        &fp,
        &md,
        reinterpret_cast<const bsl::uint8_t*>(packetData.data()));

    NTSCFG_TEST_ASSERT(match >= 0);

    const bool theirResult = match != 0;

    NTSCFG_TEST_EQ(ourResult, theirResult);

    return ourResult;

#else

    return ntsu::PacketUtil::execute(program, packetData);

#endif
}

void PacketUtilTest::verifyFilter(
    const bsl::shared_ptr<ntsa::Packet>&        packet,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsa::PacketFilter&                   packetFilter,
    const ntsa::Adapter&                        adapter,
    ntsa::DeviceType::Value                     deviceType,
    bool                                        result)
{
    ntsa::Error error;

    bdlbb::BlobBuffer packetBuffer;
    PacketUtilTest::encodePacket(&packetBuffer,
                                 packet,
                                 packetFactory,
                                 deviceType);

    ntsu::PacketFilter::Program program;
    error =
        ntsu::PacketUtil::compile(&program, deviceType, adapter, packetFilter);
    NTSCFG_TEST_OK(error);

    const bool expected = result;
    const bool found    = PacketUtilTest::execute(program, packetBuffer);

    NTSCFG_TEST_EQ(found, expected);
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyConstants)
{
#if defined(BSLS_PLATFORM_OS_DARWIN)

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

#endif
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyEthernetAcceptAll)
{
    ntsa::Error error;

    ntsa::Adapter adapter;
    if (!PacketUtilTest::discoverDefault(&adapter)) {
        return;
    }

    bsl::shared_ptr<ntsa::PacketFactory> packetFactory =
        PacketUtilTest::createPacketFactory();

    bsl::shared_ptr<ntsa::Packet> packet =
        createPacket(adapter, packetFactory);

    bdlbb::BlobBuffer packetBuffer;
    PacketUtilTest::encodePacket(&packetBuffer,
                                 packet,
                                 packetFactory,
                                 ntsa::DeviceType::e_ETHERNET);

    ntsu::PacketFilter::Program program;
    ntsu::PacketUtil::acceptAll(&program);

    const bool accept = PacketUtilTest::execute(program, packetBuffer);
    NTSCFG_TEST_TRUE(accept);
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyEthernetRejectAll)
{
    ntsa::Error error;

    ntsa::Adapter adapter;
    if (!PacketUtilTest::discoverDefault(&adapter)) {
        return;
    }

    bsl::shared_ptr<ntsa::PacketFactory> packetFactory =
        PacketUtilTest::createPacketFactory();

    bsl::shared_ptr<ntsa::Packet> packet =
        createPacket(adapter, packetFactory);

    bdlbb::BlobBuffer packetBuffer;
    PacketUtilTest::encodePacket(&packetBuffer,
                                 packet,
                                 packetFactory,
                                 ntsa::DeviceType::e_ETHERNET);

    ntsu::PacketFilter::Program program;
    ntsu::PacketUtil::rejectAll(&program);

    const bool accept = PacketUtilTest::execute(program, packetBuffer);
    NTSCFG_TEST_FALSE(accept);
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyEthernetIpv4Tcp)
{
    ntsa::Error error;

    ntsa::Adapter adapter;
    if (!PacketUtilTest::discoverDefault(&adapter)) {
        return;
    }

    if (adapter.ipv4Address().isNull()) {
        return;
    }

    const ntsa::EthernetAddress sourceEthernetAddress(
        adapter.ethernetAddress());

    const ntsa::Ipv4Address sourceIpv4Address = adapter.ipv4Address().value();

    const ntsa::Port sourceTcpPort = 32767;

    const ntsa::EthernetAddress destinationEthernetAddress(
        adapter.ethernetAddress());

    const ntsa::Ipv4Address destinationIpv4Address =
        adapter.ipv4Address().value();

    const ntsa::Port destinationTcpPort = 80;

    bsl::shared_ptr<ntsa::PacketFactory> packetFactory =
        PacketUtilTest::createPacketFactory();

    bsl::shared_ptr<ntsa::Packet> packet =
        ntsu::PacketUtil::createTcp(packetFactory,
                                    sourceEthernetAddress,
                                    sourceIpv4Address,
                                    sourceTcpPort,
                                    destinationEthernetAddress,
                                    destinationIpv4Address,
                                    destinationTcpPort);

    bdlbb::BlobBuffer payload;
    packetFactory->createOutgoingBlobBuffer(&payload);

    NTSCFG_MEMORY_COPY(payload.data(), "Hello, world!", 13);
    payload.setSize(13);

    packet->ethernet().payload().ipv4().payload().tcp().setPayload(payload);

    bsl::vector<ntsa::EthernetAddress> destinationEthernetAddressVector;


    {
        ntsa::PacketFilter packetFilter;
        packetFilter.addPacketType(ntsa::PacketType::e_IPV4);
        packetFilter.addPacketType(ntsa::PacketType::e_TCP);

        packetFilter.addDestinationEthernetAddress(destinationEthernetAddress);

        packetFilter.addDestinationEthernetAddress(
            ntsa::EthernetAddress::broadcast());

        PacketUtilTest::verifyFilter(packet,
                                     packetFactory,
                                     packetFilter,
                                     adapter,
                                     ntsa::DeviceType::e_ETHERNET,
                                     true);
    }
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyEthernetIpv4Udp)
{
    ntsa::Error error;

    ntsa::Adapter adapter;
    if (!PacketUtilTest::discoverDefault(&adapter)) {
        return;
    }

    bsl::shared_ptr<ntsa::PacketFactory> packetFactory =
        PacketUtilTest::createPacketFactory();

    bsl::shared_ptr<ntsa::Packet> packet =
        createPacket(adapter, packetFactory);

    bdlbb::BlobBuffer packetBuffer;

    PacketUtilTest::encodePacket(&packetBuffer,
                                 packet,
                                 packetFactory,
                                 ntsa::DeviceType::e_ETHERNET);

    {
        ntsa::PacketFilter packetFilter;
        packetFilter.addPacketType(ntsa::PacketType::e_IPV4);
        packetFilter.addPacketType(ntsa::PacketType::e_UDP);

        packetFilter.addDestinationEthernetAddress(
            ntsa::EthernetAddress(adapter.ethernetAddress()));
        packetFilter.addDestinationEthernetAddress(
            ntsa::EthernetAddress::broadcast());

        ntsu::PacketFilter::Program program;
        error = ntsu::PacketUtil::compile(&program,
                                          ntsa::DeviceType::e_ETHERNET,
                                          adapter,
                                          packetFilter);
        NTSCFG_TEST_OK(error);

        const bool accept = PacketUtilTest::execute(program, packetBuffer);
        NTSCFG_TEST_TRUE(accept);
    }
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyLoopbackAcceptAll)
{
    ntsa::Error error;

    ntsa::Adapter adapter;
    if (!PacketUtilTest::discoverLoopback(&adapter)) {
        return;
    }

    bsl::shared_ptr<ntsa::PacketFactory> packetFactory =
        PacketUtilTest::createPacketFactory();

    bsl::shared_ptr<ntsa::Packet> packet =
        createPacket(adapter, packetFactory);

    bdlbb::BlobBuffer packetBuffer;
    PacketUtilTest::encodePacket(&packetBuffer,
                                 packet,
                                 packetFactory,
                                 ntsa::DeviceType::e_LOCAL);

    ntsu::PacketFilter::Program program;
    ntsu::PacketUtil::acceptAll(&program);

    const bool accept = PacketUtilTest::execute(program, packetBuffer);
    NTSCFG_TEST_TRUE(accept);
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyLoopbackRejectAll)
{
    ntsa::Error error;

    ntsa::Adapter adapter;
    if (!PacketUtilTest::discoverLoopback(&adapter)) {
        return;
    }

    bsl::shared_ptr<ntsa::PacketFactory> packetFactory =
        PacketUtilTest::createPacketFactory();

    bsl::shared_ptr<ntsa::Packet> packet =
        createPacket(adapter, packetFactory);

    bdlbb::BlobBuffer packetBuffer;
    PacketUtilTest::encodePacket(&packetBuffer,
                                 packet,
                                 packetFactory,
                                 ntsa::DeviceType::e_LOCAL);

    ntsu::PacketFilter::Program program;
    ntsu::PacketUtil::rejectAll(&program);

    const bool accept = PacketUtilTest::execute(program, packetBuffer);
    NTSCFG_TEST_FALSE(accept);
}

}  // close namespace ntsu
}  // close namespace BloombergLP
