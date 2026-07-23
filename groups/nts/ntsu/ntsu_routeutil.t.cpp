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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_routeutil_t_cpp, "$Id$ $CSID$")

#include <ntsu_routeutil.h>

#include <bsls_review.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsu {

// Provide tests for 'ntsu::RouteUtil'.
class RouteUtilTest
{
    // The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSO.DEVICE.TEST");

  public:
    // Concern: Verify the system Ethernet routing table can be loaded
    // correctly.
    static void verifyEthernetRouteTable();

    // Concern: Verify the system IPv4 routing table can be loaded correctly.
    static void verifyIpv4RouteTable();

    // Concern: Verify the system IPv6 routing table can be loaded correctly.
    static void verifyIpv6RouteTable();
};

NTSCFG_TEST_FUNCTION(ntsu::RouteUtilTest::verifyEthernetRouteTable)
{
    ntsa::Error error;

    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    ntsa::EthernetRouteTable routeTable(NTSCFG_TEST_ALLOCATOR);
    error = ntsu::RouteUtil::load(&routeTable);
    NTSCFG_TEST_OK(error);

    bsl::vector<ntsa::EthernetRoute> routeVector;
    routeTable.load(&routeVector);

    const bsl::size_t WE = 21;
    const bsl::size_t W4 = ntsa::Ipv4Address::MAX_TEXT_LENGTH + 4;
    const bsl::size_t W6 = ntsa::Ipv6Address::MAX_TEXT_LENGTH + 4;

    bsl::stringstream ss;
    ss << bsl::left << bsl::setw(WE) << "Ethernet";
    ss << bsl::left << bsl::setw(W4) << "IPv4";
    ss << bsl::left << bsl::setw(W6) << "IPv6";
    ss << bsl::endl;

    for (bsl::size_t i = 0; i < routeVector.size(); ++i) {
        const ntsa::EthernetRoute& route = routeVector[i];

        bsl::string ethernetAddress;
        bsl::string ipv4Address;
        bsl::string ipv6Address;

        ethernetAddress = route.ethernetAddress().text();

        if (route.ipv4Address().has_value()) {
            ipv4Address = route.ipv4Address().value().text();
        }
        else {
            ipv4Address = "-";
        }

        if (route.ipv6Address().has_value()) {
            ipv6Address = route.ipv6Address().value().text();
        }
        else {
            ipv6Address = "-";
        }

        ss << bsl::left << bsl::setw(WE) << ethernetAddress;
        ss << bsl::left << bsl::setw(W4) << ipv4Address;
        ss << bsl::left << bsl::setw(W6) << ipv6Address;

        ss << bsl::endl;
    }

    bsl::string routeReport = ss.str();

    BALL_LOG_INFO << "Routes:\n" << routeReport << BALL_LOG_END;
}

