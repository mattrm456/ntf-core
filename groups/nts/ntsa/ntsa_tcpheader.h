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

#ifndef INCLUDED_NTSA_TCPHEADER
#define INCLUDED_NTSA_TCPHEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
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

/// Provide a Transmission Control Protocol (TCP) header.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class TcpHeader
{
    /// The source port.
    bdlb::BigEndianUint16 d_sourcePort;

    /// The destination port.
    bdlb::BigEndianUint16 d_destinationPort;

    /// The sequence number. If the SYN flag is set, then this is the initial
    /// sequence number. The sequence number of the actual first data byte and
    /// the acknowledged number in the corresponding ACK are then this sequence
    /// number plus 1. Otherwise, if the SYN flag is clear (0), then this is
    /// the accumulated sequence number of the first data byte of this segment
    /// for the current session.
    bdlb::BigEndianUint32 d_sequenceNumber;

    /// The acknowledgment number. If the ACK flag is set then the value of
    /// this field is the next sequence number that the sender of the ACK is
    /// expecting. This acknowledges receipt of all prior bytes (if any). The
    /// first ACK sent by each end acknowledges the other end's initial
    /// sequence number itself, but no data.
    bdlb::BigEndianUint32 d_acknowledgmentNumber;

#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

    /// Reserved.
    bsl::uint8_t d_reserved : 4;

    /// The size of the TCP header in 32-bit words. The minimum size header is
    /// 5 words and the maximum is 15 words thus giving the minimum size of 20
    /// bytes and maximum of 60 bytes, allowing for up to 40 bytes of options
    /// in the header. Note that the length of the payload is not specified in
    /// the segment header; it can be calculated by subtracting the combined
    /// length of the segment header and IP header from the total IP packet
    /// length specified in the IP header.
    bsl::uint8_t d_dataOffset : 4;

#else

    /// The size of the TCP header in 32-bit words. The minimum size header is
    /// 5 words and the maximum is 15 words thus giving the minimum size of 20
    /// bytes and maximum of 60 bytes, allowing for up to 40 bytes of options
    /// in the header. Note that the length of the payload is not specified in
    /// the segment header; it can be calculated by subtracting the combined
    /// length of the segment header and IP header from the total IP packet
    /// length specified in the IP header.
    bsl::uint8_t d_dataOffset : 4;

    /// Reserved.
    bsl::uint8_t d_reserved : 4;

#endif

    /// The flags.
    bsl::uint8_t d_flags;

    /// The size of the receive window, which specifies the number of window
    /// size units (by default, bytes) (beyond the segment identified by the
    /// sequence number in the acknowledgment field) that the sender of this
    /// segment is currently willing to receive.
    bdlb::BigEndianUint16 d_windowSize;

    // The 16-bit checksum field is used for error-checking of the header, the
    // Payload and a Pseudo-Header. The Pseudo-Header consists of the Source
    // IP Address, the Destination IP Address, the protocol number for the
    // TCP-Protocol (0x0006) and the length of the TCP headers including
    // Payload (in Bytes).
    bdlb::BigEndianUint16 d_checksum;

    /// If the URG flag is set, then this 16-bit field is an offset from the
    /// sequence number indicating the last urgent data byte.
    bdlb::BigEndianUint16 d_urgentPointer;

  private:
    /// Initialize the header to its default values.
    void initialize();

    /// Print a human-readable description of the specified 'flags' to the
    /// specified stream'.
    static bsl::ostream& printFlags(bsl::ostream& stream,
                                    bsl::uint8_t  flags,
                                    int           level,
                                    int           spacesPerLevel);

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
        k_MAX_OPTIONS_LENGTH = 40
    };

    /// Enumerates the flags.
    enum Flag {
        k_SYN = 1 << 1,
        k_ACK = 1 << 2,
        k_PSH = 1 << 3,
        k_FIN = 1 << 4,
        k_RST = 1 << 5,
        k_ECE = 1 << 6,
        k_CWR = 1 << 7,
        k_URG = 1 << 8
    };

    /// Create a new TCP header having a default value.
    TcpHeader();

    /// Create a new TCP header having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    TcpHeader(bslmf::MovableRef<TcpHeader> original) NTSCFG_NOEXCEPT;

    /// Create a new TCP header having the same value as the specified
    /// 'original' object.
    TcpHeader(const TcpHeader& original);

    /// Destroy this object.
    ~TcpHeader();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    TcpHeader& operator=(bslmf::MovableRef<TcpHeader> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    TcpHeader& operator=(const TcpHeader& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the source port to the specified 'value'.
    void setSourcePort(ntsa::Port value);

    /// Set the destination port to the specified 'value'.
    void setDestinationPort(ntsa::Port value);

    /// Set the sequence number to the specified 'value'.
    void setSequenceNumber(bsl::uint32_t value);

    /// Set the acknowledgement number to the specified 'value'.
    void setAcknowledgmentNumber(bsl::uint32_t value);

    /// Set the length of the header including all options, in bytes, to the
    /// specified 'value'. The behavior is undefined if 'value' is less than
    /// k_MIN_HEADER_LENGTH. The behavior is undefined if 'value' is greater
    /// than k_MAX_HEADER_LENGTH. The behavior is undefined if 'value' is not a
    /// multiple of 4.
    void setDataOffset(bsl::size_t value);

    /// Set the flags to the specified 'value'.
    void setFlags(bsl::uint8_t value);

    /// Set the flag having the specified 'value'.
    void setFlag(Flag value);

    /// Set the window size to the specified 'value'.
    void setWindowSize(bsl::uint16_t value);

    /// Set the checksum to the specified 'value'.
    void setChecksum(bsl::uint16_t value);

    /// Set the urgent pointer to the specified 'value'.
    void setUrgentPointer(bsl::uint16_t value);

    /// Decode the header from the specified 'buffer' starting at the specified
    /// 'offset' inside the framing packet having the specified 'packetSize'.
    /// Return the error.
    ntsa::Error decode(const bdlbb::BlobBuffer& buffer,
                       bsl::size_t              offset,
                       bsl::size_t              packetSize);

    /// Encode the header to the specified 'buffer' starting at the specified
    /// 'offset'. Return the
    /// error.
    ntsa::Error encode(bdlbb::BlobBuffer* buffer, bsl::size_t offset) const;

    /// Return the source port.
    ntsa::Port sourcePort() const;

    /// Return the destination port.
    ntsa::Port destinationPort() const;

    /// Return the sequence number.
    bsl::uint32_t sequenceNumber() const;

    /// Return the acknowledgment number.
    bsl::uint32_t acknowledgmentNumber() const;

    /// Return the length of the header including all options, in bytes.
    bsl::size_t dataOffset() const;

    /// Return the flags.
    bsl::uint8_t flags() const;

    /// Return true if the flag having the specified 'value' is set, otherwise
    /// return false.
    bool hasFlag(Flag value) const;

    /// Return the window size.
    bsl::uint16_t windowSize() const;

    /// Return the checksum.
    bsl::uint16_t checksum() const;

    /// Return the urgent pointer.
    bsl::uint16_t urgentPointer() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const TcpHeader& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const TcpHeader& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(TcpHeader);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(TcpHeader);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(TcpHeader);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::TcpHeader
bsl::ostream& operator<<(bsl::ostream& stream, const TcpHeader& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::TcpHeader
bool operator==(const TcpHeader& lhs, const TcpHeader& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::TcpHeader
bool operator!=(const TcpHeader& lhs, const TcpHeader& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::TcpHeader
bool operator<(const TcpHeader& lhs, const TcpHeader& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::TcpHeader
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TcpHeader& value);

NTSCFG_INLINE
void TcpHeader::initialize()
{
    setDataOffset(static_cast<bsl::size_t>(k_MIN_HEADER_LENGTH));
}

NTSCFG_INLINE
TcpHeader::TcpHeader()
{
    BSLMF_ASSERT(sizeof(*this) == k_MIN_HEADER_LENGTH);

    NTSCFG_WARNING_UNUSED(d_reserved);

    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
    initialize();
}

NTSCFG_INLINE
TcpHeader::TcpHeader(bslmf::MovableRef<TcpHeader> original) NTSCFG_NOEXCEPT
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(BSLS_UTIL_ADDRESSOF(
                    bslmf::MovableRefUtil::access(original))),
                sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
TcpHeader::TcpHeader(const TcpHeader& original)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&original),
                sizeof *this);
}

NTSCFG_INLINE
TcpHeader::~TcpHeader()
{
}

NTSCFG_INLINE
TcpHeader& TcpHeader::operator=(bslmf::MovableRef<TcpHeader> other)
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
TcpHeader& TcpHeader::operator=(const TcpHeader& other)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&other),
                sizeof *this);

    return *this;
}

