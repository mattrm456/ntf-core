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
BSLS_IDENT_RCSID(ntsa_igmprecordtype_t_cpp, "$Id$ $CSID$")

#include <ntsa_igmprecordtype.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::IgmpRecordType'.
class IgmpRecordTypeTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::IgmpRecordTypeTest::verifyFromInt)
{
    typedef ntsa::IgmpRecordType T;

    // clang-format off
    struct TestData {
        int                          d_result;
        int                          d_input;
        ntsa::IgmpRecordType::Value  d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0, 0, T::e_UNDEFINED              },
        {  0, 1, T::e_MODE_IS_INCLUDE        },
        {  0, 2, T::e_MODE_IS_EXCLUDE        },
        {  0, 3, T::e_CHANGE_TO_INCLUDE_MODE },
        {  0, 4, T::e_CHANGE_TO_EXCLUDE_MODE },
        {  0, 5, T::e_ALLOW_NEW_SOURCES      },
        {  0, 6, T::e_BLOCK_OLD_SOURCES      },
        { -1, 7, T::e_UNDEFINED              },
        { -1, -1, T::e_UNDEFINED             }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::IgmpRecordType::Value value;
        int rc = ntsa::IgmpRecordType::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::IgmpRecordTypeTest::verifyFromString)
{
    typedef ntsa::IgmpRecordType T;

    // clang-format off
    struct TestData {
        int                          d_result;
        const char*                  d_input;
        ntsa::IgmpRecordType::Value  d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0, "UNDEFINED",              T::e_UNDEFINED              },
        {  0, "MODE_IS_INCLUDE",        T::e_MODE_IS_INCLUDE        },
        {  0, "MODE_IS_EXCLUDE",        T::e_MODE_IS_EXCLUDE        },
        {  0, "CHANGE_TO_INCLUDE_MODE", T::e_CHANGE_TO_INCLUDE_MODE },
        {  0, "CHANGE_TO_EXCLUDE_MODE", T::e_CHANGE_TO_EXCLUDE_MODE },
        {  0, "ALLOW_NEW_SOURCES",      T::e_ALLOW_NEW_SOURCES      },
        {  0, "BLOCK_OLD_SOURCES",      T::e_BLOCK_OLD_SOURCES      },
        { -1, "!",                      T::e_UNDEFINED              }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::IgmpRecordType::Value value;
        int rc = ntsa::IgmpRecordType::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::IgmpRecordTypeTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        { "UNDEFINED",              0 },
        { "MODE_IS_INCLUDE",        1 },
        { "MODE_IS_EXCLUDE",        2 },
        { "CHANGE_TO_INCLUDE_MODE", 3 },
        { "CHANGE_TO_EXCLUDE_MODE", 4 },
        { "ALLOW_NEW_SOURCES",      5 },
        { "BLOCK_OLD_SOURCES",      6 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::IgmpRecordType::Value input =
            static_cast<ntsa::IgmpRecordType::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
