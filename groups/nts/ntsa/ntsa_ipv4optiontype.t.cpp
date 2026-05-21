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
BSLS_IDENT_RCSID(ntsa_ipv4optiontype_t_cpp, "$Id$ $CSID$")

#include <ntsa_ipv4optiontype.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Ipv4OptionType'.
class Ipv4OptionTypeTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::Ipv4OptionTypeTest::verifyFromInt)
{
    typedef ntsa::Ipv4OptionType T;

    // clang-format off
    struct TestData {
        int                         d_result;
        int                         d_input;
        ntsa::Ipv4OptionType::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,   0, T::e_UNDEFINED          },
        {  0,   1, T::e_PADDING            },
        {  0, 148, T::e_ALERT              },
        {  0,  68, T::e_TIMESTAMP          },
        {  0,   7, T::e_RECORD_ROUTE       },
        {  0, 131, T::e_SOURCE_ROUTE_LOOSE },
        {  0, 137, T::e_SOURCE_ROUTE_TIGHT },
        { -1,  -1, T::e_UNDEFINED          }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::Ipv4OptionType::Value value;
        int rc = ntsa::Ipv4OptionType::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4OptionTypeTest::verifyFromString)
{
    typedef ntsa::Ipv4OptionType T;

    // clang-format off
    struct TestData {
        int                         d_result;
        const char*                 d_input;
        ntsa::Ipv4OptionType::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  "UNDEFINED",          T::e_UNDEFINED          },
        {  0,  "PADDING",            T::e_PADDING            },
        {  0,  "ALERT",              T::e_ALERT              },
        {  0,  "TIMESTAMP",          T::e_TIMESTAMP          },
        {  0,  "RECORD_ROUTE",       T::e_RECORD_ROUTE       },
        {  0,  "SOURCE_ROUTE_LOOSE", T::e_SOURCE_ROUTE_LOOSE },
        {  0,  "SOURCE_ROUTE_TIGHT", T::e_SOURCE_ROUTE_TIGHT },
        { -1,  "!",                  T::e_UNDEFINED          }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::Ipv4OptionType::Value value;
        int rc = ntsa::Ipv4OptionType::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4OptionTypeTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        {  "UNDEFINED",            0 },
        {  "PADDING",              1 },
        {  "ALERT",              148 },
        {  "RECORD_ROUTE",         7 },
        {  "TIMESTAMP",           68 },
        {  "SOURCE_ROUTE_LOOSE", 131 },
        {  "SOURCE_ROUTE_TIGHT", 137 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::Ipv4OptionType::Value input =
            static_cast<ntsa::Ipv4OptionType::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