NTSCFG_INLINE
void TcpHeader::reset()
{
    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
    initialize();
}

NTSCFG_INLINE
void TcpHeader::setSourcePort(ntsa::Port value)
{
    d_sourcePort = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
void TcpHeader::setDestinationPort(ntsa::Port value)
{
    d_destinationPort = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
void TcpHeader::setSequenceNumber(bsl::uint32_t value)
{
    d_sequenceNumber = static_cast<bsl::uint32_t>(value);
}

NTSCFG_INLINE
void TcpHeader::setAcknowledgmentNumber(bsl::uint32_t value)
{
    d_acknowledgmentNumber = static_cast<bsl::uint32_t>(value);
}

NTSCFG_INLINE
void TcpHeader::setDataOffset(bsl::size_t value)
{
    BSLS_ASSERT(value >= static_cast<bsl::size_t>(k_MIN_HEADER_LENGTH));
    BSLS_ASSERT(value <= static_cast<bsl::size_t>(k_MAX_HEADER_LENGTH));
    BSLS_ASSERT(value % sizeof(bsl::uint32_t) == 0);

    d_dataOffset = static_cast<bsl::uint8_t>(value / sizeof(bsl::uint32_t));
}

NTSCFG_INLINE
void TcpHeader::setFlags(bsl::uint8_t value)
{
    d_flags = value;
}

NTSCFG_INLINE
void TcpHeader::setFlag(Flag value)
{
    d_flags |= value;
}

NTSCFG_INLINE
void TcpHeader::setWindowSize(bsl::uint16_t value)
{
    d_windowSize = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
void TcpHeader::setChecksum(bsl::uint16_t value)
{
    d_checksum = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
void TcpHeader::setUrgentPointer(bsl::uint16_t value)
{
    d_urgentPointer = static_cast<bsl::uint16_t>(value);
}

NTSCFG_INLINE
ntsa::Port TcpHeader::sourcePort() const
{
    return static_cast<ntsa::Port>(static_cast<bsl::uint16_t>(d_sourcePort));
}

NTSCFG_INLINE
ntsa::Port TcpHeader::destinationPort() const
{
    return static_cast<ntsa::Port>(
        static_cast<bsl::uint16_t>(d_destinationPort));
}

NTSCFG_INLINE
bsl::uint32_t TcpHeader::sequenceNumber() const
{
    return static_cast<bsl::uint32_t>(d_sequenceNumber);
}

NTSCFG_INLINE
bsl::uint32_t TcpHeader::acknowledgmentNumber() const
{
    return static_cast<bsl::uint32_t>(d_acknowledgmentNumber);
}

NTSCFG_INLINE
bsl::size_t TcpHeader::dataOffset() const
{
    return static_cast<bsl::size_t>(d_dataOffset) * sizeof(bsl::uint32_t);
}

NTSCFG_INLINE
bsl::uint8_t TcpHeader::flags() const
{
    return d_flags;
}

NTSCFG_INLINE
bool TcpHeader::hasFlag(Flag value) const
{
    return (d_flags & value) != 0;
}

NTSCFG_INLINE
bsl::uint16_t TcpHeader::windowSize() const
{
    return static_cast<bsl::uint32_t>(d_windowSize);
}

NTSCFG_INLINE
bsl::uint16_t TcpHeader::checksum() const
{
    return static_cast<bsl::uint32_t>(d_checksum);
}

NTSCFG_INLINE
bsl::uint16_t TcpHeader::urgentPointer() const
{
    return static_cast<bsl::uint32_t>(d_urgentPointer);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void TcpHeader::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(this), sizeof *this);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const TcpHeader& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const TcpHeader& lhs, const TcpHeader& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const TcpHeader& lhs, const TcpHeader& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const TcpHeader& lhs, const TcpHeader& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&  algorithm,
                              const TcpHeader& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
