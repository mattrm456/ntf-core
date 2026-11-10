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

#include <ntcs_blobbufferfactory.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_blobbufferfactory_cpp, "$Id$ $CSID$")

#include <bdlb_bitutil.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmf_assert.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_log.h>

// Uncomment to enable pooling.
#define NTCS_BLOBBUFFERFACTORY_POOL 1

// Uncomment to enable logging from this component.
// #define NTCS_BLOBBUFFERFACTORY_LOG 1

#if NTCS_BLOBBUFFERFACTORY_LOG

#define NTCS_BLOBBUFFERFACTORY_LOG_POOL_ACQUIRE(address,                      \
                                                numAllocated,                 \
                                                numPooled,                    \
                                                numBytesInUse)                \
    BSLS_LOG_TRACE("Acquired buffer memory: address = %p, "                   \
                   "numAllocated = %d, numPooled = %d, numBytesInUse = %d",   \
                   address,                                                   \
                   (int)(numAllocated),                                       \
                   (int)(numPooled),                                          \
                   (int)(numBytesInUse))

#define NTCS_BLOBBUFFERFACTORY_LOG_POOL_RELEASE(address,                      \
                                                numAllocated,                 \
                                                numPooled,                    \
                                                numBytesInUse)                \
    BSLS_LOG_TRACE("Released buffer memory: address = %p, "                   \
                   "numAllocated = %d, numPooled = %d, numBytesInUse = %d",   \
                   address,                                                   \
                   (int)(numAllocated),                                       \
                   (int)(numPooled),                                          \
                   (int)(numBytesInUse))

#else

#define NTCS_BLOBBUFFERFACTORY_LOG_POOL_ACQUIRE(address,                      \
                                                numAllocated,                 \
                                                numPooled,                    \
                                                numBytesInUse)
#define NTCS_BLOBBUFFERFACTORY_LOG_POOL_RELEASE(address,                      \
                                                numAllocated,                 \
                                                numPooled,                    \
                                                numBytesInUse)

#endif

namespace BloombergLP {
namespace ntcs {

const ntci::MetricMetadata BlobBufferFactoryMetrics::STATISTICS[] = {
#if NTCI_METRIC_PREFIX

    NTCI_METRIC_METADATA_GAUGE(BuffersInUse),
    NTCI_METRIC_METADATA_GAUGE(BuffersPooled),

    NTCI_METRIC_METADATA_GAUGE(BytesInUse),
    NTCI_METRIC_METADATA_GAUGE(BytesPooled),

#else
    NTCI_METRIC_METADATA_GAUGE(buffersInUse),
    NTCI_METRIC_METADATA_GAUGE(buffersPooled),

    NTCI_METRIC_METADATA_GAUGE(bytesInUse),
    NTCI_METRIC_METADATA_GAUGE(bytesPooled),
#endif
};

BlobBufferFactoryMetrics::BlobBufferFactoryMetrics(
    const bslstl::StringRef& prefix,
    const bslstl::StringRef& objectName,
    bslma::Allocator*        basicAllocator)
: d_mutex()
, d_numAllocated(0)
, d_numAvailable(0)
, d_numPooled(0)
, d_numBytesInUse(0)
, d_prefix(prefix, basicAllocator)
, d_objectName(objectName, basicAllocator)
, d_parent_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

BlobBufferFactoryMetrics::BlobBufferFactoryMetrics(
    const bslstl::StringRef&                               prefix,
    const bslstl::StringRef&                               objectName,
    const bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics>& parent,
    bslma::Allocator*                                      basicAllocator)
: d_mutex()
, d_numAllocated(0)
, d_numAvailable(0)
, d_numPooled(0)
, d_numBytesInUse(0)
, d_prefix(basicAllocator)
, d_objectName(basicAllocator)
, d_parent_sp(parent)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_parent_sp) {
        d_prefix.append(d_parent_sp->getFieldPrefix(0));
        d_prefix.append(1, '.');

        d_objectName.append(d_parent_sp->objectName());
        d_objectName.append(1, '-');
    }

    d_prefix.append(prefix);
    d_objectName.append(objectName);
}

BlobBufferFactoryMetrics::~BlobBufferFactoryMetrics()
{
}

