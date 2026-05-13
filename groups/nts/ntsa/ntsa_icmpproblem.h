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

#ifndef INCLUDED_NTSA_ICMPPROBLEM
#define INCLUDED_NTSA_ICMPPROBLEM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4header.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide the body of an ICMP type 12 parameter problem message.
///
/// @details
/// The ICMP parameter problem message is sent by a gateway or host when it
/// detects a problem with the header parameters of a received datagram that
/// prevents it from completing processing, as described in RFC 792. The
/// message is only generated when the error caused the datagram to be
/// discarded.
///
/// The full parameter problem wire format, carried inside an IP datagram, is:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |     Type      |     Code      |          Checksum             |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |    Pointer    |                   unused                      |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |      Internet Header + 64 bits of Original Data Datagram      |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// The type (12), code, and checksum fields are represented by
/// 'ntsa::IcmpHeader'. This class represents the fixed message-specific body
/// that immediately follows the header: a pointer byte and three reserved
/// bytes that must be zero. The variable-length original datagram data that
/// trails the body is not represented by this class.
///
/// The code field in the accompanying 'ntsa::IcmpHeader' identifies the
/// nature of the problem:
///
///   0 - Pointer indicates the error.
///
/// Pointer (8 bits): Identifies the octet offset within the original
/// datagram's IP header where the erroneous field begins. For example, a
/// pointer value of 1 indicates the Type of Service field is problematic,
/// while a value of 20 indicates the type code of the first IP option is
/// problematic.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpProblem
{
  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed length of the IcmpProblem body in octets.
        k_LENGTH = 4 + sizeof(ntsa::Ipv4Header) + 8 + sizeof(bsl::size_t)
    };

  private:
    /// The octet offset of the erroneous field in the original datagram
    /// header.
    bsl::uint8_t d_pointer[4];

    /// The IPv4 header of the packet for which the problem was encountered.
    ntsa::Ipv4Header d_header;

    /// The leading bytes of the payload.
    bsl::uint8_t d_payloadData[8];

    /// The number of leading bytes of the payload.
    bsl::size_t d_payloadSize;

  private:
    /// Print the specified 'data' to the specified 'stream'.
    static bsl::ostream& printData(bsl::ostream&            stream,
                                   const bslstl::StringRef& data,
                                   int                      level,
                                   int                      spacesPerLevel);

  public:
    /// Create a new ICMP problem having a default value.
    IcmpProblem();

    /// Create a new ICMP problem having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IcmpProblem(bslmf::MovableRef<IcmpProblem> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP problem having the same value as the specified
    /// 'original' object.
    IcmpProblem(const IcmpProblem& original);

    /// Destroy this object.
    ~IcmpProblem();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpProblem& operator=(bslmf::MovableRef<IcmpProblem> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpProblem& operator=(const IcmpProblem& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the pointer to the specified 'value'.
    void setPointer(bsl::uint8_t value);

    /// Set the payload to the specified 'payload' having the specified 'size'.
    /// Note that only the first 8 bytes of the payload are stored, if 'size'
    /// is greater than 8.
    void setPayload(const void* payload, bsl::size_t size);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Decode the body from the specified 'buffer' starting at the specified
    /// 'offset' inside the framing packet having the specified 'packetSize'.
    /// Return the error.
    ntsa::Error decode(const bdlbb::BlobBuffer& buffer,
                       bsl::size_t              offset,
                       bsl::size_t              packetSize);

    /// Encode the body to the specified 'buffer' starting at the specified
    /// 'offset'. Return the error.
    ntsa::Error encode(bdlbb::BlobBuffer* buffer, bsl::size_t offset) const;

    /// Return the pointer.
    bsl::uint8_t pointer() const;

    /// Return up to the first 8 bytes of the payload of the packet for which
    /// the problem was encountered.
    const bsl::uint8_t* payloadData() const;

    /// Return the payload size. Note that the maximum payload size is limited
    /// to the first 8 bytes of the payload of the packet for which the problem
    /// was encountered.
    bsl::size_t payloadSize() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IcmpProblem& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IcmpProblem& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IcmpProblem);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IcmpProblem);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IcmpProblem);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IcmpProblem
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpProblem& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpProblem
bool operator==(const IcmpProblem& lhs, const IcmpProblem& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpProblem
bool operator!=(const IcmpProblem& lhs, const IcmpProblem& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpProblem
bool operator<(const IcmpProblem& lhs, const IcmpProblem& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpProblem
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpProblem& value);

NTSCFG_INLINE
IcmpProblem::IcmpProblem()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);

    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
}

NTSCFG_INLINE
IcmpProblem::IcmpProblem(bslmf::MovableRef<IcmpProblem> original)
    NTSCFG_NOEXCEPT
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(BSLS_UTIL_ADDRESSOF(
                    bslmf::MovableRefUtil::access(original))),
                sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IcmpProblem::IcmpProblem(const IcmpProblem& original)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&original),
                sizeof *this);
}

NTSCFG_INLINE
IcmpProblem::~IcmpProblem()
{
}

NTSCFG_INLINE
IcmpProblem& IcmpProblem::operator=(bslmf::MovableRef<IcmpProblem> other)
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
IcmpProblem& IcmpProblem::operator=(const IcmpProblem& other)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&other),
                sizeof *this);

    return *this;
}

NTSCFG_INLINE
void IcmpProblem::reset()
{
    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
}

NTSCFG_INLINE
void IcmpProblem::setPointer(bsl::uint8_t value)
{
    d_pointer[0] = value;
}

NTSCFG_INLINE
void IcmpProblem::setPayload(const void* payload, bsl::size_t size)
{
    bsl::memset(d_payloadData, 0, sizeof d_payloadData);
    if (size > 0) {
        bsl::memcpy(d_payloadData,
                    payload,
                    bsl::min(size, sizeof d_payloadData));
    }
}

NTSCFG_INLINE
bsl::uint8_t IcmpProblem::pointer() const
{
    return d_pointer[0];
}

NTSCFG_INLINE
const bsl::uint8_t* IcmpProblem::payloadData() const
{
    return d_payloadData;
}

NTSCFG_INLINE
bsl::size_t IcmpProblem::payloadSize() const
{
    return d_payloadSize;
}

NTSCFG_INLINE
bool IcmpProblem::equals(const IcmpProblem& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool IcmpProblem::less(const IcmpProblem& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpProblem::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_pointer[0]);
    hashAppend(algorithm, d_header);
    if (d_payloadSize > 0) {
        algorithm(d_payloadData, d_payloadSize);
    }
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpProblem& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpProblem& lhs, const IcmpProblem& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpProblem& lhs, const IcmpProblem& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IcmpProblem& lhs, const IcmpProblem& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&    algorithm,
                              const IcmpProblem& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
