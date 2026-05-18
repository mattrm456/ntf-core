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

#ifndef INCLUDED_NTSA_ICMPUNREACHABLE
#define INCLUDED_NTSA_ICMPUNREACHABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4header.h>
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

/// Provide the body of an ICMP type 3 destination unreachable message.
///
/// @details
/// The ICMP destination unreachable message is sent by a gateway or host to
/// inform the source that a datagram could not be delivered to its
/// destination, as described in RFC 792.
///
/// A gateway sends this message when its routing tables indicate the
/// destination network is unreachable, or when the Don't Fragment flag is set
/// on a datagram that must be fragmented to be forwarded. A destination host
/// sends this message when the indicated protocol module or process port is
/// not active.
///
/// The full destination unreachable wire format, carried inside an IP
/// datagram, is:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |     Type      |     Code      |          Checksum             |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                             unused                            |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |      Internet Header + 64 bits of Original Data Datagram      |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// The type (3), code, and checksum fields are represented by
/// 'ntsa::IcmpHeader'. This class represents the fixed message-specific body
/// that immediately follows the header: a 32-bit reserved field that must be
/// zero on transmission. The variable-length original datagram data that
/// trails the body is not represented by this class.
///
/// The code field in the accompanying 'ntsa::IcmpHeader' identifies the
/// specific reason the destination was unreachable:
///
///   0 - Net unreachable (gateway).
///   1 - Host unreachable (gateway).
///   2 - Protocol unreachable (destination host).
///   3 - Port unreachable (destination host).
///   4 - Fragmentation needed but the Don't Fragment flag is set (gateway).
///   5 - Source route failed (gateway).
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpUnreachable
{
    /// Reserved field; must be zero.
    bdlb::BigEndianUint32 d_unused;

    /// The IPv4 header of the original packet.
    ntsa::Ipv4Header d_header;

    /// The leading bytes of the payload of the original packet.
    bsl::uint8_t d_payloadData[8];

    /// The number of leading bytes of the payload of the original packet.
    bsl::size_t d_payloadSize;

  private:
    /// Print the specified 'data' to the specified 'stream'.
    static bsl::ostream& printData(bsl::ostream&            stream,
                                   const bslstl::StringRef& data,
                                   int                      level,
                                   int                      spacesPerLevel);

  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed length of the IcmpUnreachable body in octets.
        k_LENGTH = sizeof(bdlb::BigEndianUint32) + sizeof(ntsa::Ipv4Header) + 8 +
                   sizeof(bsl::size_t)
    };

    /// Create a new ICMP unreachable having a default value.
    IcmpUnreachable();

    /// Create a new ICMP unreachable having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IcmpUnreachable(
        bslmf::MovableRef<IcmpUnreachable> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP unreachable having the same value as the specified
    /// 'original' object.
    IcmpUnreachable(const IcmpUnreachable& original);

    /// Destroy this object.
    ~IcmpUnreachable();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpUnreachable& operator=(
        bslmf::MovableRef<IcmpUnreachable> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpUnreachable& operator=(const IcmpUnreachable& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the IPv4 header of the original packet to the specified 'value'.
    void setHeader(const ntsa::Ipv4Header& header);

    /// Set the payload of the original packet to the specified 'payload'
    /// having the specified 'size'. Note that only the first 8 bytes of the
    /// payload are stored, if 'size' is greater than 8.
    void setPayload(const void* payload, bsl::size_t size);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the IPv4 header of the original packet.
    const ntsa::Ipv4Header& header() const;

    /// Return up to the first 8 bytes of the payload of the original packet.
    const bsl::uint8_t* payloadData() const;

    /// Return the payload size. Note that the maximum payload size is limited
    /// to the first 8 bytes of the payload of the original packet.
    bsl::size_t payloadSize() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IcmpUnreachable& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IcmpUnreachable& other) const;

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

    /// Print this object using the specified 'printer'.
    void print(bslim::Printer* printer) const;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IcmpUnreachable);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IcmpUnreachable);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IcmpUnreachable);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IcmpUnreachable
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpUnreachable& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpUnreachable
bool operator==(const IcmpUnreachable& lhs, const IcmpUnreachable& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpUnreachable
bool operator!=(const IcmpUnreachable& lhs, const IcmpUnreachable& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpUnreachable
bool operator<(const IcmpUnreachable& lhs, const IcmpUnreachable& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpUnreachable
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpUnreachable& value);

NTSCFG_INLINE
IcmpUnreachable::IcmpUnreachable()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);

    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
IcmpUnreachable::IcmpUnreachable(
    bslmf::MovableRef<IcmpUnreachable> original) NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(original)),
        sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IcmpUnreachable::IcmpUnreachable(const IcmpUnreachable& original)
{
    NTSCFG_MEMORY_COPY(this, &original, sizeof *this);
}

NTSCFG_INLINE
IcmpUnreachable::~IcmpUnreachable()
{
}

NTSCFG_INLINE
IcmpUnreachable& IcmpUnreachable::operator=(
    bslmf::MovableRef<IcmpUnreachable> other) NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(other)),
        sizeof *this);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IcmpUnreachable& IcmpUnreachable::operator=(const IcmpUnreachable& other)
{
    NTSCFG_MEMORY_COPY(this, &other, sizeof *this);

    return *this;
}

NTSCFG_INLINE
void IcmpUnreachable::reset()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
void IcmpUnreachable::setHeader(const ntsa::Ipv4Header& header)
{
    d_header = header;
}

NTSCFG_INLINE
void IcmpUnreachable::setPayload(const void* payload, bsl::size_t size)
{
    NTSCFG_MEMORY_ZERO(d_payloadData, sizeof d_payloadData);
    if (size > 0) {
        NTSCFG_MEMORY_COPY(d_payloadData,
                           payload,
                           bsl::min(size, sizeof d_payloadData));
    }
}

NTSCFG_INLINE
const ntsa::Ipv4Header& IcmpUnreachable::header() const
{
    return d_header;
}

NTSCFG_INLINE
const bsl::uint8_t* IcmpUnreachable::payloadData() const
{
    return d_payloadData;
}

NTSCFG_INLINE
bsl::size_t IcmpUnreachable::payloadSize() const
{
    return d_payloadSize;
}

NTSCFG_INLINE
bool IcmpUnreachable::equals(const IcmpUnreachable& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool IcmpUnreachable::less(const IcmpUnreachable& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpUnreachable::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, static_cast<bsl::uint32_t>(d_unused));
    hashAppend(algorithm, d_header);
    if (d_payloadSize > 0) {
        algorithm(d_payloadData, d_payloadSize);
    }
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpUnreachable& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpUnreachable& lhs, const IcmpUnreachable& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpUnreachable& lhs, const IcmpUnreachable& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IcmpUnreachable& lhs, const IcmpUnreachable& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&       algorithm,
                              const IcmpUnreachable& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