void BlobBufferFactoryMetrics::getStats(bdld::ManagedDatum* result)
{
    LockGuard guard(&d_mutex);

    bdld::DatumMutableArrayRef array;
    bdld::Datum::createUninitializedArray(&array,
                                          numOrdinals(),
                                          result->allocator());

    bsl::size_t buffersInUse  = 0;
    bsl::size_t buffersPooled = 0;
    bsl::size_t bytesInUse    = 0;
    bsl::size_t bytesPooled   = 0;

    array.data()[0] = bdld::Datum::createDouble(double(buffersInUse));
    array.data()[1] = bdld::Datum::createDouble(double(buffersPooled));
    array.data()[2] = bdld::Datum::createDouble(double(bytesInUse));
    array.data()[3] = bdld::Datum::createDouble(double(bytesPooled));

    *array.length() = numOrdinals();

    result->adopt(bdld::Datum::adoptArray(array));
}

const char* BlobBufferFactoryMetrics::getFieldPrefix(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);
    return d_prefix.c_str();
}

const char* BlobBufferFactoryMetrics::getFieldName(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return BlobBufferFactoryMetrics::STATISTICS[ordinal].d_name;
    }
    else {
        return 0;
    }
}

const char* BlobBufferFactoryMetrics::getFieldDescription(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);
    return "";
}

ntci::Monitorable::StatisticType BlobBufferFactoryMetrics::getFieldType(
    int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return BlobBufferFactoryMetrics::STATISTICS[ordinal].d_type;
    }
    else {
        return ntci::Monitorable::e_AVERAGE;
    }
}

int BlobBufferFactoryMetrics::getFieldTags(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);
    return ntci::Monitorable::e_ANONYMOUS;
}

int BlobBufferFactoryMetrics::getFieldOrdinal(const char* fieldName) const
{
    int result = 0;

    for (int ordinal = 0; ordinal < numOrdinals(); ++ordinal) {
        if (bsl::strcmp(BlobBufferFactoryMetrics::STATISTICS[ordinal].d_name,
                        fieldName) == 0)
        {
            result = ordinal;
        }
    }

    return result;
}

int BlobBufferFactoryMetrics::numOrdinals() const
{
    return sizeof BlobBufferFactoryMetrics::STATISTICS /
           sizeof BlobBufferFactoryMetrics::STATISTICS[0];
}

const char* BlobBufferFactoryMetrics::objectName() const
{
    return d_objectName.c_str();
}

const bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics>& BlobBufferFactoryMetrics::
    parent() const
{
    return d_parent_sp;
}

bsl::size_t BlobBufferFactoryMetrics::numBuffersAllocated() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numAllocated.loadRelaxed());
}

bsl::size_t BlobBufferFactoryMetrics::numBuffersAvailable() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numAvailable.loadRelaxed());
}

bsl::size_t BlobBufferFactoryMetrics::numBuffersPooled() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numPooled.loadRelaxed());
}

bsl::size_t BlobBufferFactoryMetrics::numBytesInUse() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numBytesInUse.loadRelaxed());
}

void BlobBufferFactory::updateNumPooled(bsl::uint64_t numAllocated)
{
    bsl::uint64_t numPooled = d_numPooled.loadRelaxed();
    while (numPooled < numAllocated) {
        const bsl::uint64_t previous =
            d_numPooled.testAndSwap(numPooled, numAllocated);
        if (previous == numPooled) {
            break;
        }
        numPooled = previous;
    }
}

void* BlobBufferFactory::allocate(size_type size)
{
#if NTCS_BLOBBUFFERFACTORY_POOL

    void* address = d_memoryPool.allocate(size);

    const bsl::uint64_t numAllocated = d_numAllocated.addRelaxed(1);
    this->updateNumPooled(numAllocated);

    NTCS_BLOBBUFFERFACTORY_LOG_POOL_ACQUIRE(address,
                                            numAllocated,
                                            d_numPooled.loadRelaxed(),
                                            this->numBytesInUse());

    return address;

#else

    void* address = d_memoryPoolAllocator.allocate(size);

    d_numAllocated.addRelaxed(1);
    d_numPooled.addRelaxed(1);

    return address;

#endif
}

