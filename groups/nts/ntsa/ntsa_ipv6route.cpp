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

#include <ntsa_ipv6route.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv6route_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

Ipv6Route::Ipv6Route(bslma::Allocator* basicAllocator)
: d_destinationIpv6Address()
, d_destinationIpv6Mask()
, d_destinationIpv6MaskLength(0)
, d_gatewayAdapterName(basicAllocator)
, d_gatewayAdapterIndex()
, d_gatewayEthernetAddress()
, d_gatewayIpv6Address()
, d_interfaceAdapterName(basicAllocator)
, d_interfaceAdapterIndex()
, d_interfaceEthernetAddress()
, d_interfaceIpv6Address()
, d_timeToLive()
, d_distance(0)
, d_cost(0)
, d_flags(0)
{
}

Ipv6Route::Ipv6Route(const Ipv6Route&  original,
                     bslma::Allocator* basicAllocator)
: d_destinationIpv6Address(original.d_destinationIpv6Address)
, d_destinationIpv6Mask(original.d_destinationIpv6Mask)
, d_destinationIpv6MaskLength(original.d_destinationIpv6MaskLength)
, d_gatewayAdapterName(original.d_gatewayAdapterName, basicAllocator)
, d_gatewayAdapterIndex(original.d_gatewayAdapterIndex)
, d_gatewayEthernetAddress(original.d_gatewayEthernetAddress)
, d_gatewayIpv6Address(original.d_gatewayIpv6Address)
, d_interfaceAdapterName(original.d_interfaceAdapterName, basicAllocator)
, d_interfaceAdapterIndex(original.d_interfaceAdapterIndex)
, d_interfaceEthernetAddress(original.d_interfaceEthernetAddress)
, d_interfaceIpv6Address(original.d_interfaceIpv6Address)
, d_timeToLive(original.d_timeToLive)
, d_distance(original.d_distance)
, d_cost(original.d_cost)
, d_flags(original.d_flags)
{
}

Ipv6Route::~Ipv6Route()
{
}

Ipv6Route& Ipv6Route::operator=(const Ipv6Route& other)
{
    d_destinationIpv6Address    = other.d_destinationIpv6Address;
    d_destinationIpv6Mask       = other.d_destinationIpv6Mask;
    d_destinationIpv6MaskLength = other.d_destinationIpv6MaskLength;
    d_gatewayAdapterName        = other.d_gatewayAdapterName;
    d_gatewayAdapterIndex       = other.d_gatewayAdapterIndex;
    d_gatewayEthernetAddress    = other.d_gatewayEthernetAddress;
    d_gatewayIpv6Address        = other.d_gatewayIpv6Address;
    d_interfaceAdapterName      = other.d_interfaceAdapterName;
    d_interfaceAdapterIndex     = other.d_interfaceAdapterIndex;
    d_interfaceEthernetAddress  = other.d_interfaceEthernetAddress;
    d_interfaceIpv6Address      = other.d_interfaceIpv6Address;
    d_timeToLive                = other.d_timeToLive;
    d_distance                  = other.d_distance;
    d_cost                      = other.d_cost;
    d_flags                     = other.d_flags;

    return *this;
}

void Ipv6Route::reset()
{
    d_destinationIpv6Address.reset();
    d_destinationIpv6Mask.reset();
    d_destinationIpv6MaskLength = 0;
    d_gatewayAdapterName.reset();
    d_gatewayAdapterIndex.reset();
    d_gatewayEthernetAddress.reset();
    d_gatewayIpv6Address.reset();
    d_interfaceAdapterName.reset();
    d_interfaceAdapterIndex.reset();
    d_interfaceEthernetAddress.reset();
    d_interfaceIpv6Address.reset();
    d_timeToLive.reset();
    d_distance = 0;
    d_cost     = 0;
    d_flags    = 0;
}

bool Ipv6Route::equals(const Ipv6Route& other) const
{
    return d_destinationIpv6Address == other.d_destinationIpv6Address &&
           d_destinationIpv6Mask == other.d_destinationIpv6Mask &&
           d_destinationIpv6MaskLength == other.d_destinationIpv6MaskLength &&
           d_gatewayAdapterName == other.d_gatewayAdapterName &&
           d_gatewayAdapterIndex == other.d_gatewayAdapterIndex &&
           d_gatewayEthernetAddress == other.d_gatewayEthernetAddress &&
           d_gatewayIpv6Address == other.d_gatewayIpv6Address &&
           d_interfaceAdapterName == other.d_interfaceAdapterName &&
           d_interfaceAdapterIndex == other.d_interfaceAdapterIndex &&
           d_interfaceEthernetAddress == other.d_interfaceEthernetAddress &&
           d_interfaceIpv6Address == other.d_interfaceIpv6Address &&
           d_timeToLive == other.d_timeToLive &&
           d_distance == other.d_distance && d_cost == other.d_cost &&
           d_flags == other.d_flags;
}

