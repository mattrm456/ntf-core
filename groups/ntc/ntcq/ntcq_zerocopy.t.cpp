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

#include <ntcq_send.h>
#include <ntci_sender.h>
#include <ntci_sendcallback.h>
#include <ntcs_datapool.h>
#include <ntccfg_test.h>
#include <bslma_allocator.h>

using namespace BloombergLP;

NTCCFG_TEST_CASE(1)
{
    // Concern: Test ntcq::ZeroCopyRange::intersection()
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
    // Concern: Test ntcq::ZeroCopyRange::difference()
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    ntccfg::TestAllocator ta;
    {
        // LHS:     ----- 
        // RHS: --------------

        {
            ntcq::ZeroCopyRange lhs(3, 6);
            ntcq::ZeroCopyRange rhs(0, 9);

            ntcq::ZeroCopyRange result;
            ntcq::ZeroCopyRange overflow;

            ntcq::ZeroCopyRange::difference(&result, &overflow, lhs, rhs);

            NTCCFG_TEST_TRUE(result.empty());
            NTCCFG_TEST_TRUE(overflow.empty());
        }

        // LHS: RRR----
        // RHS:    ----

        {
            ntcq::ZeroCopyRange lhs(0, 6);
            ntcq::ZeroCopyRange rhs(3, 6);

            ntcq::ZeroCopyRange result;
            ntcq::ZeroCopyRange overflow;

            ntcq::ZeroCopyRange::difference(&result, &overflow, lhs, rhs);

            NTCCFG_TEST_EQ(result.minCounter(), 0);
            NTCCFG_TEST_EQ(result.maxCounter(), 3);

            NTCCFG_TEST_TRUE(overflow.empty());
        }

        // LHS: ----OOO
        // RHS: ----

        {
            ntcq::ZeroCopyRange lhs(3, 9);
            ntcq::ZeroCopyRange rhs(3, 6);

            ntcq::ZeroCopyRange result;
            ntcq::ZeroCopyRange overflow;

            ntcq::ZeroCopyRange::difference(&result, &overflow, lhs, rhs);

            NTCCFG_TEST_EQ(result.minCounter(), 6);
            NTCCFG_TEST_EQ(result.maxCounter(), 9);

            NTCCFG_TEST_TRUE(overflow.empty());
        }

        // LHS: RRR----OOO
        // RHS:    ----

        {
            ntcq::ZeroCopyRange lhs(0, 9);
            ntcq::ZeroCopyRange rhs(3, 6);

            ntcq::ZeroCopyRange result;
            ntcq::ZeroCopyRange overflow;

            ntcq::ZeroCopyRange::difference(&result, &overflow, lhs, rhs);

            NTCCFG_TEST_EQ(result.minCounter(), 0);
            NTCCFG_TEST_EQ(result.maxCounter(), 3);

            NTCCFG_TEST_EQ(overflow.minCounter(), 6);
            NTCCFG_TEST_EQ(overflow.maxCounter(), 9);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Test 32-bit ntsa::ZeroCopy counter wraparound
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    ntccfg::TestAllocator ta;
    {
        // TODO
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

namespace test {

/// Provide a mechanism to track the transfer state of data that is 
/// zero-copied.
class Transfer {
    ntcq::SendCounter                d_group;
    bsl::size_t                      d_numRequired;
    bsl::size_t                      d_numComplete;
    bsl::shared_ptr<ntci::Sender>    d_sender_sp;
    bsl::shared_ptr<ntsa::Data>      d_data_sp;
    bsl::shared_ptr<ntci::DataPool>  d_dataPool_sp;
    bslma::Allocator                *d_allocator_p;

private:
    Transfer(const Transfer&) BSLS_KEYWORD_DELETED;
    Transfer& operator=(const Transfer&) BSLS_KEYWORD_DELETED;

private:
    /// Process the completion of a zero-copy transmission by the specified
    /// 'sender' according to the specified 'event'.
    void processComplete(const bsl::shared_ptr<ntci::Sender>& sender,
                         const ntca::SendEvent&               event);

public:
    /// Create a new transfer for the specified 'group' requiring the specified
    /// 'numOperations' to transfer all the data provided by the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Transfer(const bsl::shared_ptr<ntci::Sender>&   sender,
             ntcq::SendCounter                      group, 
             bsl::size_t                            numOperations,
             const bsl::shared_ptr<ntci::DataPool>& dataPool,
             bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~Transfer();

    /// Initiate this transfer to the specified 'zeroCopyQueue'. 
    void initiate(ntcq::ZeroCopyQueue* zeroCopyQueue);

    /// Return the identifier of the transfer.
    ntcq::SendCounter group() const;

    /// Return true if all required operations for this transfer have been
    /// completed, otherwise return false. 
    bool complete() const;

    /// Return a new transfer for the specified 'group' requiring the specified
    /// 'numOperations' to transfer all the data provided by the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static bsl::shared_ptr<Transfer> create(
        const bsl::shared_ptr<ntci::Sender>&   sender,
        ntcq::SendCounter                      group, 
        bsl::size_t                            numOperations,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator*                      basicAllocator = 0);
};

void Transfer::processComplete(const bsl::shared_ptr<ntci::Sender>& sender,
                               const ntca::SendEvent&               event)
{
    NTCCFG_TEST_EQ(sender, d_sender_sp);
    NTCCFG_TEST_EQ(event.type(), ntca::SendEventType::e_COMPLETE);
    NTCCFG_TEST_LT(d_numComplete, d_numRequired);

    ++d_numComplete;
}

Transfer::Transfer(const bsl::shared_ptr<ntci::Sender>&   sender,
                   ntcq::SendCounter                      group, 
                   bsl::size_t                            numOperations,
                   const bsl::shared_ptr<ntci::DataPool>& dataPool,
                   bslma::Allocator*                      basicAllocator)
: d_group(group)
, d_numRequired(numOperations)
, d_numComplete(0)
, d_sender_sp(sender)
, d_data_sp(dataPool->createOutgoingData())
, d_dataPool_sp(dataPool)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Transfer::~Transfer()
{
}

void Transfer::initiate(ntcq::ZeroCopyQueue* zeroCopyQueue)
{
    NTCCFG_TEST_GT(d_numRequired, 0);
    NTCCFG_TEST_EQ(d_numComplete, 0);

    ntci::SendCallback callback(
        NTCCFG_BIND(&Transfer::processComplete, 
                    this, 
                    NTCCFG_BIND_PLACEHOLDER_1, 
                    NTCCFG_BIND_PLACEHOLDER_2));

    for (bsl::size_t i = 0; i < d_numRequired; ++i) {
        if (i == 0) {
            zeroCopyQueue->push(d_group, d_data_sp, callback);
        }
        else {
            zeroCopyQueue->push(d_group);
        }
    }

    zeroCopyQueue->frame(d_group);
}

ntcq::SendCounter Transfer::group() const
{
    return d_group;
}

bool Transfer::complete() const
{
    return d_numComplete == d_numRequired;
}

bsl::shared_ptr<Transfer> Transfer::create(
    const bsl::shared_ptr<ntci::Sender>&   sender,
    ntcq::SendCounter                      group, 
    bsl::size_t                            numOperations,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<Transfer> transfer;
    transfer.createInplace(
        allocator, sender, group, numOperations, dataPool, allocator);

    return transfer;
}

} // close namespace test

NTCCFG_TEST_CASE(4)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<ntci::Sender> sender;

        bsl::shared_ptr<ntci::Strand> strand = ntci::Strand::unknown();

        bsl::shared_ptr<ntcs::DataPool> dataPool;
        dataPool.createInplace(&ta, &ta);

        ntcq::ZeroCopyQueue zeroCopyQueue(dataPool, &ta);

        bsl::shared_ptr<test::Transfer> transfer = test::Transfer::create(
            sender, 0, 1, dataPool, &ta);

        transfer->initiate(&zeroCopyQueue);

        ntsa::ZeroCopy zeroCopy(0, 0, 1);
        zeroCopyQueue.update(zeroCopy);

        ntci::SendCallback callback;
        bool exists = zeroCopyQueue.pop(&callback);

        NTCCFG_TEST_TRUE(exists);
        NTCCFG_TEST_TRUE(callback);

        ntca::SendEvent event;
        event.setType(ntca::SendEventType::e_COMPLETE);

        callback(sender, event, strand);

        NTCCFG_TEST_TRUE(transfer->complete());
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Concern: Test ntcq::ZeroCopyQueue exhaustive test
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<ntcs::DataPool> dataPool;
        dataPool.createInplace(&ta, &ta);

        ntcq::ZeroCopyQueue zeroCopyQueue(dataPool, &ta);


    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
}
NTCCFG_TEST_DRIVER_END;
