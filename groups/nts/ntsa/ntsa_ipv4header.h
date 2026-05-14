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

#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Protocol version 4 (IPv4) header.
///
/// @details
/// The Internet Protocol version 4 (IPv4) header is prepended to every IPv4
/// datagram and carries the information required to route and deliver that
/// datagram from its source to its destination across an interconnected system
/// of networks, as described in RFC 791.
///
/// The fixed portion of the header is 20 octets (160 bits). An optional
/// variable-length field may extend the header to at most 60 octets; the
/// Internet Header Length (IHL) field records the actual size in 32-bit words.
///
/// The binary layout of the header in network byte order is:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |Version|  IHL  |    DSCP   |ECN|          Total Length         |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |         Identification        |Flags|      Fragment Offset    |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |  Time to Live |    Protocol   |         Header Checksum       |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                       Source Address                          |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                    Destination Address                        |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                    Options                    |    Padding    |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// Version (4 bits): The IP format version; always 4 for IPv4.
///
/// IHL — Internet Header Length (4 bits): The length of the header in 32-bit
/// words. The minimum value is 5, giving a 20-octet header with no options.
/// The maximum value is 15, giving a 60-octet header. The payload begins
/// immediately after the last header octet.
///
/// DSCP — Differentiated Services Code Point (6 bits): Originally defined as
/// the upper six bits of the "Type of Service" octet in RFC 791 and
/// subsequently redefined by RFC 2474. Classifies the packet for
/// quality-of-service treatment at each hop. All standard DSCP values have
/// their least-significant bit clear; values whose two least-significant bits
/// are both set are reserved for local or experimental use.
///
/// ECN — Explicit Congestion Notification (2 bits): The two
/// least-significant bits of the Type of Service octet, defined by RFC 3168.
/// Allow routers to signal congestion to endpoints without dropping packets,
/// so that the transport layer can reduce its sending rate before loss occurs.
///
/// Total Length (16 bits): The total size of the datagram in octets,
/// including both the header and the payload. The maximum value is 65,535.
/// Every host must be able to receive datagrams of at least 576 octets.
///
/// Identification (16 bits): A value chosen by the sender to uniquely
/// identify this datagram among those recently sent. All fragments of the
/// same original datagram carry the same identification value, allowing a
/// receiving host to reassemble them.
///
/// Flags (3 bits): Three single-bit flags that control fragmentation:
///   Bit 0 — Reserved; must be zero.
///   Bit 1 — DF (Don't Fragment): instructs every router not to fragment this
///            datagram. If the datagram would need to be fragmented to cross a
///            link, the router discards it and returns an ICMP error.
///   Bit 2 — MF (More Fragments): set on every fragment except the last,
///            indicating that more fragments of the same datagram follow.
///
/// Fragment Offset (13 bits): The byte offset of this fragment's data within
/// the original unfragmented datagram, measured in units of 8 octets. The
/// first (or only) fragment has offset zero.
///
/// Time to Live (8 bits): An upper bound on the number of hops the datagram
/// is permitted to traverse. Each router decrements this field by at least
/// one; if it reaches zero the datagram is discarded and an ICMP Time
/// Exceeded message is sent to the source. This prevents datagrams from
/// circulating indefinitely due to routing loops.
///
/// Protocol (8 bits): Identifies the protocol used in the payload, enabling
/// the destination host to pass the data to the correct upper-layer handler
/// (e.g., 6 for TCP, 17 for UDP, 1 for ICMP).
///
/// Header Checksum (16 bits): The one's complement of the one's complement
/// sum of all 16-bit words in the header, computed with the checksum field
/// treated as zero. This covers only the header, not the payload. Because the
/// TTL field changes at every hop, each router must recompute the checksum
/// after decrementing it.
///
/// Source Address (32 bits): The IPv4 address of the host that originated the
/// datagram.
///
/// Destination Address (32 bits): The IPv4 address of the host to which the
/// datagram is being sent.
///
/// Options (variable, 0–40 octets): An optional sequence of control fields
/// used for features such as security labeling, loose or strict source
/// routing, route recording, and timestamps. The IHL field accounts for any
/// options present.
///
/// Padding (variable): Zero bits appended after the options field to ensure
/// the header ends on a 32-bit boundary.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
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

    /// The total length of the IP packet, including the header and data.
    bdlb::BigEndianUint16 d_totalLength;

    /// The unique identifier of the group of fragments of a single IP
    /// packet.
    bdlb::BigEndianUint16 d_id;

