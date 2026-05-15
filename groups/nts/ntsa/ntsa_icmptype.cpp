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

#include <ntsa_icmptype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_icmptype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int IcmpType::fromInt(IcmpType::Value* result, int number)
{
    switch (number) {
    case IcmpType::e_UNDEFINED:
    case IcmpType::e_ECHO_REQUEST:
    case IcmpType::e_ECHO_RESPONSE:
    case IcmpType::e_ROUTER_REQUEST:
    case IcmpType::e_ROUTER_RESPONSE:
    case IcmpType::e_REDIRECT:
    case IcmpType::e_UNREACHABLE:
    case IcmpType::e_TIMEOUT:
    case IcmpType::e_PROBLEM:
        *result = static_cast<IcmpType::Value>(number);
        return 0;
    default:
        *result = IcmpType::e_UNDEFINED;
        return -1;
    }
}

int IcmpType::fromString(IcmpType::Value*          result,
                         const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
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
    if (bdlb::String::areEqualCaseless(string, "ROUTER_REQUEST")) {
        *result = e_ROUTER_REQUEST;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ROUTER_RESPONSE")) {
        *result = e_ROUTER_RESPONSE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "REDIRECT")) {
        *result = e_REDIRECT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "UNREACHABLE")) {
        *result = e_UNREACHABLE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TIMEOUT")) {
        *result = e_TIMEOUT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "PROBLEM")) {
        *result = e_PROBLEM;
        return 0;
    }

    *result = e_UNDEFINED;
    return -1;
}

const char* IcmpType::toString(IcmpType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_ECHO_REQUEST: {
        return "ECHO_REQUEST";
    } break;
    case e_ECHO_RESPONSE: {
        return "ECHO_RESPONSE";
    } break;
    case e_ROUTER_REQUEST: {
        return "ROUTER_REQUEST";
    } break;
    case e_ROUTER_RESPONSE: {
        return "ROUTER_RESPONSE";
    } break;
    case e_REDIRECT: {
        return "REDIRECT";
    } break;
    case e_UNREACHABLE: {
        return "UNREACHABLE";
    } break;
    case e_TIMEOUT: {
        return "TIMEOUT";
    } break;
    case e_PROBLEM: {
        return "PROBLEM";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& IcmpType::print(bsl::ostream& stream, IcmpType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, IcmpType::Value rhs)
{
    return IcmpType::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
