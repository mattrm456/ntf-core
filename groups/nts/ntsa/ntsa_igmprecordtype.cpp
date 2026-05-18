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

#include <ntsa_igmprecordtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_igmprecordtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int IgmpRecordType::fromInt(IgmpRecordType::Value* result, int number)
{
    switch (number) {
    case IgmpRecordType::e_UNDEFINED:
    case IgmpRecordType::e_MODE_IS_INCLUDE:
    case IgmpRecordType::e_MODE_IS_EXCLUDE:
    case IgmpRecordType::e_CHANGE_TO_INCLUDE_MODE:
    case IgmpRecordType::e_CHANGE_TO_EXCLUDE_MODE:
    case IgmpRecordType::e_ALLOW_NEW_SOURCES:
    case IgmpRecordType::e_BLOCK_OLD_SOURCES:
        *result = static_cast<IgmpRecordType::Value>(number);
        return 0;
    default:
        *result = IgmpRecordType::e_UNDEFINED;
        return -1;
    }
}

int IgmpRecordType::fromString(IgmpRecordType::Value*   result,
                               const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "MODE_IS_INCLUDE")) {
        *result = e_MODE_IS_INCLUDE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "MODE_IS_EXCLUDE")) {
        *result = e_MODE_IS_EXCLUDE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CHANGE_TO_INCLUDE_MODE")) {
        *result = e_CHANGE_TO_INCLUDE_MODE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CHANGE_TO_EXCLUDE_MODE")) {
        *result = e_CHANGE_TO_EXCLUDE_MODE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ALLOW_NEW_SOURCES")) {
        *result = e_ALLOW_NEW_SOURCES;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BLOCK_OLD_SOURCES")) {
        *result = e_BLOCK_OLD_SOURCES;
        return 0;
    }

    *result = e_UNDEFINED;
    return -1;
}

const char* IgmpRecordType::toString(IgmpRecordType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_MODE_IS_INCLUDE: {
        return "MODE_IS_INCLUDE";
    } break;
    case e_MODE_IS_EXCLUDE: {
        return "MODE_IS_EXCLUDE";
    } break;
    case e_CHANGE_TO_INCLUDE_MODE: {
        return "CHANGE_TO_INCLUDE_MODE";
    } break;
    case e_CHANGE_TO_EXCLUDE_MODE: {
        return "CHANGE_TO_EXCLUDE_MODE";
    } break;
    case e_ALLOW_NEW_SOURCES: {
        return "ALLOW_NEW_SOURCES";
    } break;
    case e_BLOCK_OLD_SOURCES: {
        return "BLOCK_OLD_SOURCES";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& IgmpRecordType::print(bsl::ostream&         stream,
                                    IgmpRecordType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, IgmpRecordType::Value rhs)
{
    return IgmpRecordType::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
