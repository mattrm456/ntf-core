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
#include <ntsa_packetfactory.h>
#include <ntscfg_platform.h>
#include <ntsi_descriptor.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bslmt_threadutil.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsi {

/// Provide a packet sender.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsi
class PacketSender
{
  public:
    /// Destroy this object.
    virtual ~PacketSender();

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueuePacket(
        const bsl::shared_ptr<ntsa::Packet>& packet) = 0;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueuePacket(
        bslmf::MovableRef<bsl::shared_ptr<ntsa::Packet> > packet) = 0;
};

/// Provide a packet receiver.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsi
class PacketReceiver
{
  public:
    /// Destroy this object.
    virtual ~PacketReceiver();

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    virtual ntsa::Error dequeuePacket(bsl::shared_ptr<ntsa::Packet>* result) = 0;
};

/// Provide an abstract representation of a network device.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsi
class Device : public ntsa::PacketFactory,
               public ntsi::PacketSender,
               public ntsi::PacketReceiver
{
  public:
    /// Destroy this object.
    virtual ~Device();

    /// Open the device.
    virtual ntsa::Error open() = 0;

    /// Load into the specified 'result' a packet suitable to enqueue to to the
    /// associated device.
    virtual void createOutgoingPacket(bsl::shared_ptr<ntsa::Packet>* result)
        BSLS_KEYWORD_OVERRIDE = 0;

    /// Load into the specified 'result' a packet suitable to dequeue from the
    /// associated device.
    virtual void createIncomingPacket(bsl::shared_ptr<ntsa::Packet>* result)
        BSLS_KEYWORD_OVERRIDE = 0;

    /// Load into the specified 'result' a blob buffer suitable to enqueue to
    /// to the associated device.
    virtual void createOutgoingBlobBuffer(bdlbb::BlobBuffer* result)
        BSLS_KEYWORD_OVERRIDE = 0;

    /// Load into the specified 'result' a blob buffer suitable to dequeue from
    /// the associated device.
    virtual void createIncomingBlobBuffer(bdlbb::BlobBuffer* result)
        BSLS_KEYWORD_OVERRIDE = 0;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueuePacket(const bsl::shared_ptr<ntsa::Packet>& packet)
        BSLS_KEYWORD_OVERRIDE = 0;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueuePacket(
        bslmf::MovableRef<bsl::shared_ptr<ntsa::Packet> > packet)
        BSLS_KEYWORD_OVERRIDE = 0;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    virtual ntsa::Error dequeuePacket(bsl::shared_ptr<ntsa::Packet>* result)
        BSLS_KEYWORD_OVERRIDE = 0;

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

    /// Load into the specified 'packet' a new packet to the specified
    /// 'destinationIpv4Address' with the source IPv4 address, source Ethernet
    /// address, and destination Ethernet address of the 'packet' automatically
    /// assigned to the correct addresses according to the current IPv4 routing
    /// table. Load into the specified 'sender' an appropriate mechanism to
    /// enqueue the 'packet for transmission. Return the error.
    virtual ntsa::Error allocate(
        bsl::shared_ptr<ntsa::Packet>*       packet,
        bsl::shared_ptr<ntsi::PacketSender>* sender,
        const ntsa::Ipv4Address&             destinationIpv4Address) = 0;

    /// Load into the specified 'packet' a new packet to the specified
    /// 'destinationIpv4Address' with the source IPv4 address, source Ethernet
    /// address, and destination Ethernet address of the 'packet' automatically
    /// assigned to the correct addresses according to the current IPv4 routing
    /// table. Load into the specified 'sender' an appropriate mechanism to
    /// enqueue the 'packet for transmission. Return the error.
    virtual ntsa::Error allocate(
        bsl::shared_ptr<ntsa::Packet>*       packet,
        bsl::shared_ptr<ntsi::PacketSender>* sender,
        const ntsa::Ipv6Address&             destinationIpv6Address) = 0;

    /// Load into the specified 'receiver' an appropriate mechanism to receive
    /// packets intended for the specified 'sourceEthernetAddress'. Return the
    /// error.
    virtual ntsa::Error bind(
        bsl::shared_ptr<ntsi::PacketReceiver>* receiver,
        const ntsa::EthernetAddress&           sourceEthernetAddress) = 0;

    /// Load into the specified 'receiver' an appropriate mechanism to receive
    /// packets intended for the specified 'sourceIpv4Address'. Return the
    /// error.
    virtual ntsa::Error bind(bsl::shared_ptr<ntsi::PacketReceiver>* receiver,
                             const ntsa::Ipv4Address& sourceIpv4Address) = 0;

    /// Load into the specified 'receiver' an appropriate mechanism to receive
    /// packets intended for the specified 'sourceIpv4Address'. Return the
    /// error.
    virtual ntsa::Error bind(bsl::shared_ptr<ntsi::PacketReceiver>* receiver,
                             const ntsa::Ipv6Address& sourceIpv6Address) = 0;

    /// Close the device. Return the error.
    virtual ntsa::Error close() = 0;
};

}  // end namespace ntsi
}  // end namespace BloombergLP
#endif
