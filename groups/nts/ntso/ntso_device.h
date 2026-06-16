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

#ifndef INCLUDED_NTSO_DEVICE
#define INCLUDED_NTSO_DEVICE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_deviceconfig.h>
#include <ntscfg_config.h>
#include <ntscfg_platform.h>
#include <ntsi_device.h>
#include <ntsscm_version.h>
#include <bsl_memory.h>

// #if NTS_BUILD_WITH_RAW_SOCKETS
#define NTSO_DEVICE_ENABLED 1
// #else
// #define NTSO_DEVICE_ENABLED 0
// #endif

#if NTSO_DEVICE_ENABLED
namespace BloombergLP {
namespace ntso {

/// @internal @brief
/// Provide utilities for creating devices implemented using the 'native' API
/// on all platforms.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntso
class DeviceUtil
{
  public:
    /// Enumerate the constants used by this implementation.
    enum Constants {
        k_DEFAULT_OUTGOING_MIN_THREADS = 1,
        k_DEFAULT_OUTGOING_MAX_THREADS = 1,
        k_DEFAULT_OUTGOING_MAX_PACKETS = 100000,
        k_DEFAULT_INCOMING_MIN_THREADS = 1,
        k_DEFAULT_INCOMING_MAX_THREADS = 1,
        k_DEFAULT_INCOMING_MAX_PACKETS = 100000,
    };

    /// Create a new device. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntsi::Device> createDevice(
        const ntsa::DeviceConfig& configuration,
        bslma::Allocator*         basicAllocator = 0);

    /// Sanitize the specified 'configuration'.
    static void sanitizeConfig(ntsa::DeviceConfig* configuration);

    /// Load into the specified 'result' the adapter for the specified
    /// 'configuration'. Return the error.
    static ntsa::Error resolveAdapter(ntsa::Adapter*            result,
                                      const ntsa::DeviceConfig& configuration);

    /// Load into the specified 'result' the adapter in the specified
    /// 'adapterList' having the specified 'name'. Return the error.
    static ntsa::Error resolveAdapter(
        ntsa::Adapter*                    result,
        const bsl::vector<ntsa::Adapter>& adapterList,
        const bsl::string&                name);

    /// Load into the specified 'result' the adapter in the specified
    /// 'adapterList' having the specified 'index'. Return the error.
    static ntsa::Error resolveAdapter(
        ntsa::Adapter*                    result,
        const bsl::vector<ntsa::Adapter>& adapterList,
        bsl::uint32_t                     index);

    /// Load into the specified 'result' the adapter in the specified
    /// 'adapterList' having the specified 'ethernetAddress'. Return the
    /// error.
    static ntsa::Error resolveAdapter(
        ntsa::Adapter*                    result,
        const bsl::vector<ntsa::Adapter>& adapterList,
        const ntsa::EthernetAddress&      ethernetAddress);

    /// Load into the specified 'result' the adapter in the specified
    /// 'adapterList' having the specified 'ipv4Address'. Return the error.
    static ntsa::Error resolveAdapter(
        ntsa::Adapter*                    result,
        const bsl::vector<ntsa::Adapter>& adapterList,
        const ntsa::Ipv4Address&          ipv4Address);

    /// Load into the specified 'result' the adapter in the specified
    /// 'adapterList' having the specified 'ipv4Address'. Return the error.
    static ntsa::Error resolveAdapter(
        ntsa::Adapter*                    result,
        const bsl::vector<ntsa::Adapter>& adapterList,
        const ntsa::Ipv6Address&          ipv6Address);

    /// Validate the specified 'adapter' is compatible with the specified
    /// 'configuration'. Return the error.
    static ntsa::Error validateAdapter(
        const ntsa::Adapter&      adapter,
        const ntsa::DeviceConfig& configuration);

    /// Return true if the device is supported, otherwise return false.
    static bool isSupported();
};

}  // close package namespace
}  // close enterprise namespace
#endif
#endif