#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

    /// The offset of a particular fragment relative to the beginning of the
    /// original unfragmented IP packet.
    uint16_t d_fragmentOffset : 13;

    /// Flags used to control or identify fragments. From most significat bit
    /// to least significant bit: bit 0 is reserved, must be zero; bit 1
    /// indicates the IP packet must not be fragmented; bit 2 indicates more
    /// fragments are forthcoming.
    uint16_t d_fragmentFlags : 3;

#else

    /// Flags used to control or identify fragments. From most significat bit
    /// to least significant bit: bit 0 is reserved, must be zero; bit 1
    /// indicates the IP packet must not be fragmented; bit 2 indicates more
    /// fragments are forthcoming.
    uint16_t d_fragmentFlags : 3;

    /// The offset of a particular fragment relative to the beginning of the
    /// original unfragmented IP packet.
    uint16_t d_fragmentOffset : 13;

#endif

    /// The time-to-live of the IP packet.
    bsl::uint8_t d_timeToLive;

    /// The protocol used in the data portion of the IP packet.
    bsl::uint8_t d_protocol;

    /// The header checksum.
    bdlb::BigEndianUint16 d_checksum;

    /// The source address.
    ntsa::Ipv4Address d_sourceAddress;

    /// The destination IPv4 address.
    ntsa::Ipv4Address d_destinationAddress;

  private:
    /// Initialize the header to its default values.
    void initialize();

  public:
    /// Enumerate the constants used by the implementation.
    enum Constants {
        /// The minimum header length, in bytes.
        k_MIN_HEADER_LENGTH = 20,

        /// The maximum header length including all options, in bytes.
        k_MAX_HEADER_LENGTH = 60,

        /// The minimum length of all options, in bytes.
        k_MIN_OPTIONS_LENGTH = 0,

        /// The maximum length of all options, in bytes.
        k_MAX_OPTIONS_LENGTH = 40,

        /// The maximum fragment offset.
        k_MAX_FRAGMENT_OFFSET = 8191,

        /// The minimum total packet length, in bytes.
        k_MIN_TOTAL_LENGTH = 20,

        /// The maximum total packet length, in bytes.
        k_MAX_TOTAL_LENGTH = 65535,

        /// The maximum value of the Differentiated Services Control Point
        /// (DSCP).
        k_MAX_DSCP = 63,

        /// The maximum value of the Explicit Congestion Notification (ECN).
        k_MAX_ECN = 3,

        /// The maximum time-to-live.
        k_MAX_TIME_TO_LIVE = 255,

        /// The maximum protocol number.
        k_MAX_PROTOCOL = 255,

        /// The maximum checksum value.
        k_MAX_CHECKSUM = 65535,

        /// The default version.
        k_DEFAULT_VERSION = 4,

        /// The default time-to-live.
        k_DEFAULT_TIME_TO_LIVE = 64,

        /// The protocol number indicating the IPv4 packet carries ICMP.
        k_PROTOCOL_ICMP = 1,

        /// The protocol number indicating the IPv4 packet carries IGMP.
        k_PROTOCOL_IGMP = 2,

        /// The protocol number indicating the IPv4 packet carries TCP.
        k_PROTOCOL_TCP = 6,

        /// The protocol number indicating the IPv4 packet carries UDP.
        k_PROTOCOL_UDP = 17
    };

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

    /// Set the length of the header including all options, in bytes, to the
    /// specified 'value'. The behavior is undefined if 'value' is less than
    /// k_MIN_HEADER_LENGTH. The behavior is undefined if 'value' is greater
    /// than k_MAX_HEADER_LENGTH. The behavior is undefined if 'value' is not a
    /// multiple of 4.
    void setHeaderLength(bsl::size_t value);

    /// Set the total length of the packet, in bytes, to the specified 'value'.
    void setPacketLength(bsl::size_t value);

    /// Set the unique identifier of the group of fragments of a single IP
    /// packet to the specified 'value'.
    void setId(bsl::uint16_t value);

    /// Set the flag indicating the packet must not be fragmented to the
    /// specified 'value'.
    void setPreserve(bool value);

    /// Set the flag indicating the packet has more fragments following to the
    /// specified 'value'.
    void setMore(bool value);

    /// Set the fragment offset to the specified 'value'.
    void setFragmentOffset(bsl::uint16_t value);

    /// Set the Differentiated Services Code Point (DSCP) to the specified
    /// 'value'.
    void setDifferentiatedServicesCodePoint(bsl::uint8_t value);

    /// Set the Explicit Congestion Notification (ECN) to the specified
    /// 'value'.
    void setExplicitCongestionNotification(bsl::uint8_t value);

    /// Set the time-to-live to the specified 'value'.
    void setTimeToLive(bsl::uint8_t value);

    /// Set the protocol to the specified 'value'.
    void setProtocol(bsl::uint8_t value);

    /// Set the checksum to the specified 'value'.
    void setChecksum(bsl::uint16_t value);

    /// Set the source address to the specified 'value'.
    void setSourceAddress(const ntsa::Ipv4Address& value);

    /// Set the destination address to the specified 'value'.
    void setDestinationAddress(const ntsa::Ipv4Address& value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the length of the header including all options, in bytes.
    bsl::size_t headerLength() const;

    /// Return the total length of the packet, in bytes.
    bsl::size_t packetLength() const;

    /// Return the unique identifier of the group of fragments of a single IP
    /// packet.
    bsl::uint16_t id() const;

    /// Return the flag indicating the packet must not be fragmented.
    bool preserve() const;

    /// Return the flag indicating the packet has more fragments.
    bool more() const;

    /// Return the fragment offset.
    bsl::uint16_t fragmentOffset() const;

    /// Return the Differentiated Services Code Point (DSCP).
    bsl::uint8_t differentiatedServicesCodePoint() const;

    /// Return the Explicit Congestion Notification (ECN).
    bsl::uint8_t explicitCongestionNotification() const;

    /// Return the time-to-live of the specified 'header'.
    bsl::uint8_t timeToLive() const;

    /// Return the protocol of the specified 'header'.
    bsl::uint8_t protocol() const;

    /// Return the checksum of the specified 'header'.
    bsl::uint16_t checksum() const;

    /// Return the source address of the specified 'header'.
    const ntsa::Ipv4Address& sourceAddress() const;

    /// Return the destination address of the specified 'header'.
    const ntsa::Ipv4Address& destinationAddress() const;

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
void Ipv4Header::initialize()
{
    d_version = static_cast<bsl::uint8_t>(k_DEFAULT_VERSION);
    setTimeToLive(static_cast<bsl::uint8_t>(k_DEFAULT_TIME_TO_LIVE));
    setHeaderLength(static_cast<bsl::size_t>(k_MIN_HEADER_LENGTH));
    setPacketLength(static_cast<bsl::size_t>(k_MIN_TOTAL_LENGTH));
}

NTSCFG_INLINE
Ipv4Header::Ipv4Header()
{
    BSLMF_ASSERT(sizeof(*this) == k_MIN_HEADER_LENGTH);

    NTSCFG_MEMORY_ZERO(this, sizeof *this);
    initialize();
}

NTSCFG_INLINE
Ipv4Header::Ipv4Header(bslmf::MovableRef<Ipv4Header> original) NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(original)),
        sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