void BlobBufferFactory::deallocate(void* address)
{
    if (address == 0) {
        return;
    }

#if NTCS_BLOBBUFFERFACTORY_POOL

    d_memoryPool.deallocate(address);

    const bsl::uint64_t numAllocated = d_numAllocated.subtractRelaxed(1);

    NTCS_BLOBBUFFERFACTORY_LOG_POOL_RELEASE(address,
                                            numAllocated,
                                            d_numPooled.loadRelaxed(),
                                            this->numBytesInUse());

    NTCCFG_WARNING_UNUSED(numAllocated);

#else

    d_memoryPoolAllocator.deallocate(address);

    d_numAllocated.subtractRelaxed(1);
    d_numPooled.subtractRelaxed(1);

#endif
}

BlobBufferFactory::BlobBufferFactory(bsl::size_t       blobBufferSize,
                                     bslma::Allocator* basicAllocator)
: d_memoryPoolAllocator(basicAllocator)
, d_memoryPool(&d_memoryPoolAllocator)
, d_blobBufferSize(blobBufferSize)
, d_numAllocated(0)
, d_numPooled(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

BlobBufferFactory::~BlobBufferFactory()
{
    BSLS_ASSERT_OPT(d_numAllocated == 0);
}

void BlobBufferFactory::allocate(bdlbb::BlobBuffer* buffer)
{
    buffer->reset(bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(
                      d_blobBufferSize,
                      this),
                  NTCCFG_WARNING_NARROW(int, d_blobBufferSize));
}

bsl::size_t BlobBufferFactory::numBuffersAllocated() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numAllocated.loadRelaxed());
}

bsl::size_t BlobBufferFactory::numBuffersAvailable() const
{
    // Load the number pooled before the number allocated: the number pooled
    // is raised only after the number allocated is incremented, but a
    // concurrent allocation may still be observed between the two loads, so
    // saturate at zero.

    const bsl::uint64_t numPooled    = d_numPooled.loadAcquire();
    const bsl::uint64_t numAllocated = d_numAllocated.loadAcquire();

    if (numPooled > numAllocated) {
        return NTCCFG_WARNING_NARROW(bsl::size_t, numPooled - numAllocated);
    }
    else {
        return 0;
    }
}

bsl::size_t BlobBufferFactory::numBuffersPooled() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numPooled.loadRelaxed());
}

bsl::size_t BlobBufferFactory::numBytesInUse() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t,
                                 d_memoryPoolAllocator.numBytesInUse());
}

// 32-bits                    64-bits:
// 4 = vtable                 8 = vtable
// 4 = strong                 4 = strong
// 4 = weak                   4 = weak
// 4 = data                   8 = data
// 4 = pool                   8 = pool
// 4 = next                   4 = next
// 4 = index                  4 = index
// 4 = shard                  4 = shard
// 32                         44
// 64 - 32 = 32 bytes padding
// 64 - 44 = 20 bytes padding
BSLMF_ASSERT(sizeof(BlobBufferPoolObject) == 64);

namespace {

// The next home shard index to assign, plus one, shared by all pools.
bsls::AtomicOperations::AtomicTypes::Uint s_nextShard;

}  // close unnamed namespace

BlobBufferPoolObject::BlobBufferPoolObject(BlobBufferPool* pool,
                                           bsl::uint32_t   index)
: bslma::SharedPtrRep()
, d_data_p(0)
, d_pool_p(pool)
, d_next(0)
, d_index(index)
, d_shard(0)
, d_padding()
{
    BSLS_ASSERT(index != 0);

    d_data_p = (char*)(this) + sizeof(BlobBufferPoolObject);
    BSLS_ASSERT((bsl::size_t)(bsl::uintptr_t)(d_data_p) % 16 == 0);
}

bsl::size_t BlobBufferPool::homeShard()
{
    // Zero indicates the calling thread has not yet been assigned a shard.
    // Threads are assigned shards round-robin, which distributes them more
    // evenly than hashing a thread identifier.

    static BSLS_KEYWORD_THREAD_LOCAL unsigned int s_shard = 0;

    unsigned int shard = s_shard;
    if (NTCCFG_UNLIKELY(shard == 0)) {
        shard = bsls::AtomicOperations::addUintNvRelaxed(&s_nextShard, 1);
        if (shard == 0) {
            shard = bsls::AtomicOperations::addUintNvRelaxed(&s_nextShard, 1);
        }
        s_shard = shard;
    }

    return static_cast<bsl::size_t>(shard - 1) & (k_NUM_SHARDS - 1);
}

