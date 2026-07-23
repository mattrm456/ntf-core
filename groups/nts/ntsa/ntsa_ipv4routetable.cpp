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
#include <bsl_algorithm.h>
#include <bsl_map.h>

namespace BloombergLP {
namespace ntsa {

class Ipv4RouteTable::Sorter
{
  public:
    bool operator()(const bsl::shared_ptr<ntsa::Ipv4Route>& lhs,
                    const bsl::shared_ptr<ntsa::Ipv4Route>& rhs) const
    {
        const bsl::uint32_t lhsMaskLength = lhs->destinationIpv4MaskLength();

        const bsl::uint32_t rhsMaskLength = rhs->destinationIpv4MaskLength();

        if (rhsMaskLength < lhsMaskLength) {
            return true;
        }

        if (lhsMaskLength < rhsMaskLength) {
            return false;
        }

        const bsl::uint64_t lhsDistance = lhs->distance();
        const bsl::uint64_t rhsDistance = rhs->distance();

        if (lhsDistance < rhsDistance) {
            return true;
        }

        if (rhsDistance < lhsDistance) {
            return false;
        }

        const bsl::uint64_t lhsCost = lhs->cost();
        const bsl::uint64_t rhsCost = rhs->cost();

        return lhsCost < rhsCost;
    }
};

Ipv4RouteTable::Ipv4RouteTable(bslma::Allocator* basicAllocator)
: d_routeCache(basicAllocator)
, d_routeVector(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Ipv4RouteTable::~Ipv4RouteTable()
{
}

void Ipv4RouteTable::reset()
{
    d_routeVector.clear();
}

void Ipv4RouteTable::add(const ntsa::Ipv4Route& route)
{
    bsl::shared_ptr<ntsa::Ipv4Route> sharedRoute;
    sharedRoute.createInplace(d_allocator_p, route, d_allocator_p);

    d_routeVector.push_back(sharedRoute);
}

bool Ipv4RouteTable::find(
    ntsa::EthernetAddress*   sourceEthernetAddress,
    ntsa::EthernetAddress*   destinationEthernetAddress,
    ntsa::Ipv4Address*       sourceIpv4Address,
    const ntsa::Ipv4Address& destinationIpv4Address) const
{
    if (d_routeCache.empty()) {
        RouteCache::const_iterator it =
            d_routeCache.find(destinationIpv4Address);
        if (it != d_routeCache.end()) {
            const bsl::shared_ptr<ntsa::Ipv4Route>& route = it->second;

            if (route->interfaceEthernetAddress().has_value() &&
                route->gatewayEthernetAddress().has_value() &&
                route->interfaceIpv4Address().has_value())
            {
                *sourceEthernetAddress =
                    route->interfaceEthernetAddress().value();
                *destinationEthernetAddress =
                    route->gatewayEthernetAddress().value();
                *sourceIpv4Address = route->interfaceIpv4Address().value();
                return true;
            }
        }
    }

    RouteVector candidateRouteVector;

    for (bsl::size_t i = 0; i < d_routeVector.size(); ++i) {
        const bsl::shared_ptr<ntsa::Ipv4Route>& route = d_routeVector[i];

        if (route->destinationIpv4Address().isNull()) {
            continue;
        }

        ntsa::Ipv4Address effectiveIpv4Address;
        if (route->destinationIpv4Mask().isNull()) {
            effectiveIpv4Address = destinationIpv4Address;
        }
        else {
            bsl::uint32_t value = destinationIpv4Address.value();
            bsl::uint32_t mask  = route->destinationIpv4Mask().value().value();

            effectiveIpv4Address = ntsa::Ipv4Address(value & mask);
        }

        if (effectiveIpv4Address == route->destinationIpv4Address().value()) {
            candidateRouteVector.push_back(route);
        }
    }

    if (candidateRouteVector.empty()) {
        return false;
    }

    bsl::sort(candidateRouteVector.begin(),
              candidateRouteVector.end(),
              Sorter());

    bsl::shared_ptr<ntsa::Ipv4Route> route = candidateRouteVector.front();

    if (route->interfaceEthernetAddress().isNull()) {
        return false;
    }

    if (route->gatewayEthernetAddress().isNull()) {
        return false;
    }

    *sourceEthernetAddress      = route->interfaceEthernetAddress().value();
    *destinationEthernetAddress = route->gatewayEthernetAddress().value();
    *sourceIpv4Address          = route->interfaceIpv4Address().value();

    const_cast<Ipv4RouteTable*>(this)->d_routeCache.emplace(
        destinationIpv4Address,
        route);

    return true;
}

void Ipv4RouteTable::load(bsl::vector<ntsa::Ipv4Route>* result) const
{
    result->clear();
    result->reserve(d_routeVector.size());

    for (bsl::size_t i = 0; i < d_routeVector.size(); ++i) {
        result->push_back(*d_routeVector[i]);
    }
}

bsl::ostream& Ipv4RouteTable::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("entries", d_routeVector);

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
