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

#include <ntsa_arptype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_arptype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int ArpType::fromInt(ArpType::Value* result, int number)
{
    switch (number) {
    case ArpType::e_UNDEFINED:
    case ArpType::e_REQUEST:
    case ArpType::e_RESPONSE:
        *result = static_cast<ArpType::Value>(number);
        return 0;
    default:
        *result = ArpType::e_UNDEFINED;
        return -1;
    }
}

int ArpType::fromString(ArpType::Value*          result,
                         const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "REQUEST")) {
        *result = e_REQUEST;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RESPONSE")) {
        *result = e_RESPONSE;
        return 0;
    }

    *result = e_UNDEFINED;
    return -1;
}

const char* ArpType::toString(ArpType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_REQUEST: {
        return "REQUEST";
    } break;
    case e_RESPONSE: {
        return "RESPONSE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ArpType::print(bsl::ostream& stream, ArpType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ArpType::Value rhs)
{
    return ArpType::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
