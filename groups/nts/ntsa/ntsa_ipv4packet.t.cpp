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
// Initially, the raw input used to test correct round-trip decoding and
// encoding was gathered by raw socket reception of standard socket
// transmissions. However, the captured transport-layer checksums equal exactly
// the one's complement sum of the pseudo-header alone (0xFE1B for the
// zero-length UDP test, 0xFE28 for the UDP payload test, and 0xFE30 for the
// TCP SYN test).
//
// All of these packets were captured on Linux loopback with checksum
// offloading enabled: the kernel writes only the pseudo-header partial sum
// into the checksum field before handing the packet to the NIC for completion.
// The capture happened before the NIC finished the checksum. Verifying the
// checksums in the static captured data therefore failed because the data only
// represented the partial pseudo-header contribution.
//
// The fix is to replace the offload-partial checksums with the correct
// RFC-compliant values computed over the full pseudo-header, transport header,
// and payload.
class Ipv4PacketTest
{
    BALL_LOG_SET_CLASS_CATEGORY("NTSA.IPV4PACKET.TEST");

  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test decoding ICMP/IPv4.
    static void verifySerializationIcmpPong();

    // Test decoding IGMP/IPv4.
    static void verifySerializationIgmp();

    // Test decoding UDP/IPv4.
    static void verifySerializationUdp();

    // Test decoding TCP/IPv4.
    static void verifySerializationTcp();

    // Test usage example.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyTypeTraits)
{
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifySerializationIcmpPong)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[] = {
        0x45, 0x00, 0x00, 0x54, 0xaa, 0xfb, 0x40, 0x00, 0xfc, 0x01, 0xfa,
        0x30, 0x8b, 0x85, 0xe9, 0x02, 0x8b, 0x85, 0xd9, 0x6e, 0x00, 0x00,
        0x45, 0xda, 0x1e, 0x60, 0x00, 0x00, 0x33, 0x5e, 0x3a, 0xb8, 0x00,
        0x00, 0x42, 0xac, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
        0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
        0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
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
    {
        ntsa::PacketDecoder decoder(&incomingBlobBuffer);

        error = incomingIpv4Packet.decode(&decoder); // incomingBlobBuffer, 0);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Incoming packet = " << incomingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 84);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 43771);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 252);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 1);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 64048);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address("139.133.233.2"));
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address("139.133.217.110"));

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isIcmp());

    const ntsa::IcmpPacket& incomingIcmpPacket =
        incomingIpv4Packet.payload().icmp();

    NTSCFG_TEST_EQ(incomingIcmpPacket.header().type(), 0);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().code(), 0);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().checksum(), 17882);

    const ntsa::IcmpPong& pong = incomingIcmpPacket.payload().echoReply();

    NTSCFG_TEST_EQ(pong.identifier(), 7776);
    NTSCFG_TEST_EQ(pong.sequenceNumber().value(), 0);

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        error = incomingIpv4Packet.encode(&encoder);
        NTSCFG_TEST_OK(error);

        error = encoder.flush();
        NTSCFG_TEST_OK(error);
    }

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
    {
        ntsa::PacketDecoder decoder(&outgoingBlobBuffer);

        error = outgoingIpv4Packet.decode(&decoder);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing packet = " << outgoingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifySerializationIgmp)
{
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifySerializationUdp)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[41] = {
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
    {
        ntsa::PacketDecoder decoder(&incomingBlobBuffer);

        error = incomingIpv4Packet.decode(&decoder);
        NTSCFG_TEST_OK(error);
    }

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
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        error = incomingIpv4Packet.encode(&encoder);
        NTSCFG_TEST_OK(error);

        error = encoder.flush();
        NTSCFG_TEST_OK(error);
    }

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
    {
        ntsa::PacketDecoder decoder(&outgoingBlobBuffer);

        error = outgoingIpv4Packet.decode(&decoder);
        NTSCFG_TEST_OK(error);
    }

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

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifySerializationTcp)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[60] = {
        0x45, 0x00, 0x00, 0x3C, 0xFB, 0xED, 0x40, 0x00, 0x40, 0x06, 0x40,
        0xCC, 0x7F, 0x00, 0x00, 0x01, 0x7F, 0x00, 0x00, 0x01, 0xBB, 0x63,
        0xDB, 0x52, 0x81, 0x07, 0xEC, 0x9B, 0x00, 0x00, 0x00, 0x00, 0xA0,
        0x02, 0xFF, 0xFF, 0xCF, 0xE9, 0x00, 0x00, 0x02, 0x04, 0xFF, 0xD7,
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
    {
        ntsa::PacketDecoder decoder(&incomingBlobBuffer);

        error = incomingIpv4Packet.decode(&decoder); // incomingBlobBuffer, 0);
        NTSCFG_TEST_OK(error);
    }

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
    NTSCFG_TEST_EQ(incomingTcpPacket.header().checksum(), 53225);
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
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        error = incomingIpv4Packet.encode(&encoder);
        NTSCFG_TEST_OK(error);

        error = encoder.flush();
        NTSCFG_TEST_OK(error);
    }

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
    {
        ntsa::PacketDecoder decoder(&outgoingBlobBuffer);

        error = outgoingIpv4Packet.decode(&decoder);
        NTSCFG_TEST_OK(error);
    }

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
