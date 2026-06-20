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

#include <ntsa_ethernetroutetable.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetroutetable_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

EthernetRouteTable::EthernetRouteTable(bslma::Allocator* basicAllocator)
: d_ethernetAddressByIpv4Address(basicAllocator)
, d_ethernetAddressByIpv6Address(basicAllocator)
, d_ipv4AddressByEthernetAddress(basicAllocator)
, d_ipv6AddressByEthernetAddress(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EthernetRouteTable::~EthernetRouteTable()
{
}

EthernetRouteTable& EthernetRouteTable::operator=(const EthernetRouteTable& other)
{
    d_ethernetAddressByIpv4Address = other.d_ethernetAddressByIpv4Address;

    return *this;
}

void EthernetRouteTable::reset()
{
    d_ethernetAddressByIpv4Address.clear();
    d_ethernetAddressByIpv6Address.clear();
    d_ipv4AddressByEthernetAddress.clear();
    d_ipv6AddressByEthernetAddress.clear();
}

void EthernetRouteTable::add(const ntsa::EthernetRoute& route)
{
    if (route.ipv4Address().has_value()) {
        d_ipv4AddressByEthernetAddress[route.ethernetAddress()] =
            route.ipv4Address().value();

        d_ethernetAddressByIpv4Address[route.ipv4Address().value()] =
            route.ethernetAddress();
    }

    if (route.ipv6Address().has_value()) {
        d_ipv6AddressByEthernetAddress[route.ethernetAddress()] =
            route.ipv6Address().value();

        d_ethernetAddressByIpv6Address[route.ipv6Address().value()] =
            route.ethernetAddress();
    }
}

bool EthernetRouteTable::find(ntsa::EthernetAddress*   result,
                              const ntsa::Ipv4Address& ipv4Address) const
{
    EthernetAddressByIpv4Address::const_iterator it =
        d_ethernetAddressByIpv4Address.find(ipv4Address);

    if (it == d_ethernetAddressByIpv4Address.end()) {
        return false;
    }

    *result = it->second;
    return true;
}

bool EthernetRouteTable::find(ntsa::EthernetAddress*   result,
                              const ntsa::Ipv6Address& ipv6Address) const
{
    EthernetAddressByIpv6Address::const_iterator it =
        d_ethernetAddressByIpv6Address.find(ipv6Address);

    if (it == d_ethernetAddressByIpv6Address.end()) {
        return false;
    }

    *result = it->second;
    return true;
}

bool EthernetRouteTable::find(ntsa::Ipv4Address*           result,
                              const ntsa::EthernetAddress& ethernetAddress)
{
    Ipv4AddressByEthernetAddress::const_iterator it =
        d_ipv4AddressByEthernetAddress.find(ethernetAddress);

    if (it == d_ipv4AddressByEthernetAddress.end()) {
        return false;
    }

    *result = it->second;
    return true;
}

bool EthernetRouteTable::find(ntsa::Ipv6Address*           result,
                              const ntsa::EthernetAddress& ethernetAddress)
{
    Ipv6AddressByEthernetAddress::const_iterator it =
        d_ipv6AddressByEthernetAddress.find(ethernetAddress);

    if (it == d_ipv6AddressByEthernetAddress.end()) {
        return false;
    }

    *result = it->second;
    return true;
}

void EthernetRouteTable::load(bsl::vector<ntsa::EthernetRoute>* result) const
{
    typedef bsl::pair<bdlb::NullableValue<ntsa::Ipv4Address>,
                      bdlb::NullableValue<ntsa::Ipv6Address>
    > IpAddressPair;

    typedef bsl::unordered_map<ntsa::EthernetAddress, IpAddressPair>
    IpAddressPairByEthernetAddress;

    IpAddressPairByEthernetAddress map;

    {
        Ipv4AddressByEthernetAddress::const_iterator
            it = d_ipv4AddressByEthernetAddress.begin();

        Ipv4AddressByEthernetAddress::const_iterator
            et = d_ipv4AddressByEthernetAddress.end();

        for (; it != et; ++it) {
            map[it->first].first = it->second;
        }
    }

    {
        Ipv6AddressByEthernetAddress::const_iterator
            it = d_ipv6AddressByEthernetAddress.begin();

        Ipv6AddressByEthernetAddress::const_iterator
            et = d_ipv6AddressByEthernetAddress.end();

        for (; it != et; ++it) {
            map[it->first].second = it->second;
        }
    }

    {
        IpAddressPairByEthernetAddress::const_iterator it = map.begin();
        IpAddressPairByEthernetAddress::const_iterator et = map.end();

        result->clear();
        result->reserve(map.size());

        for (; it != et; ++it) {
            ntsa::EthernetRoute route;
            route.setEthernetAddress(it->first);

            if (it->second.first.has_value()) {
                route.setIpv4Address(it->second.first.value());
            }

            if (it->second.second.has_value()) {
                route.setIpv6Address(it->second.second.value());
            }

            result->push_back(route);
        }
    }
}

bsl::ostream& EthernetRouteTable::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    bsl::vector<ntsa::EthernetRoute> entries;
    this->load(&entries);

    printer.printAttribute("entries", entries);

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
