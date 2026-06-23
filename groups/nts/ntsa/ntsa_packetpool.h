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

#ifndef INCLUDED_NTSA_PACKETPOOL
#define INCLUDED_NTSA_PACKETPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_packet.h>
#include <ntsa_packetfactory.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlcc_sharedobjectpool.h>
#include <bsls_assert.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a packet pool.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_protocol
class PacketPool : public ntsa::PacketFactory
{
    /// Defines a type alias for a shared object pool of packets.
    typedef bdlcc::SharedObjectPool<
        ntsa::Packet,
        bdlcc::ObjectPoolFunctors::DefaultCreator,
        bdlcc::ObjectPoolFunctors::Reset<ntsa::Packet> >
        Pool;

    /// The blob buffer factory.
    bdlbb::PooledBlobBufferFactory d_blobBufferFactory;

    /// The shared object pool of packets.
    Pool d_objectPool;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

  private:
    PacketPool(const PacketPool&);
    PacketPool& operator=(const PacketPool&);

  private:
    /// Construct a new packet at the specified 'address' using the specified
    /// 'blobBufferFactory' to allocate blob buffers and the specified
    /// 'allocator' to supply memory.
    static void construct(void*                     address,
                          bdlbb::BlobBufferFactory* blobBufferFactory,
                          bslma::Allocator*         allocator);

  public:
    /// Create a new packet pool. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit PacketPool(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~PacketPool() BSLS_KEYWORD_OVERRIDE;

    /// Return a newly allocated block of memory of (at least) the specified
    /// positive 'size' (in bytes).  If 'size' is 0, a null pointer is
    /// returned with no other effect.  If this allocator cannot return the
    /// requested number of bytes, then it will throw a 'std::bad_alloc'
    /// exception in an exception-enabled build, or else will abort the
    /// program in a non-exception build.  The behavior is undefined unless
    /// '0 <= size'.  Note that the alignment of the address returned
    /// conforms to the platform requirement for any object of the specified
    /// 'size'.  Note that this virtual function hides a two-parameter
    /// non-virtual 'allocate' method inherited from 'bsl::memory_resource';
    /// to access the inherited function, upcast the object to
    /// 'bsl::memory_resource&' before calling the base-class function.
    void* allocate(size_type size) BSLS_KEYWORD_OVERRIDE;

    /// Return the memory block at the specified 'address' back to this
    /// allocator.  If 'address' is 0, this function has no effect.  The
    /// behavior is undefined unless 'address' was allocated using this
    /// allocator object and has not already been deallocated.  Note that
    /// this virtual function hides a three-parameter, non-virtual 'deallocate'
    /// method inherited from 'bsl::memory_resource'; to access the
    /// inherited function, upcast the object to 'bsl::memory_resource&'
    /// before calling the base-class function.
    void deallocate(void* address) BSLS_KEYWORD_OVERRIDE;

    /// Allocate a blob buffer from this blob buffer factory, and load it
    /// into the specified 'buffer'.
    void allocate(bdlbb::BlobBuffer* buffer) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'packet' a shared pointer to an available
    /// packet in the pool having a default value. The resulting packet is
    /// automatically returned to this pool when its reference count reaches
    /// zero.
    void allocate(bsl::shared_ptr<ntsa::Packet>* packet) BSLS_KEYWORD_OVERRIDE;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(PacketPool);
};

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
