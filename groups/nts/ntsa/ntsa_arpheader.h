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

#ifndef INCLUDED_NTSA_ARPHEADER
#define INCLUDED_NTSA_ARPHEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
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

/// Provide an Address Resolution Protocol (ARP) header.
///
/// @details
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class ArpHeader
{
    /// The hardware type.
    bdlb::BigEndianUint16 d_hardwareType;

    /// The protocol type.
    bdlb::BigEndianUint16 d_protocolType;

    /// The hardware address length, in bytes.
    bsl::uint8_t d_hardwareAddressLength;

    /// The protocol address length, in bytes.
    bsl::uint8_t d_protocolAddressLength;

    /// The operation type.
    bdlb::BigEndianUint16 d_operation;

  private:
    /// Print the specified address 'type' to the specified 'stream'.
    static bsl::ostream& printAddressType(bsl::ostream& stream,
                                          bsl::uint16_t type,
                                          int           level,
                                          int           spacesPerLevel);

  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed ARP header length.
        k_LENGTH = 8,

        /// The hardware type indicating Ethernet.
        k_HARDWARE_TYPE_ETHERNET = 1,

        /// The protocol type indicating IPv4.
        k_PROTOCOL_TYPE_IPV4 = 2048
    };

    /// Create a new ARP header having a default value.
    ArpHeader();

    /// Create a new ARP header having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    ArpHeader(bslmf::MovableRef<ArpHeader> original) NTSCFG_NOEXCEPT;

    /// Create a new ARP header having the same value as the specified
    /// 'original' object.
    ArpHeader(const ArpHeader& original);

    /// Destroy this object.
    ~ArpHeader();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    ArpHeader& operator=(bslmf::MovableRef<ArpHeader> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ArpHeader& operator=(const ArpHeader& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the hardware type to the specified 'value'.
    void setHardwareType(bsl::uint16_t value);

    /// Set the hardware address length to the specified 'value'.
    void setHardwareAddressLength(bsl::uint8_t value);

    /// Set the protocol type to the specified 'value'.
    void setProtocolType(bsl::uint16_t value);

    /// Set the protocol address length to the specified 'value'.
    void setProtocolAddressLength(bsl::uint8_t value);

    /// Set the operation to the specified 'value'.
    void setOperation(bsl::uint16_t value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the hardware type.
    bsl::uint16_t hardwareType() const;

    /// Return the hardware address length.
    bsl::uint8_t hardwareAddressLength() const;

    /// Return the protocol type.
    bsl::uint16_t protocolType() const;

    /// Return the protocol address length.
    bsl::uint8_t protocolAddressLength() const;

    /// Return the operation.
    bsl::uint16_t operation() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const ArpHeader& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const ArpHeader& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(ArpHeader);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ArpHeader);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ArpHeader);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::ArpHeader
bsl::ostream& operator<<(bsl::ostream& stream, const ArpHeader& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::ArpHeader
bool operator==(const ArpHeader& lhs, const ArpHeader& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::ArpHeader
bool operator!=(const ArpHeader& lhs, const ArpHeader& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::ArpHeader
bool operator<(const ArpHeader& lhs, const ArpHeader& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::ArpHeader
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ArpHeader& value);

NTSCFG_INLINE
ArpHeader::ArpHeader()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);

    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
ArpHeader::ArpHeader(bslmf::MovableRef<ArpHeader> original) NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(original)),
        sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
ArpHeader::ArpHeader(const ArpHeader& original)
{
    NTSCFG_MEMORY_COPY(this, &original, sizeof *this);
}

NTSCFG_INLINE
ArpHeader::~ArpHeader()
{
}

NTSCFG_INLINE
ArpHeader& ArpHeader::operator=(bslmf::MovableRef<ArpHeader> other)
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
ArpHeader& ArpHeader::operator=(const ArpHeader& other)
{
    NTSCFG_MEMORY_COPY(this, &other, sizeof *this);

    return *this;
}

NTSCFG_INLINE
void ArpHeader::reset()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
void ArpHeader::setHardwareType(bsl::uint16_t value)
{
    d_hardwareType = value;
}

NTSCFG_INLINE
void ArpHeader::setHardwareAddressLength(bsl::uint8_t value)
{
    d_hardwareAddressLength = value;
}

NTSCFG_INLINE
void ArpHeader::setProtocolType(bsl::uint16_t value)
{
    d_protocolType = value;
}

NTSCFG_INLINE
void ArpHeader::setProtocolAddressLength(bsl::uint8_t value)
{
    d_protocolAddressLength = value;
}

NTSCFG_INLINE
void ArpHeader::setOperation(bsl::uint16_t value)
{
    d_operation = value;
}

NTSCFG_INLINE
bsl::uint16_t ArpHeader::hardwareType() const
{
    return static_cast<bsl::uint16_t>(d_hardwareType);
}

NTSCFG_INLINE
bsl::uint8_t ArpHeader::hardwareAddressLength() const
{
    return d_hardwareAddressLength;
}

NTSCFG_INLINE
bsl::uint16_t ArpHeader::protocolType() const
{
    return static_cast<bsl::uint16_t>(d_protocolType);
}

NTSCFG_INLINE
bsl::uint8_t ArpHeader::protocolAddressLength() const
{
    return d_protocolAddressLength;
}

NTSCFG_INLINE
bsl::uint16_t ArpHeader::operation() const
{
    return static_cast<bsl::uint16_t>(d_operation);
}

NTSCFG_INLINE
bool ArpHeader::equals(const ArpHeader& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool ArpHeader::less(const ArpHeader& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void ArpHeader::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(this), sizeof *this);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ArpHeader& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const ArpHeader& lhs, const ArpHeader& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const ArpHeader& lhs, const ArpHeader& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const ArpHeader& lhs, const ArpHeader& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&  algorithm,
                              const ArpHeader& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
