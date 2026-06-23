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

#ifndef INCLUDED_NTSA_PACKETFACTORY
#define INCLUDED_NTSA_PACKETFACTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_packet.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bslma_allocator.h>
#include <bsls_assert.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a packet factory.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_protocol
class PacketFactory : public bdlbb::BlobBufferFactory, public bslma::Allocator
{
  public:
    /// Destroy this object.
    ~PacketFactory() BSLS_KEYWORD_OVERRIDE;

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
    virtual void* allocate(size_type size) BSLS_KEYWORD_OVERRIDE = 0;

    /// Return the memory block at the specified 'address' back to this
    /// allocator.  If 'address' is 0, this function has no effect.  The
    /// behavior is undefined unless 'address' was allocated using this
    /// allocator object and has not already been deallocated.  Note that
    /// this virtual function hides a three-parameter, non-virtual 'deallocate'
    /// method inherited from 'bsl::memory_resource'; to access the
    /// inherited function, upcast the object to 'bsl::memory_resource&'
    /// before calling the base-class function.
    void deallocate(void* address) BSLS_KEYWORD_OVERRIDE = 0;

    /// Allocate a blob buffer from this blob buffer factory, and load it
    /// into the specified 'buffer'.
    void allocate(bdlbb::BlobBuffer* buffer) BSLS_KEYWORD_OVERRIDE = 0;

    /// Load into the specified 'result' a new packet.
    virtual void allocate(bsl::shared_ptr<ntsa::Packet>* result) = 0;
};

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