void BlobBufferPool::locate(bsl::size_t*  slab,
                            bsl::size_t*  offset,
                            bsl::uint32_t index) const
{
    BSLS_ASSERT(index != 0);

    // Slab 'i' holds '2^i' objects for 'i <= L', so the indices '[1, 2^(L+1))'
    // occupy slabs '0' through 'L', and index 'n' in that range is in slab
    // 'floor(log2(n))' at offset 'n - 2^slab'. Every later slab holds '2^L'
    // objects.

    const bsl::size_t n = index;

    if (n < (static_cast<bsl::size_t>(2) << d_slabShift)) {
        const bsl::size_t log2 =
            31 - bdlb::BitUtil::numLeadingUnsetBits(index);
        *slab   = log2;
        *offset = n - (static_cast<bsl::size_t>(1) << log2);
    }
    else {
        const bsl::size_t m = n - (static_cast<bsl::size_t>(2) << d_slabShift);
        *slab   = d_slabShift + 1 + (m >> d_slabShift);
        *offset = m & ((static_cast<bsl::size_t>(1) << d_slabShift) - 1);
    }
}

BlobBufferPoolObject* BlobBufferPool::lookup(bsl::uint32_t index) const
{
    // The caller observed 'index' (with acquire semantics) after the object
    // was carved, and the slab table current when the object was carved, or
    // any later table, holds the address of its slab, so the table may be
    // read without locking. Load the table only after observing the index.

    bsl::size_t slab;
    bsl::size_t offset;
    this->locate(&slab, &offset, index);

    char** slabTable = d_slabTable.loadAcquire();

    return reinterpret_cast<BlobBufferPoolObject*>(slabTable[slab] +
                                                   offset * d_objectSize);
}

BlobBufferPoolObject* BlobBufferPool::pop(Shard* shard) const
{
    Handle::IndexType oldIndex;
    Handle::TagType   oldTag;

    shard->d_head.loadAcquire(&oldIndex, &oldTag);

    while (oldIndex != 0) {
        BlobBufferPoolObject* object = this->lookup(oldIndex);

        const Handle::IndexType newIndex = object->next();
        const Handle::TagType   newTag   = oldTag + 1;

        Handle::IndexType nowIndex;
        Handle::TagType   nowTag;

        const bool unchanged = shard->d_head.testAndSwapAcqRel(&nowIndex,
                                                               &nowTag,
                                                               oldIndex,
                                                               oldTag,
                                                               newIndex,
                                                               newTag);
        if (unchanged) {
            return object;
        }

        oldIndex = nowIndex;
        oldTag   = nowTag;
    }

    return 0;
}

void BlobBufferPool::push(Shard*                shard,
                          BlobBufferPoolObject* first,
                          BlobBufferPoolObject* last)
{
    const Handle::IndexType firstIndex = first->index();

    Handle::IndexType oldIndex;
    Handle::TagType   oldTag;

    shard->d_head.loadRelaxed(&oldIndex, &oldTag);

    while (true) {
        last->setNext(oldIndex);

        const Handle::TagType newTag = oldTag + 1;

        Handle::IndexType nowIndex;
        Handle::TagType   nowTag;

        const bool unchanged = shard->d_head.testAndSwapAcqRel(&nowIndex,
                                                               &nowTag,
                                                               oldIndex,
                                                               oldTag,
                                                               firstIndex,
                                                               newTag);
        if (unchanged) {
            return;
        }

        oldIndex = nowIndex;
        oldTag   = nowTag;
    }
}

