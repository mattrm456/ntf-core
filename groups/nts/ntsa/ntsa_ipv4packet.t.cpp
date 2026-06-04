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

    // Test encoding and decoding an ICMP type 8 echo request carried in the
    // payload of an IPv4 packet.
    static void verifyIcmpEchoRequest();

    // Test encoding and decoding an ICMP type 0 echo response carried in the
    // payload of an IPv4 packet.
    static void verifyIcmpEchoResponse();

    // Test encoding and decoding an ICMP type 10 router solicitation carried
    // in the payload of an IPv4 packet.
    static void verifyIcmpRouterRequest();

    // Test encoding and decoding an ICMP type 9 router advertisement carried
    // in the payload of an IPv4 packet.
    static void verifyIcmpRouterResponse();

    // Test encoding and decoding an ICMP type 5 redirect message carried in
    // the payload of an IPv4 packet.
    static void verifyIcmpRedirect();

    // Test encoding and decoding an ICMP type 3 destination unreachable
    // message carried in the payload of an IPv4 packet.
    static void verifyIcmpUnreachable();

    // Test encoding and decoding an ICMP type 11 time exceeded message carried
    // in the payload of an IPv4 packet.
    static void verifyIcmpTimeout();

    // Test encoding and decoding an ICMP type 12 parameter problem message
    // carried in the payload of an IPv4 packet.
    static void verifyIcmpProblem();

    // Test decoding IGMP/IPv4.
    static void verifyIgmp();

    // Test decoding UDP/IPv4.
    static void verifyUdp();

    // Test decoding TCP/IPv4.
    static void verifyTcp();

    // Test decoding TCP/IPv4 where the checksums don't match to investigate a
    // bug.
    static void verifyTcpDecodingError();

    // Test usage example.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyTypeTraits)
{
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyIcmpEchoRequest)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[] = {
        0x45, 0x00, 0x00, 0x54, 0xaa, 0xfa, 0x40, 0x00, 0x40, 0x01, 0xb6,
        0x32, 0x8b, 0x85, 0xd9, 0x6e, 0x8b, 0x85, 0xe9, 0x02, 0x08, 0x00,
        0x3d, 0xda, 0x1e, 0x60, 0x00, 0x00, 0x33, 0x5e, 0x3a, 0xb8, 0x00,
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Incoming packet = " << incomingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 84);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 43770);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 1);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 46642);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address("139.133.217.110"));
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address("139.133.233.2"));

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isIcmp());

    const ntsa::IcmpPacket& incomingIcmpPacket =
        incomingIpv4Packet.payload().icmp();

    NTSCFG_TEST_EQ(incomingIcmpPacket.header().type(), 8);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().code(), 0);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().checksum(), 15834);

    const ntsa::IcmpEchoRequest& echoRequest =
        incomingIcmpPacket.payload().echoRequest();

    NTSCFG_TEST_EQ(echoRequest.identifier(), 7776);
    NTSCFG_TEST_EQ(echoRequest.sequenceNumber().value(), 0);

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing packet = " << outgoingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyIcmpEchoResponse)
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
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

    const ntsa::IcmpEchoResponse& echoResponse =
        incomingIcmpPacket.payload().echoResponse();

    NTSCFG_TEST_EQ(echoResponse.identifier(), 7776);
    NTSCFG_TEST_EQ(echoResponse.sequenceNumber().value(), 0);

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing packet = " << outgoingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyIcmpRouterRequest)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[] = {
        0x45, 0x00, 0x00, 0x1c, 0x12, 0x34, 0x40, 0x00, 0x40, 0x01, 0x86,
        0x9e, 0xc0, 0xa8, 0x01, 0x64, 0xe0, 0x00, 0x00, 0x02, 0x0a, 0x00,
        0xf5, 0xff, 0x00, 0x00, 0x00, 0x00
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Incoming packet = " << incomingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 28);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 4660);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 1);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 34462);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address("192.168.1.100"));
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address("224.0.0.2"));

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isIcmp());

    const ntsa::IcmpPacket& incomingIcmpPacket =
        incomingIpv4Packet.payload().icmp();

    NTSCFG_TEST_EQ(incomingIcmpPacket.header().type(), 10);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().code(), 0);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().checksum(), 62975);

    NTSCFG_TEST_TRUE(incomingIcmpPacket.payload().isRouterRequest());

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing packet = " << outgoingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyIcmpRouterResponse)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[] = {
        0x45, 0x00, 0x00, 0x2c, 0x56, 0x78, 0x40, 0x00, 0xff, 0x01, 0x3b,
        0x56, 0x0a, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x01, 0x09, 0x00,
        0xd9, 0x8e, 0x02, 0x02, 0x07, 0x08, 0x0a, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x64
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Incoming packet = " << incomingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 44);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 22136);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 255);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 1);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 15190);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address("10.0.0.1"));
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address("224.0.0.1"));

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isIcmp());

    const ntsa::IcmpPacket& incomingIcmpPacket =
        incomingIpv4Packet.payload().icmp();

    NTSCFG_TEST_EQ(incomingIcmpPacket.header().type(), 9);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().code(), 0);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().checksum(), 55694);

    NTSCFG_TEST_TRUE(incomingIcmpPacket.payload().isRouterResponse());

    const ntsa::IcmpRouterResponse& routerResponse =
        incomingIcmpPacket.payload().routerResponse();

    NTSCFG_TEST_EQ(routerResponse.info().size(), 2);
    NTSCFG_TEST_EQ(routerResponse.info()[0].address(),
                   ntsa::Ipv4Address("10.0.0.1"));
    NTSCFG_TEST_EQ(routerResponse.info()[0].priority(), 0);
    NTSCFG_TEST_EQ(routerResponse.info()[1].address(),
                   ntsa::Ipv4Address("10.0.0.2"));
    NTSCFG_TEST_EQ(routerResponse.info()[1].priority(), 100);
    NTSCFG_TEST_EQ(routerResponse.timeToLive(), 1800);

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing packet = " << outgoingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyIcmpRedirect)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[] = {
        0x45, 0x00, 0x00, 0x38, 0x9a, 0xbc, 0x40, 0x00, 0xff, 0x01, 0x14,
        0xfb, 0x0a, 0x00, 0x00, 0x01, 0xc0, 0xa8, 0x01, 0x64, 0x05, 0x01,
        0x86, 0x45, 0x0a, 0x00, 0x00, 0x02, 0x45, 0x00, 0x00, 0x28, 0xab,
        0xcd, 0x40, 0x00, 0x3f, 0x06, 0x21, 0xe5, 0xc0, 0xa8, 0x01, 0x64,
        0xac, 0x10, 0x00, 0x01, 0x00, 0x50, 0x01, 0xbb, 0x12, 0x34, 0x56,
        0x78
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Incoming packet = " << incomingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 56);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 39612);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 255);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 1);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 5371);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address("10.0.0.1"));
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address("192.168.1.100"));

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isIcmp());

    const ntsa::IcmpPacket& incomingIcmpPacket =
        incomingIpv4Packet.payload().icmp();

    NTSCFG_TEST_EQ(incomingIcmpPacket.header().type(), 5);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().code(), 1);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().checksum(), 34373);

    NTSCFG_TEST_TRUE(incomingIcmpPacket.payload().isRedirect());

    const ntsa::IcmpRedirect& redirect =
        incomingIcmpPacket.payload().redirect();

    NTSCFG_TEST_EQ(redirect.gatewayAddress(), ntsa::Ipv4Address("10.0.0.2"));

    NTSCFG_TEST_EQ(redirect.header().headerLength(), 20);
    NTSCFG_TEST_EQ(redirect.header().packetLength(), 40);
    NTSCFG_TEST_EQ(redirect.header().id(), 43981);
    NTSCFG_TEST_EQ(redirect.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(redirect.header().timeToLive(), 63);
    NTSCFG_TEST_EQ(redirect.header().protocol(), 6);
    NTSCFG_TEST_EQ(redirect.header().checksum(), 8677);
    NTSCFG_TEST_EQ(redirect.header().sourceAddress(),
                   ntsa::Ipv4Address("192.168.1.100"));
    NTSCFG_TEST_EQ(redirect.header().destinationAddress(),
                   ntsa::Ipv4Address("172.16.0.1"));

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing packet = " << outgoingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyIcmpUnreachable)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[] = {
        0x45, 0x00, 0x00, 0x38, 0xbe, 0xef, 0x40, 0x00, 0x40, 0x01, 0xae,
        0xc7, 0x0a, 0x01, 0x01, 0x01, 0xc0, 0xa8, 0x01, 0x64, 0x03, 0x03,
        0x20, 0xb1, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x24, 0xfe,
        0xdc, 0x40, 0x00, 0x3f, 0x11, 0x6f, 0xde, 0xc0, 0xa8, 0x01, 0x64,
        0x0a, 0x01, 0x01, 0x01, 0x30, 0x39, 0x00, 0x35, 0x00, 0x10, 0xab,
        0xcd
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Incoming packet = " << incomingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 56);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 48879);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 1);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 44743);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address("10.1.1.1"));
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address("192.168.1.100"));

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isIcmp());

    const ntsa::IcmpPacket& incomingIcmpPacket =
        incomingIpv4Packet.payload().icmp();

    NTSCFG_TEST_EQ(incomingIcmpPacket.header().type(), 3);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().code(), 3);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().checksum(), 8369);

    NTSCFG_TEST_TRUE(incomingIcmpPacket.payload().isUnreachable());

    const ntsa::IcmpUnreachable& unreachable =
        incomingIcmpPacket.payload().unreachable();

    NTSCFG_TEST_EQ(unreachable.header().headerLength(), 20);
    NTSCFG_TEST_EQ(unreachable.header().packetLength(), 36);
    NTSCFG_TEST_EQ(unreachable.header().id(), 65244);
    NTSCFG_TEST_EQ(unreachable.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(unreachable.header().timeToLive(), 63);
    NTSCFG_TEST_EQ(unreachable.header().protocol(), 17);
    NTSCFG_TEST_EQ(unreachable.header().checksum(), 28638);
    NTSCFG_TEST_EQ(unreachable.header().sourceAddress(),
                   ntsa::Ipv4Address("192.168.1.100"));
    NTSCFG_TEST_EQ(unreachable.header().destinationAddress(),
                   ntsa::Ipv4Address("10.1.1.1"));

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing packet = " << outgoingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyIcmpTimeout)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[] = {
        0x45, 0x00, 0x00, 0x38, 0xde, 0xad, 0x40, 0x00, 0xff, 0x01, 0xd1,
        0x09, 0x0a, 0x00, 0x00, 0x01, 0xc0, 0xa8, 0x01, 0x64, 0x0b, 0x00,
        0x41, 0x30, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x54, 0x12,
        0x34, 0x40, 0x00, 0x01, 0x01, 0x95, 0x59, 0xc0, 0xa8, 0x01, 0x64,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0xab, 0xcd, 0x00, 0x01, 0x00,
        0x01
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Incoming packet = " << incomingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 56);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 57005);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 255);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 1);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 53513);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address("10.0.0.1"));
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address("192.168.1.100"));

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isIcmp());

    const ntsa::IcmpPacket& incomingIcmpPacket =
        incomingIpv4Packet.payload().icmp();

    NTSCFG_TEST_EQ(incomingIcmpPacket.header().type(), 11);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().code(), 0);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().checksum(), 16688);

    NTSCFG_TEST_TRUE(incomingIcmpPacket.payload().isTimeout());

    const ntsa::IcmpTimeout& timeout = incomingIcmpPacket.payload().timeout();

    NTSCFG_TEST_EQ(timeout.header().headerLength(), 20);
    NTSCFG_TEST_EQ(timeout.header().packetLength(), 84);
    NTSCFG_TEST_EQ(timeout.header().id(), 4660);
    NTSCFG_TEST_EQ(timeout.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(timeout.header().timeToLive(), 1);
    NTSCFG_TEST_EQ(timeout.header().protocol(), 1);
    NTSCFG_TEST_EQ(timeout.header().checksum(), 38233);
    NTSCFG_TEST_EQ(timeout.header().sourceAddress(),
                   ntsa::Ipv4Address("192.168.1.100"));
    NTSCFG_TEST_EQ(timeout.header().destinationAddress(),
                   ntsa::Ipv4Address("8.8.8.8"));

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing packet = " << outgoingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyIcmpProblem)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[] = {
        0x45, 0x00, 0x00, 0x38, 0xca, 0xfe, 0x40, 0x00, 0xff, 0x01, 0xe4,
        0xb8, 0x0a, 0x00, 0x00, 0x01, 0xc0, 0xa8, 0x01, 0x64, 0x0c, 0x00,
        0x7f, 0x75, 0x01, 0x00, 0x00, 0x00, 0x45, 0xff, 0x00, 0x28, 0x55,
        0x55, 0x40, 0x00, 0x3e, 0x06, 0x78, 0x2d, 0xc0, 0xa8, 0x01, 0x64,
        0xac, 0x10, 0x00, 0x32, 0x01, 0xbb, 0xd4, 0x31, 0xde, 0xad, 0xbe,
        0xef
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Incoming packet = " << incomingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(incomingIpv4Packet.header().headerLength(), 20);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().packetLength(), 56);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().id(), 51966);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().timeToLive(), 255);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().protocol(), 1);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().checksum(), 58552);
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().sourceAddress(),
                   ntsa::Ipv4Address("10.0.0.1"));
    NTSCFG_TEST_EQ(incomingIpv4Packet.header().destinationAddress(),
                   ntsa::Ipv4Address("192.168.1.100"));

    NTSCFG_TEST_TRUE(incomingIpv4Packet.payload().isIcmp());

    const ntsa::IcmpPacket& incomingIcmpPacket =
        incomingIpv4Packet.payload().icmp();

    NTSCFG_TEST_EQ(incomingIcmpPacket.header().type(), 12);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().code(), 0);
    NTSCFG_TEST_EQ(incomingIcmpPacket.header().checksum(), 32629);

    NTSCFG_TEST_TRUE(incomingIcmpPacket.payload().isProblem());

    const ntsa::IcmpProblem& problem = incomingIcmpPacket.payload().problem();

    NTSCFG_TEST_EQ(problem.pointer(), 1);

    NTSCFG_TEST_EQ(problem.header().headerLength(), 20);
    NTSCFG_TEST_EQ(problem.header().packetLength(), 40);
    NTSCFG_TEST_EQ(problem.header().id(), 21845);
    NTSCFG_TEST_EQ(problem.header().fragmentOffset(), 64);
    NTSCFG_TEST_EQ(problem.header().timeToLive(), 62);
    NTSCFG_TEST_EQ(problem.header().protocol(), 6);
    NTSCFG_TEST_EQ(problem.header().checksum(), 30765);
    NTSCFG_TEST_EQ(problem.header().sourceAddress(),
                   ntsa::Ipv4Address("192.168.1.100"));
    NTSCFG_TEST_EQ(problem.header().destinationAddress(),
                   ntsa::Ipv4Address("172.16.0.50"));

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing packet = " << outgoingIpv4Packet
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingIpv4Packet, incomingIpv4Packet);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyIgmp)
{
    // Add support for Ipv4 options, especially the "router alert" option.

    // Devise test cases for each IGMP message type.
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyUdp)
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
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

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
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

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyTcp)
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
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

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
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


NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyTcpDecodingError)
{
    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t k_DATA[50] = {
        // 0x1E, 0x7E, 0x79, 0x47, 0x95, 0xC7, 0x48, 0x5D,
        // 0x36, 0x9C, 0xBA, 0xE9, 0x08, 0x00,

        0x45, 0x00, 0x00, 0x28, 0x03, 0x40, 0x40, 0x00,
        0xFA, 0x06, 0xED, 0xEE, 0x45, 0xBF, 0x88, 0x2B,
        0xC0, 0xA8, 0x01, 0x0E, 0x76, 0xF7, 0xE3, 0x19,
        0x0F, 0xF0, 0xCF, 0x89, 0xB1, 0x8E, 0x63, 0x80,
        0x50, 0x10, 0xFF, 0xFF, 0xD1, 0x99, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = incomingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
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

    #if 0
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

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        error = incomingIpv4Packet.encode(&encoderContext,
                                          &encoder,
                                          encoderOptions);
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

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        error = outgoingIpv4Packet.decode(&decoderContext,
                                          &decoder,
                                          decoderOptions);
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
    #endif
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4PacketTest::verifyUsage)
{
}

}  // close namespace ntsa
}  // close namespace BloombergLP
