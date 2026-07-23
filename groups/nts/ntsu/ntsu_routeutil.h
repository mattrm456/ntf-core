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

#ifndef INCLUDED_NTSU_ROUTEUTIL
#define INCLUDED_NTSU_ROUTEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_adapter.h>
#include <ntsa_error.h>
#include <ntsa_ethernetaddress.h>
#include <ntsa_ethernetroute.h>
#include <ntsa_ethernetroutetable.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv4route.h>
#include <ntsa_ipv4routetable.h>
#include <ntsa_ipv6address.h>
#include <ntsa_ipv6route.h>
#include <ntsa_ipv6routetable.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <ball_log.h>

namespace BloombergLP {
namespace ntsu {

/// @internal @brief
/// Provide utilities for discovering route tables.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsu
class RouteUtil
{
    /// Provide a private, platform-specific implementation of utilities for
    /// discovering route tables.
    class Impl;

    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.ROUTEUTIL");

  public:
    /// Load the Ethernet route table into the specified 'result'. Return the
    /// error.
    static ntsa::Error load(ntsa::EthernetRouteTable* result);

    /// Load the Ethernet route table for the adapters in the specified
    /// 'adapterVector' into the specified 'result'. Return the error.
    static ntsa::Error load(ntsa::EthernetRouteTable*         result,
                            const bsl::vector<ntsa::Adapter>& adapterVector);

    /// Load the IPv4 route table into the specified 'result'. Return the
    /// error.
    static ntsa::Error load(ntsa::Ipv4RouteTable* result);

    /// Load the IPv4 route table into the specified 'result' using the
    /// specified 'ethernetRouteTable' for the adapters in the specified
    /// 'adapterVector'. Return the error.
    static ntsa::Error load(
        ntsa::Ipv4RouteTable*             result,
        const bsl::vector<ntsa::Adapter>& adapterVector,
        const ntsa::EthernetRouteTable&   ethernetRouteTable);

    /// Load the IPv6 route table into the specified 'result'. Return the
    /// error.
    static ntsa::Error load(ntsa::Ipv6RouteTable* result);

    /// Load the IPv6 route table into the specified 'result' using the
    /// specified 'ethernetRouteTable' for the adapters in the specified
    /// 'adapterVector'. Return the error.
    static ntsa::Error load(
        ntsa::Ipv6RouteTable*             result,
        const bsl::vector<ntsa::Adapter>& adapterVector,
        const ntsa::EthernetRouteTable&   ethernetRouteTable);
};

}  // end namespace ntsu
}  // end namespace BloombergLP
#endif
