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

#ifndef INCLUDED_NTSA_TCPTIMEPOINT
#define INCLUDED_NTSA_TCPTIMEPOINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_circular.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Define a type alias for a 32-bit unsigned integer with modular arithmetic
/// that represents a TCP time point.
///
/// @ingroup module_ntsa_identity
typedef CircularUint32 TcpTimePoint;

/// Describe pair of TCP timestamps used to calculate round trip time, or
/// how long a TCP packet took to be acknowledged.
///
/// @ingroup module_ntsa_identity
class TcpTimePointInterval
{
    TcpTimePoint d_tx;
    TcpTimePoint d_rx;

  public:
    /// Create a new TCP time point interval having a default value.
    TcpTimePointInterval();

    /// Create a new TCP time point interval having the same value as the
    /// specified 'original' object. Assign an unspecified but valid value to
    /// the 'original' original.
    TcpTimePointInterval(bslmf::MovableRef<TcpTimePointInterval> original)
        NTSCFG_NOEXCEPT;

    /// Create a new TCP time point interval having the same value as the
    /// specified 'original' object.
    TcpTimePointInterval(const TcpTimePointInterval& original);

    /// Destroy this object.
    ~TcpTimePointInterval();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    TcpTimePointInterval& operator=(
        bslmf::MovableRef<TcpTimePointInterval> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    TcpTimePointInterval& operator=(const TcpTimePointInterval& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the time point of transmission to the specified 'value'.
    void setTx(TcpTimePoint value);

    /// Set the time point of reception to the specified 'value'.
    void setRx(TcpTimePoint value);

    /// Return the time point of transmission.
    TcpTimePoint tx() const;

    /// Return the time point of reception.
    TcpTimePoint rx() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const TcpTimePointInterval& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const TcpTimePointInterval& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(TcpTimePointInterval);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(TcpTimePointInterval);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(TcpTimePointInterval);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::TcpTimePointInterval
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const TcpTimePointInterval& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::TcpTimePointInterval
bool operator==(const TcpTimePointInterval& lhs,
                const TcpTimePointInterval& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::TcpTimePointInterval
bool operator!=(const TcpTimePointInterval& lhs,
                const TcpTimePointInterval& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::TcpTimePointInterval
bool operator<(const TcpTimePointInterval& lhs,
               const TcpTimePointInterval& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::TcpTimePointInterval
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TcpTimePointInterval& value);

NTSCFG_INLINE
TcpTimePointInterval::TcpTimePointInterval()
: d_tx(0)
, d_rx(0)
{
}

NTSCFG_INLINE
TcpTimePointInterval::TcpTimePointInterval(
    bslmf::MovableRef<TcpTimePointInterval> original) NTSCFG_NOEXCEPT
: d_tx(NTSCFG_MOVE_FROM(original, d_tx)),
  d_rx(NTSCFG_MOVE_FROM(original, d_rx))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
TcpTimePointInterval::TcpTimePointInterval(
    const TcpTimePointInterval& original)
: d_tx(original.d_tx)
, d_rx(original.d_rx)
{
}

NTSCFG_INLINE
TcpTimePointInterval::~TcpTimePointInterval()
{
}

NTSCFG_INLINE
TcpTimePointInterval& TcpTimePointInterval::operator=(
    bslmf::MovableRef<TcpTimePointInterval> other) NTSCFG_NOEXCEPT
{
    d_tx = NTSCFG_MOVE_FROM(other, d_tx);
    d_rx = NTSCFG_MOVE_FROM(other, d_rx);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
TcpTimePointInterval& TcpTimePointInterval::operator=(
    const TcpTimePointInterval& other)
{
    d_tx = other.d_tx;
    d_rx = other.d_rx;

    return *this;
}

NTSCFG_INLINE
void TcpTimePointInterval::reset()
{
    d_tx = 0;
    d_rx = 0;
}

NTSCFG_INLINE
void TcpTimePointInterval::setTx(TcpTimePoint value)
{
    d_tx = value;
}

NTSCFG_INLINE
void TcpTimePointInterval::setRx(TcpTimePoint value)
{
    d_rx = value;
}

NTSCFG_INLINE
TcpTimePoint TcpTimePointInterval::tx() const
{
    return d_tx;
}

NTSCFG_INLINE
TcpTimePoint TcpTimePointInterval::rx() const
{
    return d_rx;
}

NTSCFG_INLINE
bool TcpTimePointInterval::equals(const TcpTimePointInterval& other) const
{
    return d_tx == other.d_tx && d_rx == other.d_rx;
}

NTSCFG_INLINE
bool TcpTimePointInterval::less(const TcpTimePointInterval& other) const
{
    if (d_tx < other.d_tx) {
        return true;
    }

    if (other.d_tx < d_tx) {
        return false;
    }

    return d_rx < other.d_rx;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void TcpTimePointInterval::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_tx);
    hashAppend(algorithm, d_rx);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const TcpTimePointInterval& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const TcpTimePointInterval& lhs,
                const TcpTimePointInterval& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const TcpTimePointInterval& lhs,
                const TcpTimePointInterval& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const TcpTimePointInterval& lhs,
               const TcpTimePointInterval& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&             algorithm,
                              const TcpTimePointInterval& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
