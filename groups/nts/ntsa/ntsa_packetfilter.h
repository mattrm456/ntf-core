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

#ifndef INCLUDED_NTSA_PACKETFILTER
#define INCLUDED_NTSA_PACKETFILTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ethernetaddress.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_packettype.h>
#include <ntsa_port.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_cstdint.h>
#include <bsl_memory.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Describe a packet filter.
///
/// @ingroup module_ntsa_protocol
class PacketFilter
{
    bsl::vector<ntsa::PacketType::Value>       d_packetType;
    bdlb::NullableValue<ntsa::EthernetAddress> d_sourceEthernetAddress;
    bdlb::NullableValue<ntsa::Ipv4Address>     d_sourceIpv4Address;
    bdlb::NullableValue<ntsa::Ipv6Address>     d_sourceIpv6Address;
    bsl::vector<ntsa::Port>                    d_sourceTcpPort;
    bsl::vector<ntsa::Port>                    d_sourceUdpPort;
    bdlb::NullableValue<ntsa::EthernetAddress> d_destinationEthernetAddress;
    bdlb::NullableValue<ntsa::Ipv4Address>     d_destinationIpv4Address;
    bdlb::NullableValue<ntsa::Ipv6Address>     d_destinationIpv6Address;
    bsl::vector<ntsa::Port>                    d_destinationTcpPort;
    bsl::vector<ntsa::Port>                    d_destinationUdpPort;
    bsl::uint32_t                              d_flags;

  public:
    /// Create new packet filter. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit PacketFilter(bslma::Allocator* basicAllocator = 0);

    /// Create new packet filter having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    PacketFilter(const PacketFilter& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~PacketFilter();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    PacketFilter& operator=(const PacketFilter& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Add the packet type having the specified 'value'.
    void addPacketType(ntsa::PacketType::Value value);

    /// Remove the packet type having the specified 'value'.
    void removePacketType(ntsa::PacketType::Value value);

    /// Set the source Ethernet address to the specified 'value'.
    void setSourceEthernetAddress(const ntsa::EthernetAddress& value);

    /// Set the source IPv4 address to the specified 'value'.
    void setSourceIpv4Address(const ntsa::Ipv4Address& value);

     /// Set the source IPv6 address to the specified 'value'.
    void setSourceIpv6Address(const ntsa::Ipv6Address& value);

    /// Add the source TCP port having the specified 'value'.
    void addSourceTcpPort(ntsa::Port value);

    /// Add the source UDP port having the specified 'value'.
    void addSourceUdpPort(ntsa::Port value);

    /// Remove the source TCP port having the specified 'value'.
    void removeSourceTcpPort(ntsa::Port value);

    /// Remove the source UDP port having the specified 'value'.
    void removeSourceUdpPort(ntsa::Port value);

    /// Set the destination Ethernet address to the specified 'value'.
    void setDestinationEthernetAddress(const ntsa::EthernetAddress& value);

    /// Set the destination IPv4 address to the specified 'value'.
    void setDestinationIpv4Address(const ntsa::Ipv4Address& value);

    /// Set the destination IPv6 address to the specified 'value'.
    void setDestinationIpv6Address(const ntsa::Ipv6Address& value);

    /// Add the destination TCP port having the specified 'value'.
    void addDestinationTcpPort(ntsa::Port value);

    /// Add the destination UDP port having the specified 'value'.
    void addDestinationUdpPort(ntsa::Port value);

    /// Remove the destination TCP port having the specified 'value'.
    void removeDestinationTcpPort(ntsa::Port value);

    /// Remove the destination UDP port having the specified 'value'.
    void removeDestinationUdpPort(ntsa::Port value);

    /// Return the packet type.
    const bsl::vector<ntsa::PacketType::Value>& packetType() const;

    /// Return the source Ethernet address.
    const bdlb::NullableValue<ntsa::EthernetAddress>& sourceEthernetAddress()
        const;

    /// Return the source IPv4 address.
    const bdlb::NullableValue<ntsa::Ipv4Address>& sourceIpv4Address() const;

    /// Return the source IPv6 address.
    const bdlb::NullableValue<ntsa::Ipv6Address>& sourceIpv6Address() const;

    /// Return the source TCP port.
    const bsl::vector<ntsa::Port>& sourceTcpPort() const;

    /// Return the source UDP port.
    const bsl::vector<ntsa::Port>& sourceUdpPort() const;

    /// Return the destination Ethernet address.
    const bdlb::NullableValue<ntsa::EthernetAddress>&
    destinationEthernetAddress() const;

    /// Return the destination IPv4 address.
    const bdlb::NullableValue<ntsa::Ipv4Address>& destinationIpv4Address() const;

    /// Return the destination IPv6 address.
    const bdlb::NullableValue<ntsa::Ipv6Address>& destinationIpv6Address() const;

