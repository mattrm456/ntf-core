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
BSLS_IDENT_RCSID(ntsa_ipv4packet_t_cpp, "$Id$ $CSID$")

#include <ntsa_ipv4packet.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Ipv4Packet'.
//
// Notes:
// Initialially, the raw input used to test the correct round-trip decoding
// and encoding was gathered by a raw sockets reception of a standard sockets
// transmission. However, for that initial data, both test packets fail with
// the same pattern. The captured packet's UDP checksum (0xFE1B for ZeroLength,
// 0xFE28 for the payload test) equals exactly the one's complement sum of the
// pseudo-header alone.
//
// These packets were captured on Linux loopback with UDP checksum offloading:
// the kernel writes only the pseudo-header partial sum into the checksum
// field before handing the packet to the NIC for completion. The capture
// happened before the NIC finished the checksum. So verifying the checksums
// in the static captured data failed because that captured data only
// represented the checksum for the IP pseudo header.
//
// The fix is to replace the offload-partial checksums with the correct values.
class Ipv4PacketTest
{
    BALL_LOG_SET_CLASS_CATEGORY("NTSA.IPV4PACKET.TEST");

  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test decoding UDP/IPv4.
    static void verifySerializationUdpIpv4();

    // Test decoding TCP/IPv4.
    static void verifySerializationTcpIpv4();

