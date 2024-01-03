// Copyright 2023 Bloomberg Finance L.P.
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

#include <ntcq_zerocopy.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcq_zerocopy_cpp, "$Id$ $CSID$")

#include <ntci_log.h>

namespace BloombergLP {
namespace ntcq {

ZeroCopyEntry::ZeroCopyEntry(bslma::Allocator* basicAllocator)
: d_id(0)
, d_data_sp()
, d_event()
, d_callbacks(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_event.type() == ntca::SendEventType::e_COMPLETE);
}

ZeroCopyEntry::ZeroCopyEntry(const ZeroCopyEntry& other,
                             bslma::Allocator*    basicAllocator)
: d_id(other.d_id)
, d_data_sp(other.d_data_sp)
, d_event(other.d_event)
, d_callbacks(other.d_callbacks, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ZeroCopyEntry::~ZeroCopyEntry()
{
}

void ZeroCopyEntry::setId(const bsl::uint32_t id)
{
    d_id = id;
}

void ZeroCopyEntry::setData(const bsl::shared_ptr<ntsa::Data>& data)
{
    d_data_sp = data;
}

void ZeroCopyEntry::setError(const ntsa::Error& error)
{
    ntca::SendContext context;
    context.setError(error);

    d_event.setType(ntca::SendEventType::e_ERROR);
    d_event.setContext(context);
}

void ZeroCopyEntry::addCallback(const ntci::SendCallback& callback)
{
    d_callbacks.push_back(callback);
}

void ZeroCopyEntry::dispatch(const bsl::shared_ptr<ntci::Sender>&   sender,
                             const bsl::shared_ptr<ntci::Strand>&   strand,
                             const bsl::shared_ptr<ntci::Executor>& executor,
                             bool                                   defer,
                             bslmt::Mutex*                          mutex)
{
    typedef CallbackVector::const_iterator CallbackIterator;

    CallbackVector callbacks(d_allocator_p);
    callbacks.swap(d_callbacks);

    CallbackIterator       it = callbacks.cbegin();
    const CallbackIterator et = callbacks.cend();

    for (; it != et; ++it) {
        const ntci::SendCallback& callback = *it;

        if (callback) {
            callback.dispatch(sender, d_event, strand, executor, defer, mutex);
        }
    }
}

bsl::uint32_t ZeroCopyEntry::id() const
{
    return d_id;
}

const ntca::SendContext& ZeroCopyEntry::context() const
{
    return d_event.context();
}

bslma::Allocator* ZeroCopyEntry::allocator() const
{
    return d_allocator_p;
}

ZeroCopyWaitList::ZeroCopyWaitList(bslma::Allocator* basicAllocator)
: d_entryList(basicAllocator)
, d_strand()
, d_nextId(0)
{
}

ZeroCopyWaitList::~ZeroCopyWaitList()
{
    BSLS_ASSERT(d_entryList.empty());
}

void ZeroCopyWaitList::setStrand(const bsl::shared_ptr<ntci::Strand>& strand)
{
    d_strand = strand;
}

void ZeroCopyWaitList::addEntry(ZeroCopyEntry& entry)
{
    BSLS_ASSERT(!cancelled);

    entry.setId(d_nextId++);

    d_entryList.push_back(entry);
}

bool ZeroCopyWaitList::zeroCopyAcknowledge(
    const ntsa::ZeroCopy&                  zc,
    const bsl::shared_ptr<ntci::Sender>&   sender,
    const bsl::shared_ptr<ntci::Executor>& executor)
{
    const bsl::uint32_t from = zc.from();
    const bsl::uint32_t to   = zc.to();

    const bool overflow = from > to;

    const bsl::uint32_t acknowledged =
        (!overflow ? (to - from)
                   : (bsl::numeric_limits<bsl::uint32_t>::max() - from + to)) +
        1;

    BSLS_ASSERT_OPT(acknowledged > 0);

    bsl::uint32_t                   matched = 0;
    const EntryList::const_iterator end     = d_entryList.cend();
    EntryList::iterator             it      = d_entryList.begin();
    while ((matched < acknowledged) && (it != end)) {
        ntcq::ZeroCopyEntry& entry = *it;

        bool match = false;
        if (!overflow) {
            if (entry.id() >= from && entry.id() <= to) {
                match = true;
            }
        }
        else {
            if (entry.id() >= from || entry.id() <= to) {
                match = true;
            }
        }

        matched += static_cast<bsl::uint32_t>(match);

        if (match) {
            entry.dispatch(sender,
                           d_strand,
                           executor,
                           true,
                           0);
            it = d_entryList.erase(it);
        }
        else {
            ++it;
        }
    }
    BSLS_ASSERT_OPT(matched == acknowledged);
    return acknowledged > 0;
}

void ZeroCopyWaitList::cancelWait(
    const bsl::shared_ptr<ntci::Sender>&   sender,
    const bsl::shared_ptr<ntci::Executor>& executor)
{
    const EntryList::const_iterator end = d_entryList.cend();
    EntryList::iterator             it  = d_entryList.begin();
    while (it != end) {
        ntcq::ZeroCopyEntry& entry = *it;
        entry.setError(ntsa::Error(ntsa::Error::e_CANCELLED));
        entry.dispatch(sender,
                       d_strand,
                       executor,
                       true,
                       0);
        ++it;
    }
    d_entryList.clear();
    cancelled = true;
}

}  // close package namespace
}  // close enterprise namespace
