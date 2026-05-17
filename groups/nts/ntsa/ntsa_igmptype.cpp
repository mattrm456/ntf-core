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

#include <ntsa_igmptype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_igmptype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int IgmpType::fromInt(IgmpType::Value* result, int number)
{
    switch (number) {
    case IgmpType::e_UNDEFINED:
    case IgmpType::e_QUERY:
    case IgmpType::e_REPORT_V1:
    case IgmpType::e_REPORT_V2:
    case IgmpType::e_REPORT_V3:
    case IgmpType::e_LEAVE:
        *result = static_cast<IgmpType::Value>(number);
        return 0;
    default:
        *result = IgmpType::e_UNDEFINED;
        return -1;
    }
}

int IgmpType::fromString(IgmpType::Value*          result,
                         const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "QUERY")) {
        *result = e_QUERY;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "REPORT_V1")) {
        *result = e_REPORT_V1;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "REPORT_V2")) {
        *result = e_REPORT_V2;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "REPORT_V3")) {
        *result = e_REPORT_V3;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LEAVE")) {
        *result = e_LEAVE;
        return 0;
    }

    *result = e_UNDEFINED;
    return -1;
}

const char* IgmpType::toString(IgmpType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_QUERY: {
        return "QUERY";
    } break;
    case e_REPORT_V1: {
        return "REPORT_V1";
    } break;
    case e_REPORT_V2: {
        return "REPORT_V2";
    } break;
    case e_REPORT_V3: {
        return "REPORT_V3";
    } break;
    case e_LEAVE: {
        return "LEAVE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& IgmpType::print(bsl::ostream& stream, IgmpType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, IgmpType::Value rhs)
{
    return IgmpType::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
