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

#include <ntsa_udpoptiontype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_udpoptiontype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int UdpOptionType::fromInt(UdpOptionType::Value* result, int number)
{
    switch (number) {
    case UdpOptionType::e_UNDEFINED:
    case UdpOptionType::e_PADDING:
    case UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM:
    case UdpOptionType::e_FRAGMENTATION:
    case UdpOptionType::e_MAX_DATAGRAM_SIZE:
    case UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE:
    case UdpOptionType::e_ECHO_REQUEST:
    case UdpOptionType::e_ECHO_RESPONSE:
    case UdpOptionType::e_TIMESTAMP:
        *result = static_cast<UdpOptionType::Value>(number);
        return 0;
    default:
        *result = UdpOptionType::e_UNDEFINED;
        return -1;
    }
}

int UdpOptionType::fromString(UdpOptionType::Value*  result,
                                const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "PADDING")) {
        *result = e_PADDING;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ADDITIONAL_PAYLOAD_CHECKSUM")) {
        *result = e_ADDITIONAL_PAYLOAD_CHECKSUM;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "FRAGMENTATION")) {
        *result = e_FRAGMENTATION;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "MAX_DATAGRAM_SIZE")) {
        *result = e_MAX_DATAGRAM_SIZE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "MAX_REASSEMBLED_DATAGRAM_SIZE")) {
        *result = e_MAX_REASSEMBLED_DATAGRAM_SIZE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ECHO_REQUEST")) {
        *result = e_ECHO_REQUEST;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ECHO_RESPONSE")) {
        *result = e_ECHO_RESPONSE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TIMESTAMP")) {
        *result = e_TIMESTAMP;
        return 0;
    }

    *result = e_UNDEFINED;
    return -1;
}

const char* UdpOptionType::toString(UdpOptionType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_PADDING: {
        return "PADDING";
    } break;
    case e_ADDITIONAL_PAYLOAD_CHECKSUM: {
        return "ADDITIONAL_PAYLOAD_CHECKSUM";
    } break;
    case e_FRAGMENTATION: {
        return "FRAGMENTATION";
    } break;
    case e_MAX_DATAGRAM_SIZE: {
        return "MAX_DATAGRAM_SIZE";
    } break;
    case e_MAX_REASSEMBLED_DATAGRAM_SIZE: {
        return "MAX_REASSEMBLED_DATAGRAM_SIZE";
    } break;
    case e_ECHO_REQUEST: {
        return "ECHO_REQUEST";
    } break;
    case e_ECHO_RESPONSE: {
        return "ECHO_RESPONSE";
    } break;
    case e_TIMESTAMP: {
        return "TIMESTAMP";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& UdpOptionType::print(bsl::ostream&          stream,
                                     UdpOptionType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, UdpOptionType::Value rhs)
{
    return UdpOptionType::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
