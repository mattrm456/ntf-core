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

#include <ntsa_ipv4optiontype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4optiontype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int Ipv4OptionType::fromInt(Ipv4OptionType::Value* result, int number)
{
    switch (number) {
    case Ipv4OptionType::e_UNDEFINED:
    case Ipv4OptionType::e_PADDING:
    case Ipv4OptionType::e_ALERT:
    case Ipv4OptionType::e_TIMESTAMP:
    case Ipv4OptionType::e_RECORD_ROUTE:
    case Ipv4OptionType::e_SOURCE_ROUTE_LOOSE:
    case Ipv4OptionType::e_SOURCE_ROUTE_TIGHT:
    case Ipv4OptionType::e_UNASSIGNED:
        *result = static_cast<Ipv4OptionType::Value>(number);
        return 0;
    default:
        *result = Ipv4OptionType::e_UNDEFINED;
        return -1;
    }
}

int Ipv4OptionType::fromString(Ipv4OptionType::Value*  result,
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
    if (bdlb::String::areEqualCaseless(string, "ALERT")) {
        *result = e_ALERT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TIMESTAMP")) {
        *result = e_TIMESTAMP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RECORD_ROUTE")) {
        *result = e_RECORD_ROUTE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SOURCE_ROUTE_LOOSE")) {
        *result = e_SOURCE_ROUTE_LOOSE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SOURCE_ROUTE_TIGHT")) {
        *result = e_SOURCE_ROUTE_TIGHT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "UNASSIGNED")) {
        *result = e_UNASSIGNED;
        return 0;
    }

    *result = e_UNDEFINED;
    return -1;
}

const char* Ipv4OptionType::toString(Ipv4OptionType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_PADDING: {
        return "PADDING";
    } break;
    case e_ALERT: {
        return "ALERT";
    } break;
    case e_TIMESTAMP: {
        return "TIMESTAMP";
    } break;
    case e_RECORD_ROUTE: {
        return "RECORD_ROUTE";
    } break;
    case e_SOURCE_ROUTE_LOOSE: {
        return "SOURCE_ROUTE_LOOSE";
    } break;
    case e_SOURCE_ROUTE_TIGHT: {
        return "SOURCE_ROUTE_TIGHT";
    } break;
    case e_UNASSIGNED: {
        return "UNASSIGNED";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& Ipv4OptionType::print(bsl::ostream&          stream,
                                     Ipv4OptionType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, Ipv4OptionType::Value rhs)
{
    return Ipv4OptionType::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
