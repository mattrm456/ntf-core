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

#ifndef INCLUDED_NTSA_TCPSEQUENCENUMBER
#define INCLUDED_NTSA_TCPSEQUENCENUMBER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_circular.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Define a type alias for a 32-bit unsigned integer with modular arithmetic
/// that represents a TCP sequence number.
///
/// @ingroup module_ntsa_identity
typedef CircularUint32 TcpSequenceNumber;

/// Describe a closed range of TCP sequence numbers.
///
/// @ingroup module_ntsa_identity
class TcpSequenceRange
{
    TcpSequenceNumber d_oldest;
    TcpSequenceNumber d_newest;

  public:
    /// Create a new TCP sequence number range having a default value.
    TcpSequenceRange();

    /// Create a new TCP sequence number range having the same value as the
    /// specified 'original' object. Assign an unspecified but valid value to
    /// the 'original' original.
    TcpSequenceRange(bslmf::MovableRef<TcpSequenceRange> original)
        NTSCFG_NOEXCEPT;

    /// Create a new TCP sequence number range having the same value as the
    /// specified 'original' object.
    TcpSequenceRange(const TcpSequenceRange& original);

    /// Destroy this object.
    ~TcpSequenceRange();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    TcpSequenceRange& operator=(bslmf::MovableRef<TcpSequenceRange> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    TcpSequenceRange& operator=(const TcpSequenceRange& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the oldest sequence number in the range, inclusive, to the
    /// specified 'value'.
    void setOldest(TcpSequenceNumber value);

    /// Set the newest sequence number in the range, inclusive, to the
    /// specified 'value'.
    void setNewest(TcpSequenceNumber value);

    /// Return the oldest sequence number in the range, inclusive.
    TcpSequenceNumber oldest() const;

    /// Return the newest sequence number in the range, inclusive.
    TcpSequenceNumber newest() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const TcpSequenceRange& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const TcpSequenceRange& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

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

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(TcpSequenceRange);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(TcpSequenceRange);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(TcpSequenceRange);
};

/// Define type alias for a vector of closed ranges of TCP sequence numbers.
///
/// @ingroup module_ntsa_identity
typedef bsl::vector<TcpSequenceRange> TcpSequenceRangeVector;

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::TcpSequenceRange
bsl::ostream& operator<<(bsl::ostream& stream, const TcpSequenceRange& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::TcpSequenceRange
bool operator==(const TcpSequenceRange& lhs, const TcpSequenceRange& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::TcpSequenceRange
bool operator!=(const TcpSequenceRange& lhs, const TcpSequenceRange& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::TcpSequenceRange
bool operator<(const TcpSequenceRange& lhs, const TcpSequenceRange& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::TcpSequenceRange
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TcpSequenceRange& value);

NTSCFG_INLINE
TcpSequenceRange::TcpSequenceRange()
: d_oldest(0)
, d_newest(0)
{
}

NTSCFG_INLINE
TcpSequenceRange::TcpSequenceRange(bslmf::MovableRef<TcpSequenceRange> original)
    NTSCFG_NOEXCEPT
: d_oldest(NTSCFG_MOVE_FROM(original, d_oldest))
, d_newest(NTSCFG_MOVE_FROM(original, d_newest))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
TcpSequenceRange::TcpSequenceRange(const TcpSequenceRange& original)
: d_oldest(original.d_oldest)
, d_newest(original.d_newest)
{
}

NTSCFG_INLINE
TcpSequenceRange::~TcpSequenceRange()
{
}

NTSCFG_INLINE
TcpSequenceRange& TcpSequenceRange::operator=(
    bslmf::MovableRef<TcpSequenceRange> other) NTSCFG_NOEXCEPT
{
    d_oldest = NTSCFG_MOVE_FROM(other, d_oldest);
    d_newest = NTSCFG_MOVE_FROM(other, d_newest);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
TcpSequenceRange& TcpSequenceRange::operator=(const TcpSequenceRange& other)
{
    d_oldest = other.d_oldest;
    d_newest = other.d_newest;

    return *this;
}

NTSCFG_INLINE
void TcpSequenceRange::reset()
{
    d_oldest = 0;
    d_newest = 0;
}

NTSCFG_INLINE
void TcpSequenceRange::setOldest(TcpSequenceNumber value)
{
    d_oldest = value;
}

NTSCFG_INLINE
void TcpSequenceRange::setNewest(TcpSequenceNumber value)
{
    d_newest = value;
}

NTSCFG_INLINE
TcpSequenceNumber TcpSequenceRange::oldest() const
{
    return d_oldest;
}

NTSCFG_INLINE
TcpSequenceNumber TcpSequenceRange::newest() const
{
    return d_newest;
}

NTSCFG_INLINE
bool TcpSequenceRange::equals(const TcpSequenceRange& other) const
{
    return d_oldest == other.d_oldest && d_newest == other.d_newest;
}

NTSCFG_INLINE
bool TcpSequenceRange::less(const TcpSequenceRange& other) const
{
    if (d_oldest < other.d_oldest) {
        return true;
    }

    if (other.d_oldest < d_oldest) {
        return false;
    }

    return d_newest < other.d_newest;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE
void TcpSequenceRange::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_oldest);
    hashAppend(algorithm, d_newest);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const TcpSequenceRange& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const TcpSequenceRange& lhs, const TcpSequenceRange& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const TcpSequenceRange& lhs, const TcpSequenceRange& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const TcpSequenceRange& lhs, const TcpSequenceRange& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE
void hashAppend(HASH_ALGORITHM& algorithm, const TcpSequenceRange& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
