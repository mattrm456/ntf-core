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

#include <ntsa_event.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_event_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error Event::merge(const ntsa::Event& event)
{
    if (d_handle == ntsa::k_INVALID_HANDLE) {
        d_handle = event.d_handle;
    }
    else if (d_handle != event.d_handle) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_state |= event.d_state;

    if (!event.d_bytesReadable.isNull()) {
        if (!d_bytesReadable.isNull()) {
            d_bytesReadable.value() += event.d_bytesReadable.value();
        }
        else {
            d_bytesReadable.makeValue(event.d_bytesReadable.value());
        }
    }

    if (!event.d_bytesWritable.isNull()) {
        if (!d_bytesWritable.isNull()) {
            d_bytesWritable.value() += event.d_bytesWritable.value();
        }
        else {
            d_bytesWritable.makeValue(event.d_bytesWritable.value());
        }
    }

    if (!event.d_backlog.isNull()) {
        if (!d_backlog.isNull()) {
            d_backlog.value() += event.d_backlog.value();
        }
        else {
            d_backlog.makeValue(event.d_backlog.value());
        }
    }

    return ntsa::Error();
}

bool Event::equals(const Event& other) const
{
    return (d_handle == other.d_handle && d_state == other.d_state &&
            d_bytesReadable == other.d_bytesReadable &&
            d_bytesWritable == other.d_bytesWritable &&
            d_backlog == other.d_backlog && d_error == other.d_error);
}

bool Event::less(const Event& other) const
{
    if (d_handle < other.d_handle) {
        return true;
    }

    if (other.d_handle < d_handle) {
        return false;
    }

    if (d_state < other.d_state) {
        return true;
    }

    if (other.d_state < d_state) {
        return false;
    }

    if (d_bytesReadable < other.d_bytesReadable) {
        return true;
    }

    if (other.d_bytesReadable < d_bytesReadable) {
        return false;
    }

    if (d_bytesWritable < other.d_bytesWritable) {
        return true;
    }

    if (other.d_bytesWritable < d_bytesWritable) {
        return false;
    }

    if (d_backlog < other.d_backlog) {
        return true;
    }

    if (other.d_backlog < d_backlog) {
        return false;
    }

    return d_error < other.d_error;
}

