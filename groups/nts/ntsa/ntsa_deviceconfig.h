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

#ifndef INCLUDED_NTSA_DEVICECONFIG
#define INCLUDED_NTSA_DEVICECONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_ethernetaddress.h>
#include <ntsa_packetfilter.h>
#include <ntsa_port.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Describe the configuration of an interface to a low-level networking
/// device.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b driverName:
/// The name of the implementation of the driver.  Valid values are "native"
/// and the empty string to represent the default reactor implementation for
/// the current platform.
///
/// @li @b adapterName:
/// The name of the network device adapter, e.g., "lo", "eth0", etc.
///
/// @li @b outgoingEnabled
/// The flag that controls whether outgoing packets are enabled.
///
/// @li @b outgoingPacketFilter
/// The outgoing packet filter.
///
/// @li @b outgoingMinThreads
/// The minimum number of threads processing outgoing packets.
///
/// @li @b outgoingMaxThreads
/// The maximum number of threads processing outgoing packets.
///
/// @li @b outgoingMaxPackets
/// The maximum number of outgoing packets.
///
/// @li @b incomingEnabled
/// The flag that controls whether incoming packets are enabled.
///
/// @li @b incomingPacketFilter
/// The incoming packet filter.
///
/// @li @b incomingMinThreads
/// The minimum number of threads processing incoming packets.
///
/// @li @b incomingMaxThreads
/// The maximum number of threads processing incoming packets.
///
/// @li @b incomingMaxPackets
/// The maximum number of incoming packets.
///
/// @li @b promiscuous
/// Process all packets, not just those intended for this device.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class DeviceConfig
{
    bdlb::NullableValue<bsl::string>           d_driverName;
    bdlb::NullableValue<bsl::string>           d_adapterName;
    bdlb::NullableValue<bool>                  d_outgoingEnabled;
    bdlb::NullableValue<ntsa::PacketFilter>    d_outgoingPacketFilter;
    bdlb::NullableValue<bsl::size_t>           d_outgoingMinThreads;
    bdlb::NullableValue<bsl::size_t>           d_outgoingMaxThreads;
    bdlb::NullableValue<bsl::size_t>           d_outgoingMaxPackets;
    bdlb::NullableValue<bool>                  d_incomingEnabled;
    bdlb::NullableValue<ntsa::PacketFilter>    d_incomingPacketFilter;
    bdlb::NullableValue<bsl::size_t>           d_incomingMinThreads;
    bdlb::NullableValue<bsl::size_t>           d_incomingMaxThreads;
    bdlb::NullableValue<bsl::size_t>           d_incomingMaxPackets;
    bdlb::NullableValue<bool>                  d_promiscuous;

  public:
    /// Create a new driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit DeviceConfig(bslma::Allocator* basicAllocator = 0);

    /// Create a new driver configuration having the same value as the
    /// specified 'original' driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    DeviceConfig(const DeviceConfig& original,
                 bslma::Allocator*   basicAllocator = 0);

    /// Destroy this object.
    ~DeviceConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    DeviceConfig& operator=(const DeviceConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the name of the driver implementation to the specified 'value'.
    /// Valid values are "native", "dpdk", "openonload", and the empty string
    /// to represent the default driver implementation for the current
    /// platform. Note that not all driver implementations are available on
    /// all platforms; consult the component documentation more details.
    void setDriverName(const bsl::string& value);

    /// Set the name of the adapter to the specified 'value'.
    void setAdapterName(const bsl::string& value);

    /// Set the flag that indicates outgoing packets are enabled according to
    /// the specified 'value'.
    void setOutgoingEnabled(bool value);

    /// Set the outgoing packet filter to the specified 'value'.
    void setOutgoingPacketFilter(const ntsa::PacketFilter& value);

    /// Set the minimum number of threads processing outgoing packets to the
    /// specified 'value'.
    void setOutgoingMinThreads(bsl::size_t value);

    /// Set the maximum number of threads processing outgoing packets to the
    /// specified 'value'.
    void setOutgoingMaxThreads(bsl::size_t value);

    /// Set the maximum number of outgoing packets to the specified 'value'.
    void setOutgoingMaxPackets(bsl::size_t value);

    /// Set the flag that indicates incoming packets are enabled according to
    /// the specified 'value'.
    void setIncomingEnabled(bool value);

    /// Set the incoming packet filter to the specified 'value'.
    void setIncomingPacketFilter(const ntsa::PacketFilter& value);

    /// Set the minimum number of threads processing incoming packets to the
    /// specified 'value'.
    void setIncomingMinThreads(bsl::size_t value);

    /// Set the maximum number of threads processing incoming packets to the
    /// specified 'value'.
    void setIncomingMaxThreads(bsl::size_t value);

    /// Set the maximum number of incoming packets to the specified 'value'.
    void setIncomingMaxPackets(bsl::size_t value);

    /// Set the flag to process all packets, not just those intended for the
    /// device, according to the specified 'value'.
    void setPromiscuous(bool value);

    /// Return the name of the driver implementation.
    const bdlb::NullableValue<bsl::string>& driverName() const;

    /// Return the name of the adapter.
    const bdlb::NullableValue<bsl::string>& adapterName() const;

    /// Return the flag that indicates outgoing packets are enabled.
    const bdlb::NullableValue<bool>& outgoingEnabled() const;

    /// Return the outgoing packet filter.
    const bdlb::NullableValue<ntsa::PacketFilter>& outgoingPacketFilter() const;

    /// Return the minimum number of threads processing outgoing packets.
    const bdlb::NullableValue<bsl::size_t>& outgoingMinThreads() const;

    /// Return the maximum number of threads processing outgoing packets.
    const bdlb::NullableValue<bsl::size_t>& outgoingMaxThreads() const;

    /// Return the maximum number of outgoing packets.
    const bdlb::NullableValue<bsl::size_t>& outgoingMaxPackets() const;

    /// Return the flag that indicates incoming packets are enabled.
    const bdlb::NullableValue<bool>& incomingEnabled() const;

    /// Return the incoming packet filter.
    const bdlb::NullableValue<ntsa::PacketFilter>& incomingPacketFilter() const;

    /// Return the minimum number of threads processing incoming packets.
    const bdlb::NullableValue<bsl::size_t>& incomingMinThreads() const;

    /// Return the maximum number of threads processing incoming packets.
    const bdlb::NullableValue<bsl::size_t>& incomingMaxThreads() const;

    /// Return the maximum number of incoming packets.
    const bdlb::NullableValue<bsl::size_t>& incomingMaxPackets() const;

    /// Return the flag to process all packets, not just those intended for the
    /// device.
    const bdlb::NullableValue<bool>& promiscuous() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const DeviceConfig& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const DeviceConfig& other) const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(DeviceConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::DeviceConfig
bsl::ostream& operator<<(bsl::ostream& stream, const DeviceConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::DeviceConfig
bool operator==(const DeviceConfig& lhs, const DeviceConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::DeviceConfig
bool operator!=(const DeviceConfig& lhs, const DeviceConfig& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::DeviceConfig
bool operator<(const DeviceConfig& lhs, const DeviceConfig& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::DeviceConfig
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DeviceConfig& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DeviceConfig& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.driverName());
    hashAppend(algorithm, value.adapterName());
    hashAppend(algorithm, value.outgoingEnabled());
    hashAppend(algorithm, value.outgoingPacketFilter());
    hashAppend(algorithm, value.outgoingMinThreads());
    hashAppend(algorithm, value.outgoingMaxThreads());
    hashAppend(algorithm, value.outgoingMaxPackets());
    hashAppend(algorithm, value.incomingEnabled());
    hashAppend(algorithm, value.incomingPacketFilter());
    hashAppend(algorithm, value.incomingMinThreads());
    hashAppend(algorithm, value.incomingMaxThreads());
    hashAppend(algorithm, value.incomingMaxPackets());
    hashAppend(algorithm, value.promiscuous());
}

}  // close package namespace
}  // close enterprise namespace
#endif

