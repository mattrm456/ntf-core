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

#include <ntsa_ethernetpriority.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetpriority_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int EthernetPriority::fromInt(EthernetPriority::Value* result, int number)
{
    switch (number) {
    case EthernetPriority::e_DEFAULT:
    case EthernetPriority::e_BACKGROUND:
    case EthernetPriority::e_IMPORTANT:
    case EthernetPriority::e_CRITICAL:
    case EthernetPriority::e_VIDEO:
    case EthernetPriority::e_VOICE:
    case EthernetPriority::e_CONTROL:
    case EthernetPriority::e_ROUTING:
        *result = static_cast<EthernetPriority::Value>(number);
        return 0;
    default:
        *result = EthernetPriority::e_DEFAULT;
        return -1;
    }
}

int EthernetPriority::fromString(EthernetPriority::Value* result,
                                 const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "DEFAULT")) {
        *result = e_DEFAULT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BACKGROUND")) {
        *result = e_BACKGROUND;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "IMPORTANT")) {
        *result = e_IMPORTANT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CRITICAL")) {
        *result = e_CRITICAL;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "VIDEO")) {
        *result = e_VIDEO;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "VOICE")) {
        *result = e_VOICE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CONTROL")) {
        *result = e_CONTROL;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ROUTING")) {
        *result = e_ROUTING;
        return 0;
    }

    *result = e_DEFAULT;
    return -1;
}

const char* EthernetPriority::toString(EthernetPriority::Value value)
{
    switch (value) {
    case e_DEFAULT: {
        return "DEFAULT";
    } break;
    case e_BACKGROUND: {
        return "BACKGROUND";
    } break;
    case e_IMPORTANT: {
        return "IMPORTANT";
    } break;
    case e_CRITICAL: {
        return "CRITICAL";
    } break;
    case e_VIDEO: {
        return "VIDEO";
    } break;
    case e_VOICE: {
        return "VOICE";
    } break;
    case e_CONTROL: {
        return "CONTROL";
    } break;
    case e_ROUTING: {
        return "ROUTING";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& EthernetPriority::print(bsl::ostream&           stream,
                                      EthernetPriority::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, EthernetPriority::Value rhs)
{
    return EthernetPriority::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
