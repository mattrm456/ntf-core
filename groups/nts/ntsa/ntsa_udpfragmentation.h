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

#ifndef INCLUDED_NTSA_UDPFRAGMENTATION
#define INCLUDED_NTSA_UDPFRAGMENTATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_circular.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntsa {

/// Describe UDP fragmentation.
///
/// @ingroup module_ntsa_protocol
class UdpFragmentation
{
    bsl::uint32_t                      d_identifier;
    bsl::uint16_t                      d_start;
    bsl::uint16_t                      d_offset;
    bdlb::NullableValue<bsl::uint16_t> d_rdos;

  public:
    /// Create a new UDP time point interval having a default value.
    UdpFragmentation();

    /// Create a new UDP time point interval having the same value as the
    /// specified 'original' object. Assign an unspecified but valid value to
    /// the 'original' original.
    UdpFragmentation(bslmf::MovableRef<UdpFragmentation> original)
        NTSCFG_NOEXCEPT;

    /// Create a new UDP time point interval having the same value as the
    /// specified 'original' object.
    UdpFragmentation(const UdpFragmentation& original);

    /// Destroy this object.
    ~UdpFragmentation();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    UdpFragmentation& operator=(bslmf::MovableRef<UdpFragmentation> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UdpFragmentation& operator=(const UdpFragmentation& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier of the original datagram to the specified 'value'.
    void setIdentifier(bsl::uint32_t value);

    /// Set the location of the start of the fragment data, measured from
    /// the start of the UDP header, to the specified 'value'.
    void setStart(bsl::uint16_t value);

    /// Set the location of the fragment in the original datagram, measured
    /// from the start of the UDP header, to the specified 'value'.
    void setOffset(bsl::uint16_t value);

    /// Set the reassembled datagram option start pointer to the specified
    /// 'value'.
    void setRdos(bsl::uint16_t value);

    /// Return the identifier of the original datagram.
    bsl::uint32_t identifier() const;

    /// Return the location of the start of the fragment data, measured from
    /// the start of the UDP header.
    bsl::uint16_t start() const;

    /// Return the location of the fragment in the original datagram, measured
    /// from the start of the UDP header.
    bsl::uint16_t offset() const;

    /// Return the reassembled datagram option start pointer.
    const bdlb::NullableValue<bsl::uint16_t>& rdos() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const UdpFragmentation& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const UdpFragmentation& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(UdpFragmentation);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(UdpFragmentation);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(UdpFragmentation);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::UdpFragmentation
bsl::ostream& operator<<(bsl::ostream& stream, const UdpFragmentation& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::UdpFragmentation
bool operator==(const UdpFragmentation& lhs, const UdpFragmentation& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::UdpFragmentation
bool operator!=(const UdpFragmentation& lhs, const UdpFragmentation& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::UdpFragmentation
bool operator<(const UdpFragmentation& lhs, const UdpFragmentation& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::UdpFragmentation
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UdpFragmentation& value);

NTSCFG_INLINE
UdpFragmentation::UdpFragmentation()
: d_identifier(0)
, d_start(0)
, d_offset(0)
, d_rdos()
{
}

NTSCFG_INLINE
UdpFragmentation::UdpFragmentation(
    bslmf::MovableRef<UdpFragmentation> original) NTSCFG_NOEXCEPT
: d_identifier(NTSCFG_MOVE_FROM(original, d_identifier)),
  d_start(NTSCFG_MOVE_FROM(original, d_start)),
  d_offset(NTSCFG_MOVE_FROM(original, d_offset)),
  d_rdos(NTSCFG_MOVE_FROM(original, d_rdos))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
UdpFragmentation::UdpFragmentation(const UdpFragmentation& original)
: d_identifier(original.d_identifier)
, d_start(original.d_start)
, d_offset(original.d_offset)
, d_rdos(original.d_rdos)
{
}

NTSCFG_INLINE
UdpFragmentation::~UdpFragmentation()
{
}

NTSCFG_INLINE
UdpFragmentation& UdpFragmentation::operator=(
    bslmf::MovableRef<UdpFragmentation> other) NTSCFG_NOEXCEPT
{
    d_identifier = NTSCFG_MOVE_FROM(other, d_identifier);
    d_start      = NTSCFG_MOVE_FROM(other, d_start);
    d_offset     = NTSCFG_MOVE_FROM(other, d_offset);
    d_rdos       = NTSCFG_MOVE_FROM(other, d_rdos);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
UdpFragmentation& UdpFragmentation::operator=(const UdpFragmentation& other)
{
    d_identifier = other.d_identifier;
    d_start      = other.d_start;
    d_offset     = other.d_offset;
    d_rdos       = other.d_rdos;

    return *this;
}

NTSCFG_INLINE
void UdpFragmentation::reset()
{
    d_identifier = 0;
    d_start      = 0;
    d_offset     = 0;
    d_rdos.reset();
}

NTSCFG_INLINE
void UdpFragmentation::setIdentifier(bsl::uint32_t value)
{
    d_identifier = value;
}

NTSCFG_INLINE
void UdpFragmentation::setStart(bsl::uint16_t value)
{
    d_start = value;
}

NTSCFG_INLINE
void UdpFragmentation::setOffset(bsl::uint16_t value)
{
    d_offset = value;
}

NTSCFG_INLINE
void UdpFragmentation::setRdos(bsl::uint16_t value)
{
    d_rdos = value;
}

NTSCFG_INLINE
bsl::uint32_t UdpFragmentation::identifier() const
{
    return d_identifier;
}

NTSCFG_INLINE
bsl::uint16_t UdpFragmentation::start() const
{
    return d_start;
}

NTSCFG_INLINE
bsl::uint16_t UdpFragmentation::offset() const
{
    return d_offset;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::uint16_t>& UdpFragmentation::rdos() const
{
    return d_rdos;
}

NTSCFG_INLINE
bool UdpFragmentation::equals(const UdpFragmentation& other) const
{
    return d_identifier == other.d_identifier && d_start == other.d_start &&
           d_offset == other.d_offset && d_rdos == other.d_rdos;
}

NTSCFG_INLINE
bool UdpFragmentation::less(const UdpFragmentation& other) const
{
    if (d_identifier < other.d_identifier) {
        return true;
    }

    if (other.d_identifier < d_identifier) {
        return false;
    }

    if (d_start < other.d_start) {
        return true;
    }

    if (other.d_start < d_start) {
        return false;
    }

    if (d_offset < other.d_offset) {
        return true;
    }

    if (other.d_offset < d_offset) {
        return false;
    }

    return d_rdos < other.d_rdos;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void UdpFragmentation::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifier);
    hashAppend(algorithm, d_start);
    hashAppend(algorithm, d_offset);
    hashAppend(algorithm, d_rdos);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const UdpFragmentation& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const UdpFragmentation& lhs, const UdpFragmentation& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const UdpFragmentation& lhs, const UdpFragmentation& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const UdpFragmentation& lhs, const UdpFragmentation& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&         algorithm,
                              const UdpFragmentation& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
