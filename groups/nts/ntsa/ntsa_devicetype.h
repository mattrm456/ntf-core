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

#ifndef INCLUDED_NTSA_DEVICETYPE
#define INCLUDED_NTSA_DEVICETYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Enumerate the device types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_protocol
struct DeviceType {
  public:
    /// Enumerate the device types.
    enum Value {
        /// The device type is not defined.
        e_UNDEFINED = 0,

        /// The device type is a loopback device using a 4-byte header is in
        /// host byte order (DLT_NULL).
        e_LOCAL = 1,

        /// The device type is a loopback device using a 4-byte header in
        /// network byte order (DLT_LOOP).
        e_LOOPBACK = 2,

        /// The device type uses IEEE 802.3 wired MAC frames (DLT_EN10MB).
        e_ETHERNET = 3,

        /// The device type uses IEEE 802.11 wireless MAC frames
        /// (DLT_IEEE802_11_RADIO).
        e_WIRELESS = 4,

        /// The device type uses raw IP packets (DLT_RAW).
        e_IP = 5
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
/// @related ntsa::DeviceType
bsl::ostream& operator<<(bsl::ostream& stream, DeviceType::Value rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