NTSCFG_TEST_FUNCTION(ntsu::RouteUtilTest::verifyIpv4RouteTable)
{
    ntsa::Error error;

    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    ntsa::Ipv4RouteTable routeTable(NTSCFG_TEST_ALLOCATOR);
    error = ntsu::RouteUtil::load(&routeTable);
    NTSCFG_TEST_OK(error);

    bsl::vector<ntsa::Ipv4Route> routeVector;
    routeTable.load(&routeVector);

    const bsl::size_t WN = 16;
    const bsl::size_t WI = 8;
    const bsl::size_t WE = 21;
    const bsl::size_t W4 = ntsa::Ipv4Address::MAX_TEXT_LENGTH + 4;
    const bsl::size_t W6 = ntsa::Ipv6Address::MAX_TEXT_LENGTH + 4;

    bsl::stringstream ss;
    ss << bsl::left << bsl::setw(W4 + 4) << "DstAddr";
    ss << bsl::left << bsl::setw(W4) << "DstMask";

    ss << bsl::left << bsl::setw(WN) << "GtwName";
    ss << bsl::left << bsl::setw(WI) << "GtwIdx";
    ss << bsl::left << bsl::setw(WE) << "GtwEth";
    ss << bsl::left << bsl::setw(W4) << "GtwIP";

    ss << bsl::left << bsl::setw(WN) << "IfcName";
    ss << bsl::left << bsl::setw(WI) << "IfcIdx";
    ss << bsl::left << bsl::setw(WE) << "IfcEth";
    ss << bsl::left << bsl::setw(W4) << "IfcIP";

    ss << bsl::endl;

    for (bsl::size_t i = 0; i < routeVector.size(); ++i) {
        const ntsa::Ipv4Route& route = routeVector[i];

        bsl::string dstAddr;
        bsl::string dstMask;

        bsl::string gtwName;
        bsl::string gtwIdx;
        bsl::string gtwEth;
        bsl::string gtwIp;

        bsl::string ifcName;
        bsl::string ifcIdx;
        bsl::string ifcEth;
        bsl::string ifcIp;

        if (route.destinationIpv4Address().has_value()) {
            dstAddr = route.destinationIpv4Address().value().text();

            if (route.destinationIpv4MaskLength() > 0) {
                dstAddr.append(1, '/');
                dstAddr.append(
                    bsl::to_string(route.destinationIpv4MaskLength()));
            }
        }
        else {
            dstAddr = "-";
        }

        if (route.destinationIpv4Mask().has_value()) {
            dstMask = route.destinationIpv4Mask().value().text();
        }
        else {
            dstMask = "-";
        }

        if (route.gatewayAdapterName().has_value()) {
            gtwName = route.gatewayAdapterName().value();
        }
        else {
            gtwName = "-";
        }

        if (route.gatewayAdapterIndex().has_value()) {
            gtwIdx = bsl::to_string(route.gatewayAdapterIndex().value());
        }
        else {
            gtwIdx = "-";
        }

        if (route.gatewayEthernetAddress().has_value()) {
            gtwEth = route.gatewayEthernetAddress().value().text();
        }
        else {
            gtwEth = "-";
        }

        if (route.gatewayIpv4Address().has_value()) {
            gtwIp = route.gatewayIpv4Address().value().text();
        }
        else {
            gtwIp = "-";
        }

        if (route.interfaceAdapterName().has_value()) {
            ifcName = route.interfaceAdapterName().value();
        }
        else {
            ifcName = "-";
        }

        if (route.interfaceAdapterIndex().has_value()) {
            ifcIdx = bsl::to_string(route.interfaceAdapterIndex().value());
        }
        else {
            ifcIdx = "-";
        }

        if (route.interfaceEthernetAddress().has_value()) {
            ifcEth = route.interfaceEthernetAddress().value().text();
        }
        else {
            ifcEth = "-";
        }

        if (route.interfaceIpv4Address().has_value()) {
            ifcIp = route.interfaceIpv4Address().value().text();
        }
        else {
            ifcIp = "-";
        }

        ss << bsl::left << bsl::setw(W4 + 4) << dstAddr;
        ss << bsl::left << bsl::setw(W4) << dstMask;

        ss << bsl::left << bsl::setw(WN) << gtwName;
        ss << bsl::left << bsl::setw(WI) << gtwIdx;
        ss << bsl::left << bsl::setw(WE) << gtwEth;
        ss << bsl::left << bsl::setw(W4) << gtwIp;

        ss << bsl::left << bsl::setw(WN) << ifcName;
        ss << bsl::left << bsl::setw(WI) << ifcIdx;
        ss << bsl::left << bsl::setw(WE) << ifcEth;
        ss << bsl::left << bsl::setw(W4) << ifcIp;

        ss << bsl::endl;
    }

    bsl::string routeReport = ss.str();

    BALL_LOG_INFO << "Routes:\n" << routeReport << BALL_LOG_END;
}

