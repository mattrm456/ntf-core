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

#include <ntsa_packetqueue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_packetqueue_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsa {

PacketQueue::PacketQueue(bsl::size_t       capacity,
                         bslma::Allocator* basicAllocator)
: d_queue(capacity, basicAllocator)
, d_allocator_p(basicAllocator)
{
    d_queue.enable();
}

PacketQueue::~PacketQueue()
{
}

void PacketQueue::shutdown()
{
    d_queue.tryPushBack(bsl::shared_ptr<ntsa::Packet>());
    d_queue.disable();
}

ntsa::Error PacketQueue::enqueuePacket(
    const bsl::shared_ptr<ntsa::Packet>& packet)
{
    int rc = d_queue.tryPushBack(packet);
    if (rc != 0) {
        if (d_queue.isEnabled()) {
            return ntsa::Error(ntsa::Error::e_LIMIT);
        }
        else {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
    }

    return ntsa::Error();
}

ntsa::Error PacketQueue::enqueuePacket(
    bslmf::MovableRef<bsl::shared_ptr<ntsa::Packet> > packet)
{
    int rc = d_queue.tryPushBack(NTSCFG_MOVE(packet));
    if (rc != 0) {
        if (d_queue.isEnabled()) {
            return ntsa::Error(ntsa::Error::e_LIMIT);
        }
        else {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
    }

    return ntsa::Error();
}

ntsa::Error PacketQueue::dequeuePacket(bsl::shared_ptr<ntsa::Packet>* result)
{
    result->reset();

    d_queue.popFront(result);
    if (!result->get() || (*result)->isUndefined()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
