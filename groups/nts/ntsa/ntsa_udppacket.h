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

#ifndef INCLUDED_NTSA_UDPPACKET
#define INCLUDED_NTSA_UDPPACKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_udpheader.h>
#include <ntsa_udppayload.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a User Datagram Protocol (UDP) packet.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class UdpPacket
{
    ntsa::UdpHeader  d_header;
    ntsa::UdpPayload d_payload;

  public:
    /// Create a new UDP packet having a default value.
    UdpPacket();

    /// Create a new UDP packet having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    UdpPacket(bslmf::MovableRef<UdpPacket> original) NTSCFG_NOEXCEPT;

    /// Create a new UDP packet having the same value as the specified
    /// 'original' object.
    UdpPacket(const UdpPacket& original);

    /// Destroy this object.
    ~UdpPacket();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    UdpPacket& operator=(bslmf::MovableRef<UdpPacket> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    UdpPacket& operator=(const UdpPacket& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the header to the specified 'value'.
    void setHeader(const ntsa::UdpHeader& value);

    /// Set the payload to the specified 'value'.
    void setPayload(const ntsa::UdpPayload& value);

    /// Return a reference to the modifiable header.
    ntsa::UdpHeader& header();

    /// Return a reference to the modifiable payload.
    ntsa::UdpPayload& payload();

    /// Decode the packet from the specified 'source' starting at the specified
    /// 'offset' inside the framing packet having the specified 'packetSize'.
    /// Return the error.
    ntsa::Error decode(const bdlbb::BlobBuffer& source,
                       bsl::size_t              offset,
                       bsl::size_t              packetSize);

    /// Encode the packet to the specified 'buffer' starting at the specified
    /// 'offset'. Calculate the checksum in terms of the specified
    /// 'sourceAddress' to the specified 'destinationAddress'. Return the
    /// error.
    ntsa::Error encode(bdlbb::BlobBuffer*       buffer,
                       bsl::size_t              offset,
                       bsl::size_t              packetSize,
                       const ntsa::Ipv4Address& sourceAddress,
                       const ntsa::Ipv4Address& destinationAddress) const;

    /// Encode the packet to the specified 'buffer' starting at the specified
    /// 'offset'. Calculate the checksum in terms of the specified
    /// 'sourceAddress' to the specified 'destinationAddress'. Return the
    /// error.
    ntsa::Error encode(bdlbb::BlobBuffer*       buffer,
                       bsl::size_t              offset,
                       bsl::size_t              packetSize,
                       const ntsa::Ipv6Address& sourceAddress,
                       const ntsa::Ipv6Address& destinationAddress) const;

    /// Return a reference to the non-modifiable header.
    const ntsa::UdpHeader& header() const;

    /// Return a reference to the non-modifiable payload.
    const ntsa::UdpPayload& payload() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const UdpPacket& other) const;

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

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(UdpPacket);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::UdpPacket
bsl::ostream& operator<<(bsl::ostream& stream, const UdpPacket& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::UdpPacket
bool operator==(const UdpPacket& lhs, const UdpPacket& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::UdpPacket
bool operator!=(const UdpPacket& lhs, const UdpPacket& rhs);

NTSCFG_INLINE
UdpPacket::UdpPacket()
: d_header()
, d_payload()
{
}

NTSCFG_INLINE
UdpPacket::UdpPacket(bslmf::MovableRef<UdpPacket> original) NTSCFG_NOEXCEPT
: d_header(NTSCFG_MOVE_FROM(original, d_header)),
  d_payload(NTSCFG_MOVE_FROM(original, d_payload))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
UdpPacket::UdpPacket(const UdpPacket& original)
: d_header(original.d_header)
, d_payload(original.d_payload)
{
}

NTSCFG_INLINE
UdpPacket::~UdpPacket()
{
}

NTSCFG_INLINE
UdpPacket& UdpPacket::operator=(bslmf::MovableRef<UdpPacket> other)
    NTSCFG_NOEXCEPT
{
    d_header  = NTSCFG_MOVE_FROM(other, d_header);
    d_payload = NTSCFG_MOVE_FROM(other, d_payload);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
UdpPacket& UdpPacket::operator=(const UdpPacket& other)
{
    d_header  = other.d_header;
    d_payload = other.d_payload;
    return *this;
}

NTSCFG_INLINE
void UdpPacket::reset()
{
    d_header.reset();
    d_payload.reset();
}

NTSCFG_INLINE
void UdpPacket::setHeader(const ntsa::UdpHeader& value)
{
    d_header = value;
}

NTSCFG_INLINE
void UdpPacket::setPayload(const ntsa::UdpPayload& value)
{
    d_payload = value;
}

NTSCFG_INLINE
ntsa::UdpHeader& UdpPacket::header()
{
    return d_header;
}

NTSCFG_INLINE
ntsa::UdpPayload& UdpPacket::payload()
{
    return d_payload;
}

NTSCFG_INLINE
const ntsa::UdpHeader& UdpPacket::header() const
{
    return d_header;
}

NTSCFG_INLINE
const ntsa::UdpPayload& UdpPacket::payload() const
{
    return d_payload;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const UdpPacket& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const UdpPacket& lhs, const UdpPacket& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const UdpPacket& lhs, const UdpPacket& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
#endif
