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

PacketPool::PacketPool(bslma::Allocator* basicAllocator)
: d_blobBufferFactory(1500, basicAllocator)
, d_objectPool(bdlf::BindUtil::bind(&PacketPool::construct,
                                    bdlf::PlaceHolders::_1,
                                    &d_blobBufferFactory,
                                    bdlf::PlaceHolders::_2),
               64,
               basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

PacketPool::~PacketPool()
{
}

void* PacketPool::allocate(size_type size)
{
    return d_allocator_p->allocate(size);
}

void PacketPool::deallocate(void* address)
{
    d_allocator_p->deallocate(address);
}

void PacketPool::allocate(bdlbb::BlobBuffer* buffer)
{
    d_blobBufferFactory.allocate(buffer);
}

void PacketPool::allocate(bsl::shared_ptr<ntsa::Packet>* packet)
{
    *packet = d_objectPool.getObject();
}

}  // close package namespace
}  // close enterprise namespace
