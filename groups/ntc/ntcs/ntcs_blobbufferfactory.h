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

#ifndef INCLUDED_NTCS_BLOBBUFFERFACTORY
#define INCLUDED_NTCS_BLOBBUFFERFACTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_metric.h>
#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bdlbb_blob.h>
#include <bdlma_aligningallocator.h>
#include <bdlma_concurrentpoolallocator.h>
#include <bdlma_countingallocator.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsl_memory.h>
#include <bsl_vector.h>
#include <bsl_typeinfo.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide statistics for the runtime behavior of a blob buffer pool.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class BlobBufferFactoryMetrics
: public ntci::Monitorable,
  public ntccfg::Shared<BlobBufferFactoryMetrics>
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                                   d_mutex;
    bsls::AtomicUint64                              d_numAllocated;
    bsls::AtomicUint64                              d_numAvailable;
    bsls::AtomicUint64                              d_numPooled;
    bsls::AtomicUint64                              d_numBytesInUse;
    bsl::string                                     d_prefix;
    bsl::string                                     d_objectName;
    bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics> d_parent_sp;
    bslma::Allocator*                               d_allocator_p;

    static const struct ntci::MetricMetadata STATISTICS[];

  private:
    BlobBufferFactoryMetrics(const BlobBufferFactoryMetrics&)
        BSLS_KEYWORD_DELETED;
    BlobBufferFactoryMetrics& operator=(const BlobBufferFactoryMetrics&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create new metrics for the specified 'objectName whose field names
    /// have the specified 'prefix'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    BlobBufferFactoryMetrics(const bslstl::StringRef& prefix,
                             const bslstl::StringRef& objectName,
                             bslma::Allocator*        basicAllocator = 0);

    /// Create new metrics for the specified 'objectName whose field names
    /// have the specified 'prefix'. Aggregate updates into the specified
    /// 'parent'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    BlobBufferFactoryMetrics(
        const bslstl::StringRef&                               prefix,
        const bslstl::StringRef&                               objectName,
        const bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics>& parent,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~BlobBufferFactoryMetrics() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the array of statistics from the
    /// specified 'snapshot' for this object based on the specified
    /// 'operation': if 'operation' is e_CUMULATIVE then the statistics are
    /// for the entire life of this object;  otherwise the statistics are
    /// for the period since the last call to this function. If 'operation'
    /// is e_INTERVAL_WITH_RESET then reset all internal measurements.  Note
    /// that 'result->theArray().length()' is expected to have the same
    /// value each time this function returns.
    void getStats(bdld::ManagedDatum* result) BSLS_KEYWORD_OVERRIDE;

    /// Return the prefix corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    const char* getFieldPrefix(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the field name corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    const char* getFieldName(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the field description corresponding to the field at the
    /// specified 'ordinal' position, or 0 if no field at the 'ordinal'
    /// position exists.
    const char* getFieldDescription(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the type of the statistic at the specified 'ordinal'
    /// position, or e_AVERAGE if no field at the 'ordinal' position exists
    /// or the type is unknown.
    ntci::Monitorable::StatisticType getFieldType(int ordinal) const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the flags that indicate which indexes to apply to the
    /// statistics measured by this monitorable object.
    int getFieldTags(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the ordinal of the specified 'fieldName', or a negative value
    /// if no field identified by 'fieldName' exists.
    int getFieldOrdinal(const char* fieldName) const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of elements in a datum resulting from
    /// a call to 'getStats()'.
    int numOrdinals() const BSLS_KEYWORD_OVERRIDE;

    /// Return the human-readable name of the monitorable object, or 0 or
    /// the empty string if no such human-readable name has been assigned to
    /// the monitorable object.
    const char* objectName() const BSLS_KEYWORD_OVERRIDE;

    /// Return the parent metrics object into which these metrics are
    /// aggregated, or null if no such parent object is defined.
    const bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics>& parent() const;

    /// Return the number of blob buffers that have been allocated and not
    /// returned to the pool.
    bsl::size_t numBuffersAllocated() const;

    /// Return the number of blob buffers that are pooled but not allocated.
    bsl::size_t numBuffersAvailable() const;

    /// Return the total number of blob buffers that have been pooled, that
    /// is, the sum of the number of blob buffers that have been allocated
    /// and the number of blob buffers available.
    bsl::size_t numBuffersPooled() const;

    /// Return the number of bytes allocated from the allocator supplied
    /// to this object at the time of its construction and not yet freed.
    bsl::size_t numBytesInUse() const;
};

/// @internal @brief
/// Provide a pool of blob buffers.
///
/// @details
/// The buffer statistics are tracked in terms of the observable behavior of
/// this object, and make no assumptions about the private implementation
/// details of the underlying memory pool (e.g., how many blocks it carves
/// from each chunk of memory it acquires). The number of buffers allocated
/// is the number of blob buffers currently outstanding. The number of
/// buffers pooled is the high-water mark of the number of buffers allocated:
/// the underlying pool never returns memory to its allocator before it is
/// destroyed, so at least that many buffers' worth of memory is retained for
/// reuse. The number of buffers available is the difference between the
/// two. The number of bytes in use is the actual number of bytes the
/// underlying pool has acquired from the allocator supplied at construction,
/// which may exceed 'numBuffersPooled() * blobBufferSize' because the pool
/// pre-allocates blocks in bulk.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class BlobBufferFactory : public bdlbb::BlobBufferFactory,
                          private bslma::Allocator
{
    bdlma::CountingAllocator       d_memoryPoolAllocator;
    bdlma::ConcurrentPoolAllocator d_memoryPool;
    bsl::size_t                    d_blobBufferSize;
    bsls::AtomicUint64             d_numAllocated;
    bsls::AtomicUint64             d_numPooled;
    bslma::Allocator*              d_allocator_p;

  private:
    BlobBufferFactory(const BlobBufferFactory&) BSLS_KEYWORD_DELETED;
    BlobBufferFactory& operator=(const BlobBufferFactory&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Return a newly allocated block of memory of (at least) the specified
    /// positive 'size' (in bytes).  If 'size' is 0, a null pointer is
    /// returned with no other effect.  If this allocator cannot return the
    /// requested number of bytes, then it will throw a 'bsl::bad_alloc'
    /// exception in an exception-enabled build, or else will abort the
    /// program in a non-exception build.  The behavior is undefined unless
    /// '0 <= size'.  Note that the alignment of the address returned
    /// conforms to the platform requirement for any object of the specified
    /// 'size'.
    void* allocate(size_type size) BSLS_KEYWORD_OVERRIDE;

    /// Return the memory block at the specified 'address' back to this
    /// allocator.  If 'address' is 0, this function has no effect.  The
    /// behavior is undefined unless 'address' was allocated using this
    /// allocator object and has not already been deallocated.
    void deallocate(void* address) BSLS_KEYWORD_OVERRIDE;

    /// Raise the high-water mark of the number of buffers pooled to the
    /// specified 'numAllocated', if 'numAllocated' is greater than the
    /// current high-water mark.
    void updateNumPooled(bsl::uint64_t numAllocated);

  public:
    /// Create a new blob buffer factory that allocates blob buffers each
    /// having the specified 'blobBufferSize'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit BlobBufferFactory(bsl::size_t       blobBufferSize,
                               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~BlobBufferFactory() BSLS_KEYWORD_OVERRIDE;

    /// Allocate a blob buffer from this blob buffer factory, and load it
    /// into the specified 'buffer'.
    void allocate(bdlbb::BlobBuffer* buffer) BSLS_KEYWORD_OVERRIDE;

    /// Return the number of blob buffers that have been allocated and not
    /// returned to the pool.
    bsl::size_t numBuffersAllocated() const;

    /// Return the number of blob buffers that are pooled but not allocated.
    bsl::size_t numBuffersAvailable() const;

    /// Return the total number of blob buffers that have been pooled, that
    /// is, the sum of the number of blob buffers that have been allocated
    /// and the number of blob buffers available.
    bsl::size_t numBuffersPooled() const;

    /// Return the number of bytes allocated from the allocator supplied
    /// to this object at the time of its construction and not yet freed.
    bsl::size_t numBytesInUse() const;
};

class BlobBufferPool;
class BlobBufferPoolObject;

/// @internal @brief
/// Provide utilities for manipulating a tagged index to a pooled blob
/// buffer.
///
/// @details
/// A tagged index is a 64-bit value whose low 32 bits identify an object
/// and whose high 32 bits are a tag that is changed on every update of the
/// head of a free list. The tag defends against the ABA problem: a stale
/// compare-and-swap succeeds only if the head refers to the same index and
/// the tag has wrapped around to the same value, which requires 2^32
/// intervening updates. An index of zero is the null index.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct BlobBufferPoolHandleUtil {
    /// Define a type alias for the raw tagged index type.
    typedef bsl::uint64_t RawType;

    /// Define a type alias for the index type.
    typedef bsl::uint32_t IndexType;

    /// Define a type alias for the tag type.
    typedef bsl::uint32_t TagType;

    /// Return the raw value that represents the specified 'index' and
    /// 'tag'.
    static RawType initialize(IndexType index, TagType tag);

    /// Load into the specified 'index' and 'tag' the index and tag
    /// represented by the specified 'raw' value.
    static void get(IndexType* index, TagType* tag, RawType raw);

    /// Return the index represented by the specified 'raw' value.
    static IndexType getIndex(RawType raw);

    /// Return the tag represented by the specified 'raw' value.
    static TagType getTag(RawType raw);

    /// Return the maximum index value.
    static IndexType maxIndex();

    /// Return the maximum tag value.
    static TagType maxTag();
};

NTCCFG_INLINE
BlobBufferPoolHandleUtil::RawType BlobBufferPoolHandleUtil::initialize(
    IndexType index,
    TagType   tag)
{
    return (static_cast<RawType>(tag) << 32) | static_cast<RawType>(index);
}

NTCCFG_INLINE
void BlobBufferPoolHandleUtil::get(IndexType* index, TagType* tag, RawType raw)
{
    *index = static_cast<IndexType>(raw & 0xFFFFFFFFULL);
    *tag   = static_cast<TagType>(raw >> 32);
}

NTCCFG_INLINE
BlobBufferPoolHandleUtil::IndexType BlobBufferPoolHandleUtil::getIndex(
    RawType raw)
{
    return static_cast<IndexType>(raw & 0xFFFFFFFFULL);
}

NTCCFG_INLINE
BlobBufferPoolHandleUtil::TagType BlobBufferPoolHandleUtil::getTag(
    RawType raw)
{
    return static_cast<TagType>(raw >> 32);
}

NTCCFG_INLINE
BlobBufferPoolHandleUtil::IndexType BlobBufferPoolHandleUtil::maxIndex()
{
    return 0xFFFFFFFFU;
}

NTCCFG_INLINE
BlobBufferPoolHandleUtil::TagType BlobBufferPoolHandleUtil::maxTag()
{
    return 0xFFFFFFFFU;
}

/// @internal @brief
/// Provide an atomic tagged index.
///
/// @details
/// This class implements an atomic tagged index, using only 64-bit atomic
/// operations, which are available on every supported platform. Operations
/// on objects of this class provide the sequential consistency memory
/// ordering guarantee unless explicitly qualified with a less strict
/// consistency guarantee suffix (i.e., Acquire, Release, AcqRel or
/// Relaxed).
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class BlobBufferPoolHandle
{
  public:
    /// Define a type alias for the utility.
    typedef ntcs::BlobBufferPoolHandleUtil Util;

    /// Define a type alias for the raw tagged index type.
    typedef Util::RawType RawType;

    /// Define a type alias for the index type.
    typedef Util::IndexType IndexType;

    /// Define a type alias for the tag type.
    typedef Util::TagType TagType;

  private:
    bsls::AtomicOperations::AtomicTypes::Uint64 d_value;

  private:
    BlobBufferPoolHandle(const BlobBufferPoolHandle&) BSLS_KEYWORD_DELETED;
    BlobBufferPoolHandle& operator=(const BlobBufferPoolHandle&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create an atomic tagged index having the null index and a tag of
    /// zero.
    BlobBufferPoolHandle();

    /// Create an atomic tagged index having the specified 'index' and
    /// 'tag'.
    BlobBufferPoolHandle(IndexType index, TagType tag);

    /// Destroy this object.
    ~BlobBufferPoolHandle();

    /// Atomically set the value of this object to the specified 'newIndex'
    /// and 'newTag', providing the sequential consistency memory ordering
    /// guarantee.
    void store(IndexType newIndex, TagType newTag);

    /// Atomically set the value of this object to the specified 'newIndex'
    /// and 'newTag', providing the relaxed memory ordering guarantee.
    void storeRelaxed(IndexType newIndex, TagType newTag);

    /// Atomically set the value of this object to the specified 'newIndex'
    /// and 'newTag', providing the release memory ordering guarantee.
    void storeRelease(IndexType newIndex, TagType newTag);

    /// Atomically set the value of this object to the specified 'newIndex'
    /// and 'newTag' and load into the specified 'previousIndex' and
    /// 'previousTag' the previous value. Perform the operation with the
    /// sequential consistency memory ordering guarantee.
    void swap(IndexType* previousIndex,
              TagType*   previousTag,
              IndexType  newIndex,
              TagType    newTag);

    /// Atomically set the value of this object to the specified 'newIndex'
    /// and 'newTag' and load into the specified 'previousIndex' and
    /// 'previousTag' the previous value. Perform the operation with the
    /// acquire/release memory ordering guarantee.
    void swapAcqRel(IndexType* previousIndex,
                    TagType*   previousTag,
                    IndexType  newIndex,
                    TagType    newTag);

    /// Compare the value of this object to the specified 'expectedIndex'
    /// and 'expectedTag'. If they are equal, set the value of this object
    /// to the specified 'newIndex' and 'newTag', otherwise leave this value
    /// unchanged. Load into the specified 'previousIndex' and
    /// 'previousTag' the previous value, whether or not the swap occurred,
    /// and return true if the swap occurred, otherwise return false. Note
    /// that the entire test-and-swap operation is performed atomically and
    /// provides the sequential consistency memory ordering guarantee.
    bool testAndSwap(IndexType* previousIndex,
                     TagType*   previousTag,
                     IndexType  expectedIndex,
                     TagType    expectedTag,
                     IndexType  newIndex,
                     TagType    newTag);

    /// Compare the value of this object to the specified 'expectedIndex'
    /// and 'expectedTag'. If they are equal, set the value of this object
    /// to the specified 'newIndex' and 'newTag', otherwise leave this value
    /// unchanged. Load into the specified 'previousIndex' and
    /// 'previousTag' the previous value, whether or not the swap occurred,
    /// and return true if the swap occurred, otherwise return false. Note
    /// that the entire test-and-swap operation is performed atomically and
    /// provides the acquire/release memory ordering guarantee.
    bool testAndSwapAcqRel(IndexType* previousIndex,
                           TagType*   previousTag,
                           IndexType  expectedIndex,
                           TagType    expectedTag,
                           IndexType  newIndex,
                           TagType    newTag);

    /// Load into the specified 'currentIndex' and 'currentTag' the current
    /// value of this object, providing the sequential consistency memory
    /// ordering guarantee.
    void load(IndexType* currentIndex, TagType* currentTag) const;

    /// Load into the specified 'currentIndex' and 'currentTag' the current
    /// value of this object, providing the relaxed memory ordering
    /// guarantee.
    void loadRelaxed(IndexType* currentIndex, TagType* currentTag) const;

    /// Load into the specified 'currentIndex' and 'currentTag' the current
    /// value of this object, providing the acquire memory ordering
    /// guarantee.
    void loadAcquire(IndexType* currentIndex, TagType* currentTag) const;

    /// Return the maximum tag value.
    static TagType maxTag();
};

NTCCFG_INLINE
BlobBufferPoolHandle::BlobBufferPoolHandle()
{
    bsls::AtomicOperations::initUint64(&d_value, 0);
}

NTCCFG_INLINE
BlobBufferPoolHandle::BlobBufferPoolHandle(IndexType index, TagType tag)
{
    bsls::AtomicOperations::initUint64(&d_value,
                                       Util::initialize(index, tag));
}

NTCCFG_INLINE
BlobBufferPoolHandle::~BlobBufferPoolHandle()
{
}

NTCCFG_INLINE
void BlobBufferPoolHandle::store(IndexType newIndex, TagType newTag)
{
    bsls::AtomicOperations::setUint64(&d_value,
                                      Util::initialize(newIndex, newTag));
}

NTCCFG_INLINE
void BlobBufferPoolHandle::storeRelaxed(IndexType newIndex, TagType newTag)
{
    bsls::AtomicOperations::setUint64Relaxed(
        &d_value,
        Util::initialize(newIndex, newTag));
}

NTCCFG_INLINE
void BlobBufferPoolHandle::storeRelease(IndexType newIndex, TagType newTag)
{
    bsls::AtomicOperations::setUint64Release(
        &d_value,
        Util::initialize(newIndex, newTag));
}

NTCCFG_INLINE
void BlobBufferPoolHandle::swap(IndexType* previousIndex,
                                TagType*   previousTag,
                                IndexType  newIndex,
                                TagType    newTag)
{
    const RawType previousRaw = bsls::AtomicOperations::swapUint64(
        &d_value,
        Util::initialize(newIndex, newTag));

    Util::get(previousIndex, previousTag, previousRaw);
}

NTCCFG_INLINE
void BlobBufferPoolHandle::swapAcqRel(IndexType* previousIndex,
                                      TagType*   previousTag,
                                      IndexType  newIndex,
                                      TagType    newTag)
{
    const RawType previousRaw = bsls::AtomicOperations::swapUint64AcqRel(
        &d_value,
        Util::initialize(newIndex, newTag));

    Util::get(previousIndex, previousTag, previousRaw);
}

NTCCFG_INLINE
bool BlobBufferPoolHandle::testAndSwap(IndexType* previousIndex,
                                       TagType*   previousTag,
                                       IndexType  expectedIndex,
                                       TagType    expectedTag,
                                       IndexType  newIndex,
                                       TagType    newTag)
{
    const RawType expectedRaw = Util::initialize(expectedIndex, expectedTag);

    const RawType previousRaw = bsls::AtomicOperations::testAndSwapUint64(
        &d_value,
        expectedRaw,
        Util::initialize(newIndex, newTag));

    Util::get(previousIndex, previousTag, previousRaw);

    return previousRaw == expectedRaw;
}

NTCCFG_INLINE
bool BlobBufferPoolHandle::testAndSwapAcqRel(IndexType* previousIndex,
                                             TagType*   previousTag,
                                             IndexType  expectedIndex,
                                             TagType    expectedTag,
                                             IndexType  newIndex,
                                             TagType    newTag)
{
    const RawType expectedRaw = Util::initialize(expectedIndex, expectedTag);

    const RawType previousRaw =
        bsls::AtomicOperations::testAndSwapUint64AcqRel(
            &d_value,
            expectedRaw,
            Util::initialize(newIndex, newTag));

    Util::get(previousIndex, previousTag, previousRaw);

    return previousRaw == expectedRaw;
}

NTCCFG_INLINE
void BlobBufferPoolHandle::load(IndexType* currentIndex,
                                TagType*   currentTag) const
{
    Util::get(currentIndex,
              currentTag,
              bsls::AtomicOperations::getUint64(&d_value));
}

NTCCFG_INLINE
void BlobBufferPoolHandle::loadRelaxed(IndexType* currentIndex,
                                       TagType*   currentTag) const
{
    Util::get(currentIndex,
              currentTag,
              bsls::AtomicOperations::getUint64Relaxed(&d_value));
}

NTCCFG_INLINE
void BlobBufferPoolHandle::loadAcquire(IndexType* currentIndex,
                                       TagType*   currentTag) const
{
    Util::get(currentIndex,
              currentTag,
              bsls::AtomicOperations::getUint64Acquire(&d_value));
}

NTCCFG_INLINE
BlobBufferPoolHandle::TagType BlobBufferPoolHandle::maxTag()
{
    return Util::maxTag();
}

/// @internal @brief Provide padding to ensure the correct size of a blob
/// buffer pool object.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
struct BlobBufferPoolObjectPadding {
    BlobBufferPoolObjectPadding()
    {
        bsl::memset(this->zero, 0, sizeof this->zero);
    }

#if defined(BSLS_PLATFORM_CPU_64_BIT)
    bsl::uint8_t zero[20];
#elif defined(BSLS_PLATFORM_CPU_32_BIT)
    bsl::uint8_t zero[32];
#else
#error Not implemented
#endif
};

/// @internal @brief
/// Provide an intrusively linked-list of blob buffers implementing a shared
/// pointer representation.
///
/// @details
/// Each object is the header of a contiguous region of memory: the object
/// itself occupies the first 64 bytes and the blob buffer data immediately
/// follows it. Each object is identified by a non-zero index, and objects
/// are linked into free lists by index rather than by address. The link to
/// the next object is atomic because a thread popping an object from a free
/// list may read the link while another thread that has already popped the
/// same object overwrites it; the tagged head of the free list detects that
/// case and rejects the stale pop, but the read itself must not be a data
/// race.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class BlobBufferPoolObject : public bslma::SharedPtrRep
{
    char*                       d_data_p;
    BlobBufferPool*             d_pool_p;
    bsls::AtomicUint            d_next;
    bsl::uint32_t               d_index;
    bsl::uint32_t               d_shard;
    BlobBufferPoolObjectPadding d_padding;

  private:
    BlobBufferPoolObject(const BlobBufferPoolObject&) BSLS_KEYWORD_DELETED;
    BlobBufferPoolObject& operator=(const BlobBufferPoolObject&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new blob buffer pool object identified by the specified
    /// non-zero 'index' that belongs to the specified 'pool'.
    BlobBufferPoolObject(BlobBufferPool* pool, bsl::uint32_t index);

    /// Destroy this object.
    ~BlobBufferPoolObject() BSLS_KEYWORD_OVERRIDE;

    /// Set the blob buffer data to which this object manages to the
    /// specified 'data'.
    void setData(char* data);

    /// Set the index of the next object in linked list to the specified
    /// 'next' index, or zero if this object is the last in the list.
    void setNext(bsl::uint32_t next);

    /// Set the index of the free list to which this object is returned
    /// when released to the specified 'shard'.
    void setShard(bsl::uint32_t shard);

    /// Destroy the object referred to by this representation.  This method
    /// is invoked by 'releaseRef' when the number of shared references
    /// reaches zero and should not be explicitly invoked otherwise.
    void disposeObject() BSLS_KEYWORD_OVERRIDE;

    /// Destroy this representation object and deallocate the associated
    /// memory.  This method is invoked by 'releaseRef' and 'releaseWeakRef'
    /// when the number of weak references and the number of shared
    /// references both reach zero and should not be explicitly invoked
    /// otherwise.  The behavior is undefined unless 'disposeObject' has
    /// already been called for this representation.  Note that this method
    /// effectively serves as the representation object's destructor.
    void disposeRep() BSLS_KEYWORD_OVERRIDE;

    /// Return a pointer to the deleter stored by the derived representation
    /// if the deleter has the same type as that described by the specified
    /// 'type', and a null pointer otherwise.
    void* getDeleter(const bsl::type_info& type) BSLS_KEYWORD_OVERRIDE;

    /// Return the blob buffer data to which this object manages.
    char* data() const;

    /// Return the index of the next object in linked list, or zero if this
    /// object is the last in the list.
    bsl::uint32_t next() const;

    /// Return the index that identifies this object.
    bsl::uint32_t index() const;

    /// Return the index of the free list to which this object is returned
    /// when released.
    bsl::uint32_t shard() const;

    /// Return the (untyped) address of the modifiable shared object to
    /// which this object refers.
    void* originalPtr() const BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide a pool of blob buffers.
///
/// @details
/// This class implements a pool of blob buffers as a set of lock-free,
/// intrusive free lists ("shards"), each headed by a tagged index. Each
/// thread is assigned a home shard the first time it uses any pool. A
/// buffer is allocated by popping from the home shard, then, if the home
/// shard is empty, by popping from the other shards, and only if every
/// shard is empty by carving a new buffer from the memory pool. A buffer is
/// released by pushing it back onto the home shard of the thread that
/// allocated it, so buffers flow back to the threads that need them, and
/// releasing a buffer requires no thread-local lookup. Each shard occupies
/// its own cache line, so threads with different home shards do not
/// contend.
///
/// Memory is acquired in slabs that grow geometrically up to a maximum size,
/// and buffers are carved from the current slab one at a time, on demand,
/// so the number of buffers pooled counts only buffers that have actually
/// been needed. Memory is not returned to the allocator supplied at
/// construction until this object is destroyed.
///
/// Buffers are identified by index, starting at one, in the order they are
/// carved. Slab 'i' holds '2^i' buffers for 'i <= L', and every later slab
/// holds '2^L' buffers, where '2^L' is the maximum number of buffers per
/// slab, so the slab and offset of an index are computed directly. The
/// addresses of the slabs are kept in a table that is replaced, but never
/// freed, when it grows, so threads may read the table without locking.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class BlobBufferPool : public bdlbb::BlobBufferFactory
{
    /// Enumerate the constants used in the implementation.
    enum Constants {
        /// The assumed maximum size of a cache line, which is 128 bytes on
        /// some platforms (e.g. Apple Silicon) and which also accounts for
        /// adjacent-line prefetching on others.
        k_CACHE_LINE_SIZE = 128,

        /// The alignment of each blob buffer pool object. Objects are
        /// aligned to the cache line size so that the header of one object
        /// never shares a cache line with the data of its neighbor, which
        /// may be in use by a different thread.
        k_ALIGNMENT = k_CACHE_LINE_SIZE,

        /// The number of free lists. Must be a power of two.
        k_NUM_SHARDS = 16,

        /// The maximum number of objects carved from a single slab. Must be
        /// a power of two.
        k_MAX_SLAB_OBJECTS = 64,

        /// The maximum number of bytes in a single slab, unless a single
        /// object is larger.
        k_MAX_SLAB_SIZE = 1024 * 1024,

        /// The initial capacity of the slab table.
        k_INITIAL_SLAB_TABLE_CAPACITY = 16
    };

    /// Define a type alias for a tagged index to the head of a free list.
    typedef ntcs::BlobBufferPoolHandle Handle;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    /// Describe a free list and the number of buffers allocated by the
    /// threads whose home is this free list and not yet released, padded
    /// to occupy a cache line.
    struct Shard {
        Handle             d_head;
        bsls::AtomicInt64  d_numAllocated;
        char               d_padding[k_CACHE_LINE_SIZE - sizeof(Handle) -
                                     sizeof(bsls::AtomicInt64)];
    };

    /// Describe the array of shards, padded on both sides so that no shard
    /// shares a cache line with the fields read by every allocation or
    /// written by the slow path.
    struct ShardArray {
        char  d_leadingPadding[k_CACHE_LINE_SIZE];
        Shard d_shard[k_NUM_SHARDS];
        char  d_trailingPadding[k_CACHE_LINE_SIZE];
    };

    bsl::size_t                d_blobBufferSize;
    bsl::size_t                d_objectSize;
    bsl::size_t                d_slabShift;
    bsls::AtomicPointer<char*> d_slabTable;
    ShardArray                 d_shards;
    Mutex                      d_mutex;
    bsl::size_t                d_slabTableCapacity;
    bsl::size_t                d_numSlabs;
    bsl::uint32_t              d_numCarved;
    bsls::AtomicUint64         d_numPooled;
    bdlma::CountingAllocator   d_countingAllocator;
    bdlma::AligningAllocator   d_aligningAllocator;
    bsl::vector<char**>        d_slabTables;
    bslma::Allocator*          d_allocator_p;

  private:
    BlobBufferPool(const BlobBufferPool&) BSLS_KEYWORD_DELETED;
    BlobBufferPool& operator=(const BlobBufferPool&) BSLS_KEYWORD_DELETED;

  private:
    /// Return the index of the home shard of the calling thread.
    static bsl::size_t homeShard();

    /// Load into the specified 'slab' and 'offset' the index of the slab
    /// and the position within that slab of the object identified by the
    /// specified non-zero 'index'.
    void locate(bsl::size_t* slab, bsl::size_t* offset, bsl::uint32_t index)
        const;

    /// Return the object identified by the specified non-zero 'index'. The
    /// behavior is undefined unless the object has been carved and its
    /// index was observed after it was carved.
    BlobBufferPoolObject* lookup(bsl::uint32_t index) const;

    /// Pop an object from the specified 'shard' and return it, or return
    /// null if 'shard' is empty.
    BlobBufferPoolObject* pop(Shard* shard) const;

    /// Push the chain of objects from the specified 'first' through the
    /// specified 'last', linked by their 'next' pointers, onto the
    /// specified 'shard'.
    static void push(Shard*                shard,
                     BlobBufferPoolObject* first,
                     BlobBufferPoolObject* last);

    /// Construct a new object in memory carved from the current slab,
    /// acquiring a new slab, and growing the slab table, if necessary, and
    /// return it. The behavior is undefined unless 'd_mutex' is locked.
    BlobBufferPoolObject* carve();

    /// Return a new object carved from the memory pool.
    BlobBufferPoolObject* replenish();

  public:
    /// Create a new blob buffer pool that allocates blob buffers each
    /// having the specified 'blobBufferSize'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit BlobBufferPool(bsl::size_t       blobBufferSize,
                            bslma::Allocator* basicAllocator = 0);

    /// Destroy this object. The behavior is undefined unless every blob
    /// buffer allocated from this object has been released.
    ~BlobBufferPool() BSLS_KEYWORD_OVERRIDE;

    /// Allocate a blob buffer from this blob buffer factory, and load it
    /// into the specified 'buffer'.
    void allocate(bdlbb::BlobBuffer* buffer) BSLS_KEYWORD_OVERRIDE;

    /// Return the specified 'object' to the pool.
    void release(BlobBufferPoolObject* object);

    /// Reserve the specified 'numObjects' to be available in the pool.
    void reserve(bsl::size_t numObjects);

    /// Return the number of blob buffers that have been allocated and not
    /// returned to the pool.
    bsl::size_t numBuffersAllocated() const;

    /// Return the number of blob buffers that are pooled but not allocated.
    bsl::size_t numBuffersAvailable() const;

    /// Return the total number of blob buffers that have been pooled, that
    /// is, the sum of the number of blob buffers that have been allocated
    /// and the number of blob buffers available.
    bsl::size_t numBuffersPooled() const;

    /// Return the number of bytes allocated from the allocator supplied
    /// to this object at the time of its construction and not yet freed.
    bsl::size_t numBytesInUse() const;
};

NTCCFG_INLINE
BlobBufferPoolObject::~BlobBufferPoolObject()
{
}

NTCCFG_INLINE
void BlobBufferPoolObject::setData(char* data)
{
    d_data_p = data;
}

NTCCFG_INLINE
void BlobBufferPoolObject::setNext(bsl::uint32_t next)
{
    d_next.storeRelaxed(next);
}

NTCCFG_INLINE
void BlobBufferPoolObject::setShard(bsl::uint32_t shard)
{
    d_shard = shard;
}

NTCCFG_INLINE
void BlobBufferPoolObject::disposeObject()
{
}

NTCCFG_INLINE
void BlobBufferPoolObject::disposeRep()
{
    d_pool_p->release(this);
}

NTCCFG_INLINE
void* BlobBufferPoolObject::getDeleter(const bsl::type_info& type)
{
    NTCCFG_WARNING_UNUSED(type);
    return 0;
}

NTCCFG_INLINE
char* BlobBufferPoolObject::data() const
{
    return d_data_p;
}

NTCCFG_INLINE
bsl::uint32_t BlobBufferPoolObject::next() const
{
    return d_next.loadRelaxed();
}

NTCCFG_INLINE
bsl::uint32_t BlobBufferPoolObject::index() const
{
    return d_index;
}

NTCCFG_INLINE
bsl::uint32_t BlobBufferPoolObject::shard() const
{
    return d_shard;
}

NTCCFG_INLINE
void* BlobBufferPoolObject::originalPtr() const
{
    return (void*)(d_data_p);
}

}  // close package namespace
}  // close enterprise namespace
#endif
