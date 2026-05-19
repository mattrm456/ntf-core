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

#include <ntsa_packettype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_packettype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int PacketType::fromInt(PacketType::Value* result, int number)
{
    switch (number) {
    case PacketType::e_UNDEFINED:
    case PacketType::e_ETHERNET:
    case PacketType::e_ARP:
    case PacketType::e_RARP:
    case PacketType::e_IPV4:
    case PacketType::e_IPV6:
    case PacketType::e_ICMP:
    case PacketType::e_IGMP:
    case PacketType::e_TCP:
    case PacketType::e_UDP:
        *result = static_cast<PacketType::Value>(number);
        return 0;
    default:
        *result = PacketType::e_UNDEFINED;
        return -1;
    }
}

int PacketType::fromString(PacketType::Value*          result,
                         const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ETHERNET")) {
        *result = e_ETHERNET;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ARP")) {
        *result = e_ARP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RARP")) {
        *result = e_RARP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "IPV4")) {
        *result = e_IPV4;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "IPV6")) {
        *result = e_IPV6;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ICMP")) {
        *result = e_ICMP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "IGMP")) {
        *result = e_IGMP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TCP")) {
        *result = e_TCP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "UDP")) {
        *result = e_UDP;
        return 0;
    }

    *result = e_UNDEFINED;
    return -1;
}

const char* PacketType::toString(PacketType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_ETHERNET: {
        return "ETHERNET";
    } break;
    case e_ARP: {
        return "ARP";
    } break;
    case e_RARP: {
        return "RARP";
    } break;
    case e_IPV4: {
        return "IPV4";
    } break;
    case e_IPV6: {
        return "IPV6";
    } break;
    case e_ICMP: {
        return "ICMP";
    } break;
    case e_IGMP: {
        return "IGMP";
    } break;
    case e_TCP: {
        return "TCP";
    } break;
    case e_UDP: {
        return "UDP";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& PacketType::print(bsl::ostream& stream, PacketType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, PacketType::Value rhs)
{
    return PacketType::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
