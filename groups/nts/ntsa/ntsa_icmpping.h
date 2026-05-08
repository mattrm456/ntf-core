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

#ifndef INCLUDED_NTSA_ICMPPING
#define INCLUDED_NTSA_ICMPPING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_circular.h>
#include <ntsa_error.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide the body of an ICMP type 8 echo request message.
///
/// @details
/// The ICMP echo request message, commonly known as a "ping", is sent by a
/// host to verify that a destination is reachable and to measure round-trip
/// latency. The destination responds with an ICMP echo reply (type 0)
/// containing the same identifier and sequence number, as described in
/// RFC 792.
///
/// The full echo request wire format, carried inside an IP datagram, is:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |     Type      |     Code      |          Checksum             |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |           Identifier          |        Sequence Number        |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |     Data ...
/// +-+-+-+-+-
///```
///
/// The type (8), code (0), and checksum fields are represented by
/// 'ntsa::IcmpHeader'. This class represents the message-specific body
/// that immediately follows the header: the identifier and sequence number.
///
/// Identifier (16 bits): An arbitrary value chosen by the sender to
/// associate echo replies with their corresponding requests. A common
/// convention is to use the sender's process ID.
///
/// Sequence Number (16 bits): A counter incremented by the sender on each
/// successive echo request. Together with the identifier, it allows the
/// sender to match each reply to a specific outstanding request.
///
/// Both fields are echoed back unchanged in the reply and are stored in
/// network byte order.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpPing
{
    /// The identifier.
    bdlb::BigEndianUint16 d_identifier;

    /// The sequence number.
    bdlb::BigEndianUint16 d_sequenceNumber;

  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed length of the IcmpPing body in octets.
        k_LENGTH = 4
    };

    /// Create a new ICMP ping having a default value.
    IcmpPing();

    /// Create a new ICMP ping having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IcmpPing(bslmf::MovableRef<IcmpPing> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP ping having the same value as the specified
    /// 'original' object.
    IcmpPing(const IcmpPing& original);

    /// Destroy this object.
    ~IcmpPing();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpPing& operator=(bslmf::MovableRef<IcmpPing> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpPing& operator=(const IcmpPing& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(bsl::uint16_t value);

    /// Set the sequence number to the specified 'value'.
    void setSequenceNumber(ntsa::CircularUint16 value);

    /// Decode the body from the specified 'buffer' starting at the specified
    /// 'offset' inside the framing packet having the specified 'packetSize'.
    /// Return the error.
    ntsa::Error decode(const bdlbb::BlobBuffer& buffer,
                       bsl::size_t              offset,
                       bsl::size_t              packetSize);

    /// Encode the body to the specified 'buffer' starting at the specified
    /// 'offset'. Return the error.
    ntsa::Error encode(bdlbb::BlobBuffer* buffer, bsl::size_t offset) const;

    /// Return the identifier.
    bsl::uint16_t identifier() const;

    /// Return the sequence number.
    ntsa::CircularUint16 sequenceNumber() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IcmpPing& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IcmpPing& other) const;

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

    /// Print this object using the specified 'printer'.
    void print(bslim::Printer* printer) const;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IcmpPing);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IcmpPing);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IcmpPing);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IcmpPing
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpPing& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpPing
bool operator==(const IcmpPing& lhs, const IcmpPing& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpPing
bool operator!=(const IcmpPing& lhs, const IcmpPing& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpPing
bool operator<(const IcmpPing& lhs, const IcmpPing& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpPing
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpPing& value);

NTSCFG_INLINE
IcmpPing::IcmpPing()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);

    d_identifier     = static_cast<bsl::uint16_t>(0);
    d_sequenceNumber = static_cast<bsl::uint16_t>(0);
}

NTSCFG_INLINE
IcmpPing::IcmpPing(bslmf::MovableRef<IcmpPing> original) NTSCFG_NOEXCEPT
: d_identifier(NTSCFG_MOVE_FROM(original, d_identifier))
, d_sequenceNumber(NTSCFG_MOVE_FROM(original, d_sequenceNumber))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IcmpPing::IcmpPing(const IcmpPing& original)
: d_identifier(original.d_identifier)
, d_sequenceNumber(original.d_sequenceNumber)
{
}

NTSCFG_INLINE
IcmpPing::~IcmpPing()
{
}

NTSCFG_INLINE
IcmpPing& IcmpPing::operator=(bslmf::MovableRef<IcmpPing> other)
    NTSCFG_NOEXCEPT
{
    d_identifier     = NTSCFG_MOVE_FROM(other, d_identifier);
    d_sequenceNumber = NTSCFG_MOVE_FROM(other, d_sequenceNumber);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IcmpPing& IcmpPing::operator=(const IcmpPing& other)
{
    d_identifier     = other.d_identifier;
    d_sequenceNumber = other.d_sequenceNumber;

    return *this;
}

NTSCFG_INLINE
void IcmpPing::reset()
{
    d_identifier     = static_cast<bsl::uint16_t>(0);
    d_sequenceNumber = static_cast<bsl::uint16_t>(0);
}

NTSCFG_INLINE
void IcmpPing::setIdentifier(bsl::uint16_t value)
{
    d_identifier = value;
}

NTSCFG_INLINE
void IcmpPing::setSequenceNumber(ntsa::CircularUint16 value)
{
    d_sequenceNumber = value.value();
}

NTSCFG_INLINE
bsl::uint16_t IcmpPing::identifier() const
{
    return static_cast<bsl::uint16_t>(d_identifier);
}

NTSCFG_INLINE
ntsa::CircularUint16 IcmpPing::sequenceNumber() const
{
    return ntsa::CircularUint16(static_cast<bsl::uint16_t>(d_sequenceNumber));
}

NTSCFG_INLINE
bool IcmpPing::equals(const IcmpPing& other) const
{
    return d_identifier == other.d_identifier &&
           d_sequenceNumber == other.d_sequenceNumber;
}

NTSCFG_INLINE
bool IcmpPing::less(const IcmpPing& other) const
{
    if (static_cast<bsl::uint16_t>(d_identifier) <
        static_cast<bsl::uint16_t>(other.d_identifier))
    {
        return true;
    }

    if (static_cast<bsl::uint16_t>(other.d_identifier) <
        static_cast<bsl::uint16_t>(d_identifier))
    {
        return false;
    }

    return static_cast<bsl::uint16_t>(d_sequenceNumber) <
           static_cast<bsl::uint16_t>(other.d_sequenceNumber);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpPing::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, static_cast<bsl::uint16_t>(d_identifier));
    hashAppend(algorithm, static_cast<bsl::uint16_t>(d_sequenceNumber));
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpPing& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpPing& lhs, const IcmpPing& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpPing& lhs, const IcmpPing& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IcmpPing& lhs, const IcmpPing& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM& algorithm, const IcmpPing& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
