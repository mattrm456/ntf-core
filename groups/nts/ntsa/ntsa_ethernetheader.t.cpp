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
BSLS_IDENT_RCSID(ntsa_ethernetheader_t_cpp, "$Id$ $CSID$")

#include <ntsa_ethernetheader.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::EthernetHeader'.
class EthernetHeaderTest
{
    BALL_LOG_SET_CLASS_CATEGORY("NTSA.ETHERNETHEADER.TEST");

  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test encoding.
    static void verifyEncoding();

    // Test usage example.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntsa::EthernetHeaderTest::verifyTypeTraits)
{
    const bool isBitwiseInitializable =
        NTSCFG_TYPE_CHECK_BITWISE_INITIALIZABLE(ntsa::EthernetHeader);

    NTSCFG_TEST_TRUE(isBitwiseInitializable);

    const bool isBitwiseMovable =
        NTSCFG_TYPE_CHECK_BITWISE_MOVABLE(ntsa::EthernetHeader);

    NTSCFG_TEST_TRUE(isBitwiseMovable);

    const bool isBitwiseCopyable =
        NTSCFG_TYPE_CHECK_BITWISE_COPYABLE(ntsa::EthernetHeader);

    NTSCFG_TEST_TRUE(isBitwiseCopyable);
}

NTSCFG_TEST_FUNCTION(ntsa::EthernetHeaderTest::verifyEncoding)
{
    ntsa::Error error;

    // clang-format off
    static const bsl::uint8_t k_DATA[14] = {
        0x08, 0x00, 0x20, 0x86, 0x35, 0x4b, 0x00, 0xe0, 0xf7, 0x26, 0x3f,
        0xe9, 0x08, 0x00
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

    ntsa::EthernetHeader incomingEthernetHeader;
    {
        ntsa::PacketDecoder decoder(&incomingBlobBuffer);

        error = incomingEthernetHeader.decode(&decoder);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Incoming header = " << incomingEthernetHeader
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(incomingEthernetHeader.source(),
                   ntsa::EthernetAddress("00:e0:f7:26:3f:e9"));

    NTSCFG_TEST_EQ(incomingEthernetHeader.destination(),
                   ntsa::EthernetAddress("08:00:20:86:35:4b"));

    NTSCFG_TEST_EQ(incomingEthernetHeader.protocol(),
                   ntsa::EthernetProtocol::e_IPV4);

    bdlbb::BlobBuffer outgoingBlobBuffer;
    {
        blobBufferFactory.allocate(&outgoingBlobBuffer);
        NTSCFG_TEST_EQ(outgoingBlobBuffer.size(), k_DATA_SIZE);

        ntsa::PacketEncoder encoder(&outgoingBlobBuffer);

        error = incomingEthernetHeader.encode(&encoder);
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

    ntsa::EthernetHeader outgoingEthernetHeader;
    {
        ntsa::PacketDecoder decoder(&outgoingBlobBuffer);

        error = outgoingEthernetHeader.decode(&decoder);
        NTSCFG_TEST_OK(error);
    }

    BALL_LOG_DEBUG << "Outgoing header = " << outgoingEthernetHeader
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(outgoingEthernetHeader, incomingEthernetHeader);
}

NTSCFG_TEST_FUNCTION(ntsa::EthernetHeaderTest::verifyUsage)
{
    ntsa::EthernetHeader header;

    header.setSource(ntsa::EthernetAddress("36:2c:a1:55:0c:c0"));
    header.setDestination(ntsa::EthernetAddress("3e:07:f4:b5:bf:48"));
    header.setProtocol(ntsa::EthernetProtocol::e_IPV4);

    NTSCFG_TEST_LOG_DEBUG << "Ethernet = " << header << NTSCFG_TEST_LOG_END;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