bool Ipv6Route::less(const Ipv6Route& other) const
{
    if (d_destinationIpv6Address < other.d_destinationIpv6Address) {
        return true;
    }

    if (other.d_destinationIpv6Address < d_destinationIpv6Address) {
        return false;
    }

    if (d_destinationIpv6Mask < other.d_destinationIpv6Mask) {
        return true;
    }

    if (other.d_destinationIpv6Mask < d_destinationIpv6Mask) {
        return false;
    }

    if (d_destinationIpv6MaskLength < other.d_destinationIpv6MaskLength) {
        return true;
    }

    if (other.d_destinationIpv6MaskLength < d_destinationIpv6MaskLength) {
        return false;
    }

    if (d_gatewayAdapterName < other.d_gatewayAdapterName) {
        return true;
    }

    if (other.d_gatewayAdapterName < d_gatewayAdapterName) {
        return false;
    }

    if (d_gatewayAdapterIndex < other.d_gatewayAdapterIndex) {
        return true;
    }

    if (other.d_gatewayAdapterIndex < d_gatewayAdapterIndex) {
        return false;
    }

    if (d_gatewayEthernetAddress < other.d_gatewayEthernetAddress) {
        return true;
    }

    if (other.d_gatewayEthernetAddress < d_gatewayEthernetAddress) {
        return false;
    }

    if (d_gatewayIpv6Address < other.d_gatewayIpv6Address) {
        return true;
    }

    if (other.d_gatewayIpv6Address < d_gatewayIpv6Address) {
        return false;
    }

    if (d_interfaceAdapterName < other.d_interfaceAdapterName) {
        return true;
    }

    if (other.d_interfaceAdapterName < d_interfaceAdapterName) {
        return false;
    }

    if (d_interfaceAdapterIndex < other.d_interfaceAdapterIndex) {
        return true;
    }

    if (other.d_interfaceAdapterIndex < d_interfaceAdapterIndex) {
        return false;
    }

    if (d_interfaceEthernetAddress < other.d_interfaceEthernetAddress) {
        return true;
    }

    if (other.d_interfaceEthernetAddress < d_interfaceEthernetAddress) {
        return false;
    }

    if (d_interfaceIpv6Address < other.d_interfaceIpv6Address) {
        return true;
    }

    if (other.d_interfaceIpv6Address < d_interfaceIpv6Address) {
        return false;
    }

    if (d_timeToLive < other.d_timeToLive) {
        return true;
    }

    if (other.d_timeToLive < d_timeToLive) {
        return false;
    }

    if (d_distance < other.d_distance) {
        return true;
    }

    if (other.d_distance < d_distance) {
        return false;
    }

    if (d_cost < other.d_cost) {
        return true;
    }

    if (other.d_cost < d_cost) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& Ipv6Route::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_destinationIpv6Address.has_value()) {
        printer.printAttribute("destinationIpv6Address",
                               d_destinationIpv6Address.value());
    }
    else {
        printer.printAttribute("destinationIpv6Address", "default");
    }

    if (d_destinationIpv6Mask.has_value()) {
        printer.printAttribute("destinationIpv6Mask",
                               d_destinationIpv6Mask.value());
    }

    if (d_gatewayAdapterName.has_value()) {
        printer.printAttribute("gatewayAdapterName",
                               d_gatewayAdapterName.value());
    }

    if (d_gatewayAdapterIndex.has_value()) {
        printer.printAttribute("gatewayAdapterIndex",
                               d_gatewayAdapterIndex.value());
    }

    if (d_gatewayEthernetAddress.has_value()) {
        printer.printAttribute("gatewayEthernetAddress",
                               d_gatewayEthernetAddress.value());
    }

    if (d_gatewayIpv6Address.has_value()) {
        printer.printAttribute("gatewayIpv6Address",
                               d_gatewayIpv6Address.value());
    }

    if (d_interfaceAdapterName.has_value()) {
        printer.printAttribute("interfaceAdapterName",
                               d_interfaceAdapterName.value());
    }

    if (d_interfaceAdapterIndex.has_value()) {
        printer.printAttribute("interfaceAdapterIndex",
                               d_interfaceAdapterIndex.value());
    }

    if (d_interfaceEthernetAddress.has_value()) {
        printer.printAttribute("interfaceEthernetAddress",
                               d_interfaceEthernetAddress.value());
    }

    if (d_interfaceIpv6Address.has_value()) {
        printer.printAttribute("interfaceIpv6Address",
                               d_interfaceIpv6Address.value());
    }

    if (d_timeToLive.has_value()) {
        printer.printAttribute("timeToLive", d_timeToLive.value());
    }

    if (d_distance != 0) {
        printer.printAttribute("distance", d_distance);
    }

    if (d_cost != 0) {
        printer.printAttribute("cost", d_cost);
    }

    if (d_flags != 0) {
        printer.printAttribute("flags", d_flags);
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
