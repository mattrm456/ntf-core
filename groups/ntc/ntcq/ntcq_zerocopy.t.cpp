// Copyright 2023 Bloomberg Finance L.P.
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

#include <ntcq_zerocopy.h>
#include <ntcs_datapool.h>

#include <ntccfg_test.h>

#include <bslma_allocator.h>

using namespace BloombergLP;

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    // Case 1: (invalid: we can't complete that which we haven't started)
    //
    // WQ:        ----- 
    // ZC:  -----
    //
    // Case 2: (invalid: we can't complete that which we haven't started)
    //
    // WQ:    XXX--
    // ZC:  --XXX
    //
    // Case 3:
    //
    // WQ:  XX--- 
    // ZC:  XX 
    //
    // Case 4:
    //
    // WQ:  ----- 
    // ZC:  ----- 
    //
    // Case 5:
    //
    // WQ:  ---XX 
    // ZC:     XX 
    //
    // Case 6: (invalid: we can't complete that which we haven't started)
    //
    // WQ:  --XXX 
    // ZC:    XXX-- 
    //
    // Case 7: (invalid: we can't complete that which we haven't started)
    //
    // WQ:  ----- 
    // ZC:        ------

    // Case 8: split
    //
    // WQ:  ---------- 
    // ZC:    ------

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    ntccfg::TestAllocator ta;
    {
        // clang-format off
        struct Data {
            bsl::size_t           d_line;
            ntcq::ZeroCopyCounter d_lhsMin;
            ntcq::ZeroCopyCounter d_lhsMax;
            ntcq::ZeroCopyCounter d_rhsMin;
            ntcq::ZeroCopyCounter d_rhsMax;
            ntcq::ZeroCopyCounter d_intersectionMin;
            ntcq::ZeroCopyCounter d_intersectionMax;
            bsl::size_t           d_intersectionSize;
        } DATA[] = {
            { __LINE__, 0, 1,    0, 1,    0, 1, 1 },

            { __LINE__, 3, 6,    0, 3,    0, 0,    0 }, // Case 1

            { __LINE__, 3, 6,    0, 4,    3, 4,    1 }, // Case 2, size 1
            { __LINE__, 3, 6,    0, 5,    3, 5,    2 }, // Case 2, size 2
            { __LINE__, 3, 6,    0, 6,    3, 6,    3 }, // Case 2, size 3

            { __LINE__, 3, 6,    3, 4,    3, 4,    1 }, // Case 3, size 1
            { __LINE__, 3, 6,    3, 5,    3, 5,    2 }, // Case 3, size 2
            { __LINE__, 3, 6,    3, 6,    3, 6,    3 }, // Case 3, size 3

            { __LINE__, 3, 6,    3, 6,    3, 6,    3 }, // Case 4, size 3

            { __LINE__, 3, 6,    3, 6,    3, 6,    3 }, // Case 5, size 3
            { __LINE__, 3, 6,    4, 6,    4, 6,    2 }, // Case 5, size 2
            { __LINE__, 3, 6,    5, 6,    5, 6,    1 }, // Case 5, size 1
            
            { __LINE__, 3, 6,    3, 6,    3, 6,    3 }, // Case 6, size 3
            { __LINE__, 3, 6,    4, 6,    4, 6,    2 }, // Case 6, size 2
            { __LINE__, 3, 6,    5, 8,    5, 6,    1 }, // Case 6, size 1
            
            { __LINE__, 3, 6,    6, 9,    0, 0,    0 }, // Case 7

            { __LINE__, 0, 0,    0, 0,    0, 0, 0 }
        };
        // clang-format on

        enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

        for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
            const Data& data = DATA[i];

            ntcq::ZeroCopyRange lhs(data.d_lhsMin, data.d_lhsMax);
            ntcq::ZeroCopyRange rhs(data.d_rhsMin, data.d_rhsMax);

            ntcq::ZeroCopyRange expectedIntersection(
                data.d_intersectionMin, 
                data.d_intersectionMax);

            ntcq::ZeroCopyRange intersection = 
                ntcq::ZeroCopyRange::intersect(lhs, rhs);

            NTCI_LOG_STREAM_DEBUG 
                << "Testing line " << data.d_line
                << "\nL: " << lhs
                << "\nR: " << rhs
                << "\nE: " << expectedIntersection
                << "\nF: " << intersection
                << NTCI_LOG_STREAM_END;

            if (expectedIntersection.empty()) {
                NTCCFG_TEST_TRUE(intersection.empty());
            }
            else {
                NTCCFG_TEST_EQ(intersection, expectedIntersection);
            }
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
}
NTCCFG_TEST_DRIVER_END;
