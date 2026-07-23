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
BSLS_IDENT_RCSID(ntsa_igmptype_t_cpp, "$Id$ $CSID$")

#include <ntsa_igmptype.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::IgmpType'.
class IgmpTypeTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::IgmpTypeTest::verifyFromInt)
{
    typedef ntsa::IgmpType T;

    // clang-format off
    struct TestData {
        int                    d_result;
        int                    d_input;
        ntsa::IgmpType::Value  d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0, 255, T::e_UNDEFINED   },
        {  0,  17, T::e_QUERY       },
        {  0,  18, T::e_REPORT_V1   },
        {  0,  22, T::e_REPORT_V2   },
        {  0,  34, T::e_REPORT_V3   },
        {  0,  23, T::e_LEAVE       },
        { -1,  -1, T::e_UNDEFINED   }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT =
        sizeof(k_TEST_DATA) / sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::IgmpType::Value value;
        int rc = ntsa::IgmpType::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::IgmpTypeTest::verifyFromString)
{
    typedef ntsa::IgmpType T;

    // clang-format off
    struct TestData {
        int                    d_result;
        const char*            d_input;
        ntsa::IgmpType::Value  d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0, "UNDEFINED", T::e_UNDEFINED },
        {  0, "QUERY",     T::e_QUERY     },
        {  0, "REPORT_V1", T::e_REPORT_V1 },
        {  0, "REPORT_V2", T::e_REPORT_V2 },
        {  0, "REPORT_V3", T::e_REPORT_V3 },
        {  0, "LEAVE",     T::e_LEAVE     },
        { -1, "!",         T::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT =
        sizeof(k_TEST_DATA) / sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::IgmpType::Value value;
        int rc = ntsa::IgmpType::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::IgmpTypeTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        { "UNDEFINED", 255 },
        { "QUERY",      17 },
        { "REPORT_V1",  18 },
        { "REPORT_V2",  22 },
        { "REPORT_V3",  34 },
        { "LEAVE",      23 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT =
        sizeof(k_TEST_DATA) / sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::IgmpType::Value input =
            static_cast<ntsa::IgmpType::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream&      result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
