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

#include <ntsa_ipv4route.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4route_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

Ipv4Route::Ipv4Route(bslma::Allocator* basicAllocator)
: d_destinationIpv4Address()
, d_destinationIpv4Mask()
, d_gatewayAdapterName(basicAllocator)
, d_gatewayAdapterIndex()
, d_gatewayEthernetAddress()
, d_gatewayIpv4Address()
, d_interfaceAdapterName(basicAllocator)
, d_interfaceAdapterIndex()
, d_interfaceEthernetAddress()
, d_interfaceIpv4Address()
, d_timeToLive()
, d_flags(0)
{
}

Ipv4Route::Ipv4Route(const Ipv4Route&  original,
                     bslma::Allocator* basicAllocator)
: d_destinationIpv4Address(original.d_destinationIpv4Address)
, d_destinationIpv4Mask(original.d_destinationIpv4Mask)
, d_gatewayAdapterName(original.d_gatewayAdapterName, basicAllocator)
, d_gatewayAdapterIndex(original.d_gatewayAdapterIndex)
, d_gatewayEthernetAddress(original.d_gatewayEthernetAddress)
, d_gatewayIpv4Address(original.d_gatewayIpv4Address)
, d_interfaceAdapterName(original.d_interfaceAdapterName, basicAllocator)
, d_interfaceAdapterIndex(original.d_interfaceAdapterIndex)
, d_interfaceEthernetAddress(original.d_interfaceEthernetAddress)
, d_interfaceIpv4Address(original.d_interfaceIpv4Address)
, d_timeToLive(original.d_timeToLive)
, d_flags(original.d_flags)
{
}

Ipv4Route::~Ipv4Route()
{
}

Ipv4Route& Ipv4Route::operator=(const Ipv4Route& other)
{
    d_destinationIpv4Address   = other.d_destinationIpv4Address;
    d_destinationIpv4Mask      = other.d_destinationIpv4Mask;
    d_gatewayAdapterName       = other.d_gatewayAdapterName;
    d_gatewayAdapterIndex      = other.d_gatewayAdapterIndex;
    d_gatewayEthernetAddress   = other.d_gatewayEthernetAddress;
    d_gatewayIpv4Address       = other.d_gatewayIpv4Address;
    d_interfaceAdapterName     = other.d_interfaceAdapterName;
    d_interfaceAdapterIndex    = other.d_interfaceAdapterIndex;
    d_interfaceEthernetAddress = other.d_interfaceEthernetAddress;
    d_interfaceIpv4Address     = other.d_interfaceIpv4Address;
    d_timeToLive               = other.d_timeToLive;
    d_flags                    = other.d_flags;

    return *this;
}

void Ipv4Route::reset()
{
    d_destinationIpv4Address.reset();
    d_destinationIpv4Mask.reset();
    d_gatewayAdapterName.reset();
    d_gatewayAdapterIndex.reset();
    d_gatewayEthernetAddress.reset();
    d_gatewayIpv4Address.reset();
    d_interfaceAdapterName.reset();
    d_interfaceAdapterIndex.reset();
    d_interfaceEthernetAddress.reset();
    d_interfaceIpv4Address.reset();
    d_timeToLive.reset();
    d_flags = 0;
}

bool Ipv4Route::equals(const Ipv4Route& other) const
{
    return d_destinationIpv4Address == other.d_destinationIpv4Address &&
           d_destinationIpv4Mask == other.d_destinationIpv4Mask &&
           d_gatewayAdapterName == other.d_gatewayAdapterName &&
           d_gatewayAdapterIndex == other.d_gatewayAdapterIndex &&
           d_gatewayEthernetAddress == other.d_gatewayEthernetAddress &&
           d_gatewayIpv4Address == other.d_gatewayIpv4Address &&
           d_interfaceAdapterName == other.d_interfaceAdapterName &&
           d_interfaceAdapterIndex == other.d_interfaceAdapterIndex &&
           d_interfaceEthernetAddress == other.d_interfaceEthernetAddress &&
           d_interfaceIpv4Address == other.d_interfaceIpv4Address &&
           d_timeToLive == other.d_timeToLive && d_flags == other.d_flags;
}

bool Ipv4Route::less(const Ipv4Route& other) const
{
    if (d_destinationIpv4Address < other.d_destinationIpv4Address) {
        return true;
    }

    if (other.d_destinationIpv4Address < d_destinationIpv4Address) {
        return false;
    }

    if (d_destinationIpv4Mask < other.d_destinationIpv4Mask) {
        return true;
    }

    if (other.d_destinationIpv4Mask < d_destinationIpv4Mask) {
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

    if (d_gatewayIpv4Address < other.d_gatewayIpv4Address) {
        return true;
    }

    if (other.d_gatewayIpv4Address < d_gatewayIpv4Address) {
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

    if (d_interfaceIpv4Address < other.d_interfaceIpv4Address) {
        return true;
    }

    if (other.d_interfaceIpv4Address < d_interfaceIpv4Address) {
        return false;
    }

    if (d_timeToLive < other.d_timeToLive) {
        return true;
    }

    if (other.d_timeToLive < d_timeToLive) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& Ipv4Route::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_destinationIpv4Address.has_value()) {
        printer.printAttribute("destinationIpv4Address",
                               d_destinationIpv4Address.value());
    }
    else {
        printer.printAttribute("destinationIpv4Address", "default");
    }

    if (d_destinationIpv4Mask.has_value()) {
        printer.printAttribute("destinationIpv4Mask",
                               d_destinationIpv4Mask.value());
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

    if (d_gatewayIpv4Address.has_value()) {
        printer.printAttribute("gatewayIpv4Address",
                               d_gatewayIpv4Address.value());
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

    if (d_interfaceIpv4Address.has_value()) {
        printer.printAttribute("interfaceIpv4Address",
                               d_interfaceIpv4Address.value());
    }

    if (d_timeToLive.has_value()) {
        printer.printAttribute("timeToLive", d_timeToLive.value());
    }

    if (d_flags != 0) {
        printer.printAttribute("flags", d_flags);
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