BlobBufferPoolObject* BlobBufferPool::carve()
{
    if (NTCCFG_UNLIKELY(d_numCarved == Handle::Util::maxIndex())) {
        bsls::BslExceptionUtil::throwBadAlloc();
    }

    const bsl::uint32_t index = d_numCarved + 1;

    bsl::size_t slab;
    bsl::size_t offset;
    this->locate(&slab, &offset, index);

    char** slabTable = d_slabTable.loadRelaxed();

    if (offset == 0) {
        BSLS_ASSERT(slab == d_numSlabs);

        // Allocate the slab table, or grow it, if necessary, by copying it to
        // a table twice the size. Publish the new table but retain the old
        // one, since other threads may still be reading it.

        if (d_numSlabs == d_slabTableCapacity) {
            const bsl::size_t newCapacity =
                d_slabTableCapacity == 0
                    ? static_cast<bsl::size_t>(k_INITIAL_SLAB_TABLE_CAPACITY)
                    : d_slabTableCapacity * 2;

            d_slabTables.reserve(d_slabTables.size() + 1);

            char** newSlabTable = static_cast<char**>(
                d_countingAllocator.allocate(newCapacity * sizeof(char*)));

            for (bsl::size_t i = 0; i < d_numSlabs; ++i) {
                newSlabTable[i] = slabTable[i];
            }

            d_slabTables.push_back(newSlabTable);
            d_slabTable.storeRelease(newSlabTable);

            slabTable           = newSlabTable;
            d_slabTableCapacity = newCapacity;
        }

        const bsl::size_t numObjects =
            (slab <= d_slabShift) ? (static_cast<bsl::size_t>(1) << slab)
                                  : (static_cast<bsl::size_t>(1)
                                     << d_slabShift);

        char* memory = static_cast<char*>(
            d_aligningAllocator.allocate(numObjects * d_objectSize));
        BSLS_ASSERT((bsl::size_t)(bsl::uintptr_t)(memory) % k_ALIGNMENT == 0);

        // The address of the slab is published to other threads by the
        // release semantics of the push of any object carved from it.

        slabTable[slab] = memory;
        ++d_numSlabs;
    }

    void* arena = slabTable[slab] + offset * d_objectSize;

    BlobBufferPoolObject* object =
        new (arena) BlobBufferPoolObject(this, index);
    object->resetCountsRaw(0, 0);

    d_numCarved = index;
    d_numPooled.addRelaxed(1);

    return object;
}

BlobBufferPoolObject* BlobBufferPool::replenish()
{
    LockGuard guard(&d_mutex);
    return this->carve();
}

BlobBufferPool::BlobBufferPool(bsl::size_t       blobBufferSize,
                               bslma::Allocator* basicAllocator)
: d_blobBufferSize(blobBufferSize)
, d_objectSize(0)
, d_slabShift(0)
, d_slabTable(0)
, d_shards()
, d_mutex()
, d_slabTableCapacity(0)
, d_numSlabs(0)
, d_numCarved(0)
, d_numPooled(0)
, d_countingAllocator(basicAllocator)
, d_aligningAllocator(k_ALIGNMENT, &d_countingAllocator)
, d_slabTables(&d_countingAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLMF_ASSERT(sizeof(Shard) == k_CACHE_LINE_SIZE);
    BSLMF_ASSERT((k_NUM_SHARDS & (k_NUM_SHARDS - 1)) == 0);
    BSLMF_ASSERT((k_MAX_SLAB_OBJECTS & (k_MAX_SLAB_OBJECTS - 1)) == 0);
    BSLMF_ASSERT(k_ALIGNMENT >= sizeof(BlobBufferPoolObject));

    // Round the size of each object up to the alignment so that every
    // object carved from a slab is aligned.

    d_objectSize = sizeof(BlobBufferPoolObject) + d_blobBufferSize;
    d_objectSize = (d_objectSize + (k_ALIGNMENT - 1)) &
                   ~static_cast<bsl::size_t>(k_ALIGNMENT - 1);

    // Choose the maximum number of objects per slab: the largest power of
    // two no greater than either limit, but at least one.

    bsl::size_t maxSlabObjects = k_MAX_SLAB_SIZE / d_objectSize;
    if (maxSlabObjects > k_MAX_SLAB_OBJECTS) {
        maxSlabObjects = k_MAX_SLAB_OBJECTS;
    }

    while ((static_cast<bsl::size_t>(2) << d_slabShift) <= maxSlabObjects) {
        ++d_slabShift;
    }

    // The slab table is allocated when the first object is carved, so that
    // a pool that is never used allocates no memory.
}

BlobBufferPool::~BlobBufferPool()
{
    BSLS_ASSERT_OPT(this->numBuffersAllocated() == 0);

    // Each object is trivially destructible in effect, so release the slabs
    // without destroying the objects carved from them, then release every
    // slab table.

    char** slabTable = d_slabTable.loadRelaxed();

    BSLS_ASSERT(d_numSlabs == 0 || slabTable != 0);

    for (bsl::size_t i = 0; i < d_numSlabs; ++i) {
        d_aligningAllocator.deallocate(slabTable[i]);
    }

    for (bsl::size_t i = 0; i < d_slabTables.size(); ++i) {
        d_countingAllocator.deallocate(d_slabTables[i]);
    }
}

