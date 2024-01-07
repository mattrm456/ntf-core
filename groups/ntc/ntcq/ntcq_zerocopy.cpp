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
#include <bslim_printer.h>

namespace BloombergLP {
namespace ntcq {

bsl::ostream& ZeroCopyRange::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_minCounter == d_maxCounter) {
        printer.printValue("EMPTY");
    }
    else {
        printer.printAttribute("min", d_minCounter);
        printer.printAttribute("max", d_maxCounter);
    }

    printer.end();
    return stream;
}

bsl::ostream& ZeroCopyEntry::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("group", d_group);
    printer.printAttribute("range", d_range);

    if (this->complete()) {
        printer.printAttribute("state", "COMPLETE");
    }
    else {
        printer.printAttribute("state", "WAITING");
    }

    printer.end();
    return stream;
}

ZeroCopyQueue::ZeroCopyQueue(
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
: d_counter(0)
, d_bias(0)
, d_waitList(basicAllocator)
, d_doneList(basicAllocator)
, d_dataPool_sp(dataPool)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ZeroCopyQueue::~ZeroCopyQueue()
{
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter  group,
                                          const bdlbb::Blob& data)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_counter++;

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    dataContainer->makeBlob(data);

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(dataContainer);

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter         group,
                                          const bdlbb::Blob&        data, 
                                          const ntci::SendCallback& callback)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_counter++;

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    dataContainer->makeBlob(data);

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(dataContainer);

    if (callback) {
        entry.setCallback(callback);
    }

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter group, 
                                          const ntsa::Data& data)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_counter++;

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    *dataContainer = data;

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(dataContainer);

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter         group, 
                                          const ntsa::Data&         data, 
                                          const ntci::SendCallback& callback)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_counter++;

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    *dataContainer = data;

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(dataContainer);

    if (callback) {
        entry.setCallback(callback);
    }

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(
    ntcq::SendCounter                  group, 
    const bsl::shared_ptr<ntsa::Data>& data)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_counter++;

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(data);

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(
    ntcq::SendCounter                  group,
    const bsl::shared_ptr<ntsa::Data>& data, 
    const ntci::SendCallback&          callback)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_counter++;

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(data);

    if (callback) {
        entry.setCallback(callback);
    }

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter group)
{
    NTCCFG_WARNING_UNUSED(group);

    ntcq::ZeroCopyCounter counter = d_counter++;

    ZeroCopyEntry& entry = d_waitList.back();
    BSLS_ASSERT(entry.group() == group);

    entry.setMaxCounter(counter + 1);

    return counter;
}

void ZeroCopyQueue::frame(ntcq::SendCounter group)
{
    NTCCFG_WARNING_UNUSED(group);

    ZeroCopyEntry& entry = d_waitList.back();
    BSLS_ASSERT(entry.group() == group);

    entry.setFramed(true);

    if (entry.complete()) {
        d_doneList.push_back(entry);
        d_waitList.pop_back();
    }
}

ntsa::Error ZeroCopyQueue::update(const ntsa::ZeroCopy& zeroCopy)
{
    ntcq::ZeroCopyRange zeroCopyRange;

    if (zeroCopy.from() > zeroCopy.to()) {
        // Handle 32-bit unsigned integer wraparound and convert to 64-bit
        // unsigned integers.

        bsl::ptrdiff_t distance = 
            (bsl::numeric_limits<bsl::uint32_t>::max() - zeroCopy.from()) + 
            zeroCopy.to();

        zeroCopyRange.setMinCounter(d_bias + zeroCopy.from());

        d_bias += bsl::numeric_limits<bsl::uint32_t>::max();

        zeroCopyRange.setMaxCounter(d_bias + zeroCopy.from() + distance);
    }
    else {
        zeroCopyRange.setMinCounter(d_bias + zeroCopy.from());
        zeroCopyRange.setMaxCounter(d_bias + zeroCopy.to() + 1);
    }

    // For each zero-copy entry waiting to be completed...

    EntryList::iterator it = d_waitList.begin();
    EntryList::iterator et = d_waitList.end();

    for (; it != et; ++it) {
        ZeroCopyEntry& entry = *it;

        if (zeroCopyRange.maxCounter() < entry.minCounter()) {
            break;
        }

        entry.match(zeroCopyRange);
         
        if (entry.complete()) {
            if (entry.callback()) {
                d_doneList.push_back(entry);
            }
            d_waitList.erase(d_waitList.begin());
        }

        if (zeroCopyRange.empty()) {
            break;
        }
    }

    return ntsa::Error();

#if 0
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
#endif
}

bool ZeroCopyQueue::pop(ntci::SendCallback* result)
{
    while (!d_doneList.empty()) {
        if (d_doneList.front().callback()) {
            *result = d_doneList.front().callback();
            d_doneList.erase(d_doneList.begin());
            return true;
        }
        else {
            d_doneList.erase(d_doneList.begin());
        }
    }

    return false;
}

bool ZeroCopyQueue::pop(bsl::vector<ntci::SendCallback>* result)
{
    bool found = false;

    while (!d_doneList.empty()) {
        if (d_doneList.front().callback()) {
            result->push_back(d_doneList.front().callback());
            d_doneList.erase(d_doneList.begin());
            found = true;
        }
        else {
            d_doneList.erase(d_doneList.begin());
        }
    }

    return found;
}

void ZeroCopyQueue::clear()
{
    d_waitList.clear();
    d_doneList.clear();
}

void ZeroCopyQueue::clear(bsl::vector<ntci::SendCallback>* result)
{
    if (!d_doneList.empty()) {
        EntryList::iterator it = d_doneList.begin();
        EntryList::iterator et = d_doneList.end();

        for (; it != et; ++it) {
            const ZeroCopyEntry& entry = *it;
            if (entry.callback()) {
                result->push_back(entry.callback());
            }
        }
    }

    if (!d_waitList.empty()) {
        EntryList::iterator it = d_waitList.begin();
        EntryList::iterator et = d_waitList.end();

        for (; it != et; ++it) {
            const ZeroCopyEntry& entry = *it;
            if (entry.callback()) {
                result->push_back(entry.callback());
            }
        }
    }

    d_doneList.clear();
    d_waitList.clear();
}

bslma::Allocator* ZeroCopyQueue::allocator() const
{
    return d_allocator_p;
}

void ZeroCopyQueue::load(bsl::vector<ntcq::ZeroCopyEntry>* result) const
{
    result->insert(result->end(), d_doneList.begin(), d_doneList.end());
    result->insert(result->end(), d_waitList.begin(), d_waitList.end());
}

bool ZeroCopyQueue::ready() const
{
    return !d_doneList.empty();
}

}  // close package namespace
}  // close enterprise namespace
