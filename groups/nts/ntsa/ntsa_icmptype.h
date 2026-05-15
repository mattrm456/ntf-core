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

#ifndef INCLUDED_NTSA_ICMPTYPE
#define INCLUDED_NTSA_ICMPTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Enumerate the ICMP message types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_protocol
struct IcmpType {
  public:
    /// Enumerate the ICMP message types.
    enum Value {
        /// The ICMP message type is not defined.
        e_UNDEFINED = 255,

        /// Echo request (type 8). Sent by a host to verify that a
        /// destination is reachable and to measure round-trip latency.
        e_ECHO_REQUEST = 8,

        /// Echo response (type 0). Sent by a host in response to an echo
        /// request to confirm reachability.
        e_ECHO_RESPONSE = 0,

        /// Router solicitation (type 10). Sent by a host to discover available
        /// routers.
        e_ROUTER_REQUEST = 10,

        /// Router advertisement (type 9). Sent by a router to to announce
        /// itself to hosts.
        e_ROUTER_RESPONSE = 9,

        /// Redirect (type 5). Sent by a gateway to inform a source host that
        /// a better route to a destination exists.
        e_REDIRECT = 5,

        /// Destination unreachable (type 3). Sent by a gateway or host when
        /// a datagram cannot be delivered to its destination.
        e_UNREACHABLE = 3,

        /// Time exceeded (type 11). Sent by a gateway when a datagram's
        /// time-to-live reaches zero, or by a host when fragment reassembly
        /// times out.
        e_TIMEOUT = 11,

        /// Parameter problem (type 12). Sent by a gateway or host when an
        /// error in the IP header parameters causes a datagram to be
        /// discarded.
        e_PROBLEM = 12
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
/// @related ntsa::IcmpType
bsl::ostream& operator<<(bsl::ostream& stream, IcmpType::Value rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