NTSCFG_TEST_FUNCTION(ntsu::RouteUtilTest::verifyIpv6RouteTable)
{
    ntsa::Error error;

    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    ntsa::Ipv6RouteTable routeTable(NTSCFG_TEST_ALLOCATOR);
    error = ntsu::RouteUtil::load(&routeTable);
    NTSCFG_TEST_OK(error);

    bsl::vector<ntsa::Ipv6Route> routeVector;
    routeTable.load(&routeVector);

    const bsl::size_t WN = 16;
    const bsl::size_t WI = 8;
    const bsl::size_t WE = 21;
    const bsl::size_t W4 = ntsa::Ipv6Address::MAX_TEXT_LENGTH + 4;
    const bsl::size_t W6 = ntsa::Ipv6Address::MAX_TEXT_LENGTH + 4;

    bsl::stringstream ss;
    ss << bsl::left << bsl::setw(W6) << "DstAddr";
    ss << bsl::left << bsl::setw(W6) << "DstMask";

    ss << bsl::left << bsl::setw(WN) << "GtwName";
    ss << bsl::left << bsl::setw(WI) << "GtwIdx";
    ss << bsl::left << bsl::setw(WE) << "GtwEth";
    ss << bsl::left << bsl::setw(W6) << "GtwIP";

    ss << bsl::left << bsl::setw(WN) << "IfcName";
    ss << bsl::left << bsl::setw(WI) << "IfcIdx";
    ss << bsl::left << bsl::setw(WE) << "IfcEth";
    ss << bsl::left << bsl::setw(W6) << "IfcIP";

    ss << bsl::endl;

    for (bsl::size_t i = 0; i < routeVector.size(); ++i) {
        const ntsa::Ipv6Route& route = routeVector[i];

        bsl::string dstAddr;
        bsl::string dstMask;

        bsl::string gtwName;
        bsl::string gtwIdx;
        bsl::string gtwEth;
        bsl::string gtwIp;

        bsl::string ifcName;
        bsl::string ifcIdx;
        bsl::string ifcEth;
        bsl::string ifcIp;

        if (route.destinationIpv6Address().has_value()) {
            dstAddr = route.destinationIpv6Address().value().text();

            if (route.destinationIpv6MaskLength() > 0) {
                dstAddr.append(1, '/');
                dstAddr.append(
                    bsl::to_string(route.destinationIpv6MaskLength()));
            }
        }
        else {
            dstAddr = "-";
        }

        if (route.destinationIpv6Mask().has_value()) {
            dstMask = route.destinationIpv6Mask().value().text();
        }
        else {
            dstMask = "-";
        }

        if (route.gatewayAdapterName().has_value()) {
            gtwName = route.gatewayAdapterName().value();
        }
        else {
            gtwName = "-";
        }

        if (route.gatewayAdapterIndex().has_value()) {
            gtwIdx = bsl::to_string(route.gatewayAdapterIndex().value());
        }
        else {
            gtwIdx = "-";
        }

        if (route.gatewayEthernetAddress().has_value()) {
            gtwEth = route.gatewayEthernetAddress().value().text();
        }
        else {
            gtwEth = "-";
        }

        if (route.gatewayIpv6Address().has_value()) {
            gtwIp = route.gatewayIpv6Address().value().text();
        }
        else {
            gtwIp = "-";
        }

        if (route.interfaceAdapterName().has_value()) {
            ifcName = route.interfaceAdapterName().value();
        }
        else {
            ifcName = "-";
        }

        if (route.interfaceAdapterIndex().has_value()) {
            ifcIdx = bsl::to_string(route.interfaceAdapterIndex().value());
        }
        else {
            ifcIdx = "-";
        }

        if (route.interfaceEthernetAddress().has_value()) {
            ifcEth = route.interfaceEthernetAddress().value().text();
        }
        else {
            ifcEth = "-";
        }

        if (route.interfaceIpv6Address().has_value()) {
            ifcIp = route.interfaceIpv6Address().value().text();
        }
        else {
            ifcIp = "-";
        }

        ss << bsl::left << bsl::setw(W4) << dstAddr;
        ss << bsl::left << bsl::setw(W4) << dstMask;

        ss << bsl::left << bsl::setw(WN) << gtwName;
        ss << bsl::left << bsl::setw(WI) << gtwIdx;
        ss << bsl::left << bsl::setw(WE) << gtwEth;
        ss << bsl::left << bsl::setw(W4) << gtwIp;

        ss << bsl::left << bsl::setw(WN) << ifcName;
        ss << bsl::left << bsl::setw(WI) << ifcIdx;
        ss << bsl::left << bsl::setw(WE) << ifcEth;
        ss << bsl::left << bsl::setw(W4) << ifcIp;

        ss << bsl::endl;
    }

    bsl::string routeReport = ss.str();

    BALL_LOG_INFO << "Routes:\n" << routeReport << BALL_LOG_END;
}

}  // close namespace ntsu
}  // close namespace BloombergLP