    // Test usage example.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyTypeTraits)
{
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifySerializationUdpIpv4)
{
    ntsa::Error error;

    // clang-format off
    static const bsl::uint8_t k_DATA[41] = {
        0x45, 0x00, 0x00, 0x29, 0x3F, 0x09, 0x40, 0x00, 0x40, 0x11, 0xFD,
        0xB8, 0x7F, 0x00, 0x00, 0x01, 0x7F, 0x00, 0x00, 0x01, 0xE4, 0x09,
        0xDB, 0x51, 0x00, 0x15, 0x01, 0x1A, 0x48, 0x65, 0x6C, 0x6C, 0x6F,
        0x2C, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x21
    };
    // clang-format on

    const bsl::size_t k_DATA_SIZE = sizeof(k_DATA);

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(k_DATA_SIZE,
                                                     NTSCFG_TEST_ALLOCATOR);

    bdlbb::BlobBuffer incomingBlobBuffer;
    blobBufferFactory.allocate(&incomingBlobBuffer);
    NTSCFG_TEST_EQ(incomingBlobBuffer.size(), k_DATA_SIZE);

    bsl::memcpy(incomingBlobBuffer.data(), k_DATA, k_DATA_SIZE);

    BALL_LOG_DEBUG << "Incoming data:\n"
                   << bdlb::PrintStringHexDumper(incomingBlobBuffer.data(),
                                                 incomingBlobBuffer.size())
                   << BALL_LOG_END;

    ntsa::Ipv4Packet incomingIpv4Packet;
    error = incomingIpv4Packet.decode(incomingBlobBuffer, 0);
    NTSCFG_TEST_OK(error);

    BALL_LOG_DEBUG_BLOCK
    {
        BALL_LOG_OUTPUT_STREAM << "Incoming packet = " << incomingIpv4Packet;

        if (incomingIpv4Packet.payload().udp().payload().size() > 0) {
            BALL_LOG_OUTPUT_STREAM
                << "\n"
                << bdlb::PrintStringHexDumper(
                       incomingIpv4Packet.payload().udp().payload().data(),
                       incomingIpv4Packet.payload().udp().payload().size());
        }
    }

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 41);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 16137);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 17);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 64952);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address::loopback());

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isUdp());

    const ntsa::UdpPacket& incomingUdpPacket =
        incomingIpv4Packet.payload().udp();

    NTSCFG_TEST_EQ(incomingUdpPacket.header().sourcePort(), 58377);
    NTSCFG_TEST_EQ(incomingUdpPacket.header().destinationPort(), 56145);
    NTSCFG_TEST_EQ(incomingUdpPacket.header().packetLength(), 21);
    NTSCFG_TEST_EQ(incomingUdpPacket.header().checksum(), 282);

    bdlbb::BlobBuffer outgoingBlobBuffer;
    blobBufferFactory.allocate(&outgoingBlobBuffer);
    NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

    error = incomingIpv4Packet.encode(&outgoingBlobBuffer, 0);
    NTSCFG_TEST_OK(error);

    BALL_LOG_DEBUG << "Outgoing data:\n"
                   << bdlb::PrintStringHexDumper(outgoingBlobBuffer.data(),
                                                 outgoingBlobBuffer.size())
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), incomingBlobBuffer.size());

    const int compare =
        bsl::memcmp(outgoingBlobBuffer.data(),
                    incomingBlobBuffer.data(),
                    static_cast<bsl::size_t>(outgoingBlobBuffer.size()));
    NTSCFG_TEST_EQ(compare, 0);

    ntsa::Ipv4Packet outgoingIpv4Packet;
    error = outgoingIpv4Packet.decode(outgoingBlobBuffer, 0);
    NTSCFG_TEST_OK(error);

    BALL_LOG_DEBUG_BLOCK
    {
        BALL_LOG_OUTPUT_STREAM << "Outgoing packet = " << outgoingIpv4Packet;

        if (outgoingIpv4Packet.payload().udp().payload().size() > 0) {
            BALL_LOG_OUTPUT_STREAM
                << "\n"
                << bdlb::PrintStringHexDumper(
                       outgoingIpv4Packet.payload().udp().payload().data(),
                       outgoingIpv4Packet.payload().udp().payload().size());
        }
    }

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifySerializationTcpIpv4)
{
    ntsa::Error error;

    // clang-format off
    static const bsl::uint8_t k_DATA[60] = {
        0x45, 0x00, 0x00, 0x3C, 0xFB, 0xED, 0x40, 0x00, 0x40, 0x06, 0x40,
        0xCC, 0x7F, 0x00, 0x00, 0x01, 0x7F, 0x00, 0x00, 0x01, 0xBB, 0x63,
        0xDB, 0x52, 0x81, 0x07, 0xEC, 0x9B, 0x00, 0x00, 0x00, 0x00, 0xA0,
        0x02, 0xFF, 0xFF, 0xFE, 0x30, 0x00, 0x00, 0x02, 0x04, 0xFF, 0xD7,
        0x04, 0x02, 0x08, 0x0A, 0xDD, 0x39, 0x9E, 0x58, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x03, 0x03, 0x0B
    };
    // clang-format on

    const bsl::size_t k_DATA_SIZE = sizeof(k_DATA);

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(k_DATA_SIZE,
                                                     NTSCFG_TEST_ALLOCATOR);

    bdlbb::BlobBuffer incomingBlobBuffer;
    blobBufferFactory.allocate(&incomingBlobBuffer);
    NTSCFG_TEST_EQ(incomingBlobBuffer.size(), k_DATA_SIZE);

    bsl::memcpy(incomingBlobBuffer.data(), k_DATA, k_DATA_SIZE);

    BALL_LOG_DEBUG << "Incoming data:\n"
                   << bdlb::PrintStringHexDumper(incomingBlobBuffer.data(),
                                                 incomingBlobBuffer.size())
                   << BALL_LOG_END;

    ntsa::Ipv4Packet incomingIpv4Packet;
    error = incomingIpv4Packet.decode(incomingBlobBuffer, 0);
    NTSCFG_TEST_OK(error);

    BALL_LOG_DEBUG_BLOCK
    {
        BALL_LOG_OUTPUT_STREAM << "Incoming packet = " << incomingIpv4Packet;

        if (incomingIpv4Packet.payload().tcp().payload().size() > 0) {
            BALL_LOG_OUTPUT_STREAM
                << "\n"
                << bdlb::PrintStringHexDumper(
                       incomingIpv4Packet.payload().tcp().payload().data(),
                       incomingIpv4Packet.payload().tcp().payload().size());
        }
    }

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 60);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 64493);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 6);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 16588);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address::loopback());

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isTcp());

    const ntsa::TcpPacket& incomingTcpPacket =
        incomingIpv4Packet.payload().tcp();

    NTSCFG_TEST_EQ(incomingTcpPacket.header().sourcePort(), 47971);
    NTSCFG_TEST_EQ(incomingTcpPacket.header().destinationPort(), 56146);
    NTSCFG_TEST_EQ(incomingTcpPacket.header().sequenceNumber(), 2164780187);
    NTSCFG_TEST_EQ(incomingTcpPacket.header().acknowledgmentNumber(), 0);
    NTSCFG_TEST_EQ(incomingTcpPacket.header().dataOffset(), 40);
    NTSCFG_TEST_EQ(incomingTcpPacket.header().windowSize(), 65535);
    NTSCFG_TEST_EQ(incomingTcpPacket.header().checksum(), 65072);
    NTSCFG_TEST_EQ(incomingTcpPacket.header().urgentPointer(), 0);

    NTSCFG_TEST_TRUE(
        incomingTcpPacket.header().hasFlag(ntsa::TcpHeader::k_SYN));

    NTSCFG_TEST_FALSE(
        incomingTcpPacket.header().hasFlag(ntsa::TcpHeader::k_ACK));

    NTSCFG_TEST_FALSE(
        incomingTcpPacket.header().hasFlag(ntsa::TcpHeader::k_PSH));

    NTSCFG_TEST_FALSE(
        incomingTcpPacket.header().hasFlag(ntsa::TcpHeader::k_FIN));

    NTSCFG_TEST_FALSE(
        incomingTcpPacket.header().hasFlag(ntsa::TcpHeader::k_RST));

    NTSCFG_TEST_FALSE(
        incomingTcpPacket.header().hasFlag(ntsa::TcpHeader::k_ECE));

    NTSCFG_TEST_FALSE(
        incomingTcpPacket.header().hasFlag(ntsa::TcpHeader::k_CWR));

    NTSCFG_TEST_FALSE(
        incomingTcpPacket.header().hasFlag(ntsa::TcpHeader::k_URG));

    bdlbb::BlobBuffer outgoingBlobBuffer;
    blobBufferFactory.allocate(&outgoingBlobBuffer);
    NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

    error = incomingIpv4Packet.encode(&outgoingBlobBuffer, 0);
    NTSCFG_TEST_OK(error);

    BALL_LOG_DEBUG << "Outgoing data:\n"
                   << bdlb::PrintStringHexDumper(outgoingBlobBuffer.data(),
                                                 outgoingBlobBuffer.size())
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), incomingBlobBuffer.size());

    const int compare =
        bsl::memcmp(outgoingBlobBuffer.data(),
                    incomingBlobBuffer.data(),
                    static_cast<bsl::size_t>(outgoingBlobBuffer.size()));
    NTSCFG_TEST_EQ(compare, 0);

    ntsa::Ipv4Packet outgoingIpv4Packet;
    error = outgoingIpv4Packet.decode(outgoingBlobBuffer, 0);
    NTSCFG_TEST_OK(error);

    BALL_LOG_DEBUG_BLOCK
    {
        BALL_LOG_OUTPUT_STREAM << "Outgoing packet = " << outgoingIpv4Packet;

        if (outgoingIpv4Packet.payload().tcp().payload().size() > 0) {
            BALL_LOG_OUTPUT_STREAM
                << "\n"
                << bdlb::PrintStringHexDumper(
                       outgoingIpv4Packet.payload().tcp().payload().data(),
                       outgoingIpv4Packet.payload().tcp().payload().size());
        }
    }

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyUsage)
{
}

}  // close namespace ntsa
}  // close namespace BloombergLP
