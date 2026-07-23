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

#include <ntsa_packetpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_packetpool_cpp, "$Id$ $CSID$")

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

namespace BloombergLP {
namespace ntsa {

void PacketPool::construct(void*                     address,
                           bdlbb::BlobBufferFactory* blobBufferFactory,
                           bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (address) ntsa::Packet(allocator);
}

PacketPool::PacketPool(bsl::size_t       outgoingBlobBufferSize,
                       bsl::size_t       incomingBlobBufferSize,
                       bslma::Allocator* basicAllocator)
: d_outgoingBlobBufferFactory(static_cast<int>(outgoingBlobBufferSize),
                              basicAllocator)
, d_incomingBlobBufferFactory(static_cast<int>(incomingBlobBufferSize),
                              basicAllocator)
, d_outgoingPacketPool(bdlf::BindUtil::bind(&PacketPool::construct,
                                            bdlf::PlaceHolders::_1,
                                            &d_outgoingBlobBufferFactory,
                                            bdlf::PlaceHolders::_2),
                       64,
                       basicAllocator)
, d_incomingPacketPool(bdlf::BindUtil::bind(&PacketPool::construct,
                                            bdlf::PlaceHolders::_1,
                                            &d_incomingBlobBufferFactory,
                                            bdlf::PlaceHolders::_2),
                       1,
                       basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

PacketPool::~PacketPool()
{
}

void PacketPool::createOutgoingPacket(bsl::shared_ptr<ntsa::Packet>* result)
{
    *result = d_outgoingPacketPool.getObject();
}

void PacketPool::createIncomingPacket(bsl::shared_ptr<ntsa::Packet>* result)
{
    *result = d_incomingPacketPool.getObject();
}

void PacketPool::createOutgoingBlobBuffer(bdlbb::BlobBuffer* result)
{
    d_outgoingBlobBufferFactory.allocate(result);
}

void PacketPool::createIncomingBlobBuffer(bdlbb::BlobBuffer* result)
{
    d_incomingBlobBufferFactory.allocate(result);
}

}  // close package namespace
}  // close enterprise namespace
