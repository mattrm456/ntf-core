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
BSLS_IDENT_RCSID(ntsa_ethernetpriority_t_cpp, "$Id$ $CSID$")

#include <ntsa_ethernetpriority.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::EthernetPriority'.
class EthernetPriorityTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::EthernetPriorityTest::verifyFromInt)
{
    typedef ntsa::EthernetPriority T;

    // clang-format off
    struct TestData {
        int                           d_result;
        int                           d_input;
        ntsa::EthernetPriority::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,   0, T::e_DEFAULT    },
        {  0,   1, T::e_BACKGROUND },
        {  0,   2, T::e_IMPORTANT  },
        {  0,   3, T::e_CRITICAL   },
        {  0,   4, T::e_VIDEO      },
        {  0,   5, T::e_VOICE      },
        {  0,   6, T::e_CONTROL    },
        {  0,   7, T::e_ROUTING    },
        { -1,  -1, T::e_DEFAULT    }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT =
        sizeof(k_TEST_DATA) / sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::EthernetPriority::Value value;
        int rc = ntsa::EthernetPriority::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::EthernetPriorityTest::verifyFromString)
{
    typedef ntsa::EthernetPriority T;

    // clang-format off
    struct TestData {
        int                           d_result;
        const char*                   d_input;
        ntsa::EthernetPriority::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0, "DEFAULT",    T::e_DEFAULT    },
        {  0, "BACKGROUND", T::e_BACKGROUND },
        {  0, "IMPORTANT",  T::e_IMPORTANT  },
        {  0, "CRITICAL",   T::e_CRITICAL   },
        {  0, "VIDEO",      T::e_VIDEO      },
        {  0, "VOICE",      T::e_VOICE      },
        {  0, "CONTROL",    T::e_CONTROL    },
        {  0, "ROUTING",    T::e_ROUTING    },
        { -1, "!",          T::e_DEFAULT    }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT =
        sizeof(k_TEST_DATA) / sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::EthernetPriority::Value value;
        int rc = ntsa::EthernetPriority::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::EthernetPriorityTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        { "DEFAULT",    0 },
        { "BACKGROUND", 1 },
        { "IMPORTANT",  2 },
        { "CRITICAL",   3 },
        { "VIDEO",      4 },
        { "VOICE",      5 },
        { "CONTROL",    6 },
        { "ROUTING",    7 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT =
        sizeof(k_TEST_DATA) / sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::EthernetPriority::Value input =
            static_cast<ntsa::EthernetPriority::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream&      result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