Ipv4Header::Ipv4Header(const Ipv4Header& original)
{
    NTSCFG_MEMORY_COPY(this, &original, sizeof *this);
}

NTSCFG_INLINE
Ipv4Header::~Ipv4Header()
{
}

NTSCFG_INLINE
Ipv4Header& Ipv4Header::operator=(bslmf::MovableRef<Ipv4Header> other)
    NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(other)),
        sizeof *this);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Ipv4Header& Ipv4Header::operator=(const Ipv4Header& other)
{
    NTSCFG_MEMORY_COPY(this, &other, sizeof *this);

    return *this;
}

NTSCFG_INLINE
void Ipv4Header::reset()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
    initialize();
}

NTSCFG_INLINE
void Ipv4Header::setHeaderLength(bsl::size_t value)
{
    BSLS_ASSERT(value >= static_cast<bsl::size_t>(k_MIN_HEADER_LENGTH));
    BSLS_ASSERT(value <= static_cast<bsl::size_t>(k_MAX_HEADER_LENGTH));
    BSLS_ASSERT(value % sizeof(bsl::uint32_t) == 0);

    d_ihl = static_cast<bsl::uint8_t>(value / sizeof(bsl::uint32_t));
}

NTSCFG_INLINE
void Ipv4Header::setPacketLength(bsl::size_t value)
{
    BSLS_ASSERT(value >= static_cast<bsl::size_t>(k_MIN_TOTAL_LENGTH));
    BSLS_ASSERT(value <= static_cast<bsl::size_t>(k_MAX_TOTAL_LENGTH));

    d_totalLength = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
void Ipv4Header::setId(bsl::uint16_t value)
{
    d_id = value;
}

NTSCFG_INLINE
void Ipv4Header::setPreserve(bool value)
{
    if (value) {
        d_fragmentFlags |= (1 << 1);
    }
    else {
        d_fragmentFlags &= ~(1 << 1);
    }
}

NTSCFG_INLINE
void Ipv4Header::setMore(bool value)
{
    if (value) {
        d_fragmentFlags |= (1 << 2);
    }
    else {
        d_fragmentFlags &= ~(1 << 2);
    }
}

NTSCFG_INLINE
void Ipv4Header::setFragmentOffset(bsl::uint16_t value)
{
    BSLS_ASSERT(value <= static_cast<bsl::uint16_t>(k_MAX_FRAGMENT_OFFSET));

    d_fragmentOffset = value;
}

NTSCFG_INLINE
void Ipv4Header::setDifferentiatedServicesCodePoint(bsl::uint8_t value)
{
    BSLS_ASSERT(value <= static_cast<bsl::uint8_t>(k_MAX_DSCP));

    d_dscp = value;
}

NTSCFG_INLINE
void Ipv4Header::setExplicitCongestionNotification(bsl::uint8_t value)
{
    BSLS_ASSERT(value <= static_cast<bsl::uint8_t>(k_MAX_ECN));

    d_ecn = value;
}

NTSCFG_INLINE
void Ipv4Header::setTimeToLive(bsl::uint8_t value)
{
    BSLS_ASSERT(value <= static_cast<bsl::uint8_t>(k_MAX_TIME_TO_LIVE));

    d_timeToLive = value;
}

NTSCFG_INLINE
void Ipv4Header::setProtocol(bsl::uint8_t value)
{
    BSLS_ASSERT(value <= static_cast<bsl::uint8_t>(k_MAX_PROTOCOL));

    d_protocol = value;
}

NTSCFG_INLINE
void Ipv4Header::setChecksum(bsl::uint16_t value)
{
    BSLS_ASSERT(value <= static_cast<bsl::uint16_t>(k_MAX_CHECKSUM));

    d_checksum = value;
}

NTSCFG_INLINE
void Ipv4Header::setSourceAddress(const ntsa::Ipv4Address& value)
{
    d_sourceAddress = value;
}

NTSCFG_INLINE
void Ipv4Header::setDestinationAddress(const ntsa::Ipv4Address& value)
{
    d_destinationAddress = value;
}

NTSCFG_INLINE
bsl::size_t Ipv4Header::headerLength() const
{
    return static_cast<bsl::size_t>(d_ihl) * sizeof(bsl::uint32_t);
}

NTSCFG_INLINE
bsl::size_t Ipv4Header::packetLength() const
{
    return static_cast<bsl::size_t>(static_cast<bsl::uint16_t>(d_totalLength));
}

NTSCFG_INLINE
bsl::uint16_t Ipv4Header::id() const
{
    return static_cast<bsl::uint16_t>(d_id);
}

NTSCFG_INLINE
bool Ipv4Header::preserve() const
{
    return ((d_fragmentFlags & (1 << 1)) != 0);
}

NTSCFG_INLINE
bool Ipv4Header::more() const
{
    return ((d_fragmentFlags & (1 << 2)) != 0);
}

NTSCFG_INLINE
bsl::uint16_t Ipv4Header::fragmentOffset() const
{
    return static_cast<bsl::uint16_t>(d_fragmentOffset);
}

NTSCFG_INLINE
bsl::uint8_t Ipv4Header::differentiatedServicesCodePoint() const
{
    return static_cast<bsl::uint8_t>(d_dscp);
}

NTSCFG_INLINE
bsl::uint8_t Ipv4Header::explicitCongestionNotification() const
{
    return static_cast<bsl::uint8_t>(d_ecn);
}

NTSCFG_INLINE
bsl::uint8_t Ipv4Header::timeToLive() const
{
    return static_cast<bsl::uint8_t>(d_timeToLive);
}

NTSCFG_INLINE
bsl::uint8_t Ipv4Header::protocol() const
{
    return static_cast<bsl::uint8_t>(d_protocol);
}

NTSCFG_INLINE
bsl::uint16_t Ipv4Header::checksum() const
{
    return static_cast<bsl::uint16_t>(d_checksum);
}

NTSCFG_INLINE
const ntsa::Ipv4Address& Ipv4Header::sourceAddress() const
{
    return d_sourceAddress;
}

NTSCFG_INLINE
const ntsa::Ipv4Address& Ipv4Header::destinationAddress() const
{
    return d_destinationAddress;
}

NTSCFG_INLINE
bool Ipv4Header::equals(const Ipv4Header& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool Ipv4Header::less(const Ipv4Header& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Ipv4Header::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(this), sizeof *this);
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
