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

#ifndef INCLUDED_NTSA_PACKETDECODERCONTEXT
#define INCLUDED_NTSA_PACKETDECODERCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ethernetaddress.h>
#include <ntsa_ipaddress.h>
#include <ntsa_port.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntsa {

/// Describe the context of a packet decoder.
///
/// @ingroup module_ntsa_protocol
class PacketDecoderContext
{
    bdlb::NullableValue<ntsa::EthernetAddress> d_sourceEthernetAddress;
    bdlb::NullableValue<ntsa::IpAddress>       d_sourceIpAddress;
    bdlb::NullableValue<ntsa::Port>            d_sourceTcpPort;
    bdlb::NullableValue<ntsa::Port>            d_sourceUdpPort;
    bdlb::NullableValue<ntsa::EthernetAddress> d_destinationEthernetAddress;
    bdlb::NullableValue<ntsa::IpAddress>       d_destinationIpAddress;
    bdlb::NullableValue<ntsa::Port>            d_destinationTcpPort;
    bdlb::NullableValue<ntsa::Port>            d_destinationUdpPort;

  public:
    /// Create a new packet decoder context having a default value.
    PacketDecoderContext();

    /// Create a new packet decoder context having the same value as the
    /// specified 'original' object. Assign an unspecified but valid value to
    /// the 'original' original.
    PacketDecoderContext(bslmf::MovableRef<PacketDecoderContext> original)
        NTSCFG_NOEXCEPT;

    /// Create a new packet decoder context having the same value as the
    /// specified 'original' object.
    PacketDecoderContext(const PacketDecoderContext& original);

