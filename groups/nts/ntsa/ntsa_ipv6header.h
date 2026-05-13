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
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
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
/// @ingroup module_ntsa_protocol
class Ipv6Header
{
#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

    /// The high nibble of the traffic class (bits [7:4] of byte 0).
    bsl::uint8_t d_trafficClassHi : 4;

    /// The version. IPv6 always sets this to 6.
    bsl::uint8_t d_version : 4;

#else

    /// The version. IPv6 always sets this to 6.
    bsl::uint8_t d_version : 4;

    /// The high nibble of the traffic class (bits [7:4] of byte 0).
    bsl::uint8_t d_trafficClassHi : 4;

#endif

#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

    /// The high nibble of the flow label (bits [19:16]).
    bsl::uint8_t d_flowLabelHi : 4;

    /// The low nibble of the traffic class (bits [3:0] of byte 1).
    bsl::uint8_t d_trafficClassLo : 4;

#else

    /// The low nibble of the traffic class (bits [3:0] of byte 1).
    bsl::uint8_t d_trafficClassLo : 4;

    /// The high nibble of the flow label (bits [19:16]).
    bsl::uint8_t d_flowLabelHi : 4;

#endif

    /// The low 16 bits of the flow label (bits [15:0]).
    bdlb::BigEndianUint16 d_flowLabelLo;

    /// The payload length.
    bdlb::BigEndianUint16 d_payloadLength;

    /// The next header.
    bsl::uint8_t d_nextHeader;

    /// The hop limit.
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
        k_DEFAULT_VERSION = 6,

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

    /// Set the payload length to the specified 'value'.
    void setPayloadLength(bsl::uint16_t value);

    /// Set the next header type to the specified 'value'.
    void setNextHeader(bsl::uint8_t value);

    /// Set the hop limit to the specified 'value'.
    void setHopLimit(bsl::uint8_t value);

    /// Set the Differentiated Services Code Point (DSCP) to the specified
    /// 'value'. DSCP occupies the six most-significant bits of the traffic
    /// class field and is used to classify packets for quality-of-service
    /// handling. All standard DSCP values have their least-significant bit
    /// clear; values whose two least-significant bits are both set are
    /// reserved for local or experimental use. Note that only the low 6 bits
    /// of 'value' are used; the upper 2 bits are ignored.
    void setDscp(bsl::uint8_t value);

    /// Set the Explicit Congestion Notification (ECN) to the specified
    /// 'value'. ECN occupies the two least-significant bits of the traffic
    /// class field. Note that only the low 2 bits of 'value' are used; the
    /// upper 6 bits are ignored.
    void setEcn(bsl::uint8_t value);

    /// Set the flow label to the specified 'value'. Note that only the low
    /// 20 bits of 'value' are used; the upper 12 bits are ignored.
    void setFlowLabel(bsl::uint32_t value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the source address.
    const ntsa::Ipv6Address& sourceAddress() const;

    /// Return the destination address.
    const ntsa::Ipv6Address& destinationAddress() const;

    /// Return the payload length.
    bsl::uint16_t payloadLength() const;

    /// Return the next header type.
    bsl::uint8_t nextHeader() const;

    /// Return the hop limit.
    bsl::uint8_t hopLimit() const;

    /// Return the Differentiated Services Code Point (DSCP). DSCP occupies
    /// the six most-significant bits of the traffic class field and is used
    /// to classify packets for quality-of-service handling. All standard DSCP
    /// values have their least-significant bit clear; values whose two
    /// least-significant bits are both set are reserved for local or
    /// experimental use. Note that only the low 6 bits of the returned value
    /// are significant; the upper 2 bits are always zero.
    bsl::uint8_t dscp() const;

    /// Return the Explicit Congestion Notification (ECN). ECN occupies the
    /// two least-significant bits of the traffic class field. Note that only
    /// the low 2 bits of the returned value are significant; the upper 6 bits
    /// are always zero.
    bsl::uint8_t ecn() const;

    /// Return the flow label. Note that only the low 20 bits of the returned
    /// value are significant; the upper 12 bits are always zero.
    bsl::uint32_t flowLabel() const;

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
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
    initialize();
}

NTSCFG_INLINE
Ipv6Header::Ipv6Header(bslmf::MovableRef<Ipv6Header> original) NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(original)),
        sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
Ipv6Header::Ipv6Header(const Ipv6Header& original)
{
    NTSCFG_MEMORY_COPY(this, &original, sizeof *this);
}

NTSCFG_INLINE
Ipv6Header::~Ipv6Header()
{
}

NTSCFG_INLINE
Ipv6Header& Ipv6Header::operator=(
    bslmf::MovableRef<Ipv6Header> other) NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(other)),
        sizeof *this);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Ipv6Header& Ipv6Header::operator=(const Ipv6Header& other)
{
    NTSCFG_MEMORY_COPY(this, &other, sizeof *this);

    return *this;
}

NTSCFG_INLINE
void Ipv6Header::reset()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
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

NTSCFG_INLINE
void Ipv6Header::setPayloadLength(bsl::uint16_t value)
{
    d_payloadLength = value;
}

NTSCFG_INLINE
void Ipv6Header::setNextHeader(bsl::uint8_t value)
{
    d_nextHeader = value;
}

NTSCFG_INLINE
void Ipv6Header::setHopLimit(bsl::uint8_t value)
{
    d_hopLimit = value;
}

NTSCFG_INLINE
bsl::uint16_t Ipv6Header::payloadLength() const
{
    return d_payloadLength;
}

NTSCFG_INLINE
bsl::uint8_t Ipv6Header::nextHeader() const
{
    return d_nextHeader;
}

NTSCFG_INLINE
bsl::uint8_t Ipv6Header::hopLimit() const
{
    return d_hopLimit;
}

NTSCFG_INLINE
void Ipv6Header::setDscp(bsl::uint8_t value)
{
    d_trafficClassHi = static_cast<bsl::uint8_t>(value >> 2);
    d_trafficClassLo = static_cast<bsl::uint8_t>(((value & 0x03) << 2) |
                                                  (d_trafficClassLo & 0x03));
}

NTSCFG_INLINE
void Ipv6Header::setEcn(bsl::uint8_t value)
{
    d_trafficClassLo = static_cast<bsl::uint8_t>((d_trafficClassLo & 0x0C) |
                                                  (value & 0x03));
}

NTSCFG_INLINE
bsl::uint8_t Ipv6Header::dscp() const
{
    return static_cast<bsl::uint8_t>((d_trafficClassHi << 2) |
                                     (d_trafficClassLo >> 2));
}

NTSCFG_INLINE
bsl::uint8_t Ipv6Header::ecn() const
{
    return static_cast<bsl::uint8_t>(d_trafficClassLo & 0x03);
}

NTSCFG_INLINE
void Ipv6Header::setFlowLabel(bsl::uint32_t value)
{
    d_flowLabelHi = static_cast<bsl::uint8_t>((value >> 16) & 0x0F);
    d_flowLabelLo = static_cast<bsl::uint16_t>(value & 0xFFFF);
}

NTSCFG_INLINE
bsl::uint32_t Ipv6Header::flowLabel() const
{
    return (static_cast<bsl::uint32_t>(d_flowLabelHi) << 16) |
           static_cast<bsl::uint32_t>(static_cast<bsl::uint16_t>(d_flowLabelLo));
}

NTSCFG_INLINE
bool Ipv6Header::equals(const Ipv6Header& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool Ipv6Header::less(const Ipv6Header& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
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
