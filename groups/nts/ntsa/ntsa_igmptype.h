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

#ifndef INCLUDED_NTSA_IGMPTYPE
#define INCLUDED_NTSA_IGMPTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Enumerate the IGMP message types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_protocol
struct IgmpType {
  public:
    /// Enumerate the IGMP message types.
    enum Value {
        /// The IGMP message type is not defined.
        e_UNDEFINED = 255,

        /// Membership query as defined in both IGMPv2 and IGMPv3.
        e_QUERY = 0x11,

        /// Membership report as defined in IGMPv1.
        e_REPORT_V1 = 0x12,

        /// Membership report as defined in IGMPv2.
        e_REPORT_V2 = 0x16,

        /// Membership report as defined in IGMPv3.
        e_REPORT_V3 = 0x22,

        /// Leave group as defined in IGMPv2.
        e_LEAVE = 0x17
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
/// @related ntsa::IgmpType
bsl::ostream& operator<<(bsl::ostream& stream, IgmpType::Value rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
