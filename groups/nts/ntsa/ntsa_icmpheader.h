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

#ifndef INCLUDED_NTSA_ICMPHEADER
#define INCLUDED_NTSA_ICMPHEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntsa_port.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a Internet Control Message Protocol (ICMP) header.
///
/// @details
/// The Internet Control Message Protocol (ICMP) is an integral part of IP,
/// used by gateways and hosts to report errors and diagnostic information back
/// to the source of a datagram, as described in RFC 792. ICMP messages are
/// carried directly inside IP datagrams using IP protocol number 1.
///
/// The fixed portion of the ICMP header is 4 octets (32 bits). The remaining
/// content of the ICMP message is specific to the message type and code, and
/// is not represented by this class.
///
/// The binary layout of the header in network byte order is:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |     Type      |     Code      |          Checksum             |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// Type (8 bits): Identifies the category of ICMP message. The defined types
/// are:
///
///   0  - Echo Reply
///   3  - Destination Unreachable
///   4  - Source Quench
///   5  - Redirect
///   8  - Echo
///   11 - Time Exceeded
///   12 - Parameter Problem
///   13 - Timestamp
///   14 - Timestamp Reply
///   15 - Information Request
///   16 - Information Reply
///
/// Code (8 bits): A sub-type that qualifies the meaning of the Type field.
/// For example, a Destination Unreachable message (type 3) uses the code to
/// distinguish between net unreachable (0), host unreachable (1), protocol
/// unreachable (2), port unreachable (3), fragmentation needed but DF set (4),
/// and source route failed (5). Types that have no sub-types use code 0.
///
/// Checksum (16 bits): The 16-bit one's complement of the one's complement sum
/// of the entire ICMP message, starting from the Type field. The checksum
/// field is treated as zero when computing the sum.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class IcmpHeader
{
    /// The type.
    bsl::uint8_t d_type;

    /// The code.
    bsl::uint8_t d_code;

    /// The checksum.
    bdlb::BigEndianUint16 d_checksum;

  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed ICMP header length.
        k_LENGTH = 4,

        /// The protocol number indicating the IPv4 packet carries ICMP.
        k_PROTOCOL_ICMP = 1
    };

    /// Create a new ICMP header having a default value.
    IcmpHeader();

    /// Create a new ICMP header having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IcmpHeader(bslmf::MovableRef<IcmpHeader> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP header having the same value as the specified
    /// 'original' object.
    IcmpHeader(const IcmpHeader& original);

    /// Destroy this object.
    ~IcmpHeader();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpHeader& operator=(bslmf::MovableRef<IcmpHeader> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpHeader& operator=(const IcmpHeader& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of the message to the specified 'value'.
    void setType(bsl::uint8_t value);

    /// Set the sub-type of the message to the specified 'value'.
    void setCode(bsl::uint8_t value);

    /// Set the checksum to the specified 'value'.
    void setChecksum(bsl::uint16_t value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the type of the message.
    bsl::uint8_t type() const;

    /// Return the sub-type of the message.
    bsl::uint8_t code() const;

    /// Return the checksum.
    bsl::uint16_t checksum() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const IcmpHeader& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const IcmpHeader& other) const;

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

    /// Print this object using the specified 'printer'.
    void print(bslim::Printer* printer) const;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IcmpHeader);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IcmpHeader);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IcmpHeader);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::IcmpHeader
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpHeader& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpHeader
bool operator==(const IcmpHeader& lhs, const IcmpHeader& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpHeader
bool operator!=(const IcmpHeader& lhs, const IcmpHeader& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpHeader
bool operator<(const IcmpHeader& lhs, const IcmpHeader& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpHeader
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpHeader& value);

NTSCFG_INLINE
IcmpHeader::IcmpHeader()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);

    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
IcmpHeader::IcmpHeader(bslmf::MovableRef<IcmpHeader> original) NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(original)),
        sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IcmpHeader::IcmpHeader(const IcmpHeader& original)
{
    NTSCFG_MEMORY_COPY(this, &original, sizeof *this);
}

NTSCFG_INLINE
IcmpHeader::~IcmpHeader()
{
}

NTSCFG_INLINE
IcmpHeader& IcmpHeader::operator=(bslmf::MovableRef<IcmpHeader> other)
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
IcmpHeader& IcmpHeader::operator=(const IcmpHeader& other)
{
    NTSCFG_MEMORY_COPY(this, &other, sizeof *this);

    return *this;
}

NTSCFG_INLINE
void IcmpHeader::reset()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
void IcmpHeader::setType(bsl::uint8_t value)
{
    d_type = value;
}

NTSCFG_INLINE
void IcmpHeader::setCode(bsl::uint8_t value)
{
    d_code = value;
}

NTSCFG_INLINE
void IcmpHeader::setChecksum(bsl::uint16_t value)
{
    d_checksum = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
bsl::uint8_t IcmpHeader::type() const
{
    return d_type;
}

NTSCFG_INLINE
bsl::uint8_t IcmpHeader::code() const
{
    return d_code;
}

NTSCFG_INLINE
bsl::uint16_t IcmpHeader::checksum() const
{
    return static_cast<bsl::uint16_t>(d_checksum);
}

NTSCFG_INLINE
bool IcmpHeader::equals(const IcmpHeader& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool IcmpHeader::less(const IcmpHeader& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpHeader::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(this), sizeof *this);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpHeader& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpHeader& lhs, const IcmpHeader& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpHeader& lhs, const IcmpHeader& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IcmpHeader& lhs, const IcmpHeader& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&  algorithm,
                              const IcmpHeader& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
