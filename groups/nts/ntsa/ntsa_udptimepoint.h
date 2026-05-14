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

#ifndef INCLUDED_NTSA_UDPTIMEPOINT
#define INCLUDED_NTSA_UDPTIMEPOINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_circular.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Define a type alias for a 32-bit unsigned integer with modular arithmetic
/// that represents a UDP time point.
///
/// @ingroup module_ntsa_protocol
typedef CircularUint32 UdpTimePoint;

/// Describe pair of UDP timestamps used to calculate round trip time, or
/// how long a UDP packet took to be acknowledged.
///
/// @ingroup module_ntsa_protocol
class UdpTimePointInterval
{
    UdpTimePoint d_tx;
    UdpTimePoint d_rx;

  public:
    /// Create a new UDP time point interval having a default value.
    UdpTimePointInterval();

    /// Create a new UDP time point interval having the same value as the
    /// specified 'original' object. Assign an unspecified but valid value to
    /// the 'original' original.
    UdpTimePointInterval(bslmf::MovableRef<UdpTimePointInterval> original)
        NTSCFG_NOEXCEPT;

    /// Create a new UDP time point interval having the same value as the
    /// specified 'original' object.
    UdpTimePointInterval(const UdpTimePointInterval& original);

    /// Destroy this object.
    ~UdpTimePointInterval();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    UdpTimePointInterval& operator=(
        bslmf::MovableRef<UdpTimePointInterval> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UdpTimePointInterval& operator=(const UdpTimePointInterval& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the time point of transmission to the specified 'value'.
    void setTx(UdpTimePoint value);

    /// Set the time point of reception to the specified 'value'.
    void setRx(UdpTimePoint value);

    /// Return the time point of transmission.
    UdpTimePoint tx() const;

    /// Return the time point of reception.
    UdpTimePoint rx() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const UdpTimePointInterval& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const UdpTimePointInterval& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(UdpTimePointInterval);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(UdpTimePointInterval);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(UdpTimePointInterval);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::UdpTimePointInterval
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const UdpTimePointInterval& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::UdpTimePointInterval
bool operator==(const UdpTimePointInterval& lhs,
                const UdpTimePointInterval& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::UdpTimePointInterval
bool operator!=(const UdpTimePointInterval& lhs,
                const UdpTimePointInterval& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::UdpTimePointInterval
bool operator<(const UdpTimePointInterval& lhs,
               const UdpTimePointInterval& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::UdpTimePointInterval
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UdpTimePointInterval& value);

NTSCFG_INLINE
UdpTimePointInterval::UdpTimePointInterval()
: d_tx(0)
, d_rx(0)
{
}

NTSCFG_INLINE
UdpTimePointInterval::UdpTimePointInterval(
    bslmf::MovableRef<UdpTimePointInterval> original) NTSCFG_NOEXCEPT
: d_tx(NTSCFG_MOVE_FROM(original, d_tx)),
  d_rx(NTSCFG_MOVE_FROM(original, d_rx))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
UdpTimePointInterval::UdpTimePointInterval(
    const UdpTimePointInterval& original)
: d_tx(original.d_tx)
, d_rx(original.d_rx)
{
}

NTSCFG_INLINE
UdpTimePointInterval::~UdpTimePointInterval()
{
}

NTSCFG_INLINE
UdpTimePointInterval& UdpTimePointInterval::operator=(
    bslmf::MovableRef<UdpTimePointInterval> other) NTSCFG_NOEXCEPT
{
    d_tx = NTSCFG_MOVE_FROM(other, d_tx);
    d_rx = NTSCFG_MOVE_FROM(other, d_rx);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
UdpTimePointInterval& UdpTimePointInterval::operator=(
    const UdpTimePointInterval& other)
{
    d_tx = other.d_tx;
    d_rx = other.d_rx;

    return *this;
}

NTSCFG_INLINE
void UdpTimePointInterval::reset()
{
    d_tx = 0;
    d_rx = 0;
}

NTSCFG_INLINE
void UdpTimePointInterval::setTx(UdpTimePoint value)
{
    d_tx = value;
}

NTSCFG_INLINE
void UdpTimePointInterval::setRx(UdpTimePoint value)
{
    d_rx = value;
}

NTSCFG_INLINE
UdpTimePoint UdpTimePointInterval::tx() const
{
    return d_tx;
}

NTSCFG_INLINE
UdpTimePoint UdpTimePointInterval::rx() const
{
    return d_rx;
}

NTSCFG_INLINE
bool UdpTimePointInterval::equals(const UdpTimePointInterval& other) const
{
    return d_tx == other.d_tx && d_rx == other.d_rx;
}

NTSCFG_INLINE
bool UdpTimePointInterval::less(const UdpTimePointInterval& other) const
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
NTSCFG_INLINE void UdpTimePointInterval::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_tx);
    hashAppend(algorithm, d_rx);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const UdpTimePointInterval& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const UdpTimePointInterval& lhs,
                const UdpTimePointInterval& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const UdpTimePointInterval& lhs,
                const UdpTimePointInterval& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const UdpTimePointInterval& lhs,
               const UdpTimePointInterval& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&             algorithm,
                              const UdpTimePointInterval& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
