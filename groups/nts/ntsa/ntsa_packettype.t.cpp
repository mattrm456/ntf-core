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
BSLS_IDENT_RCSID(ntsa_packettype_t_cpp, "$Id$ $CSID$")

#include <ntsa_packettype.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::PacketType'.
class PacketTypeTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::PacketTypeTest::verifyFromInt)
{
    typedef ntsa::PacketType T;

    // clang-format off
    struct TestData {
        int                      d_result;
        int                      d_input;
        ntsa::PacketType::Value  d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,   0, T::e_UNDEFINED },
        {  0,   1, T::e_ETHERNET  },
        {  0,   2, T::e_ARP       },
        {  0,   3, T::e_RARP      },
        {  0,   4, T::e_IPV4      },
        {  0,   5, T::e_IPV6      },
        {  0,   6, T::e_ICMP      },
        {  0,   7, T::e_IGMP      },
        {  0,   8, T::e_TCP       },
        {  0,   9, T::e_UDP       },
        { -1,  -1, T::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::PacketType::Value value;
        int rc = ntsa::PacketType::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::PacketTypeTest::verifyFromString)
{
    typedef ntsa::PacketType T;

    // clang-format off
    struct TestData {
        int                    d_result;
        const char*            d_input;
        ntsa::PacketType::Value  d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0, "UNDEFINED",  T::e_UNDEFINED },
        {  0, "ETHERNET",   T::e_ETHERNET  },
        {  0, "ARP",        T::e_ARP       },
        {  0, "RARP",       T::e_RARP      },
        {  0, "IPV4",       T::e_IPV4      },
        {  0, "IPV6",       T::e_IPV6      },
        {  0, "ICMP",       T::e_ICMP      },
        {  0, "IGMP",       T::e_IGMP      },
        {  0, "TCP",        T::e_TCP       },
        {  0, "UDP",        T::e_UDP       },
        { -1, "!",          T::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::PacketType::Value value;
        int rc = ntsa::PacketType::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::PacketTypeTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        { "UNDEFINED", 0 },
        { "ETHERNET",  1 },
        { "ARP",       2 },
        { "RARP",      3 },
        { "IPV4",      4 },
        { "IPV6",      5 },
        { "ICMP",      6 },
        { "IGMP",      7 },
        { "TCP",       8 },
        { "UDP",       9 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::PacketType::Value input =
            static_cast<ntsa::PacketType::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
