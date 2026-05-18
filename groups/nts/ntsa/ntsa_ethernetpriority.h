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

#ifndef INCLUDED_NTSA_ETHERNETPRIORITY
#define INCLUDED_NTSA_ETHERNETPRIORITY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Enumerate the Ethernet priorities.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_protocol
struct EthernetPriority {
  public:
    /// Enumerate the Ethernet priorities.
    enum Value {
        /// Best effort delivery, the default traffic class for standard data
        /// and the lowest default priority.
        e_DEFAULT = 0,

        /// Bulk data transfers, emails, or low-priority downloads.
        e_BACKGROUND = 1,

        /// General data deemed important but not strictly time-sensitive.
        e_IMPORTANT = 2,

        /// Business-critical applications or transactional data.
        e_CRITICAL = 3,

        /// Video streaming with strict latency limits.
        e_VIDEO = 4,

        /// Voice over IP (VoIP) with very strict latency limits.
        e_VOICE = 5,

        /// Network management and internetworking controls.
        e_CONTROL = 6,

        /// Network critical operations such as routing protocols and spanning
        /// tree updates.
        e_ROUTING = 7
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
/// @related ntsa::EthernetPriority
bsl::ostream& operator<<(bsl::ostream& stream, EthernetPriority::Value rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
