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

#ifndef INCLUDED_NTSA_IPV4TIMESTAMP
#define INCLUDED_NTSA_IPV4TIMESTAMP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv4address.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntsa {

/// Describe the IPv4 address and timestamp of a router than processes an
/// IPv4 packet.
///
/// @ingroup module_ntsa_protocol
class Ipv4Timestamp
{
    bdlb::NullableValue<ntsa::Ipv4Address> d_address;
    bdlb::NullableValue<bsl::uint32_t>     d_timestamp;

  public:
    /// Create a new IPv4 timestamp having a default value.
    Ipv4Timestamp();

    /// Create a new IPv4 timestamp having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    Ipv4Timestamp(bslmf::MovableRef<Ipv4Timestamp> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv4 timestamp having the same value as the specified
    /// 'original' object.
    Ipv4Timestamp(const Ipv4Timestamp& original);

    /// Destroy this object.
    ~Ipv4Timestamp();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Ipv4Timestamp& operator=(bslmf::MovableRef<Ipv4Timestamp> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv4Timestamp& operator=(const Ipv4Timestamp& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the address to the specified 'value'.
    void setAddress(const ntsa::Ipv4Address& value);

    /// Set the timestamp to the specified 'value'.
    void setTimestamp(bsl::uint32_t value);

    /// Return the address.
    const bdlb::NullableValue<ntsa::Ipv4Address>& address() const;

    /// Return the timestamp.
    const bdlb::NullableValue<bsl::uint32_t>& timestamp() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Ipv4Timestamp& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Ipv4Timestamp& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(Ipv4Timestamp);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Ipv4Timestamp);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Ipv4Timestamp);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::Ipv4Timestamp
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Timestamp& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Timestamp
bool operator==(const Ipv4Timestamp& lhs, const Ipv4Timestamp& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Timestamp
bool operator!=(const Ipv4Timestamp& lhs, const Ipv4Timestamp& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv4Timestamp
bool operator<(const Ipv4Timestamp& lhs, const Ipv4Timestamp& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv4Timestamp
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4Timestamp& value);

NTSCFG_INLINE
Ipv4Timestamp::Ipv4Timestamp()
: d_address()
, d_timestamp()
{
}

NTSCFG_INLINE
Ipv4Timestamp::Ipv4Timestamp(bslmf::MovableRef<Ipv4Timestamp> original)
    NTSCFG_NOEXCEPT : d_address(NTSCFG_MOVE_FROM(original, d_address)),
                      d_timestamp(NTSCFG_MOVE_FROM(original, d_timestamp))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
Ipv4Timestamp::Ipv4Timestamp(const Ipv4Timestamp& original)
: d_address(original.d_address)
, d_timestamp(original.d_timestamp)
{
}

NTSCFG_INLINE
Ipv4Timestamp::~Ipv4Timestamp()
{
}

NTSCFG_INLINE
Ipv4Timestamp& Ipv4Timestamp::operator=(bslmf::MovableRef<Ipv4Timestamp> other)
    NTSCFG_NOEXCEPT
{
    d_address   = NTSCFG_MOVE_FROM(other, d_address);
    d_timestamp = NTSCFG_MOVE_FROM(other, d_timestamp);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Ipv4Timestamp& Ipv4Timestamp::operator=(const Ipv4Timestamp& other)
{
    d_address   = other.d_address;
    d_timestamp = other.d_timestamp;

    return *this;
}

NTSCFG_INLINE
void Ipv4Timestamp::reset()
{
    d_address   = 0;
    d_timestamp = 0;
}

NTSCFG_INLINE
void Ipv4Timestamp::setAddress(const ntsa::Ipv4Address& value)
{
    d_address = value;
}

NTSCFG_INLINE
void Ipv4Timestamp::setTimestamp(bsl::uint32_t value)
{
    d_timestamp = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv4Address>& Ipv4Timestamp::address() const
{
    return d_address;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::uint32_t>& Ipv4Timestamp::timestamp() const
{
    return d_timestamp;
}

NTSCFG_INLINE
bool Ipv4Timestamp::equals(const Ipv4Timestamp& other) const
{
    return d_address == other.d_address && d_timestamp == other.d_timestamp;
}

NTSCFG_INLINE
bool Ipv4Timestamp::less(const Ipv4Timestamp& other) const
{
    if (d_address < other.d_address) {
        return true;
    }

    if (other.d_address < d_address) {
        return false;
    }

    return d_timestamp < other.d_timestamp;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Ipv4Timestamp::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_address);
    hashAppend(algorithm, d_timestamp);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Timestamp& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Ipv4Timestamp& lhs, const Ipv4Timestamp& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Ipv4Timestamp& lhs, const Ipv4Timestamp& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Ipv4Timestamp& lhs, const Ipv4Timestamp& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&      algorithm,
                              const Ipv4Timestamp& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
