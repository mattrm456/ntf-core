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
BSLS_IDENT_RCSID(ntcs_blobbufferfactory_t_cpp, "$Id$ $CSID$")

#include <ntcs_blobbufferfactory.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::BlobBufferFactory'.
class BlobBufferFactoryTest
{
    static void logAfterAllocation(
        const ntcs::BlobBufferFactory& blobBufferFactory);

    static void logAfterAllocation(const ntcs::BlobBufferPool& blobBufferPool);

    static void work(bdlbb::BlobBufferFactory* blobBufferFactory,
                     bslmt::Barrier*           barrier,
                     bsl::size_t               numIterations,
                     bsl::size_t               maxThreads);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();

    // TODO
    static void verifyCase5();

    // TODO
    static void verifyCase6();

    // TODO
    static void verifyCase7();

    // TODO
    static void verifyCase8();

    // TODO
    static void verifyCase9();

    // TODO
    static void verifyCase10();

    // TODO
    static void verifyCase11();

    // Verify 'ntcs::BlobBufferFactory' tracks the number of buffers
    // allocated, available, and pooled, and the number of bytes in use,
    // independently of how the underlying memory pool acquires memory.
    static void verifyCase12();
};

void BlobBufferFactoryTest::logAfterAllocation(
    const ntcs::BlobBufferFactory& blobBufferFactory)
{
    BSLS_LOG_TRACE("Allocated blob buffer: "
                   "numBuffersAllocated = %d, "
                   "numBuffersAvailable = %d, "
                   "numBuffersPooled = %d, "
                   "numBytesInUse = %d",
                   (int)(blobBufferFactory.numBuffersAllocated()),
                   (int)(blobBufferFactory.numBuffersAvailable()),
                   (int)(blobBufferFactory.numBuffersPooled()),
                   (int)(blobBufferFactory.numBytesInUse()));
}

void BlobBufferFactoryTest::logAfterAllocation(
    const ntcs::BlobBufferPool& blobBufferPool)
{
    BSLS_LOG_TRACE("Allocated blob buffer: "
                   "numBuffersAllocated = %d, "
                   "numBuffersAvailable = %d, "
                   "numBuffersPooled = %d, "
                   "numBytesInUse = %d",
                   (int)(blobBufferPool.numBuffersAllocated()),
                   (int)(blobBufferPool.numBuffersAvailable()),
                   (int)(blobBufferPool.numBuffersPooled()),
                   (int)(blobBufferPool.numBytesInUse()));
}

