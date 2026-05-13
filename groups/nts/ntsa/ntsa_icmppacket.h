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

#ifndef INCLUDED_NTSA_ICMPPACKET
#define INCLUDED_NTSA_ICMPPACKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_icmpheader.h>
#include <ntsa_icmppayload.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Control Message Protocol (ICMP) packet.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpPacket
{
    ntsa::IcmpHeader  d_header;
    ntsa::IcmpPayload d_payload;

  public:
    /// Create a new ICMP packet having a default value.
    IcmpPacket();

    /// Create a new ICMP packet having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IcmpPacket(bslmf::MovableRef<IcmpPacket> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP packet having the same value as the specified
    /// 'original' object.
    IcmpPacket(const IcmpPacket& original);

    /// Destroy this object.
    ~IcmpPacket();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpPacket& operator=(bslmf::MovableRef<IcmpPacket> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    IcmpPacket& operator=(const IcmpPacket& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the header to the specified 'value'.
    void setHeader(const ntsa::IcmpHeader& value);

    /// Set the payload to the specified 'value'.
    void setPayload(const ntsa::IcmpPayload& value);

    /// Return a reference to the modifiable header.
    ntsa::IcmpHeader& header();

    /// Return a reference to the modifiable payload.
    ntsa::IcmpPayload& payload();

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder*     encoder,
                       const ntsa::Ipv4Address& sourceAddress,
                       const ntsa::Ipv4Address& destinationAddress) const;

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder*     encoder,
                       const ntsa::Ipv6Address& sourceAddress,
                       const ntsa::Ipv6Address& destinationAddress) const;

    /// Decode the packet from the specified 'buffer' starting at the specified
    /// 'offset' inside the framing packet having the specified 'packetSize'.
    /// Return the error.
    ntsa::Error decode(const bdlbb::BlobBuffer& buffer,
                       bsl::size_t              offset,
                       bsl::size_t              packetSize);

    /// Encode the packet to the specified 'buffer' starting at the specified
    /// 'offset'. Calculate the checksum in terms of the specified
    /// 'sourceAddress' to the specified 'destinationAddress'. Return the
    /// error.
    ntsa::Error encode(bdlbb::BlobBuffer*       buffer,
                       bsl::size_t              offset,
                       const ntsa::Ipv4Address& sourceAddress,
                       const ntsa::Ipv4Address& destinationAddress) const;

    /// Encode the packet to the specified 'buffer' starting at the specified
    /// 'offset'. Calculate the checksum in terms of the specified
    /// 'sourceAddress' to the specified 'destinationAddress'. Return the
    /// error.
    ntsa::Error encode(bdlbb::BlobBuffer*       buffer,
                       bsl::size_t              offset,
                       const ntsa::Ipv6Address& sourceAddress,
                       const ntsa::Ipv6Address& destinationAddress) const;

    /// Return a reference to the non-modifiable header.
    const ntsa::IcmpHeader& header() const;

    /// Return a reference to the non-modifiable payload.
    const ntsa::IcmpPayload& payload() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const IcmpPacket& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IcmpPacket);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::IcmpPacket
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpPacket& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpPacket
bool operator==(const IcmpPacket& lhs, const IcmpPacket& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpPacket
bool operator!=(const IcmpPacket& lhs, const IcmpPacket& rhs);

NTSCFG_INLINE
IcmpPacket::IcmpPacket()
: d_header()
, d_payload()
{
}

NTSCFG_INLINE
IcmpPacket::IcmpPacket(bslmf::MovableRef<IcmpPacket> original) NTSCFG_NOEXCEPT
: d_header(NTSCFG_MOVE_FROM(original, d_header)),
  d_payload(NTSCFG_MOVE_FROM(original, d_payload))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IcmpPacket::IcmpPacket(const IcmpPacket& original)
: d_header(original.d_header)
, d_payload(original.d_payload)
{
}

NTSCFG_INLINE
IcmpPacket::~IcmpPacket()
{
}

NTSCFG_INLINE
IcmpPacket& IcmpPacket::operator=(bslmf::MovableRef<IcmpPacket> other)
    NTSCFG_NOEXCEPT
{
    d_header  = NTSCFG_MOVE_FROM(other, d_header);
    d_payload = NTSCFG_MOVE_FROM(other, d_payload);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IcmpPacket& IcmpPacket::operator=(const IcmpPacket& other)
{
    d_header  = other.d_header;
    d_payload = other.d_payload;
    return *this;
}

NTSCFG_INLINE
void IcmpPacket::reset()
{
    d_header.reset();
    d_payload.reset();
}

NTSCFG_INLINE
void IcmpPacket::setHeader(const ntsa::IcmpHeader& value)
{
    d_header = value;
}

NTSCFG_INLINE
void IcmpPacket::setPayload(const ntsa::IcmpPayload& value)
{
    d_payload = value;
}

NTSCFG_INLINE
ntsa::IcmpHeader& IcmpPacket::header()
{
    return d_header;
}

NTSCFG_INLINE
ntsa::IcmpPayload& IcmpPacket::payload()
{
    return d_payload;
}

NTSCFG_INLINE
const ntsa::IcmpHeader& IcmpPacket::header() const
{
    return d_header;
}

NTSCFG_INLINE
const ntsa::IcmpPayload& IcmpPacket::payload() const
{
    return d_payload;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpPacket& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpPacket& lhs, const IcmpPacket& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpPacket& lhs, const IcmpPacket& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
#endif
