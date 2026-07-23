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

#ifndef INCLUDED_NTSA_IGMPHEADER
#define INCLUDED_NTSA_IGMPHEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Group Management Protocol (IGMP) header.
///
/// @details
/// The Internet Group Management Protocol (IGMP) is used by IPv4 systems to
/// report their multicast group memberships to neighboring multicast routers,
/// as described in RFC 2236 (IGMPv2) and RFC 9776 (IGMPv3). IGMP messages are
/// carried directly inside IP datagrams using IP protocol number 2.
///
/// The fixed portion of the IGMP header common to all IGMP message types is
/// 4 octets (32 bits). The remaining content of the IGMP message is specific
/// to the message type and is not represented by this class.
///
/// The binary layout of the header in network byte order is:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |      Type     | Max Resp Code |           Checksum            |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// Type (8 bits): Identifies the IGMP message type. The defined types are:
///
///   0x11 - Membership Query (IGMPv2 and IGMPv3)
///   0x12 - IGMPv1 Membership Report
///   0x16 - IGMPv2 Membership Report
///   0x17 - IGMPv2 Leave Group
///   0x22 - IGMPv3 Membership Report
///
/// Max Resp Code (8 bits): In Membership Query messages, specifies the maximum
/// time allowed before sending a responding report in units of 1/10 second. In
/// all other messages, this field is set to zero by the sender and ignored by
/// receivers.
///
/// Checksum (16 bits): The 16-bit one's complement of the one's complement sum
/// of the entire IGMP message (the full IP payload). The checksum field is
/// treated as zero when computing the sum.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IgmpHeader
{
    /// The type.
    bsl::uint8_t d_type;

    /// The max response code.
    bsl::uint8_t d_maxResponseCode;

    /// The checksum.
    bdlb::BigEndianUint16 d_checksum;

  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed IGMP header length.
        k_LENGTH = 4,

        /// The protocol number indicating the IPv4 packet carries IGMP.
        k_PROTOCOL_IGMP = 2
    };

    /// Create a new IGMP header having a default value.
    IgmpHeader();

    /// Create a new IGMP header having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IgmpHeader(bslmf::MovableRef<IgmpHeader> original) NTSCFG_NOEXCEPT;

    /// Create a new IGMP header having the same value as the specified
    /// 'original' object.
    IgmpHeader(const IgmpHeader& original);

    /// Destroy this object.
    ~IgmpHeader();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IgmpHeader& operator=(bslmf::MovableRef<IgmpHeader> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IgmpHeader& operator=(const IgmpHeader& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of the message to the specified 'value'.
    void setType(bsl::uint8_t value);

    /// Set the max response code to the specified 'value'.
    void setMaxResponseCode(bsl::uint8_t value);

    /// Set the checksum to the specified 'value'.
    void setChecksum(bsl::uint16_t value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the type of the message.
    bsl::uint8_t type() const;

    /// Return the max response code.
    bsl::uint8_t maxResponseCode() const;

    /// Return the checksum.
    bsl::uint16_t checksum() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const IgmpHeader& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const IgmpHeader& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IgmpHeader);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IgmpHeader);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IgmpHeader);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::IgmpHeader
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpHeader& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IgmpHeader
bool operator==(const IgmpHeader& lhs, const IgmpHeader& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IgmpHeader
bool operator!=(const IgmpHeader& lhs, const IgmpHeader& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IgmpHeader
bool operator<(const IgmpHeader& lhs, const IgmpHeader& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IgmpHeader
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IgmpHeader& value);

NTSCFG_INLINE
IgmpHeader::IgmpHeader()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);

    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
IgmpHeader::IgmpHeader(bslmf::MovableRef<IgmpHeader> original) NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(original)),
        sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IgmpHeader::IgmpHeader(const IgmpHeader& original)
{
    NTSCFG_MEMORY_COPY(this, &original, sizeof *this);
}

NTSCFG_INLINE
IgmpHeader::~IgmpHeader()
{
}

NTSCFG_INLINE
IgmpHeader& IgmpHeader::operator=(bslmf::MovableRef<IgmpHeader> other)
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
IgmpHeader& IgmpHeader::operator=(const IgmpHeader& other)
{
    NTSCFG_MEMORY_COPY(this, &other, sizeof *this);

    return *this;
}

NTSCFG_INLINE
void IgmpHeader::reset()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
void IgmpHeader::setType(bsl::uint8_t value)
{
    d_type = value;
}

NTSCFG_INLINE
void IgmpHeader::setMaxResponseCode(bsl::uint8_t value)
{
    d_maxResponseCode = value;
}

NTSCFG_INLINE
void IgmpHeader::setChecksum(bsl::uint16_t value)
{
    d_checksum = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
bsl::uint8_t IgmpHeader::type() const
{
    return d_type;
}

NTSCFG_INLINE
bsl::uint8_t IgmpHeader::maxResponseCode() const
{
    return d_maxResponseCode;
}

NTSCFG_INLINE
bsl::uint16_t IgmpHeader::checksum() const
{
    return static_cast<bsl::uint16_t>(d_checksum);
}

NTSCFG_INLINE
bool IgmpHeader::equals(const IgmpHeader& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool IgmpHeader::less(const IgmpHeader& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IgmpHeader::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(this), sizeof *this);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpHeader& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IgmpHeader& lhs, const IgmpHeader& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IgmpHeader& lhs, const IgmpHeader& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IgmpHeader& lhs, const IgmpHeader& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&   algorithm,
                              const IgmpHeader& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
