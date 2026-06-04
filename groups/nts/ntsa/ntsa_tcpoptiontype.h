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

#ifndef INCLUDED_NTSA_TCPOPTIONTYPE
#define INCLUDED_NTSA_TCPOPTIONTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Enumerate the TCP option types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_protocol
struct TcpOptionType {
  public:
    /// Enumerate the accept event types.
    enum Value {
        /// The end option type. This option type indicates the end of the
        /// options list.
        e_UNDEFINED = 0,

        /// The padding option type. This option type indicates padding between
        /// two options or between an option and the end of the options list.
        e_PADDING = 1,

        /// The maximum segment size (MSS) option type.
        e_MAX_SEGMENT_SIZE = 2,

        /// The window scale option type.
        e_WINDOW_SCALE = 3,

        /// The selective acknowledgement (SACK) permitted option type.
        e_SELECTIVE_ACK_PERMITTED = 4,

        /// The selective acknowledgement (SACK) option type.
        e_SELECTIVE_ACK = 5,

        /// The timestamp option type.
        e_TIMESTAMP = 8,

        /// The fast open option type.
        e_FAST_OPEN = 34,

        /// The any option type, to represent unassigned or unrecognized
        /// options.
        e_UNASSIGNED = 255
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
/// @related ntsa::TcpOptionType
bsl::ostream& operator<<(bsl::ostream& stream, TcpOptionType::Value rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
