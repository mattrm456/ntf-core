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

    /// The outgoing blob buffer factory.
    bdlbb::PooledBlobBufferFactory d_outgoingBlobBufferFactory;

    /// The incoming blob buffer factory.
    bdlbb::PooledBlobBufferFactory d_incomingBlobBufferFactory;

    /// The shared object pool of outgoing packets.
    Pool d_outgoingPacketPool;

    /// The shared object pool of incoming packets.
    Pool d_incomingPacketPool;

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
    /// Enumerates the constants used by this implementation.
    enum Constants {
        /// The default maximum transmission unit.
        k_MTU = 1500
    };

    /// Create a new packet pool. Allocate blob buffers of the specified
    /// 'outgoingBlobBufferSize' intended for outgoing data. Allocate blob
    /// buffers of the specified 'incomingBlobBufferSize' intended for incoming
    /// data. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    PacketPool(bsl::size_t       outgoingBlobBufferSize,
               bsl::size_t       incomingBlobBufferSize,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~PacketPool() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a packet suitable to enqueue to to the
    /// associated device.
    void createOutgoingPacket(bsl::shared_ptr<ntsa::Packet>* result)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a packet suitable to dequeue from the
    /// associated device.
    void createIncomingPacket(bsl::shared_ptr<ntsa::Packet>* result)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a blob buffer suitable to enqueue to
    /// to the associated device.
    void createOutgoingBlobBuffer(bdlbb::BlobBuffer* result)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a blob buffer suitable to dequeue from
    /// the associated device.
    void createIncomingBlobBuffer(bdlbb::BlobBuffer* result)
        BSLS_KEYWORD_OVERRIDE;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(PacketPool);
};

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
