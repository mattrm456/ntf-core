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

    // Return a packet factory.
    static bsl::shared_ptr<ntsa::PacketFactory> createPacketFactory();

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

    // Verify the specified 'packetFilter' for the specified 'deviceType'
    // returns the specified expected 'result' when run on the specified
    // 'packet' created through the specified 'packetFactory'.
    static void verifyFilter(
        const bsl::shared_ptr<ntsa::Packet>&        packet,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
        const ntsa::PacketFilter&                   packetFilter,
        ntsa::DeviceType::Value                     deviceType,
        bool                                        result);

    static void verifyFilter(
        const bsl::shared_ptr<ntsa::Packet>&        packet,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
        const ntsu::PacketFilter::Program&          packetFilterProgram,
        ntsa::DeviceType::Value                     deviceType,
        bool                                        result);

  public:
    // Verify constants match the constants defined by the operating system
    // and/or thirdparty libraries.
    static void verifyConstants();

    // Verify a program that conditionally accepts or rejects TCP/IPv4 Ethernet
    // packets.
    static void verifyEthernetIpv4Tcp();

    // Verify a program that conditionally accepts or rejects UDP/IPv4 Ethernet
    // packets.
    static void verifyEthernetIpv4Udp();
};

bsl::shared_ptr<ntsa::PacketFactory> PacketUtilTest::createPacketFactory()
{
    bsl::shared_ptr<ntsa::PacketPool> packetPool;
    packetPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                             static_cast<bsl::size_t>(ntsa::PacketPool::k_MTU),
                             static_cast<bsl::size_t>(ntsa::PacketPool::k_MTU),
                             NTSCFG_TEST_ALLOCATOR);

    return packetPool;
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
    ntsa::DeviceType::Value                     deviceType,
    bool                                        result)
{
    BALL_LOG_INFO << "Filtering packet " << packet << " through filter "
                  << packetFilter << BALL_LOG_END;

    ntsa::Error error;

    bdlbb::BlobBuffer packetBuffer;
    PacketUtilTest::encodePacket(&packetBuffer,
                                 packet,
                                 packetFactory,
                                 deviceType);

    ntsu::PacketFilter::Program program;
    error = ntsu::PacketUtil::compile(&program, deviceType, packetFilter);
    NTSCFG_TEST_OK(error);

    const bool expected = result;
    const bool found    = PacketUtilTest::execute(program, packetBuffer);

    NTSCFG_TEST_EQ(found, expected);
}

