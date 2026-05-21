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

#ifndef INCLUDED_NTSA_IPV4OPTIONTYPE
#define INCLUDED_NTSA_IPV4OPTIONTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Enumerate the Internet Protocol version 4 (IPv4) option types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_protocol
struct Ipv4OptionType {
  public:
    /// Enumerate the Internet Protocol version 4 (IPv4) event types.
    enum Value {
        /// The end option type. This option type indicates the end of the
        /// options list.
        e_UNDEFINED = 0,

        /// The padding option type. This option type indicates padding between
        /// two options or between an option and the end of the options list.
        e_PADDING = 1,

        /// The router alert option type. This option instructs a router to
        /// examine the packet more closely.
        e_ALERT = 148,

        /// The timestamp option type. This option instructs routers to record
        /// timestamps at each hop to measure network latency and delays.
        e_TIMESTAMP = 68,

        /// The record route option type. This option instructs routers to
        /// append their IP addresses to the packet header, tracing the exact
        /// path taken.
        e_RECORD_ROUTE = 7,

        /// The loose source route option type. This option specifies a list of
        /// routers the packet must visit, allowing for intermediate nodes.
        e_SOURCE_ROUTE_LOOSE = 131,

        /// The strict source route option type. This option forces the packet
        /// to follow a rigidly defined list of routers.
        e_SOURCE_ROUTE_TIGHT = 137
    };

    /// Return the string representation exactly matching the enumerator name
    /// corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the specified
    /// 'string'.  Return 0 on success, and a non-zero value with no effect on
    /// 'result' otherwise (i.e., 'string' does not match any enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the specified
    /// 'number'.  Return 0 on success, and a non-zero value with no effect on
    /// 'result' otherwise (i.e., 'number' does not match any enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::Ipv4OptionType
bsl::ostream& operator<<(bsl::ostream& stream, Ipv4OptionType::Value rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
