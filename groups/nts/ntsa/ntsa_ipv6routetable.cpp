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

#include <ntsa_ipv6routetable.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv6routetable_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_algorithm.h>
#include <bsl_map.h>

namespace BloombergLP {
namespace ntsa {

class Ipv6RouteTable::Sorter
{
  public:
    bool operator()(const bsl::shared_ptr<ntsa::Ipv6Route>& lhs,
                    const bsl::shared_ptr<ntsa::Ipv6Route>& rhs) const
    {
        const bsl::uint32_t lhsMaskLength = lhs->destinationIpv6MaskLength();

        const bsl::uint32_t rhsMaskLength = rhs->destinationIpv6MaskLength();

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

Ipv6RouteTable::Ipv6RouteTable(bslma::Allocator* basicAllocator)
: d_routeCache(basicAllocator)
, d_routeVector(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Ipv6RouteTable::~Ipv6RouteTable()
{
}

void Ipv6RouteTable::reset()
{
    d_routeVector.clear();
}

void Ipv6RouteTable::add(const ntsa::Ipv6Route& route)
{
    bsl::shared_ptr<ntsa::Ipv6Route> sharedRoute;
    sharedRoute.createInplace(d_allocator_p, route, d_allocator_p);

    d_routeVector.push_back(sharedRoute);
}

bool Ipv6RouteTable::find(
    ntsa::EthernetAddress*   sourceEthernetAddress,
    ntsa::EthernetAddress*   destinationEthernetAddress,
    ntsa::Ipv6Address*       sourceIpv6Address,
    const ntsa::Ipv6Address& destinationIpv6Address) const
{
    if (d_routeCache.empty()) {
        RouteCache::const_iterator it =
            d_routeCache.find(destinationIpv6Address);
        if (it != d_routeCache.end()) {
            const bsl::shared_ptr<ntsa::Ipv6Route>& route = it->second;

            if (route->interfaceEthernetAddress().has_value() &&
                route->gatewayEthernetAddress().has_value())
            {
                *sourceEthernetAddress =
                    route->interfaceEthernetAddress().value();
                *destinationEthernetAddress =
                    route->gatewayEthernetAddress().value();
                *sourceIpv6Address = route->interfaceIpv6Address().value();
                return true;
            }
        }
    }

    RouteVector candidateRouteVector;

    for (bsl::size_t i = 0; i < d_routeVector.size(); ++i) {
        const bsl::shared_ptr<ntsa::Ipv6Route>& route = d_routeVector[i];

        if (route->destinationIpv6Address().isNull()) {
            continue;
        }

        ntsa::Ipv6Address effectiveIpv6Address;
        if (route->destinationIpv6Mask().isNull()) {
            effectiveIpv6Address = destinationIpv6Address;
        }
        else {
            bsl::uint64_t v[2];
            destinationIpv6Address.copyTo(v, sizeof v);

            bsl::uint64_t m[2];
            route->destinationIpv6Mask().value().copyTo(m, sizeof m);

            bsl::uint64_t r[2];
            r[0] = v[0] & m[0];
            r[1] = v[1] & m[1];

            effectiveIpv6Address.copyFrom(r, sizeof r);
        }

        if (effectiveIpv6Address == route->destinationIpv6Address().value()) {
            candidateRouteVector.push_back(route);
        }
    }

    if (candidateRouteVector.empty()) {
        return false;
    }

    bsl::sort(candidateRouteVector.begin(),
              candidateRouteVector.end(),
              Sorter());

    bsl::shared_ptr<ntsa::Ipv6Route> route = candidateRouteVector.front();

    if (route->interfaceEthernetAddress().isNull()) {
        return false;
    }

    if (route->gatewayEthernetAddress().isNull()) {
        return false;
    }

    *sourceEthernetAddress      = route->interfaceEthernetAddress().value();
    *destinationEthernetAddress = route->gatewayEthernetAddress().value();
    *sourceIpv6Address          = route->interfaceIpv6Address().value();

    const_cast<Ipv6RouteTable*>(this)->d_routeCache.emplace(
        destinationIpv6Address,
        route);

    return true;
}

void Ipv6RouteTable::load(bsl::vector<ntsa::Ipv6Route>* result) const
{
    result->clear();
    result->reserve(d_routeVector.size());

    for (bsl::size_t i = 0; i < d_routeVector.size(); ++i) {
        result->push_back(*d_routeVector[i]);
    }
}

bsl::ostream& Ipv6RouteTable::print(bsl::ostream& stream,
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
