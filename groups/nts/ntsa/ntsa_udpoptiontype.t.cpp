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
BSLS_IDENT_RCSID(ntsa_udpoptiontype_t_cpp, "$Id$ $CSID$")

#include <ntsa_udpoptiontype.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::UdpOptionType'.
class UdpOptionTypeTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::UdpOptionTypeTest::verifyFromInt)
{
    typedef ntsa::UdpOptionType T;

    // clang-format off
    struct TestData {
        int                        d_result;
        int                        d_input;
        ntsa::UdpOptionType::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,   0, T::e_UNDEFINED                     },
        {  0,   1, T::e_PADDING                       },
        {  0,   2, T::e_ADDITIONAL_PAYLOAD_CHECKSUM   },
        {  0,   3, T::e_FRAGMENTATION                 },
        {  0,   4, T::e_MAX_DATAGRAM_SIZE             },
        {  0,   5, T::e_MAX_REASSEMBLED_DATAGRAM_SIZE },
        {  0,   6, T::e_ECHO_REQUEST                  },
        {  0,   7, T::e_ECHO_RESPONSE                 },
        {  0,   8, T::e_TIMESTAMP                     },
        {  0, 255, T::e_UNASSIGNED                    },
        { -1,  -1, T::e_UNDEFINED                     }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::UdpOptionType::Value value;
        int rc = ntsa::UdpOptionType::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::UdpOptionTypeTest::verifyFromString)
{
    typedef ntsa::UdpOptionType T;

    // clang-format off
    struct TestData {
        int                        d_result;
        const char*                d_input;
        ntsa::UdpOptionType::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  "UNDEFINED",
           T::e_UNDEFINED },
        {  0,  "PADDING",
           T::e_PADDING },
        {  0,  "ADDITIONAL_PAYLOAD_CHECKSUM",
           T::e_ADDITIONAL_PAYLOAD_CHECKSUM },
        {  0,  "FRAGMENTATION",
           T::e_FRAGMENTATION },
        {  0,  "MAX_DATAGRAM_SIZE",
           T::e_MAX_DATAGRAM_SIZE },
        {  0,  "MAX_REASSEMBLED_DATAGRAM_SIZE",
           T::e_MAX_REASSEMBLED_DATAGRAM_SIZE },
        {  0,  "ECHO_REQUEST",
           T::e_ECHO_REQUEST },
        {  0,  "ECHO_RESPONSE",
           T::e_ECHO_RESPONSE },
        {  0,  "TIMESTAMP",
           T::e_TIMESTAMP },
        {  0,  "UNASSIGNED",
           T::e_UNASSIGNED },
        { -1,  "!",
           T::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::UdpOptionType::Value value;
        int rc = ntsa::UdpOptionType::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::UdpOptionTypeTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        {  "UNDEFINED",                       0 },
        {  "PADDING",                         1 },
        {  "ADDITIONAL_PAYLOAD_CHECKSUM",     2 },
        {  "FRAGMENTATION",                   3 },
        {  "MAX_DATAGRAM_SIZE",               4 },
        {  "MAX_REASSEMBLED_DATAGRAM_SIZE",   5 },
        {  "ECHO_REQUEST",                    6 },
        {  "ECHO_RESPONSE",                   7 },
        {  "TIMESTAMP",                       8 },
        {  "UNASSIGNED",                    255 },
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::UdpOptionType::Value input =
            static_cast<ntsa::UdpOptionType::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
