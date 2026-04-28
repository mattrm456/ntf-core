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

#ifndef INCLUDED_NTSA_UDPHEADER
#define INCLUDED_NTSA_UDPHEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_port.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a User Datagram Protocol (UDP) checksum.
class UdpChecksum
{
    /// The accumulated value.
    bsl::uint32_t d_accumulator;

  public:
    /// Create a new UDP checksum.
    UdpChecksum();

    /// Create a new UDP checksum having the same value as the specified
    /// 'original' object.
    UdpChecksum(const UdpChecksum& original);

    /// Destroy this object.
    ~UdpChecksum();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    UdpChecksum& operator=(const UdpChecksum& other);

    /// Reset the object to its value upon default construction.
    void reset();

    /// Add the pseudo header for a packet from the specified 'sourceAddress'
    /// to the specified 'destinationAddress' having the specified 'length'
    /// of the UDP packet, including the UDP header.
    void add(const ntsa::Ipv4Address& sourceAddress,
             const ntsa::Ipv4Address& destinationAddress,
             bsl::size_t              length);

    /// Add the specified 'data' having the specified 'size' to the checksum.
    void add(const void* data, bsl::size_t size);

    /// Return the accumulator.
    bsl::uint32_t accumulator() const;

    /// Return the checksum value.
    bsl::uint16_t value() const;
};

/// Provide a User Datagram Protocol (UDP) header.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class UdpHeader
{
    /// The source port.
    bdlb::BigEndianUint16 d_sourcePort;

    /// The destination port.
    bdlb::BigEndianUint16 d_destinationPort;

    /// The length of the datagram, in bytes, including the header.
    bdlb::BigEndianUint16 d_length;

    /// The checksum.
    bdlb::BigEndianUint16 d_checksum;

  private:
    /// Calculate the checksum for the specified 'data' having the specified
    /// 'size', in bytes, using the specified 'initializer' for the acumulator.
    /// The behavior is undefined unless 'data' is 2-byte aligned.
    static bsl::uint16_t calculateChecksum(bsl::uint32_t initializer,
                                           const void*   data,
                                           bsl::size_t   size);

  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed UDP header length.
        k_LENGTH = 8,

        /// The protocol number indicating the IPv4 packet carries UDP.
        k_PROTOCOL_UDP = 17
    };

    /// Create a new UDP header having a default value.
    UdpHeader();

    /// Create a new UDP header having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    UdpHeader(bslmf::MovableRef<UdpHeader> original) NTSCFG_NOEXCEPT;

    /// Create a new UDP header having the same value as the specified
    /// 'original' object.
    UdpHeader(const UdpHeader& original);

    /// Destroy this object.
    ~UdpHeader();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    UdpHeader& operator=(bslmf::MovableRef<UdpHeader> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UdpHeader& operator=(const UdpHeader& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the source port to the specified 'value'.
    void setSourcePort(ntsa::Port value);

    /// Set the destination port to the specified 'value'.
    void setDestinationPort(ntsa::Port value);

    /// Set the length of the datagram, in bytes, including the header, to the
    /// specified 'value'.
    void setPacketLength(bsl::size_t value);

    /// Set the checksum to the specified 'value'.
    void setChecksum(bsl::uint16_t value);

    /// Decode the header from the specified 'source' having the specified
    /// 'size'. Return the error.
    ntsa::Error decode(const void* source, const bsl::size_t size);

    /// Decode the header from the specified 'source'. Return the error.
    ntsa::Error decode(const bdlbb::BlobBuffer& source);

    /// Encode the header to the specified 'buffer' starting at the specified
    /// 'offset'. Return the
    /// error.
    ntsa::Error encode(bdlbb::BlobBuffer* buffer, bsl::size_t offset) const;

    /// Return the source port.
    ntsa::Port sourcePort() const;

    /// Return the destination port.
    ntsa::Port destinationPort() const;

    /// Return the length of the header, in bytes.
    bsl::size_t headerLength() const;

    /// Return the length of the packet, in bytes, including the header.
    bsl::size_t packetLength() const;

    /// Return the checksum.
    bsl::uint16_t checksum() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const UdpHeader& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const UdpHeader& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(UdpHeader);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(UdpHeader);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(UdpHeader);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::UdpHeader
bsl::ostream& operator<<(bsl::ostream& stream, const UdpHeader& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::UdpHeader
bool operator==(const UdpHeader& lhs, const UdpHeader& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::UdpHeader
bool operator!=(const UdpHeader& lhs, const UdpHeader& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::UdpHeader
bool operator<(const UdpHeader& lhs, const UdpHeader& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::UdpHeader
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UdpHeader& value);

NTSCFG_INLINE
UdpHeader::UdpHeader()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);

    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
}

NTSCFG_INLINE
UdpHeader::UdpHeader(bslmf::MovableRef<UdpHeader> original) NTSCFG_NOEXCEPT
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(BSLS_UTIL_ADDRESSOF(
                    bslmf::MovableRefUtil::access(original))),
                sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
UdpHeader::UdpHeader(const UdpHeader& original)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&original),
                sizeof *this);
}

NTSCFG_INLINE
UdpHeader::~UdpHeader()
{
}

NTSCFG_INLINE
UdpHeader& UdpHeader::operator=(bslmf::MovableRef<UdpHeader> other)
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
UdpHeader& UdpHeader::operator=(const UdpHeader& other)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&other),
                sizeof *this);

    return *this;
}

NTSCFG_INLINE
void UdpHeader::reset()
{
    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
}

NTSCFG_INLINE
void UdpHeader::setSourcePort(ntsa::Port value)
{
    d_sourcePort = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
void UdpHeader::setDestinationPort(ntsa::Port value)
{
    d_destinationPort = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
void UdpHeader::setPacketLength(bsl::size_t value)
{
    d_length = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
void UdpHeader::setChecksum(bsl::uint16_t value)
{
    d_checksum = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
ntsa::Port UdpHeader::sourcePort() const
{
    return static_cast<ntsa::Port>(static_cast<bsl::uint16_t>(d_sourcePort));
}

NTSCFG_INLINE
ntsa::Port UdpHeader::destinationPort() const
{
    return static_cast<ntsa::Port>(
        static_cast<bsl::uint16_t>(d_destinationPort));
}

NTSCFG_INLINE
bsl::size_t UdpHeader::headerLength() const
{
    return static_cast<bsl::size_t>(k_LENGTH);
}

NTSCFG_INLINE
bsl::size_t UdpHeader::packetLength() const
{
    return static_cast<bsl::size_t>(static_cast<bsl::uint16_t>(d_length));
}

NTSCFG_INLINE
bsl::uint16_t UdpHeader::checksum() const
{
    return static_cast<bsl::uint16_t>(d_checksum);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void UdpHeader::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(this), sizeof *this);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const UdpHeader& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const UdpHeader& lhs, const UdpHeader& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const UdpHeader& lhs, const UdpHeader& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const UdpHeader& lhs, const UdpHeader& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&  algorithm,
                              const UdpHeader& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
