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

#ifndef INCLUDED_NTSA_PACKETQUEUE
#define INCLUDED_NTSA_PACKETQUEUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_packet.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlcc_fixedqueue.h>
#include <bsls_assert.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a packet queue.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_protocol
class PacketQueue
{
    /// The queue container.
    bdlcc::FixedQueue<bsl::shared_ptr<ntsa::Packet> > d_queue;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

  private:
    PacketQueue(const PacketQueue&);
    PacketQueue& operator=(const PacketQueue&);

  public:
    /// Create a new packet queue with the specified maximum 'capacity'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit PacketQueue(bsl::size_t       capacity,
                         bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~PacketQueue();

    /// Enqueue the specified 'packet' to the queue. Return the error.
    ntsa::Error enqueue(const bsl::shared_ptr<ntsa::Packet>& packet);

    /// Enqueue the specified 'packet' to the queue. Return the error.
    ntsa::Error enqueue(
        bslmf::MovableRef<bsl::shared_ptr<ntsa::Packet> > packet);

    /// Dequeue the next available packet and load it into the specified
    /// 'result'.
    ntsa::Error dequeue(bsl::shared_ptr<ntsa::Packet>* result);

    /// Disable enqueing packets from the queue and unblock any thread blocked
    /// on dequeuing a packet.
    void shutdown();

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(PacketQueue);
};

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
