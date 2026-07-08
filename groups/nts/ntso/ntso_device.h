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
#include <ntsa_packet.h>
#include <ntsa_packetfactory.h>
#include <ntsa_packetpool.h>
#include <ntsa_packetqueue.h>
#include <ntscfg_config.h>
#include <ntscfg_platform.h>
#include <ntsi_device.h>
#include <ntsscm_version.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bsl_memory.h>
#include <ball_log.h>

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

    /// Validate the specified 'adapter' is compatible with the specified
    /// 'configuration'. Return the error.
    static ntsa::Error validateAdapter(
        const ntsa::Adapter&      adapter,
        const ntsa::DeviceConfig& configuration);

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

    /// Define a type alias for a mutex.
    typedef bslmt::Mutex Mutex;

    /// Define a type alias for a lock guard.
    typedef bslmt::LockGuard<bslmt::Mutex> LockGuard;

    /// Defines a type alias for a map of devices indexed by their Ethernet
    /// address.
    typedef bsl::unordered_map<ntsa::EthernetAddress,
                               bsl::shared_ptr<ntsi::Device> >
        DeviceByEthernetAddress;

    /// Defines a type alias for a map of devices indexed by their IPv4
    /// address.
    typedef bsl::unordered_map<ntsa::Ipv4Address,
                               bsl::shared_ptr<ntsi::Device> >
        DeviceByIpv4Address;

    /// Defines a type alias for a map of devices indexed by their IPv4
    /// address.
    typedef bsl::unordered_map<ntsa::Ipv6Address,
                               bsl::shared_ptr<ntsi::Device> >
        DeviceByIpv6Address;

    /// The mutex.
    mutable Mutex d_mutex;

    /// The blob buffer factory.
    bsl::shared_ptr<bdlbb::BlobBufferFactory> d_blobBufferFactory;

    /// The map of TX devices indexed by their Ethernet address.
    DeviceByEthernetAddress d_txDeviceByEthernetAddress;

    /// The map of TX devices indexed by their IPv4 address.
    DeviceByIpv4Address d_txDeviceByIpv4Address;

    /// The map of TX devices indexed by their IPv6 address.
    DeviceByIpv6Address d_txDeviceByIpv6Address;

    /// The map of RX devices indexed by their Ethernet address.
    DeviceByEthernetAddress d_rxDeviceByEthernetAddress;

    /// The map of RX devices indexed by their IPv4 address.
    DeviceByIpv4Address d_rxDeviceByIpv4Address;

    /// The map of RX devices indexed by their IPv6 address.
    DeviceByIpv6Address d_rxDeviceByIpv6Address;

    /// The packet filter.
    ntsa::PacketFilter d_packetFilter;

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
    /// Lookup or create a device for sending packets from the specified source
    /// 'ethernetAddress'. Load the device into the specified 'device'. Return
    /// the error.
    ntsa::Error ensureTxDevice(bsl::shared_ptr<ntsi::Device>* device,
                               const ntsa::EthernetAddress&   ethernetAddress);

    /// Lookup or create a device for sending packets from the specified source
    /// 'ipv4Address'. Load the device into the specified 'device'. Return the
    /// error.
    ntsa::Error ensureTxDevice(bsl::shared_ptr<ntsi::Device>* device,
                               const ntsa::Ipv4Address&       ipv4Address);

    /// Lookup or create a device for sending packets from the specified source
    /// 'ipv6Address'. Load the device into the specified 'device'. Return the
    /// error.
    ntsa::Error ensureTxDevice(bsl::shared_ptr<ntsi::Device>* device,
                               const ntsa::Ipv6Address&       ipv6Address);

    /// Lookup or create a device for receiving packets to the specified
    /// destination 'ethernetAddress'. Load the device into the specified
    /// 'device'. Return the error.
    ntsa::Error ensureRxDevice(bsl::shared_ptr<ntsi::Device>* device,
                               const ntsa::EthernetAddress&   ethernetAddress);

    /// Lookup or create a device for receiving packets to the specified
    /// destination 'ipv4Address'. Load the device into the specified 'device'.
    /// Return the error.
    ntsa::Error ensureRxDevice(bsl::shared_ptr<ntsi::Device>* device,
                               const ntsa::Ipv4Address&       ipv4Address);

    /// Lookup or create a device for receiving packets to the specified
    /// destination 'ipv6Address'. Load the device into the specified 'device'.
    /// Return the error.
    ntsa::Error ensureRxDevice(bsl::shared_ptr<ntsi::Device>* device,
                               const ntsa::Ipv6Address&       ipv6Address);

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

    /// Load into the specified 'packet' a new packet to the specified
    /// 'destinationIpv4Address' with the source IPv4 address, source Ethernet
    /// address, and destination Ethernet address of the 'packet' automatically
    /// assigned to the correct addresses according to the current IPv4 routing
    /// table. Load into the specified 'sender' an appropriate mechanism to
    /// enqueue the 'packet for transmission. Return the error.
    ntsa::Error allocate(bsl::shared_ptr<ntsa::Packet>*       packet,
                         bsl::shared_ptr<ntsi::PacketSender>* sender,
                         const ntsa::Ipv4Address& destinationIpv4Address)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'packet' a new packet to the specified
    /// 'destinationIpv4Address' with the source IPv4 address, source Ethernet
    /// address, and destination Ethernet address of the 'packet' automatically
    /// assigned to the correct addresses according to the current IPv4 routing
    /// table. Load into the specified 'sender' an appropriate mechanism to
    /// enqueue the 'packet for transmission. Return the error.
    ntsa::Error allocate(bsl::shared_ptr<ntsa::Packet>*       packet,
                         bsl::shared_ptr<ntsi::PacketSender>* sender,
                         const ntsa::Ipv6Address& destinationIpv6Address)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'receiver' an appropriate mechanism to receive
    /// packets intended for the specified 'sourceEthernetAddress'. Return the
    /// error.
    ntsa::Error bind(bsl::shared_ptr<ntsi::PacketReceiver>* receiver,
                     const ntsa::EthernetAddress& sourceEthernetAddress)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'receiver' an appropriate mechanism to receive
    /// packets intended for the specified 'sourceIpv4Address'. Return the
    /// error.
    ntsa::Error bind(bsl::shared_ptr<ntsi::PacketReceiver>* receiver,
                     const ntsa::Ipv4Address&               sourceIpv4Address)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'receiver' an appropriate mechanism to receive
    /// packets intended for the specified 'sourceIpv4Address'. Return the
    /// error.
    ntsa::Error bind(bsl::shared_ptr<ntsi::PacketReceiver>* receiver,
                     const ntsa::Ipv6Address&               sourceIpv6Address)
        BSLS_KEYWORD_OVERRIDE;

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
