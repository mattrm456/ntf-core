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

#ifndef INCLUDED_NTSA_IGMPRECORDTYPE
#define INCLUDED_NTSA_IGMPRECORDTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Enumerate the IGMPv3 group record types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_protocol
struct IgmpRecordType {
  public:
    /// Enumerate the IGMPv3 group record types.
    enum Value {
        /// The group record type is not defined.
        e_UNDEFINED = 0,

        /// Current-State Record indicating the interface has a filter
        /// mode of INCLUDE for the specified multicast address.
        e_MODE_IS_INCLUDE = 1,

        /// Current-State Record indicating the interface has a filter
        /// mode of EXCLUDE for the specified multicast address.
        e_MODE_IS_EXCLUDE = 2,

        /// Filter-Mode-Change Record indicating the interface has
        /// changed to INCLUDE filter mode for the specified multicast
        /// address.
        e_CHANGE_TO_INCLUDE_MODE = 3,

        /// Filter-Mode-Change Record indicating the interface has
        /// changed to EXCLUDE filter mode for the specified multicast
        /// address.
        e_CHANGE_TO_EXCLUDE_MODE = 4,

        /// Source-List-Change Record indicating that the source
        /// addresses listed should be added to the existing source list
        /// for the specified multicast address.
        e_ALLOW_NEW_SOURCES = 5,

        /// Source-List-Change Record indicating that the source
        /// addresses listed should be removed from the existing source
        /// list for the specified multicast address.
        e_BLOCK_OLD_SOURCES = 6
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
/// @related ntsa::IgmpRecordType
bsl::ostream& operator<<(bsl::ostream& stream, IgmpRecordType::Value rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