void BlobBufferFactoryTest::work(bdlbb::BlobBufferFactory* blobBufferFactory,
                                 bslmt::Barrier*           barrier,
                                 bsl::size_t               numIterations,
                                 bsl::size_t               maxThreads)
{
    barrier->wait();
    barrier->wait();

    for (bsl::size_t iteration = 0; iteration < numIterations; ++iteration) {
        bdlbb::BlobBuffer blobBuffer;
        blobBufferFactory->allocate(&blobBuffer);
        bsl::memcpy(blobBuffer.data(), &iteration, sizeof iteration);
        // bslmt::ThreadUtil::yield();
    }

    barrier->wait();
    barrier->wait();
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase1)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE,
                                              NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

    {
        bdlbb::BlobBuffer blobBuffer1;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase2)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE,
                                              NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase3)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE,
                                              NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;
        bdlbb::BlobBuffer blobBuffer3;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 3");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer3 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 3");
        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 3);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;
        bdlbb::BlobBuffer blobBuffer3;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer2 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 3");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer3 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 3");
        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase4)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferPool blobBufferFactory(BLOB_BUFFER_SIZE,
                                           NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

    {
        bdlbb::BlobBuffer blobBuffer1;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase5)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferPool blobBufferFactory(BLOB_BUFFER_SIZE,
                                           NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase6)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferPool blobBufferFactory(BLOB_BUFFER_SIZE,
                                           NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;
        bdlbb::BlobBuffer blobBuffer3;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 3");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer3 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 3");
        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 3);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;
        bdlbb::BlobBuffer blobBuffer3;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer2 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 3");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer3 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 3");
        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase7)
{
    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    // clang-format off
    struct Data {
        bsl::size_t d_numThreads;
        bsl::size_t d_numIterations;
    }
    DATA[] = {
#if NTC_BUILD_WITH_VALGRIND
        { 1,    100000 },
        { 2,     50000 },
        { 4,     25000 },
        { 10,    10000 }
#else
        { 1,  10000000 },
        { 2,   5000000 },
        { 4,   2500000 },
        { 10,  1000000 }
#endif
    };
    // clang-format on

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    bsl::vector<bsls::Stopwatch> stopwatchList(NUM_DATA);

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(64);
        bslmt::Barrier barrier(DATA[variation].d_numThreads + 1);

        bslmt::ThreadGroup threadGroup;
        threadGroup.addThreads(
            bdlf::BindUtil::bind(&BlobBufferFactoryTest::work,
                                 &blobBufferFactory,
                                 &barrier,
                                 DATA[variation].d_numIterations,
                                 DATA[variation].d_numThreads),
            DATA[variation].d_numThreads);

        barrier.wait();
        stopwatchList[variation].start(true);
        barrier.wait();

        barrier.wait();
        stopwatchList[variation].stop();
        barrier.wait();

        threadGroup.joinAll();
    }

    const bsl::size_t W = 15;

    bsl::cout << "bdlbb::PooledBlobBufferFactory" << bsl::endl;
    bsl::cout << bsl::setw(W) << bsl::right << "Threads";
    bsl::cout << bsl::setw(W) << bsl::right << "Iterations";
    bsl::cout << bsl::setw(W) << bsl::right << "Wall";
    bsl::cout << bsl::setw(W) << bsl::right << "CPU";
    bsl::cout << bsl::setw(W) << bsl::right << "User";
    bsl::cout << bsl::setw(W) << bsl::right << "System";
    bsl::cout << bsl::endl;

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numThreads;
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numIterations;
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].accumulatedWallTime()
                     ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].accumulatedUserTime() +
                         stopwatchList[variation].accumulatedSystemTime()
                     ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].accumulatedUserTime()
                     ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].accumulatedSystemTime()
                    ).totalMicroseconds();
        bsl::cout << bsl::endl;
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase8)
{
    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    // clang-format off
    struct Data {
        bsl::size_t d_numThreads;
        bsl::size_t d_numIterations;
    }
    DATA[] = {
#if NTC_BUILD_WITH_VALGRIND
        { 1,    100000 },
        { 2,     50000 },
        { 4,     25000 },
        { 10,    10000 }
#else
        { 1,  10000000 },
        { 2,   5000000 },
        { 4,   2500000 },
        { 10,  1000000 }
#endif
    };
    // clang-format on

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    bsl::vector<bsl::pair<bsls::Stopwatch, bsl::size_t> > stopwatchList(
        NUM_DATA);

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        ntcs::BlobBufferFactory blobBufferFactory(64);
        bslmt::Barrier          barrier(DATA[variation].d_numThreads + 1);

        // MRM: blobBufferFactory.reserve(DATA[variation].d_numThreads + 1);

        bslmt::ThreadGroup threadGroup;
        threadGroup.addThreads(
            bdlf::BindUtil::bind(&BlobBufferFactoryTest::work,
                                 &blobBufferFactory,
                                 &barrier,
                                 DATA[variation].d_numIterations,
                                 DATA[variation].d_numThreads),
            DATA[variation].d_numThreads);

        barrier.wait();
        stopwatchList[variation].first.start(true);
        barrier.wait();

        barrier.wait();
        stopwatchList[variation].first.stop();
        barrier.wait();

        threadGroup.joinAll();

        stopwatchList[variation].second = blobBufferFactory.numBuffersPooled();
    }

    const bsl::size_t W = 15;

    bsl::cout << "ntcs::PooledBlobBufferFactory" << bsl::endl;
    bsl::cout << bsl::setw(W) << bsl::right << "Threads";
    bsl::cout << bsl::setw(W) << bsl::right << "Iterations";
    bsl::cout << bsl::setw(W) << bsl::right << "Wall";
    bsl::cout << bsl::setw(W) << bsl::right << "CPU";
    bsl::cout << bsl::setw(W) << bsl::right << "User";
    bsl::cout << bsl::setw(W) << bsl::right << "System";
    bsl::cout << bsl::setw(W) << bsl::right << "NumPooled";
    bsl::cout << bsl::endl;

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numThreads;
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numIterations;
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].first.accumulatedWallTime()
                     ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].first.accumulatedUserTime() +
                         stopwatchList[variation].first.accumulatedSystemTime()
                     ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].first.accumulatedUserTime()
                     ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].first.accumulatedSystemTime()
                    ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].second;
        bsl::cout << bsl::endl;
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase9)
{
#if 1
    bslma::Allocator *allocator = bslma::Default::defaultAllocator();

    // clang-format off
    struct Data {
        bsl::size_t d_numThreads;
        bsl::size_t d_numIterations;
    }
    DATA[] = {
#if NTC_BUILD_WITH_VALGRIND
        { 1,    100000 },
        { 2,     50000 },
        { 4,     25000 },
        { 10,    10000 }
#else
        { 1,  10000000 },
        { 2,   5000000 },
        { 4,   2500000 },
        { 10,  1000000 }
#endif
    };
    // clang-format on

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    bsl::vector<bsl::pair<bsls::Stopwatch, bsl::size_t> > stopwatchList(
                                                                     NUM_DATA);

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        ntcs::BlobBufferPool blobBufferFactory(64);
        bslmt::Barrier       barrier(DATA[variation].d_numThreads + 1);

        // MRM: blobBufferFactory.reserve(DATA[variation].d_numThreads + 1);

        bslmt::ThreadGroup threadGroup;
        threadGroup.addThreads(
            bdlf::BindUtil::bind(&BlobBufferFactoryTest::work,
                                 &blobBufferFactory,
                                 &barrier,
                                 DATA[variation].d_numIterations,
                                 DATA[variation].d_numThreads),
            DATA[variation].d_numThreads);

        barrier.wait();
        stopwatchList[variation].first.start(true);
        barrier.wait();

        barrier.wait();
        stopwatchList[variation].first.stop();
        barrier.wait();

        threadGroup.joinAll();

        stopwatchList[variation].second = blobBufferFactory.numBuffersPooled();
    }

    const bsl::size_t W = 15;

    bsl::cout << "ntcs::PooledBlobBufferPool (*)" << bsl::endl;
    bsl::cout << bsl::setw(W) << bsl::right << "Threads";
    bsl::cout << bsl::setw(W) << bsl::right << "Iterations";
    bsl::cout << bsl::setw(W) << bsl::right << "Wall";
    bsl::cout << bsl::setw(W) << bsl::right << "CPU";
    bsl::cout << bsl::setw(W) << bsl::right << "User";
    bsl::cout << bsl::setw(W) << bsl::right << "System";
    bsl::cout << bsl::setw(W) << bsl::right << "NumPooled";
    bsl::cout << bsl::endl;

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numThreads;
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numIterations;
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].first.accumulatedWallTime()
                     ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].first.accumulatedUserTime() +
                         stopwatchList[variation].first.accumulatedSystemTime()
                     ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].first.accumulatedUserTime()
                     ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << bsls::TimeInterval(
                         stopwatchList[variation].first.accumulatedSystemTime()
                    ).totalMicroseconds();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].second;
        bsl::cout << bsl::endl;
    }