    /// Destroy this object.
    ~PacketDecoderContext();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    PacketDecoderContext& operator=(
        bslmf::MovableRef<PacketDecoderContext> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    PacketDecoderContext& operator=(const PacketDecoderContext& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the source Ethernet address to the specified 'value'.
    void setSourceEthernetAddress(const ntsa::EthernetAddress& value);

    /// Set the source IP address to the specified 'value'.
    void setSourceIpAddress(const ntsa::IpAddress& value);

    /// Set the source TCP port to the specified 'value'.
    void setSourceTcpPort(ntsa::Port value);

    /// Set the source UDP port to the specified 'value'.
    void setSourceUdpPort(ntsa::Port value);

    /// Set the destination Ethernet address to the specified 'value'.
    void setDestinationEthernetAddress(const ntsa::EthernetAddress& value);

    /// Set the destination IP address to the specified 'value'.
    void setDestinationIpAddress(const ntsa::IpAddress& value);

    /// Set the destination TCP port to the specified 'value'.
    void setDestinationTcpPort(ntsa::Port value);

    /// Set the destination UDP port to the specified 'value'.
    void setDestinationUdpPort(ntsa::Port value);

    /// Return the source Ethernet address.
    const bdlb::NullableValue<ntsa::EthernetAddress>& sourceEthernetAddress()
        const;

    /// Return the source IP address.
    const bdlb::NullableValue<ntsa::IpAddress>& sourceIpAddress() const;

    /// Return the source TCP port.
    const bdlb::NullableValue<ntsa::Port>& sourceTcpPort() const;

    /// Return the source UDP port.
    const bdlb::NullableValue<ntsa::Port>& sourceUdpPort() const;

    /// Return the destination Ethernet address.
    const bdlb::NullableValue<ntsa::EthernetAddress>&
    destinationEthernetAddress() const;

    /// Return the destination IP address.
    const bdlb::NullableValue<ntsa::IpAddress>& destinationIpAddress() const;

    /// Return the destination TCP port.
    const bdlb::NullableValue<ntsa::Port>& destinationTcpPort() const;

    /// Return the destination UDP port.
    const bdlb::NullableValue<ntsa::Port>& destinationUdpPort() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const PacketDecoderContext& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const PacketDecoderContext& other) const;

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

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(PacketDecoderContext);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(PacketDecoderContext);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(PacketDecoderContext);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::PacketDecoderContext
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const PacketDecoderContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::PacketDecoderContext
bool operator==(const PacketDecoderContext& lhs,
                const PacketDecoderContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::PacketDecoderContext
bool operator!=(const PacketDecoderContext& lhs,
                const PacketDecoderContext& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::PacketDecoderContext
bool operator<(const PacketDecoderContext& lhs,
               const PacketDecoderContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::PacketDecoderContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const PacketDecoderContext& value);

NTSCFG_INLINE
PacketDecoderContext::PacketDecoderContext()
: d_sourceEthernetAddress()
, d_sourceIpAddress()
, d_sourceTcpPort()
, d_sourceUdpPort()
, d_destinationEthernetAddress()
, d_destinationIpAddress()
, d_destinationTcpPort()
, d_destinationUdpPort()
{
}

NTSCFG_INLINE
PacketDecoderContext::PacketDecoderContext(
    bslmf::MovableRef<PacketDecoderContext> original) NTSCFG_NOEXCEPT
: d_sourceEthernetAddress(NTSCFG_MOVE_FROM(original, d_sourceEthernetAddress)),
  d_sourceIpAddress(NTSCFG_MOVE_FROM(original, d_sourceIpAddress)),
  d_sourceTcpPort(NTSCFG_MOVE_FROM(original, d_sourceTcpPort)),
  d_sourceUdpPort(NTSCFG_MOVE_FROM(original, d_sourceUdpPort)),
  d_destinationEthernetAddress(NTSCFG_MOVE_FROM(original,
                                                d_destinationEthernetAddress)),
  d_destinationIpAddress(NTSCFG_MOVE_FROM(original, d_destinationIpAddress)),
  d_destinationTcpPort(NTSCFG_MOVE_FROM(original, d_destinationTcpPort)),
  d_destinationUdpPort(NTSCFG_MOVE_FROM(original, d_destinationUdpPort))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
PacketDecoderContext::PacketDecoderContext(
    const PacketDecoderContext& original)
: d_sourceEthernetAddress(original.d_sourceEthernetAddress)
, d_sourceIpAddress(original.d_sourceIpAddress)
, d_sourceTcpPort(original.d_sourceTcpPort)
, d_sourceUdpPort(original.d_sourceUdpPort)
, d_destinationEthernetAddress(original.d_destinationEthernetAddress)
, d_destinationIpAddress(original.d_destinationIpAddress)
, d_destinationTcpPort(original.d_destinationTcpPort)
, d_destinationUdpPort(original.d_destinationUdpPort)
{
}

NTSCFG_INLINE
PacketDecoderContext::~PacketDecoderContext()
{
}

NTSCFG_INLINE
PacketDecoderContext& PacketDecoderContext::operator=(
    bslmf::MovableRef<PacketDecoderContext> other) NTSCFG_NOEXCEPT
{
    d_sourceEthernetAddress = NTSCFG_MOVE_FROM(other, d_sourceEthernetAddress);
    d_sourceIpAddress       = NTSCFG_MOVE_FROM(other, d_sourceIpAddress);
    d_sourceTcpPort         = NTSCFG_MOVE_FROM(other, d_sourceTcpPort);
    d_sourceUdpPort         = NTSCFG_MOVE_FROM(other, d_sourceUdpPort);
    d_destinationEthernetAddress =
        NTSCFG_MOVE_FROM(other, d_destinationEthernetAddress);
    d_destinationIpAddress = NTSCFG_MOVE_FROM(other, d_destinationIpAddress);
    d_destinationTcpPort   = NTSCFG_MOVE_FROM(other, d_destinationTcpPort);
    d_destinationUdpPort   = NTSCFG_MOVE_FROM(other, d_destinationUdpPort);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
PacketDecoderContext& PacketDecoderContext::operator=(
    const PacketDecoderContext& other)
{
    d_sourceEthernetAddress      = other.d_sourceEthernetAddress;
    d_sourceIpAddress            = other.d_sourceIpAddress;
    d_sourceTcpPort              = other.d_sourceTcpPort;
    d_sourceUdpPort              = other.d_sourceUdpPort;
    d_destinationEthernetAddress = other.d_destinationEthernetAddress;
    d_destinationIpAddress       = other.d_destinationIpAddress;
    d_destinationTcpPort         = other.d_destinationTcpPort;
    d_destinationUdpPort         = other.d_destinationUdpPort;

    return *this;
}

NTSCFG_INLINE
void PacketDecoderContext::reset()
{
    d_sourceEthernetAddress.reset();
    d_sourceIpAddress.reset();
    d_sourceTcpPort.reset();
    d_sourceUdpPort.reset();
    d_destinationEthernetAddress.reset();
    d_destinationIpAddress.reset();
    d_destinationTcpPort.reset();
    d_destinationUdpPort.reset();
}

NTSCFG_INLINE
void PacketDecoderContext::setSourceEthernetAddress(
    const ntsa::EthernetAddress& value)
{
    d_sourceEthernetAddress = value;
}

NTSCFG_INLINE
void PacketDecoderContext::setSourceIpAddress(const ntsa::IpAddress& value)
{
    d_sourceIpAddress = value;
}

NTSCFG_INLINE
void PacketDecoderContext::setSourceTcpPort(ntsa::Port value)
{
    d_sourceTcpPort = value;
}

NTSCFG_INLINE
void PacketDecoderContext::setSourceUdpPort(ntsa::Port value)
{
    d_sourceUdpPort = value;
}

NTSCFG_INLINE
void PacketDecoderContext::setDestinationEthernetAddress(
    const ntsa::EthernetAddress& value)
{
    d_destinationEthernetAddress = value;
}

NTSCFG_INLINE
void PacketDecoderContext::setDestinationIpAddress(
    const ntsa::IpAddress& value)
{
    d_destinationIpAddress = value;
}

NTSCFG_INLINE
void PacketDecoderContext::setDestinationTcpPort(ntsa::Port value)
{
    d_destinationTcpPort = value;
}

NTSCFG_INLINE
void PacketDecoderContext::setDestinationUdpPort(ntsa::Port value)
{
    d_destinationUdpPort = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::EthernetAddress>& PacketDecoderContext::
    sourceEthernetAddress() const
{
    return d_sourceEthernetAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::IpAddress>& PacketDecoderContext::
    sourceIpAddress() const
{
    return d_sourceIpAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Port>& PacketDecoderContext::sourceTcpPort()
    const
{
    return d_sourceTcpPort;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Port>& PacketDecoderContext::sourceUdpPort()
    const
{
    return d_sourceUdpPort;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::EthernetAddress>& PacketDecoderContext::
    destinationEthernetAddress() const
{
    return d_destinationEthernetAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::IpAddress>& PacketDecoderContext::
    destinationIpAddress() const
{
    return d_destinationIpAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Port>& PacketDecoderContext::
    destinationTcpPort() const
{
    return d_destinationTcpPort;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Port>& PacketDecoderContext::
    destinationUdpPort() const
{
    return d_destinationUdpPort;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void PacketDecoderContext::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_sourceEthernetAddress);
    hashAppend(algorithm, d_sourceIpAddress);
    hashAppend(algorithm, d_sourceTcpPort);
    hashAppend(algorithm, d_sourceUdpPort);
    hashAppend(algorithm, d_destinationEthernetAddress);
    hashAppend(algorithm, d_destinationIpAddress);
    hashAppend(algorithm, d_destinationTcpPort);
    hashAppend(algorithm, d_destinationUdpPort);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const PacketDecoderContext& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const PacketDecoderContext& lhs,
                const PacketDecoderContext& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const PacketDecoderContext& lhs,
                const PacketDecoderContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const PacketDecoderContext& lhs,
               const PacketDecoderContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&             algorithm,
                              const PacketDecoderContext& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
