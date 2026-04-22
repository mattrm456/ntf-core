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

#ifndef INCLUDED_NTSA_IPV6PACKET
#define INCLUDED_NTSA_IPV6PACKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv6header.h>
#include <ntsa_ipv6payload.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a Internet Protocol version 6 (IPv6) packet.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class Ipv6Packet
{
    ntsa::Ipv6Header  d_header;
    ntsa::Ipv6Payload d_payload;

  public:
    /// Create a new IPv6 packet having a default value.
    Ipv6Packet();

    /// Create a new IPv6 packet having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    Ipv6Packet(bslmf::MovableRef<Ipv6Packet> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv6 packet having the same value as the specified
    /// 'original' object.
    Ipv6Packet(const Ipv6Packet& original);

    /// Destroy this object.
    ~Ipv6Packet();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Ipv6Packet& operator=(bslmf::MovableRef<Ipv6Packet> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Ipv6Packet& operator=(const Ipv6Packet& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the header to the specified 'value'.
    void setHeader(const ntsa::Ipv6Header& value);

    /// Set the payload to the specified 'value'.
    void setPayload(const ntsa::Ipv6Payload& value);

    /// Return a reference to the modifiable header.
    ntsa::Ipv6Header& header();

    /// Return a reference to the modifiable payload.
    ntsa::Ipv6Payload& payload();

    /// Return a reference to the non-modifiable header.
    const ntsa::Ipv6Header& header() const;

    /// Return a reference to the non-modifiable payload.
    const ntsa::Ipv6Payload& payload() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Ipv6Packet& other) const;

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
                        int           spacesPerLevel = 6) const;

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Ipv6Packet);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::Ipv6Packet
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Packet& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Packet
bool operator==(const Ipv6Packet& lhs, const Ipv6Packet& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Packet
bool operator!=(const Ipv6Packet& lhs, const Ipv6Packet& rhs);

NTSCFG_INLINE
Ipv6Packet::Ipv6Packet()
: d_header()
, d_payload()
{
}

NTSCFG_INLINE
Ipv6Packet::Ipv6Packet(bslmf::MovableRef<Ipv6Packet> original) NTSCFG_NOEXCEPT
: d_header(NTSCFG_MOVE_FROM(original, d_header)),
  d_payload(NTSCFG_MOVE_FROM(original, d_payload))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
Ipv6Packet::Ipv6Packet(const Ipv6Packet& original)
: d_header(original.d_header)
, d_payload(original.d_payload)
{
}

NTSCFG_INLINE
Ipv6Packet::~Ipv6Packet()
{
}

NTSCFG_INLINE
Ipv6Packet& Ipv6Packet::operator=(bslmf::MovableRef<Ipv6Packet> other)
    NTSCFG_NOEXCEPT
{
    d_header  = NTSCFG_MOVE_FROM(other, d_header);
    d_payload = NTSCFG_MOVE_FROM(other, d_payload);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Ipv6Packet& Ipv6Packet::operator=(const Ipv6Packet& other)
{
    d_header  = other.d_header;
    d_payload = other.d_payload;
    return *this;
}

NTSCFG_INLINE
void Ipv6Packet::reset()
{
    d_header.reset();
    d_payload.reset();
}

NTSCFG_INLINE
void Ipv6Packet::setHeader(const ntsa::Ipv6Header& value)
{
    d_header = value;
}

NTSCFG_INLINE
void Ipv6Packet::setPayload(const ntsa::Ipv6Payload& value)
{
    d_payload = value;
}

NTSCFG_INLINE
ntsa::Ipv6Header& Ipv6Packet::header()
{
    return d_header;
}

NTSCFG_INLINE
ntsa::Ipv6Payload& Ipv6Packet::payload()
{
    return d_payload;
}

NTSCFG_INLINE
const ntsa::Ipv6Header& Ipv6Packet::header() const
{
    return d_header;
}

NTSCFG_INLINE
const ntsa::Ipv6Payload& Ipv6Packet::payload() const
{
    return d_payload;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Packet& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Ipv6Packet& lhs, const Ipv6Packet& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Ipv6Packet& lhs, const Ipv6Packet& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
#endif
