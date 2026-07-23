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

#include <ntsa_tcpoptiontype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcpoptiontype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int TcpOptionType::fromInt(TcpOptionType::Value* result, int number)
{
    switch (number) {
    case TcpOptionType::e_UNDEFINED:
    case TcpOptionType::e_PADDING:
    case TcpOptionType::e_MAX_SEGMENT_SIZE:
    case TcpOptionType::e_WINDOW_SCALE:
    case TcpOptionType::e_SELECTIVE_ACK_PERMITTED:
    case TcpOptionType::e_SELECTIVE_ACK:
    case TcpOptionType::e_TIMESTAMP:
    case TcpOptionType::e_FAST_OPEN:
    case TcpOptionType::e_UNASSIGNED:
        *result = static_cast<TcpOptionType::Value>(number);
        return 0;
    default:
        *result = TcpOptionType::e_UNDEFINED;
        return -1;
    }
}

int TcpOptionType::fromString(TcpOptionType::Value*    result,
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
    if (bdlb::String::areEqualCaseless(string, "MAX_SEGMENT_SIZE")) {
        *result = e_MAX_SEGMENT_SIZE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "WINDOW_SCALE")) {
        *result = e_WINDOW_SCALE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SELECTIVE_ACK_PERMITTED")) {
        *result = e_SELECTIVE_ACK_PERMITTED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SELECTIVE_ACK")) {
        *result = e_SELECTIVE_ACK;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TIMESTAMP")) {
        *result = e_TIMESTAMP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "FAST_OPEN")) {
        *result = e_FAST_OPEN;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "UNASSIGNED")) {
        *result = e_UNASSIGNED;
        return 0;
    }

    *result = e_UNDEFINED;
    return -1;
}

const char* TcpOptionType::toString(TcpOptionType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_PADDING: {
        return "PADDING";
    } break;
    case e_MAX_SEGMENT_SIZE: {
        return "MAX_SEGMENT_SIZE";
    } break;
    case e_WINDOW_SCALE: {
        return "WINDOW_SCALE";
    } break;
    case e_SELECTIVE_ACK_PERMITTED: {
        return "SELECTIVE_ACK_PERMITTED";
    } break;
    case e_SELECTIVE_ACK: {
        return "SELECTIVE_ACK";
    } break;
    case e_TIMESTAMP: {
        return "TIMESTAMP";
    } break;
    case e_FAST_OPEN: {
        return "FAST_OPEN";
    } break;
    case e_UNASSIGNED: {
        return "UNASSIGNED";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& TcpOptionType::print(bsl::ostream&        stream,
                                   TcpOptionType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TcpOptionType::Value rhs)
{
    return TcpOptionType::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
