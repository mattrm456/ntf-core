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
#include <ntsa_ethernetaddress.h>
#include <ntsa_ethernetroute.h>
#include <ntsa_ethernetroutetable.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv4route.h>
#include <ntsa_ipv4routetable.h>
#include <ntsa_ipv6address.h>
#include <ntsa_ipv6route.h>
#include <ntsa_ipv6routetable.h>
#include <ntscfg_config.h>
#include <ntscfg_platform.h>
#include <ntsi_device.h>
#include <ntsscm_version.h>
#include <bsl_memory.h>
#include <ball_log.h>

// #if NTS_BUILD_WITH_RAW_SOCKETS
#define NTSO_DEVICE_ENABLED 1
// #else
// #define NTSO_DEVICE_ENABLED 0
// #endif

#if NTSO_DEVICE_ENABLED
namespace BloombergLP {
namespace ntso {

/// @internal @brief
/// Provide utilities for creating devices.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntso
class DeviceUtil
{
    BALL_LOG_SET_CLASS_CATEGORY("NTSO.DEVICE");

    /// Provide a implementation of the device utilities.
    class Impl;

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

    /// Return true if the device is supported, otherwise return false.
    static bool isSupported();
};

/// @internal @brief
/// Provide a tree of devices.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntso
class Network : public ntsi::Network
{
    /// Enumerates the constants used by this implementation.
    enum Constants {
        /// The default maximum transmission unit.
        k_MTU = 1500
    };

    /// The blob buffer factory.
    bsl::shared_ptr<bdlbb::BlobBufferFactory> d_blobBufferFactory;

    /// The adapter vector.
    bsl::vector<ntsa::Adapter> d_adapterVector;

    /// The Ethernet route table.
    ntsa::EthernetRouteTable d_ethernetRouteTable;

    /// The IPv4 route table.
    ntsa::Ipv4RouteTable d_ipv4RouteTable;

    /// The IPv6 route table.
    ntsa::Ipv6RouteTable d_ipv6RouteTable;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSO.DEVICE");

  private:
    Network(const Network&) BSLS_KEYWORD_DELETED;
    Network& operator=(const Network&) BSLS_KEYWORD_DELETED;

  private:
    /// Prepare the specified 'ethernet' and 'ipv4' header for a transmission
    /// to the specified 'destinationIpv4Address'.
    ntsa::Error prepare(ntsa::EthernetHeader*    ethernet,
                        ntsa::Ipv4Header*        ipv4,
                        const ntsa::Ipv4Address& destinationIpv4Address);

    /// Prepare the specified 'ethernet' and 'ipv6' header for a transmission
    /// to the specified 'destinationIpv4Address'.
    ntsa::Error prepare(ntsa::EthernetHeader*    ethernet,
                        ntsa::Ipv6Header*        ipv6,
                        const ntsa::Ipv6Address& destinationIpv6Address);

  public:
    /// Create a new network. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Network(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Network() BSLS_KEYWORD_OVERRIDE;

    /// Open the device.
    ntsa::Error open() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'buffer' a new buffer whose size is the
    /// maximum transmission unit of this device.
    ntsa::Error allocate(bdlbb::BlobBuffer* buffer) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'packet' a new packets to the specified
    /// 'destinationIpv4Address' with the source IPv4 address, source Ethernet
    /// address, and destination Ethernet address of theh 'packet'
    /// automatically assigned to the correct addresses according to the
    /// current IPv4 routing table. Return the error.
    ntsa::Error allocate(
        ntsa::Packet*            packet,
        const ntsa::Ipv4Address& destinationIpv4Address) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'packet' a new packets to the specified
    /// 'destinationIpv6Address' with the source IPv6 address, source Ethernet
    /// address, and destination Ethernet address of theh 'packet'
    /// automatically assigned to the correct addresses according to the
    /// current IPv6 routing table. Return the error.
    ntsa::Error allocate(
        ntsa::Packet*            packet,
        const ntsa::Ipv6Address& destinationIpv6Address) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueue(const ntsa::Packet& packet) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueue(bslmf::MovableRef<ntsa::Packet> packet)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    ntsa::Error dequeue(ntsa::Packet* result) BSLS_KEYWORD_OVERRIDE;

    /// Close the device. Return the error.
    ntsa::Error close() BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide utilities for creating networks.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntso
class NetworkUtil
{
    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSO.DEVICE");

  public:
    /// Create a new device tree. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntsi::Network> createNetwork(
        bslma::Allocator* basicAllocator = 0);
};

}  // close package namespace
}  // close enterprise namespace
#endif
#endif
