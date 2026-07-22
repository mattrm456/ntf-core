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

#include <ntsa_ethernetroute.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetroute_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

EthernetRoute::EthernetRoute(bslma::Allocator* basicAllocator)
: d_domainName()
, d_ethernetAddress()
, d_ipv4Address()
, d_ipv6Address()
, d_flags(0)
{
    NTSCFG_WARNING_UNUSED(basicAllocator);
}

EthernetRoute::EthernetRoute(const EthernetRoute&  original,
                             bslma::Allocator*     basicAllocator)
: d_domainName(original.d_domainName)
, d_ethernetAddress(original.d_ethernetAddress)
, d_ipv4Address(original.d_ipv4Address)
, d_ipv6Address(original.d_ipv6Address)
, d_flags(original.d_flags)
{
    NTSCFG_WARNING_UNUSED(basicAllocator);
}

EthernetRoute::~EthernetRoute()
{
}

EthernetRoute& EthernetRoute::operator=(const EthernetRoute& other)
{
    d_domainName = other.d_domainName;
    d_ethernetAddress    = other.d_ethernetAddress;
    d_ipv4Address = other.d_ipv4Address;
    d_ipv6Address     = other.d_ipv6Address;
    d_flags                  = other.d_flags;

    return *this;
}

void EthernetRoute::reset()
{
    d_domainName.reset();
    d_ethernetAddress.reset();
    d_ipv4Address.reset();
    d_ipv6Address.reset();
    d_flags = 0;
}

bool EthernetRoute::equals(const EthernetRoute& other) const
{
    return d_domainName == other.d_domainName &&
           d_ethernetAddress == other.d_ethernetAddress &&
           d_ipv4Address == other.d_ipv4Address &&
           d_ipv6Address == other.d_ipv6Address &&
           d_flags == other.d_flags;
}

bool EthernetRoute::less(const EthernetRoute& other) const
{
    if (d_domainName < other.d_domainName) {
        return true;
    }

    if (other.d_domainName < d_domainName) {
        return false;
    }

    if (d_ethernetAddress < other.d_ethernetAddress) {
        return true;
    }

    if (other.d_ethernetAddress < d_ethernetAddress) {
        return false;
    }

    if (d_ipv4Address < other.d_ipv4Address) {
        return true;
    }

    if (other.d_ipv4Address < d_ipv4Address) {
        return false;
    }

    if (d_ipv6Address < other.d_ipv6Address) {
        return true;
    }

    if (other.d_ipv6Address < d_ipv6Address) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& EthernetRoute::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_domainName.has_value()) {
        printer.printAttribute("domainName",
                               d_domainName.value());
    }


        printer.printAttribute("ethernetAddress",
                               d_ethernetAddress);

    if (d_ipv4Address.has_value()) {
        printer.printAttribute("ipv4Address",
                               d_ipv4Address.value());
    }

    if (d_ipv6Address.has_value()) {
        printer.printAttribute("ipv6Address",
                               d_ipv6Address.value());
    }

    if (d_flags != 0) {
        printer.printAttribute("flags", d_flags);
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
