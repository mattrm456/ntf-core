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

#ifndef INCLUDED_NTSA_PACKETTYPE
#define INCLUDED_NTSA_PACKETTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Enumerate the packet types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_protocol
struct PacketType {
  public:
    /// Enumerate the packet types.
    enum Value {
        /// The packet type is not defined.
        e_UNDEFINED = 0,

        /// The protocol of the packet is the the ethernet protocol.
        e_ETHERNET = 1,

        /// The packet is an Address Resolution Protocol (ARP) packet.
        e_ARP = 2,

        /// The packet is a Reverse Address Resolution Protocol (RARP) packet.
        e_RARP = 3,

        /// The packet is an Internet Protocol, version 4 (IPv4) packet.
        e_IPV4 = 4,

        /// The packet is an Internet Protocol, version 6 (IPv6) packet.
        e_IPV6 = 5,

        /// The packet is an Internet Control Message Protocol (ICMP) packet.
        e_ICMP = 6,

        /// The packet is an Internet Group Management Protocol (IGMP) packet.
        e_IGMP = 7,

        /// The packet is a Transmission Control Protocol (TCP) packet.
        e_TCP = 8,

        /// The packet is a User Datagram Protocol (UDP) packet.
        e_UDP = 9
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
/// @related ntsa::PacketType
bsl::ostream& operator<<(bsl::ostream& stream, PacketType::Value rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