void BlobBufferPool::allocate(bdlbb::BlobBuffer* buffer)
{
    const bsl::size_t home  = BlobBufferPool::homeShard();
    Shard*            shard = &d_shards.d_shard[home];

    BlobBufferPoolObject* object = this->pop(shard);

    if (NTCCFG_UNLIKELY(object == 0)) {
        for (bsl::size_t i = 1; i < k_NUM_SHARDS; ++i) {
            object = this->pop(
                &d_shards.d_shard[(home + i) & (k_NUM_SHARDS - 1)]);
            if (object != 0) {
                break;
            }
        }

        if (object == 0) {
            object = this->replenish();
        }
    }

    object->setNext(0);
    object->setShard(static_cast<bsl::uint32_t>(home));

    shard->d_numAllocated.addRelaxed(1);

    BSLS_ASSERT(object->data() != 0);
    BSLS_ASSERT(object->numReferences() == 0);
    BSLS_ASSERT(object->numWeakReferences() == 0);

    object->resetCountsRaw(1, 0);

    buffer->buffer().reset(object->data(), object);
    buffer->setSize(NTCCFG_WARNING_NARROW(int, d_blobBufferSize));
}

void BlobBufferPool::release(BlobBufferPoolObject* object)
{
    BSLS_ASSERT(object);
    BSLS_ASSERT(object->data() != 0);
    BSLS_ASSERT(object->next() == 0);
    BSLS_ASSERT(object->numReferences() == 0);
    BSLS_ASSERT(object->numWeakReferences() == 0);

    Shard* shard = &d_shards.d_shard[object->shard()];

    BlobBufferPool::push(shard, object, object);

    shard->d_numAllocated.subtractRelaxed(1);
}

void BlobBufferPool::reserve(bsl::size_t numObjects)
{
    // Carve the objects under the lock, linking them into one chain per
    // shard, then push each chain with a single operation, distributing the
    // reserved objects evenly across the shards.

    BlobBufferPoolObject* first[k_NUM_SHARDS] = {};
    BlobBufferPoolObject* last[k_NUM_SHARDS]  = {};

    {
        LockGuard guard(&d_mutex);

        for (bsl::size_t i = 0; i < numObjects; ++i) {
            BlobBufferPoolObject* object = this->carve();

            const bsl::size_t index = i & (k_NUM_SHARDS - 1);
            object->setNext(first[index] != 0 ? first[index]->index() : 0);
            first[index] = object;
            if (last[index] == 0) {
                last[index] = object;
            }
        }
    }

    for (bsl::size_t index = 0; index < k_NUM_SHARDS; ++index) {
        if (first[index] != 0) {
            BlobBufferPool::push(&d_shards.d_shard[index],
                                 first[index],
                                 last[index]);
        }
    }
}

bsl::size_t BlobBufferPool::numBuffersAllocated() const
{
    // Each allocation and its release are counted on the same shard, but the
    // shards are not read atomically as a whole, so the sum may be
    // transiently inconsistent while other threads allocate and release
    // concurrently.

    bsls::Types::Int64 numAllocated = 0;
    for (bsl::size_t index = 0; index < k_NUM_SHARDS; ++index) {
        numAllocated += d_shards.d_shard[index].d_numAllocated.loadRelaxed();
    }

    if (numAllocated > 0) {
        return NTCCFG_WARNING_NARROW(bsl::size_t, numAllocated);
    }
    else {
        return 0;
    }
}

bsl::size_t BlobBufferPool::numBuffersAvailable() const
{
    const bsl::uint64_t numPooled    = d_numPooled.loadRelaxed();
    const bsl::uint64_t numAllocated = this->numBuffersAllocated();

    if (numPooled > numAllocated) {
        return NTCCFG_WARNING_NARROW(bsl::size_t, numPooled - numAllocated);
    }
    else {
        return 0;
    }
}

bsl::size_t BlobBufferPool::numBuffersPooled() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numPooled.loadRelaxed());
}

bsl::size_t BlobBufferPool::numBytesInUse() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t,
                                 d_countingAllocator.numBytesInUse());
}

}  // close package namespace
}  // close enterprise namespace
