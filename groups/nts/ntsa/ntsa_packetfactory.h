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
class PacketFactory
{
  public:
    /// Destroy this object.
    virtual ~PacketFactory();

    /// Load into the specified 'result' a packet suitable to enqueue to to the
    /// associated device.
    virtual void createOutgoingPacket(
        bsl::shared_ptr<ntsa::Packet>* result) = 0;

    /// Load into the specified 'result' a packet suitable to dequeue from the
    /// associated device.
    virtual void createIncomingPacket(
        bsl::shared_ptr<ntsa::Packet>* result) = 0;

    /// Load into the specified 'result' a blob buffer suitable to enqueue to
    /// to the associated device.
    virtual void createOutgoingBlobBuffer(bdlbb::BlobBuffer* result) = 0;

    /// Load into the specified 'result' a blob buffer suitable to dequeue from
    /// the associated device.
    virtual void createIncomingBlobBuffer(bdlbb::BlobBuffer* result) = 0;
};

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
