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

#ifndef INCLUDED_NTSA_IPV6HEADER
#define INCLUDED_NTSA_IPV6HEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv6address.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Protocol version 6 (IPv6) header.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class Ipv6Header
{
#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

    // The priority.
    bsl::uint8_t d_priority : 4;

    // The version. The Internet Protocol version 4 always sets this to 6.
    bsl::uint8_t d_version : 4;

#else

    // The version. The Internet Protocol version 4 always sets this to 6.
    bsl::uint8_t d_version : 4;

    // The priority.
    bsl::uint8_t d_priority : 4;

#endif

    // The flow label.
    bsl::uint8_t d_flowLabel[3];

    // The payload length.
    bdlb::BigEndianUint16 d_payloadLength;

    // The next header.
    bsl::uint8_t d_nextHeader;

    // The hop limit.
    bsl::uint8_t d_hopLimit;

    /// The source address.
    ntsa::Ipv6Address d_sourceAddress;

    /// The destination address.
    ntsa::Ipv6Address d_destinationAddress;

  private:
    /// Initialize the header to its default values.
    void initialize();

  public:
    /// Enumerate the constants used by this implementation.
    enum Constants {
        /// The minimum header length, in bytes.
        k_MIN_HEADER_LENGTH = 40,

        /// The maximum header length including all options, in bytes.
        k_MAX_HEADER_LENGTH = 1500,

        /// The default version.
        k_DEFAULT_VERSION = 4,

        /// The protocol number indicating the IPv4 packet carries TCP.
        k_PROTOCOL_TCP = 6,

        /// The protocol number indicating the IPv4 packet carries UDP.
        k_PROTOCOL_UDP = 17
    };

    /// Create a new IPv6 header having a default value.
    Ipv6Header();

    /// Create a new IPv6 header having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    Ipv6Header(bslmf::MovableRef<Ipv6Header> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv6 header having the same value as the specified
    /// 'original' object.
    Ipv6Header(const Ipv6Header& original);

    /// Destroy this object.
    ~Ipv6Header();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Ipv6Header& operator=(bslmf::MovableRef<Ipv6Header> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv6Header& operator=(const Ipv6Header& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the source address to the specified 'value'.
    void setSourceAddress(const ntsa::Ipv6Address& value);

    /// Set the destination address to the specified 'value'.
    void setDestinationAddress(const ntsa::Ipv6Address& value);

    /// Return the source address.
    const ntsa::Ipv6Address& sourceAddress() const;

    /// Return the destination address.
    const ntsa::Ipv6Address& destinationAddress() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Ipv6Header& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Ipv6Header& other) const;

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
                        int           spacesPerLevel = 6) const;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(Ipv6Header);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Ipv6Header);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Ipv6Header);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::Ipv6Header
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Header& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Header
bool operator==(const Ipv6Header& lhs, const Ipv6Header& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Header
bool operator!=(const Ipv6Header& lhs, const Ipv6Header& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv6Header
bool operator<(const Ipv6Header& lhs, const Ipv6Header& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv6Header
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv6Header& value);

NTSCFG_INLINE
void Ipv6Header::initialize()
{
    d_version = static_cast<bsl::uint8_t>(k_DEFAULT_VERSION);
}

NTSCFG_INLINE
Ipv6Header::Ipv6Header()
{
    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
    initialize();
}

NTSCFG_INLINE
Ipv6Header::Ipv6Header(bslmf::MovableRef<Ipv6Header> original) NTSCFG_NOEXCEPT
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(BSLS_UTIL_ADDRESSOF(
                    bslmf::MovableRefUtil::access(original))),
                sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
Ipv6Header::Ipv6Header(const Ipv6Header& original)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&original),
                sizeof *this);
}

NTSCFG_INLINE
Ipv6Header::~Ipv6Header()
{
}

NTSCFG_INLINE
Ipv6Header& Ipv6Header::operator=(
    bslmf::MovableRef<Ipv6Header> other) NTSCFG_NOEXCEPT
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(
                    BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(other))),
                sizeof *this);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Ipv6Header& Ipv6Header::operator=(const Ipv6Header& other)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&other),
                sizeof *this);
    return *this;
}

NTSCFG_INLINE
void Ipv6Header::reset()
{
    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
    initialize();
}

NTSCFG_INLINE
void Ipv6Header::setSourceAddress(const ntsa::Ipv6Address& value)
{
    d_sourceAddress = value;
}

NTSCFG_INLINE
void Ipv6Header::setDestinationAddress(const ntsa::Ipv6Address& value)
{
    d_destinationAddress = value;
}

NTSCFG_INLINE
const ntsa::Ipv6Address& Ipv6Header::sourceAddress() const
{
    return d_sourceAddress;
}

NTSCFG_INLINE
const ntsa::Ipv6Address& Ipv6Header::destinationAddress() const
{
    return d_destinationAddress;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Ipv6Header::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(this), sizeof *this);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Header& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Ipv6Header& lhs, const Ipv6Header& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Ipv6Header& lhs, const Ipv6Header& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Ipv6Header& lhs, const Ipv6Header& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&       algorithm,
                              const Ipv6Header& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
