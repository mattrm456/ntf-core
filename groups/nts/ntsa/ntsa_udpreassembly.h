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

#ifndef INCLUDED_NTSA_UDPREASSEMBLY
#define INCLUDED_NTSA_UDPREASSEMBLY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_circular.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntsa {

/// Describe UDP reassembly.
///
/// @ingroup module_ntsa_protocol
class UdpReassembly
{
    bsl::uint16_t d_maxSize;
    bsl::uint8_t  d_maxFragments;

  public:
    /// Create new UDP reassembly capabilities having a default value.
    UdpReassembly();

    /// Create new UDP reassembly capabilities having the same value as the
    /// specified 'original' object. Assign an unspecified but valid value to
    /// the 'original' original.
    UdpReassembly(bslmf::MovableRef<UdpReassembly> original) NTSCFG_NOEXCEPT;

    /// Create new UDP reassembly capabilities having the same value as the
    /// specified 'original' object.
    UdpReassembly(const UdpReassembly& original);

    /// Destroy this object.
    ~UdpReassembly();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    UdpReassembly& operator=(bslmf::MovableRef<UdpReassembly> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UdpReassembly& operator=(const UdpReassembly& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the maximum reassembled datagram size to the specified 'value'.
    void setMaxSize(bsl::uint16_t value);

    /// Set the maximum number of fragments to the specified 'value'.
    void setMaxFragments(bsl::uint8_t value);

    /// Return the maximum reassembled datagram size.
    bsl::uint16_t maxSize() const;

    /// Return the maximum number of fragments.
    bsl::uint8_t maxFragments() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const UdpReassembly& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const UdpReassembly& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(UdpReassembly);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(UdpReassembly);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(UdpReassembly);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::UdpReassembly
bsl::ostream& operator<<(bsl::ostream& stream, const UdpReassembly& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::UdpReassembly
bool operator==(const UdpReassembly& lhs, const UdpReassembly& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::UdpReassembly
bool operator!=(const UdpReassembly& lhs, const UdpReassembly& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::UdpReassembly
bool operator<(const UdpReassembly& lhs, const UdpReassembly& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::UdpReassembly
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UdpReassembly& value);

NTSCFG_INLINE
UdpReassembly::UdpReassembly()
: d_maxSize(0)
, d_maxFragments(0)
{
}

NTSCFG_INLINE
UdpReassembly::UdpReassembly(bslmf::MovableRef<UdpReassembly> original)
    NTSCFG_NOEXCEPT
: d_maxSize(NTSCFG_MOVE_FROM(original, d_maxSize)),
  d_maxFragments(NTSCFG_MOVE_FROM(original, d_maxFragments))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
UdpReassembly::UdpReassembly(const UdpReassembly& original)
: d_maxSize(original.d_maxSize)
, d_maxFragments(original.d_maxFragments)
{
}

NTSCFG_INLINE
UdpReassembly::~UdpReassembly()
{
}

NTSCFG_INLINE
UdpReassembly& UdpReassembly::operator=(bslmf::MovableRef<UdpReassembly> other)
    NTSCFG_NOEXCEPT
{
    d_maxSize      = NTSCFG_MOVE_FROM(other, d_maxSize);
    d_maxFragments = NTSCFG_MOVE_FROM(other, d_maxFragments);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
UdpReassembly& UdpReassembly::operator=(const UdpReassembly& other)
{
    d_maxSize      = other.d_maxSize;
    d_maxFragments = other.d_maxFragments;

    return *this;
}

NTSCFG_INLINE
void UdpReassembly::reset()
{
    d_maxSize      = 0;
    d_maxFragments = 0;
}

NTSCFG_INLINE
void UdpReassembly::setMaxSize(bsl::uint16_t value)
{
    d_maxSize = value;
}

NTSCFG_INLINE
void UdpReassembly::setMaxFragments(bsl::uint8_t value)
{
    d_maxFragments = value;
}

NTSCFG_INLINE
bsl::uint16_t UdpReassembly::maxSize() const
{
    return d_maxSize;
}

NTSCFG_INLINE
bsl::uint8_t UdpReassembly::maxFragments() const
{
    return d_maxFragments;
}

NTSCFG_INLINE
bool UdpReassembly::equals(const UdpReassembly& other) const
{
    return d_maxSize == other.d_maxSize &&
           d_maxFragments == other.d_maxFragments;
}

NTSCFG_INLINE
bool UdpReassembly::less(const UdpReassembly& other) const
{
    if (d_maxSize < other.d_maxSize) {
        return true;
    }

    if (other.d_maxSize < d_maxSize) {
        return false;
    }

    return d_maxFragments < other.d_maxFragments;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void UdpReassembly::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_maxSize);
    hashAppend(algorithm, d_maxFragments);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const UdpReassembly& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const UdpReassembly& lhs, const UdpReassembly& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const UdpReassembly& lhs, const UdpReassembly& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const UdpReassembly& lhs, const UdpReassembly& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&      algorithm,
                              const UdpReassembly& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
