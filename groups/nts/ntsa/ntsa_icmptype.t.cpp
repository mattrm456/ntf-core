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
BSLS_IDENT_RCSID(ntsa_icmptype_t_cpp, "$Id$ $CSID$")

#include <ntsa_icmptype.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::IcmpType'.
class IcmpTypeTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::IcmpTypeTest::verifyFromInt)
{
    typedef ntsa::IcmpType T;

    // clang-format off
    struct TestData {
        int                    d_result;
        int                    d_input;
        ntsa::IcmpType::Value  d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0, 255, T::e_UNDEFINED       },
        {  0,   8, T::e_ECHO_REQUEST    },
        {  0,   0, T::e_ECHO_RESPONSE   },
        {  0,   9, T::e_ROUTER_RESPONSE },
        {  0,  10, T::e_ROUTER_REQUEST  },
        {  0,   5, T::e_REDIRECT        },
        {  0,   3, T::e_UNREACHABLE     },
        {  0,  11, T::e_TIMEOUT         },
        {  0,  12, T::e_PROBLEM         },
        { -1,  -1, T::e_UNDEFINED       }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT =
        sizeof(k_TEST_DATA) / sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::IcmpType::Value value;
        int rc = ntsa::IcmpType::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::IcmpTypeTest::verifyFromString)
{
    typedef ntsa::IcmpType T;

    // clang-format off
    struct TestData {
        int                    d_result;
        const char*            d_input;
        ntsa::IcmpType::Value  d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0, "UNDEFINED",       T::e_UNDEFINED       },
        {  0, "ECHO_REQUEST",    T::e_ECHO_REQUEST    },
        {  0, "ECHO_RESPONSE",   T::e_ECHO_RESPONSE   },
        {  0, "ROUTER_REQUEST",  T::e_ROUTER_REQUEST  },
        {  0, "ROUTER_RESPONSE", T::e_ROUTER_RESPONSE },
        {  0, "REDIRECT",        T::e_REDIRECT        },
        {  0, "UNREACHABLE",     T::e_UNREACHABLE     },
        {  0, "TIMEOUT",         T::e_TIMEOUT         },
        {  0, "PROBLEM",         T::e_PROBLEM         },
        { -1, "!",               T::e_UNDEFINED       }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT =
        sizeof(k_TEST_DATA) / sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::IcmpType::Value value;
        int rc = ntsa::IcmpType::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::IcmpTypeTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        { "UNDEFINED",       255 },
        { "ECHO_REQUEST",      8 },
        { "ECHO_RESPONSE",     0 },
        { "ROUTER_REQUEST",   10 },
        { "ROUTER_RESPONSE",   9 },
        { "REDIRECT",          5 },
        { "UNREACHABLE",       3 },
        { "TIMEOUT",          11 },
        { "PROBLEM",          12 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT =
        sizeof(k_TEST_DATA) / sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::IcmpType::Value input =
            static_cast<ntsa::IcmpType::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream&      result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
