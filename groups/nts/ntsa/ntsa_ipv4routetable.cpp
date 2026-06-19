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

#include <ntsa_ipv4routetable.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4routetable_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

Ipv4RouteTable::Ipv4RouteTable(bslma::Allocator* basicAllocator)
: d_entries(basicAllocator)
{
}

Ipv4RouteTable::Ipv4RouteTable(const Ipv4RouteTable& original,
                     bslma::Allocator* basicAllocator)
: d_entries(original.d_entries, basicAllocator)
{
}

Ipv4RouteTable::~Ipv4RouteTable()
{
}

Ipv4RouteTable& Ipv4RouteTable::operator=(const Ipv4RouteTable& other)
{
    d_entries = other.d_entries;

    return *this;
}

void Ipv4RouteTable::reset()
{
    d_entries.clear();
}

void Ipv4RouteTable::add(const ntsa::Ipv4Route& route)
{
    d_entries.push_back(route);
}

const bsl::vector<ntsa::Ipv4Route>& Ipv4RouteTable::entries() const
{
    return d_entries;
}

bool Ipv4RouteTable::equals(const Ipv4RouteTable& other) const
{
    return d_entries == other.d_entries;
}

bool Ipv4RouteTable::less(const Ipv4RouteTable& other) const
{
    return d_entries < other.d_entries;
}

bsl::ostream& Ipv4RouteTable::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("entries", d_entries);

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