void PacketUtilTest::verifyFilter(
    const bsl::shared_ptr<ntsa::Packet>&        packet,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const ntsu::PacketFilter::Program&          packetFilterProgram,
    ntsa::DeviceType::Value                     deviceType,
    bool                                        result)
{
    ntsa::Error error;

    bdlbb::BlobBuffer packetBuffer;
    PacketUtilTest::encodePacket(&packetBuffer,
                                 packet,
                                 packetFactory,
                                 deviceType);

    const bool expected = result;
    const bool found =
        PacketUtilTest::execute(packetFilterProgram, packetBuffer);

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

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyEthernetIpv4Tcp)
{
    ntsa::Error error;

    const ntsa::DeviceType::Value deviceType = ntsa::DeviceType::e_ETHERNET;

    const ntsa::EthernetAddress sourceEthernetAddressA("1a:b2:c3:d4:5e:f6");

    const ntsa::EthernetAddress sourceEthernetAddressB("2b:2f:57:57:11:65");

    const ntsa::EthernetAddress sourceEthernetAddressC("3c:c9:0c:06:8d:0a");

    const ntsa::Ipv4Address sourceIpv4AddressA("192.168.1.100");
    const ntsa::Ipv4Address sourceIpv4AddressB("192.168.1.101");
    const ntsa::Ipv4Address sourceIpv4AddressC("192.168.1.102");

    const ntsa::Port sourceTcpPortA = 40000;
    const ntsa::Port sourceTcpPortB = 40001;
    const ntsa::Port sourceTcpPortC = 40002;

    const ntsa::EthernetAddress destinationEthernetAddressA(
        "4a:2b:3c:4d:5e:6f");

    const ntsa::EthernetAddress destinationEthernetAddressB(
        "5b:88:4f:2b:c5:dd");

    const ntsa::EthernetAddress destinationEthernetAddressC(
        "6c:3e:1a:6f:44:c3");

    const ntsa::Ipv4Address destinationIpv4AddressA("10.0.1.200");
    const ntsa::Ipv4Address destinationIpv4AddressB("10.0.1.201");
    const ntsa::Ipv4Address destinationIpv4AddressC("10.0.1.202");

    const ntsa::Port destinationTcpPortA = 80;
    const ntsa::Port destinationTcpPortB = 81;
    const ntsa::Port destinationTcpPortC = 82;

    bsl::shared_ptr<ntsa::PacketFactory> packetFactory =
        PacketUtilTest::createPacketFactory();

    bsl::shared_ptr<ntsa::Packet> packet =
        ntsu::PacketUtil::createTcp(packetFactory,
                                    sourceEthernetAddressA,
                                    sourceIpv4AddressA,
                                    sourceTcpPortA,
                                    destinationEthernetAddressA,
                                    destinationIpv4AddressA,
                                    destinationTcpPortA);

    bdlbb::BlobBuffer payload;
    packetFactory->createOutgoingBlobBuffer(&payload);

    NTSCFG_MEMORY_COPY(payload.data(), "Hello, world!", 13);
    payload.setSize(13);

    packet->ethernet().payload().ipv4().payload().tcp().setPayload(payload);

    {
        ntsu::PacketFilter::Program program;
        ntsu::PacketUtil::acceptAll(&program);

        verifyFilter(packet, packetFactory, program, deviceType, true);
    }

    {
        ntsu::PacketFilter::Program program;
        ntsu::PacketUtil::rejectAll(&program);

        verifyFilter(packet, packetFactory, program, deviceType, false);
    }

    {
        ntsa::PacketFilter filter;

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Reject: packet type missing IPv4

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV6);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationEthernetAddress(
            ntsa::EthernetAddress::broadcast());

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Reject: packet type missing TCP

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationEthernetAddress(
            ntsa::EthernetAddress::broadcast());

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: source Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressA);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Reject: source Ethernet address does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: destination Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressA);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Reject: destination Ethernet address does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: source IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);
        filter.addSourceIpv4Address(sourceIpv4AddressA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressC);
        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source IPv4 address does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: destination IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);
        filter.addDestinationIpv4Address(destinationIpv4AddressA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationIpv4Address(destinationIpv4AddressC);
        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination IPv4 address does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: source TCP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceTcpPort(sourceTcpPortA);
        filter.addSourceTcpPort(sourceTcpPortB);
        filter.addSourceTcpPort(sourceTcpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source TCP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addSourceTcpPort(sourceTcpPortA);
        filter.addSourceTcpPort(sourceTcpPortB);
        filter.addSourceTcpPort(sourceTcpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source TCP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addSourceTcpPort(sourceTcpPortB);
        filter.addSourceTcpPort(sourceTcpPortC);
        filter.addSourceTcpPort(sourceTcpPortA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source TCP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addSourceTcpPort(sourceTcpPortC);
        filter.addSourceTcpPort(sourceTcpPortA);
        filter.addSourceTcpPort(sourceTcpPortB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source TCP port does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addSourceTcpPort(sourceTcpPortB);
        filter.addSourceTcpPort(sourceTcpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: destination TCP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addDestinationTcpPort(destinationTcpPortA);
        filter.addDestinationTcpPort(destinationTcpPortB);
        filter.addDestinationTcpPort(destinationTcpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination TCP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addDestinationTcpPort(destinationTcpPortA);
        filter.addDestinationTcpPort(destinationTcpPortB);
        filter.addDestinationTcpPort(destinationTcpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination TCP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addDestinationTcpPort(destinationTcpPortB);
        filter.addDestinationTcpPort(destinationTcpPortC);
        filter.addDestinationTcpPort(destinationTcpPortA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination TCP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addDestinationTcpPort(destinationTcpPortC);
        filter.addDestinationTcpPort(destinationTcpPortA);
        filter.addDestinationTcpPort(destinationTcpPortB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination TCP port does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addDestinationTcpPort(destinationTcpPortB);
        filter.addDestinationTcpPort(destinationTcpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }
}

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyEthernetIpv4Udp)
{
    ntsa::Error error;

    const ntsa::DeviceType::Value deviceType = ntsa::DeviceType::e_ETHERNET;

    const ntsa::EthernetAddress sourceEthernetAddressA("1a:b2:c3:d4:5e:f6");

    const ntsa::EthernetAddress sourceEthernetAddressB("2b:2f:57:57:11:65");

    const ntsa::EthernetAddress sourceEthernetAddressC("3c:c9:0c:06:8d:0a");

    const ntsa::Ipv4Address sourceIpv4AddressA("192.168.1.100");
    const ntsa::Ipv4Address sourceIpv4AddressB("192.168.1.101");
    const ntsa::Ipv4Address sourceIpv4AddressC("192.168.1.102");

    const ntsa::Port sourceUdpPortA = 40000;
    const ntsa::Port sourceUdpPortB = 40001;
    const ntsa::Port sourceUdpPortC = 40002;

    const ntsa::EthernetAddress destinationEthernetAddressA(
        "4a:2b:3c:4d:5e:6f");

    const ntsa::EthernetAddress destinationEthernetAddressB(
        "5b:88:4f:2b:c5:dd");

    const ntsa::EthernetAddress destinationEthernetAddressC(
        "6c:3e:1a:6f:44:c3");

    const ntsa::Ipv4Address destinationIpv4AddressA("10.0.1.200");
    const ntsa::Ipv4Address destinationIpv4AddressB("10.0.1.201");
    const ntsa::Ipv4Address destinationIpv4AddressC("10.0.1.202");

    const ntsa::Port destinationUdpPortA = 80;
    const ntsa::Port destinationUdpPortB = 81;
    const ntsa::Port destinationUdpPortC = 82;

    bsl::shared_ptr<ntsa::PacketFactory> packetFactory =
        PacketUtilTest::createPacketFactory();

    bsl::shared_ptr<ntsa::Packet> packet =
        ntsu::PacketUtil::createUdp(packetFactory,
                                    sourceEthernetAddressA,
                                    sourceIpv4AddressA,
                                    sourceUdpPortA,
                                    destinationEthernetAddressA,
                                    destinationIpv4AddressA,
                                    destinationUdpPortA);

    bdlbb::BlobBuffer payload;
    packetFactory->createOutgoingBlobBuffer(&payload);

    NTSCFG_MEMORY_COPY(payload.data(), "Hello, world!", 13);
    payload.setSize(13);

    packet->ethernet().payload().ipv4().payload().udp().setPayload(payload);

    {
        ntsu::PacketFilter::Program program;
        ntsu::PacketUtil::acceptAll(&program);

        verifyFilter(packet, packetFactory, program, deviceType, true);
    }

    {
        ntsu::PacketFilter::Program program;
        ntsu::PacketUtil::rejectAll(&program);

        verifyFilter(packet, packetFactory, program, deviceType, false);
    }

    {
        ntsa::PacketFilter filter;

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Reject: packet type missing IPv4

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV6);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationEthernetAddress(
            ntsa::EthernetAddress::broadcast());

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Reject: packet type missing UDP

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_TCP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationEthernetAddress(
            ntsa::EthernetAddress::broadcast());

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: source Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressA);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Reject: source Ethernet address does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: destination Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressA);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination Ethernet address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Reject: destination Ethernet address does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: source IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);
        filter.addSourceIpv4Address(sourceIpv4AddressA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressC);
        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source IPv4 address does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: destination IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);
        filter.addDestinationIpv4Address(destinationIpv4AddressA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination IPv4 address match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationIpv4Address(destinationIpv4AddressC);
        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination IPv4 address does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: source UDP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceUdpPort(sourceUdpPortA);
        filter.addSourceUdpPort(sourceUdpPortB);
        filter.addSourceUdpPort(sourceUdpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source UDP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addSourceUdpPort(sourceUdpPortA);
        filter.addSourceUdpPort(sourceUdpPortB);
        filter.addSourceUdpPort(sourceUdpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source UDP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addSourceUdpPort(sourceUdpPortB);
        filter.addSourceUdpPort(sourceUdpPortC);
        filter.addSourceUdpPort(sourceUdpPortA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source UDP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addSourceUdpPort(sourceUdpPortC);
        filter.addSourceUdpPort(sourceUdpPortA);
        filter.addSourceUdpPort(sourceUdpPortB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: source UDP port does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addSourceUdpPort(sourceUdpPortB);
        filter.addSourceUdpPort(sourceUdpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }

    // Accept: destination UDP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addDestinationUdpPort(destinationUdpPortA);
        filter.addDestinationUdpPort(destinationUdpPortB);
        filter.addDestinationUdpPort(destinationUdpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination UDP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addDestinationUdpPort(destinationUdpPortA);
        filter.addDestinationUdpPort(destinationUdpPortB);
        filter.addDestinationUdpPort(destinationUdpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination UDP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addDestinationUdpPort(destinationUdpPortB);
        filter.addDestinationUdpPort(destinationUdpPortC);
        filter.addDestinationUdpPort(destinationUdpPortA);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination UDP port match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addDestinationUdpPort(destinationUdpPortC);
        filter.addDestinationUdpPort(destinationUdpPortA);
        filter.addDestinationUdpPort(destinationUdpPortB);

        verifyFilter(packet, packetFactory, filter, deviceType, true);
    }

    // Accept: destination UDP port does not match

    {
        ntsa::PacketFilter filter;
        filter.addPacketType(ntsa::PacketType::e_IPV4);
        filter.addPacketType(ntsa::PacketType::e_UDP);

        filter.addSourceEthernetAddress(sourceEthernetAddressC);
        filter.addSourceEthernetAddress(sourceEthernetAddressB);
        filter.addSourceEthernetAddress(sourceEthernetAddressA);

        filter.addDestinationEthernetAddress(destinationEthernetAddressC);
        filter.addDestinationEthernetAddress(destinationEthernetAddressB);
        filter.addDestinationEthernetAddress(destinationEthernetAddressA);

        filter.addSourceIpv4Address(sourceIpv4AddressA);
        filter.addSourceIpv4Address(sourceIpv4AddressB);
        filter.addSourceIpv4Address(sourceIpv4AddressC);

        filter.addDestinationIpv4Address(destinationIpv4AddressA);
        filter.addDestinationIpv4Address(destinationIpv4AddressB);
        filter.addDestinationIpv4Address(destinationIpv4AddressC);

        filter.addDestinationUdpPort(destinationUdpPortB);
        filter.addDestinationUdpPort(destinationUdpPortC);

        verifyFilter(packet, packetFactory, filter, deviceType, false);
    }
}

}  // close namespace ntsu
}  // close namespace BloombergLP
