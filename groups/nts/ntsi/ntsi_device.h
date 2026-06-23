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

#ifndef INCLUDED_NTSI_DEVICE
#define INCLUDED_NTSI_DEVICE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_adapter.h>
#include <ntsa_buffer.h>
#include <ntsa_data.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_packet.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsi_descriptor.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bslmt_threadutil.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsi {

/// Provide an abstract representation of a network device.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsi
class Device
{
  public:
    /// Destroy this object.
    virtual ~Device();

    /// Open the device.
    virtual ntsa::Error open() = 0;

    /// Load into the specified 'buffer' a new buffer whose size is the
    /// maximum transmission unit of this device.
    virtual ntsa::Error allocate(bdlbb::BlobBuffer* buffer) = 0;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueue(const ntsa::Packet& packet) = 0;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueue(bslmf::MovableRef<ntsa::Packet> packet) = 0;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    virtual ntsa::Error dequeue(ntsa::Packet* result) = 0;

    /// Close the device. Return the error.
    virtual ntsa::Error close() = 0;

    /// Return the adapter for this device.
    virtual const ntsa::Adapter& adapter() const = 0;
};

/// Provide an abstract representation of a network.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsi
class Network
{
  public:
    /// Destroy this object.
    virtual ~Network();

    /// Open the device.
    virtual ntsa::Error open() = 0;

    /// Load into the specified 'buffer' a new buffer whose size is the
    /// maximum transmission unit of this device.
    virtual ntsa::Error allocate(bdlbb::BlobBuffer* buffer) = 0;

    /// Load into the specified 'packet' a new packets to the specified
    /// 'destinationIpv4Address' with the source IPv4 address, source Ethernet
    /// address, and destination Ethernet address of theh 'packet'
    /// automatically assigned to the correct addresses according to the
    /// current IPv4 routing table. Return the error.
    virtual ntsa::Error allocate(
        ntsa::Packet*            packet,
        const ntsa::Ipv4Address& destinationIpv4Address) = 0;

    /// Load into the specified 'packet' a new packets to the specified
    /// 'destinationIpv6Address' with the source IPv6 address, source Ethernet
    /// address, and destination Ethernet address of theh 'packet'
    /// automatically assigned to the correct addresses according to the
    /// current IPv6 routing table. Return the error.
    virtual ntsa::Error allocate(
        ntsa::Packet*            packet,
        const ntsa::Ipv6Address& destinationIpv6Address) = 0;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueue(const ntsa::Packet& packet) = 0;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueue(bslmf::MovableRef<ntsa::Packet> packet) = 0;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    virtual ntsa::Error dequeue(ntsa::Packet* result) = 0;

    /// Close the device. Return the error.
    virtual ntsa::Error close() = 0;
};

}  // end namespace ntsi
}  // end namespace BloombergLP
#endif
