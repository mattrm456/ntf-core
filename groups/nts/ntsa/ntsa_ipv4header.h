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

#ifndef INCLUDED_NTSA_IPV4HEADER
#define INCLUDED_NTSA_IPV4HEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv4address.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Protocol version 4 (IPv4) header.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class Ipv4Header
{
#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

    /// The Internet header length, in 32-bit words. The minimum value is 5,
    /// indicating a length of 5 * 32 bits = 160 bits = 20 bytes. The maximum
    /// value is 15, indicating a length of 15 * 32 bits = 480 bits = 60 bytes.
    /// Therefore, the maximum size of the options is 60 - 20 = 40 bytes.
    bsl::uint8_t d_ihl : 4;

    /// The version. The Internet Protocol version 4 always sets this to 4.
    bsl::uint8_t d_version : 4;

#else

    /// The version. The Internet Protocol version 4 always sets this to 4.
    bsl::uint8_t d_version : 4;

    /// The Internet header length, in 32-bit words. The minimum value is 5,
    /// indicating a length of 5 * 32 bits = 160 bits = 20 bytes. The maximum
    /// value is 15, indicating a length of 15 * 32 bits = 480 bits = 60 bytes.
    /// Therefore, the maximum size of the options is 60 - 20 = 40 bytes.
    bsl::uint8_t d_ihl : 4;

#endif

#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

    /// The Explicit Congestion Notification (ECN).
    bsl::uint8_t d_ecn : 2;

    /// The Differentiated Services Code Point (DSCP).
    bsl::uint8_t d_dscp : 6;

#else

    /// The Differentiated Services Code Point (DSCP).
    bsl::uint8_t d_dscp : 6;

    /// The Explicit Congestion Notification (ECN).
    bsl::uint8_t d_ecn : 2;

#endif

    /// The total length of the IP datagram, including the header and data.
    bdlb::BigEndianUint16 d_totalLength;

    /// The unique identifier of the group of fragments of a single IP
    /// datagram.
    bdlb::BigEndianUint16 d_id;

#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

    /// The offset of a particular fragment relative to the beginning of the
    /// original unfragmented IP datagram.
    uint16_t d_fragmentOffset : 13;

    /// Flags used to control or identify fragments. From most significat bit
    /// to least significant bit: bit 0 is reserved, must be zero; bit 1
    /// indicates the IP datagram must not be fragmented; bit 2 indicates more
    /// fragments are forthcoming.
    uint16_t d_fragmentFlags : 3;

#else

    /// Flags used to control or identify fragments. From most significat bit
    /// to least significant bit: bit 0 is reserved, must be zero; bit 1
    /// indicates the IP datagram must not be fragmented; bit 2 indicates more
    /// fragments are forthcoming.
    uint16_t d_fragmentFlags : 3;

    /// The offset of a particular fragment relative to the beginning of the
    /// original unfragmented IP datagram.
    uint16_t d_fragmentOffset : 13;

#endif

    /// The time-to-live of the IP datagram.
    bsl::uint8_t d_timeToLive;

    /// The protocol used in the data portion of the IP datagram.
    bsl::uint8_t d_protocol;

    /// The header checksum.
    bdlb::BigEndianUint16 d_checksum;

    /// The source address.
    ntsa::Ipv4Address d_source;

    /// The destination IPv4 address.
    ntsa::Ipv4Address d_destination;

  public:
    /// Create a new IPv4 header having a default value.
    Ipv4Header();

    /// Create a new IPv4 header having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    Ipv4Header(bslmf::MovableRef<Ipv4Header> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv4 header having the same value as the specified
    /// 'original' object.
    Ipv4Header(const Ipv4Header& original);

    /// Destroy this object.
    ~Ipv4Header();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Ipv4Header& operator=(bslmf::MovableRef<Ipv4Header> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv4Header& operator=(const Ipv4Header& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Ipv4Header& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Ipv4Header& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(Ipv4Header);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Ipv4Header);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Ipv4Header);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::Ipv4Header
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Header& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Header
bool operator==(const Ipv4Header& lhs, const Ipv4Header& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Header
bool operator!=(const Ipv4Header& lhs, const Ipv4Header& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv4Header
bool operator<(const Ipv4Header& lhs, const Ipv4Header& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv4Header
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4Header& value);

NTSCFG_INLINE
Ipv4Header::Ipv4Header()
{
    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
}

NTSCFG_INLINE
Ipv4Header::Ipv4Header(bslmf::MovableRef<Ipv4Header> original) NTSCFG_NOEXCEPT
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(BSLS_UTIL_ADDRESSOF(
                    bslmf::MovableRefUtil::access(original))),
                sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
Ipv4Header::Ipv4Header(const Ipv4Header& original)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&original),
                sizeof *this);
}

NTSCFG_INLINE
Ipv4Header::~Ipv4Header()
{
}

NTSCFG_INLINE
Ipv4Header& Ipv4Header::operator=(bslmf::MovableRef<Ipv4Header> other)
    NTSCFG_NOEXCEPT
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(
                    BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(other))),
                sizeof *this);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Ipv4Header& Ipv4Header::operator=(const Ipv4Header& other)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&other),
                sizeof *this);
    return *this;
}

NTSCFG_INLINE
void Ipv4Header::reset()
{
    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Ipv4Header::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    // hashAppend(algorithm, d_source);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Header& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Ipv4Header& lhs, const Ipv4Header& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Ipv4Header& lhs, const Ipv4Header& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Ipv4Header& lhs, const Ipv4Header& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&   algorithm,
                              const Ipv4Header& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