bsl::ostream& Event::print(bsl::ostream& stream,
                           int           level,
                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_handle != ntsa::k_INVALID_HANDLE) {
        printer.printAttribute("handle", d_handle);
    }
    else {
        printer.printAttribute("handle", "INVALID");
    }

    if (d_state != 0) {
        bsl::string stateDescription;
        bsl::size_t numFlags = 0;

        if ((d_state & (1 << e_READABLE)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("READABLE", 8);
            ++numFlags;
        }

        if ((d_state & (1 << e_WRITABLE)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("WRITABLE", 8);
            ++numFlags;
        }

        if ((d_state & (1 << e_EXCEPTIONAL)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("EXCEPTIONAL", 11);
            ++numFlags;
        }

        if ((d_state & (1 << e_ERROR)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("ERROR", 5);
            ++numFlags;
        }

        if ((d_state & (1 << e_SHUTDOWN)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("SHUTDOWN", 8);
            ++numFlags;
        }

        if ((d_state & (1 << e_HANGUP)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("HANGUP", 12);
            ++numFlags;
        }

        printer.printAttribute("state", stateDescription);
    }

    if (!d_bytesReadable.isNull()) {
        printer.printAttribute("bytesReadable", d_bytesReadable);
    }

    if (!d_bytesWritable.isNull()) {
        printer.printAttribute("bytesWritable", d_bytesWritable);
    }

    if (!d_backlog.isNull()) {
        printer.printAttribute("backlog", d_backlog);
    }

    if (d_error) {
        printer.printAttribute("error", d_error);
    }

    printer.end();
    return stream;
}

EventSet::EventSet(bslma::Allocator* basicAllocator)
: d_map(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EventSet::EventSet(const EventSet& original, bslma::Allocator* basicAllocator)
: d_map(original.d_map, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{

}

EventSet::~EventSet()
{

}

EventSet& EventSet::operator=(const EventSet& other)
{
    if (this != &other) {
        d_map = other.d_map;
    }

    return *this;
}

void EventSet::overwrite(const ntsa::Event& event)
{
    d_map[event.handle()] = event;
}

void EventSet::merge(const ntsa::Event& event)
{
    d_map[event.handle()].merge(event);
}

void EventSet::clear()
{
    d_map.clear();
}

void EventSet::setReadable(ntsa::Handle socket)
{
    ntsa::Event& event = d_map[socket];

    event.setHandle(socket);
    event.setReadable();
}

void EventSet::setReadable(ntsa::Handle socket, bsl::size_t units)
{
    ntsa::Event& event = d_map[socket];

    event.setHandle(socket);
    event.setReadable();
    event.setBytesReadable(units);
}

void EventSet::setWritable(ntsa::Handle socket)
{
    ntsa::Event& event = d_map[socket];

    event.setHandle(socket);
    event.setWritable();
}

void EventSet::setWritable(ntsa::Handle socket, bsl::size_t units)
{
    ntsa::Event& event = d_map[socket];

    event.setHandle(socket);
    event.setWritable();
    event.setBytesWritable(units);
}

void EventSet::setExceptional(ntsa::Handle socket)
{
    ntsa::Event& event = d_map[socket];

    event.setHandle(socket);
    event.setExceptional();
}

void EventSet::setShutdown(ntsa::Handle socket)
{
    ntsa::Event& event = d_map[socket];

    event.setHandle(socket);
    event.setShutdown();
}

void EventSet::setHangup(ntsa::Handle socket)
{
    ntsa::Event& event = d_map[socket];

    event.setHandle(socket);
    event.setHangup();
}

void EventSet::setError(ntsa::Handle socket, const ntsa::Error& error)
{
    ntsa::Event& event = d_map[socket];

    event.setHandle(socket);
    event.setError(error);
}

bool EventSet::isReadable(ntsa::Handle socket) const
{
    Map::const_iterator it = d_map.find(socket);
    if (it == d_map.end()) {
        return false;
    }

    const ntsa::Event& event = it->second;

    return event.isReadable();
}

bool EventSet::isWritable(ntsa::Handle socket) const
{
    Map::const_iterator it = d_map.find(socket);
    if (it == d_map.end()) {
        return false;
    }

    const ntsa::Event& event = it->second;

    return event.isWritable();
}

bool EventSet::isExceptional(ntsa::Handle socket) const
{
    Map::const_iterator it = d_map.find(socket);
    if (it == d_map.end()) {
        return false;
    }

    const ntsa::Event& event = it->second;

    return event.isExceptional();
}

bool EventSet::isError(ntsa::Handle socket) const
{
    Map::const_iterator it = d_map.find(socket);
    if (it == d_map.end()) {
        return false;
    }

    const ntsa::Event& event = it->second;

    return event.isError();
}

bool EventSet::isShutdown(ntsa::Handle socket) const
{
    Map::const_iterator it = d_map.find(socket);
    if (it == d_map.end()) {
        return false;
    }

    const ntsa::Event& event = it->second;

    return event.isShutdown();
}

bool EventSet::isHangup(ntsa::Handle socket) const
{
    Map::const_iterator it = d_map.find(socket);
    if (it == d_map.end()) {
        return false;
    }

    const ntsa::Event& event = it->second;

    return event.isHangup();
}

bool EventSet::equals(const EventSet& other) const
{
    return d_map == other.d_map;
}

bsl::ostream& EventSet::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    for (Map::const_iterator it  = d_map.begin();
                             it != d_map.end();
                           ++it)
    {
        const ntsa::Event& event = it->second;
        event.print(stream, level, spacesPerLevel);
    }

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
