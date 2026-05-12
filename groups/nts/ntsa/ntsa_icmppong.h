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

#ifndef INCLUDED_NTSA_ICMPPONG
#define INCLUDED_NTSA_ICMPPONG

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

/// Provide the body of an ICMP type 0 echo reply message.
///
/// @details
/// The ICMP echo reply message is sent in response to an ICMP echo request
/// (type 8). The replying host copies the identifier and sequence number from
/// the request back into the reply unchanged, allowing the original sender to
/// match each reply to the outstanding request that triggered it, as described
/// in RFC 792.
///
/// The full echo reply wire format, carried inside an IP datagram, is:
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
/// The type (0), code (0), and checksum fields are represented by
/// 'ntsa::IcmpHeader'. This class represents the message-specific body
/// that immediately follows the header: the identifier and sequence number.
///
/// Identifier (16 bits): Copied unchanged from the echo request. Used by the
/// original sender to associate the reply with a particular session or
/// process.
///
/// Sequence Number (16 bits): Copied unchanged from the echo request. Used
/// by the original sender to match the reply to a specific outstanding
/// request.
///
/// Both fields are stored in network byte order.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpPong
{
    /// The identifier.
    bdlb::BigEndianUint16 d_identifier;

    /// The sequence number.
    bdlb::BigEndianUint16 d_sequenceNumber;

    /// The data.
    bdlbb::BlobBuffer d_data;

  private:
    /// Print the specified 'data' to the specified 'stream'.
    static bsl::ostream& printData(bsl::ostream&            stream,
                                   const bdlbb::BlobBuffer& data,
                                   int                      level,
                                   int                      spacesPerLevel);

  public:
    /// Create a new ICMP pong having a default value.
    IcmpPong();

    /// Create a new ICMP pong having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IcmpPong(bslmf::MovableRef<IcmpPong> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP pong having the same value as the specified
    /// 'original' object.
    IcmpPong(const IcmpPong& original);

    /// Destroy this object.
    ~IcmpPong();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpPong& operator=(bslmf::MovableRef<IcmpPong> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpPong& operator=(const IcmpPong& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(bsl::uint16_t value);

    /// Set the sequence number to the specified 'value'.
    void setSequenceNumber(ntsa::CircularUint16 value);

    /// Set the data to the specified 'value'.
    void setData(const bdlbb::BlobBuffer& value);

    /// Set the data to the specified 'value'. Assign an unspecified but valid
    /// value to the 'original' original.
    void setData(bslmf::MovableRef<bdlbb::BlobBuffer> value);

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

    /// Return the data.
    const bdlbb::BlobBuffer& data() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IcmpPong& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IcmpPong& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IcmpPong);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IcmpPong);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IcmpPong);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IcmpPong
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpPong& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpPong
bool operator==(const IcmpPong& lhs, const IcmpPong& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpPong
bool operator!=(const IcmpPong& lhs, const IcmpPong& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpPong
bool operator<(const IcmpPong& lhs, const IcmpPong& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpPong
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpPong& value);

NTSCFG_INLINE
IcmpPong::IcmpPong()
: d_identifier()
, d_sequenceNumber()
, d_data()
{
}

NTSCFG_INLINE
IcmpPong::IcmpPong(bslmf::MovableRef<IcmpPong> original) NTSCFG_NOEXCEPT
: d_identifier(NTSCFG_MOVE_FROM(original, d_identifier)),
  d_sequenceNumber(NTSCFG_MOVE_FROM(original, d_sequenceNumber)),
  d_data(NTSCFG_MOVE_FROM(original, d_data))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IcmpPong::IcmpPong(const IcmpPong& original)
: d_identifier(original.d_identifier)
, d_sequenceNumber(original.d_sequenceNumber)
, d_data(original.d_data)
{
}

NTSCFG_INLINE
IcmpPong::~IcmpPong()
{
}

NTSCFG_INLINE
IcmpPong& IcmpPong::operator=(bslmf::MovableRef<IcmpPong> other)
    NTSCFG_NOEXCEPT
{
    d_identifier     = NTSCFG_MOVE_FROM(other, d_identifier);
    d_sequenceNumber = NTSCFG_MOVE_FROM(other, d_sequenceNumber);
    d_data           = NTSCFG_MOVE_FROM(other, d_data);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IcmpPong& IcmpPong::operator=(const IcmpPong& other)
{
    d_identifier     = other.d_identifier;
    d_sequenceNumber = other.d_sequenceNumber;
    d_data           = other.d_data;

    return *this;
}

NTSCFG_INLINE
void IcmpPong::reset()
{
    d_identifier     = static_cast<bsl::uint16_t>(0);
    d_sequenceNumber = static_cast<bsl::uint16_t>(0);
    d_data.reset();
}

NTSCFG_INLINE
void IcmpPong::setIdentifier(bsl::uint16_t value)
{
    d_identifier = value;
}

NTSCFG_INLINE
void IcmpPong::setSequenceNumber(ntsa::CircularUint16 value)
{
    d_sequenceNumber = value.value();
}

NTSCFG_INLINE
void IcmpPong::setData(const bdlbb::BlobBuffer& value)
{
    d_data = value;
}

NTSCFG_INLINE
void IcmpPong::setData(bslmf::MovableRef<bdlbb::BlobBuffer> value)
{
    d_data = NTSCFG_MOVE(value);
}

NTSCFG_INLINE
bsl::uint16_t IcmpPong::identifier() const
{
    return static_cast<bsl::uint16_t>(d_identifier);
}

NTSCFG_INLINE
ntsa::CircularUint16 IcmpPong::sequenceNumber() const
{
    return ntsa::CircularUint16(static_cast<bsl::uint16_t>(d_sequenceNumber));
}

NTSCFG_INLINE
const bdlbb::BlobBuffer& IcmpPong::data() const
{
    return d_data;
}

NTSCFG_INLINE
bool IcmpPong::equals(const IcmpPong& other) const
{
    if (d_identifier != other.d_identifier) {
        return false;
    }

    if (d_sequenceNumber != other.d_sequenceNumber) {
        return false;
    }

    if (d_data.size() != other.d_data.size()) {
        return false;
    }

    const int compare =
        bsl::memcmp(d_data.data(), other.d_data.data(), d_data.size());
    if (compare != 0) {
        return false;
    }

    return true;
}

NTSCFG_INLINE
bool IcmpPong::less(const IcmpPong& other) const
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

    if (static_cast<bsl::uint16_t>(d_sequenceNumber) <
        static_cast<bsl::uint16_t>(other.d_sequenceNumber))
    {
        return true;
    }

    if (static_cast<bsl::uint16_t>(other.d_sequenceNumber) <
        static_cast<bsl::uint16_t>(d_sequenceNumber))
    {
        return false;
    }

    const int compare =
        bsl::memcmp(d_data.data(), other.d_data.data(), d_data.size());
    if (compare >= 0) {
        return false;
    }

    return true;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpPong::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, static_cast<bsl::uint16_t>(d_identifier));
    hashAppend(algorithm, static_cast<bsl::uint16_t>(d_sequenceNumber));

    if (d_data.size() > 0) {
        algorithm(d_data.data(), d_data.size());
    }
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpPong& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpPong& lhs, const IcmpPong& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpPong& lhs, const IcmpPong& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IcmpPong& lhs, const IcmpPong& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM& algorithm, const IcmpPong& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
