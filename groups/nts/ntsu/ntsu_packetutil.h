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

#ifndef INCLUDED_NTSU_PACKETUTIL
#define INCLUDED_NTSU_PACKETUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_adapter.h>
#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <ball_log.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsu {

/// @internal @brief
/// Provide utilities for processing packets and packet filters.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsu
class PacketUtil {
    /// Provide a private, platform-specific implementation of utilities for
    /// processing packets and packet filters.
    class Impl;

    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.PACKETUTIL");

public:

};

}  // end namespace ntsu
}  // end namespace BloombergLP
#endif