    /// Return the destination TCP port.
    const bsl::vector<ntsa::Port>& destinationTcpPort() const;

    /// Return the destination UDP port.
    const bsl::vector<ntsa::Port>& destinationUdpPort() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const PacketFilter& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const PacketFilter& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(PacketFilter);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::PacketFilter
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const PacketFilter& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::PacketFilter
bool operator==(const PacketFilter& lhs,
                const PacketFilter& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::PacketFilter
bool operator!=(const PacketFilter& lhs,
                const PacketFilter& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::PacketFilter
bool operator<(const PacketFilter& lhs,
               const PacketFilter& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::PacketFilter
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const PacketFilter& value);

NTSCFG_INLINE
PacketFilter::PacketFilter(bslma::Allocator* basicAllocator)
: d_packetType(basicAllocator)
, d_sourceEthernetAddress()
, d_sourceIpv4Address()
, d_sourceIpv6Address()
, d_sourceTcpPort(basicAllocator)
, d_sourceUdpPort(basicAllocator)
, d_destinationEthernetAddress()
, d_destinationIpv4Address()
, d_destinationIpv6Address()
, d_destinationTcpPort(basicAllocator)
, d_destinationUdpPort(basicAllocator)
, d_flags(0)
{
}

NTSCFG_INLINE
PacketFilter::PacketFilter(
    const PacketFilter& original, bslma::Allocator* basicAllocator)
: d_packetType(original.d_packetType, basicAllocator)
, d_sourceEthernetAddress(original.d_sourceEthernetAddress)
, d_sourceIpv4Address(original.d_sourceIpv4Address)
, d_sourceIpv6Address(original.d_sourceIpv6Address)
, d_sourceTcpPort(original.d_sourceTcpPort, basicAllocator)
, d_sourceUdpPort(original.d_sourceUdpPort, basicAllocator)
, d_destinationEthernetAddress(original.d_destinationEthernetAddress)
, d_destinationIpv4Address(original.d_destinationIpv4Address)
, d_destinationIpv6Address(original.d_destinationIpv6Address)
, d_destinationTcpPort(original.d_destinationTcpPort, basicAllocator)
, d_destinationUdpPort(original.d_destinationUdpPort, basicAllocator)
, d_flags(original.d_flags)
{
}

NTSCFG_INLINE
PacketFilter::~PacketFilter()
{
}

NTSCFG_INLINE
PacketFilter& PacketFilter::operator=(
    const PacketFilter& other)
{
    d_packetType = other.d_packetType;
    d_sourceEthernetAddress      = other.d_sourceEthernetAddress;
    d_sourceIpv4Address            = other.d_sourceIpv4Address;
    d_sourceIpv6Address            = other.d_sourceIpv6Address;
    d_sourceTcpPort              = other.d_sourceTcpPort;
    d_sourceUdpPort              = other.d_sourceUdpPort;
    d_destinationEthernetAddress = other.d_destinationEthernetAddress;
    d_destinationIpv4Address       = other.d_destinationIpv4Address;
    d_destinationIpv6Address       = other.d_destinationIpv6Address;
    d_destinationTcpPort         = other.d_destinationTcpPort;
    d_destinationUdpPort         = other.d_destinationUdpPort;
    d_flags                      = other.d_flags;

    return *this;
}

NTSCFG_INLINE
void PacketFilter::reset()
{
    d_packetType.clear();
    d_sourceEthernetAddress.reset();
    d_sourceIpv4Address.reset();
    d_sourceIpv6Address.reset();
    d_sourceTcpPort.clear();
    d_sourceUdpPort.clear();
    d_destinationEthernetAddress.reset();
    d_destinationIpv4Address.reset();
    d_destinationIpv6Address.reset();
    d_destinationTcpPort.clear();
    d_destinationUdpPort.clear();
    d_flags = 0;
}

NTSCFG_INLINE
void PacketFilter::addPacketType(ntsa::PacketType::Value value)
{
    d_packetType.push_back(value);
}

NTSCFG_INLINE
void PacketFilter::removePacketType(ntsa::PacketType::Value value)
{
    d_packetType.erase(
        bsl::remove(
            d_packetType.begin(), d_packetType.end(), value),
        d_packetType.end());
}

NTSCFG_INLINE
void PacketFilter::setSourceEthernetAddress(
    const ntsa::EthernetAddress& value)
{
    d_sourceEthernetAddress = value;
}

NTSCFG_INLINE
void PacketFilter::setSourceIpv4Address(const ntsa::Ipv4Address& value)
{
    d_sourceIpv4Address = value;
}

NTSCFG_INLINE
void PacketFilter::setSourceIpv6Address(const ntsa::Ipv6Address& value)
{
    d_sourceIpv6Address = value;
}

NTSCFG_INLINE
void PacketFilter::addSourceTcpPort(ntsa::Port value)
{
    d_sourceTcpPort.push_back(value);
}

NTSCFG_INLINE
void PacketFilter::addSourceUdpPort(ntsa::Port value)
{
    d_sourceUdpPort.push_back(value);
}

NTSCFG_INLINE
void PacketFilter::removeSourceTcpPort(ntsa::Port value)
{
    d_sourceTcpPort.erase(
        bsl::remove(
            d_sourceTcpPort.begin(), d_sourceTcpPort.end(), value),
        d_sourceTcpPort.end());
}

NTSCFG_INLINE
void PacketFilter::removeSourceUdpPort(ntsa::Port value)
{
    d_sourceUdpPort.erase(
        bsl::remove(
            d_sourceUdpPort.begin(), d_sourceUdpPort.end(), value),
        d_sourceUdpPort.end());
}

NTSCFG_INLINE
void PacketFilter::setDestinationEthernetAddress(
    const ntsa::EthernetAddress& value)
{
    d_destinationEthernetAddress = value;
}

NTSCFG_INLINE
void PacketFilter::setDestinationIpv4Address(
    const ntsa::Ipv4Address& value)
{
    d_destinationIpv4Address = value;
}

NTSCFG_INLINE
void PacketFilter::setDestinationIpv6Address(
    const ntsa::Ipv6Address& value)
{
    d_destinationIpv6Address = value;
}

NTSCFG_INLINE
void PacketFilter::addDestinationTcpPort(ntsa::Port value)
{
    d_destinationTcpPort.push_back(value);
}

NTSCFG_INLINE
void PacketFilter::addDestinationUdpPort(ntsa::Port value)
{
    d_destinationUdpPort.push_back(value);
}

NTSCFG_INLINE
void PacketFilter::removeDestinationTcpPort(ntsa::Port value)
{
    d_destinationTcpPort.erase(
        bsl::remove(
            d_destinationTcpPort.begin(), d_destinationTcpPort.end(), value),
        d_destinationTcpPort.end());
}

NTSCFG_INLINE
void PacketFilter::removeDestinationUdpPort(ntsa::Port value)
{
    d_destinationUdpPort.erase(
        bsl::remove(
            d_destinationUdpPort.begin(), d_destinationUdpPort.end(), value),
        d_destinationUdpPort.end());
}

NTSCFG_INLINE
const bsl::vector<ntsa::PacketType::Value>& PacketFilter::packetType() const
{
    return d_packetType;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::EthernetAddress>& PacketFilter::
    sourceEthernetAddress() const
{
    return d_sourceEthernetAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv4Address>& PacketFilter::
    sourceIpv4Address() const
{
    return d_sourceIpv4Address;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv6Address>& PacketFilter::
    sourceIpv6Address() const
{
    return d_sourceIpv6Address;
}

NTSCFG_INLINE
const bsl::vector<ntsa::Port>& PacketFilter::sourceTcpPort()
    const
{
    return d_sourceTcpPort;
}

NTSCFG_INLINE
const bsl::vector<ntsa::Port>& PacketFilter::sourceUdpPort()
    const
{
    return d_sourceUdpPort;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::EthernetAddress>& PacketFilter::
    destinationEthernetAddress() const
{
    return d_destinationEthernetAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv4Address>& PacketFilter::
    destinationIpv4Address() const
{
    return d_destinationIpv4Address;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv6Address>& PacketFilter::
    destinationIpv6Address() const
{
    return d_destinationIpv6Address;
}

NTSCFG_INLINE
const bsl::vector<ntsa::Port>& PacketFilter::
    destinationTcpPort() const
{
    return d_destinationTcpPort;
}

NTSCFG_INLINE
const bsl::vector<ntsa::Port>& PacketFilter::
    destinationUdpPort() const
{
    return d_destinationUdpPort;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void PacketFilter::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_packetType);
    hashAppend(algorithm, d_sourceEthernetAddress);
    hashAppend(algorithm, d_sourceIpv4Address);
    hashAppend(algorithm, d_sourceIpv6Address);
    hashAppend(algorithm, d_sourceTcpPort);
    hashAppend(algorithm, d_sourceUdpPort);
    hashAppend(algorithm, d_destinationEthernetAddress);
    hashAppend(algorithm, d_destinationIpv4Address);
    hashAppend(algorithm, d_destinationIpv6Address);
    hashAppend(algorithm, d_destinationTcpPort);
    hashAppend(algorithm, d_destinationUdpPort);
    hashAppend(algorithm, d_flags);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const PacketFilter& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const PacketFilter& lhs,
                const PacketFilter& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const PacketFilter& lhs,
                const PacketFilter& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const PacketFilter& lhs,
               const PacketFilter& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&             algorithm,
                              const PacketFilter& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
