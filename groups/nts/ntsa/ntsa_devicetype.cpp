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

#include <ntsa_devicetype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_devicetype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int DeviceType::fromInt(DeviceType::Value* result, int number)
{
    switch (number) {
    case DeviceType::e_UNDEFINED:
    case DeviceType::e_LOCAL:
    case DeviceType::e_LOOPBACK:
    case DeviceType::e_ETHERNET:
    case DeviceType::e_WIRELESS:
    case DeviceType::e_IP:
        *result = static_cast<DeviceType::Value>(number);
        return 0;
    default:
        *result = DeviceType::e_UNDEFINED;
        return -1;
    }
}

int DeviceType::fromString(DeviceType::Value*          result,
                         const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LOCAL")) {
        *result = e_LOCAL;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LOOPBACK")) {
        *result = e_LOOPBACK;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ETHERNET")) {
        *result = e_ETHERNET;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "WIRELESS")) {
        *result = e_WIRELESS;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "IP")) {
        *result = e_IP;
        return 0;
    }

    *result = e_UNDEFINED;
    return -1;
}

const char* DeviceType::toString(DeviceType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_LOCAL: {
        return "LOCAL";
    } break;
    case e_LOOPBACK: {
        return "LOOPBACK";
    } break;
    case e_ETHERNET: {
        return "ETHERNET";
    } break;
    case e_WIRELESS: {
        return "WIRELESS";
    } break;
    case e_IP: {
        return "IP";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& DeviceType::print(bsl::ostream& stream, DeviceType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, DeviceType::Value rhs)
{
    return DeviceType::print(stream, rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