#endif
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase10)
{
    // Concern: A tagged index encodes and decodes every index and tag.
    //
    // Plan: Round-trip representative and boundary values through the
    // utility, and verify the tag wraps around to zero.

    typedef ntcs::BlobBufferPoolHandleUtil Util;

    NTSCFG_TEST_EQ(Util::maxIndex(), 0xFFFFFFFFU);
    NTSCFG_TEST_EQ(Util::maxTag(), 0xFFFFFFFFU);

    const Util::IndexType INDEXES[] = {
        0, 1, 2, 0x7FFFFFFFU, 0x80000000U, 0xFFFFFFFEU, 0xFFFFFFFFU
    };

    const Util::TagType TAGS[] = {
        0, 1, 2, 0x7FFFFFFFU, 0x80000000U, 0xFFFFFFFEU, 0xFFFFFFFFU
    };

    for (bsl::size_t i = 0; i < sizeof INDEXES / sizeof INDEXES[0]; ++i) {
        for (bsl::size_t j = 0; j < sizeof TAGS / sizeof TAGS[0]; ++j) {
            const Util::RawType raw = Util::initialize(INDEXES[i], TAGS[j]);

            NTSCFG_TEST_EQ(Util::getIndex(raw), INDEXES[i]);
            NTSCFG_TEST_EQ(Util::getTag(raw), TAGS[j]);

            Util::IndexType index = 0;
            Util::TagType   tag   = 0;
            Util::get(&index, &tag, raw);

            NTSCFG_TEST_EQ(index, INDEXES[i]);
            NTSCFG_TEST_EQ(tag, TAGS[j]);
        }
    }

    NTSCFG_TEST_EQ(Util::initialize(0, 0), 0);

    const Util::TagType nextTag = Util::maxTag() + 1;
    NTSCFG_TEST_EQ(nextTag, 0);
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase11)
{
    // Concern: The atomic tagged index loads, stores, swaps, and
    // tests-and-swaps both the index and the tag, and a test-and-swap fails
    // if either the index or the tag differs from its expected value.
    //
    // Plan: Exercise each operation and verify the resulting values.

    typedef ntcs::BlobBufferPoolHandle Handle;

    const Handle::IndexType INDEX1 = 1;
    const Handle::IndexType INDEX2 = 2;

    Handle handle;

    {
        Handle::IndexType currentIndex;
        Handle::TagType   currentTag;

        handle.load(&currentIndex, &currentTag);

        NTSCFG_TEST_EQ(currentIndex, 0);
        NTSCFG_TEST_EQ(currentTag, 0);
    }

    {
        Handle::IndexType previousIndex;
        Handle::TagType   previousTag;

        handle.swap(&previousIndex, &previousTag, INDEX1, 1);

        NTSCFG_TEST_EQ(previousIndex, 0);
        NTSCFG_TEST_EQ(previousTag, 0);
    }

    {
        Handle::IndexType currentIndex;
        Handle::TagType   currentTag;

        handle.load(&currentIndex, &currentTag);

        NTSCFG_TEST_EQ(currentIndex, INDEX1);
        NTSCFG_TEST_EQ(currentTag, 1);
    }

    handle.store(0, 0);

    {
        Handle::IndexType currentIndex;
        Handle::TagType   currentTag;

        handle.load(&currentIndex, &currentTag);

        NTSCFG_TEST_EQ(currentIndex, 0);
        NTSCFG_TEST_EQ(currentTag, 0);
    }

    {
        Handle::IndexType previousIndex;
        Handle::TagType   previousTag;

        bool result = handle.testAndSwap(&previousIndex,
                                         &previousTag,
                                         0,
                                         0,
                                         INDEX1,
                                         1);

        NTSCFG_TEST_TRUE(result);

        NTSCFG_TEST_EQ(previousIndex, 0);
        NTSCFG_TEST_EQ(previousTag, 0);
    }

    {
        Handle::IndexType currentIndex;
        Handle::TagType   currentTag;

        handle.load(&currentIndex, &currentTag);

        NTSCFG_TEST_EQ(currentIndex, INDEX1);
        NTSCFG_TEST_EQ(currentTag, 1);
    }

    handle.store(INDEX1, 2);

    // The index matches but the tag does not.

    {
        Handle::IndexType previousIndex;
        Handle::TagType   previousTag;

        bool result = handle.testAndSwap(&previousIndex,
                                         &previousTag,
                                         INDEX1,
                                         1,
                                         INDEX2,
                                         2);

        NTSCFG_TEST_FALSE(result);

        NTSCFG_TEST_EQ(previousIndex, INDEX1);
        NTSCFG_TEST_EQ(previousTag, 2);
    }

    // The tag matches but the index does not.

    {
        Handle::IndexType previousIndex;
        Handle::TagType   previousTag;

        bool result = handle.testAndSwapAcqRel(&previousIndex,
                                               &previousTag,
                                               INDEX2,
                                               2,
                                               INDEX2,
                                               3);

        NTSCFG_TEST_FALSE(result);

        NTSCFG_TEST_EQ(previousIndex, INDEX1);
        NTSCFG_TEST_EQ(previousTag, 2);
    }

    {
        Handle::IndexType currentIndex;
        Handle::TagType   currentTag;

        handle.loadAcquire(&currentIndex, &currentTag);

        NTSCFG_TEST_EQ(currentIndex, INDEX1);
        NTSCFG_TEST_EQ(currentTag, 2);
    }

    {
        Handle::IndexType previousIndex;
        Handle::TagType   previousTag;

        bool result = handle.testAndSwapAcqRel(&previousIndex,
                                               &previousTag,
                                               INDEX1,
                                               2,
                                               INDEX2,
                                               3);

        NTSCFG_TEST_TRUE(result);

        NTSCFG_TEST_EQ(previousIndex, INDEX1);
        NTSCFG_TEST_EQ(previousTag, 2);
    }

    {
        Handle::IndexType currentIndex;
        Handle::TagType   currentTag;

        handle.loadRelaxed(&currentIndex, &currentTag);

        NTSCFG_TEST_EQ(currentIndex, INDEX2);
        NTSCFG_TEST_EQ(currentTag, 3);
    }

    // The tag wraps around to zero.

    handle.storeRelease(INDEX2, Handle::maxTag());

    {
        Handle::IndexType previousIndex;
        Handle::TagType   previousTag;

        bool result = handle.testAndSwap(&previousIndex,
                                         &previousTag,
                                         INDEX2,
                                         Handle::maxTag(),
                                         INDEX1,
                                         Handle::maxTag() + 1);

        NTSCFG_TEST_TRUE(result);

        NTSCFG_TEST_EQ(previousIndex, INDEX2);
        NTSCFG_TEST_EQ(previousTag, Handle::maxTag());
    }

    {
        Handle::IndexType currentIndex;
        Handle::TagType   currentTag;

        handle.load(&currentIndex, &currentTag);

        NTSCFG_TEST_EQ(currentIndex, INDEX1);
        NTSCFG_TEST_EQ(currentTag, 0);
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase12)
{
    // Concern: The number of buffers allocated, available, and pooled, and
    // the number of bytes in use, are tracked correctly regardless of how
    // many blocks the underlying memory pool acquires at a time.
    //
    // Plan: Allocate more buffers than the underlying memory pool acquires
    // in its initial allocation, release them, then allocate a smaller
    // number of buffers again. Verify the buffer counts exactly, and verify
    // the number of bytes in use only against properties guaranteed by the
    // contract of the underlying memory pool: it never shrinks while the
    // factory exists, it is sufficient to hold every pooled buffer, and all
    // memory is returned when the factory is destroyed.

    const bsl::size_t BLOB_BUFFER_SIZE = 4096;
    const bsl::size_t NUM_BUFFERS      = 1000;
    const bsl::size_t NUM_REUSED       = 10;

    bdlma::CountingAllocator countingAllocator(NTSCFG_TEST_ALLOCATOR);

    {
        ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE,
                                                  &countingAllocator);

        NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
        NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
        NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

        bsl::size_t numBytesInUse = 0;

        {
            bsl::vector<bdlbb::BlobBuffer> blobBufferVector(
                NTSCFG_TEST_ALLOCATOR);
            blobBufferVector.resize(NUM_BUFFERS);

            for (bsl::size_t i = 0; i < NUM_BUFFERS; ++i) {
                blobBufferFactory.allocate(&blobBufferVector[i]);

                NTSCFG_TEST_NE(blobBufferVector[i].data(), 0);
                NTSCFG_TEST_EQ(static_cast<bsl::size_t>(blobBufferVector[i].size()),
                               BLOB_BUFFER_SIZE);

                NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), i + 1);
                NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), i + 1);

                NTSCFG_TEST_GE(blobBufferFactory.numBytesInUse(),
                               numBytesInUse);
                NTSCFG_TEST_GE(blobBufferFactory.numBytesInUse(),
                               (i + 1) * BLOB_BUFFER_SIZE);

                numBytesInUse = blobBufferFactory.numBytesInUse();
            }

            NTSCFG_TEST_GE(countingAllocator.numBytesInUse(),
                           static_cast<bsls::Types::Int64>(numBytesInUse));

            for (bsl::size_t i = 0; i < NUM_BUFFERS; ++i) {
                blobBufferVector[i].reset();

                NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(),
                               NUM_BUFFERS - (i + 1));
                NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(),
                               i + 1);
                NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(),
                               NUM_BUFFERS);

                NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(),
                               numBytesInUse);
            }
        }

        {
            bsl::vector<bdlbb::BlobBuffer> blobBufferVector(
                NTSCFG_TEST_ALLOCATOR);
            blobBufferVector.resize(NUM_REUSED);

            for (bsl::size_t i = 0; i < NUM_REUSED; ++i) {
                blobBufferFactory.allocate(&blobBufferVector[i]);

                NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), i + 1);
                NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(),
                               NUM_BUFFERS - (i + 1));
                NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(),
                               NUM_BUFFERS);

                NTSCFG_TEST_GE(blobBufferFactory.numBytesInUse(),
                               numBytesInUse);

                numBytesInUse = blobBufferFactory.numBytesInUse();
            }
        }

        NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), NUM_BUFFERS);
        NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), NUM_BUFFERS);
        NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), numBytesInUse);
    }

    NTSCFG_TEST_EQ(countingAllocator.numBytesInUse(), 0);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
