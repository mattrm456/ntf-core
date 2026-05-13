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

#ifndef INCLUDED_NTSA_ICMPTIMEOUT
#define INCLUDED_NTSA_ICMPTIMEOUT

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

/// Provide the body of an ICMP type 11 time exceeded message.
///
/// @details
/// The ICMP time exceeded message is sent by a gateway or host when a
/// datagram cannot be delivered because a time constraint has been violated,
/// as described in RFC 792.
///
/// A gateway sends this message when it discards a datagram whose time-to-live
/// field has reached zero. A host sends this message when it discards an
/// incomplete fragmented datagram because the reassembly timer expired before
/// all fragments arrived.
///
/// The full time exceeded wire format, carried inside an IP datagram, is:
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
/// The type (11), code, and checksum fields are represented by
/// 'ntsa::IcmpHeader'. This class represents the fixed message-specific body
/// that immediately follows the header: a 32-bit reserved field that must be
/// zero on transmission. The variable-length original datagram data that
/// trails the body is not represented by this class.
///
/// The code field in the accompanying 'ntsa::IcmpHeader' identifies the
/// specific condition:
///
///   0 - Time to live exceeded in transit (sent by a gateway).
///   1 - Fragment reassembly time exceeded (sent by a host).
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpTimeout
{
    /// Reserved field; must be zero.
    bdlb::BigEndianUint32 d_unused;

  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed length of the IcmpTimeout body in octets.
        k_LENGTH = 4
    };

    /// Create a new ICMP timeout having a default value.
    IcmpTimeout();

    /// Create a new ICMP timeout having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IcmpTimeout(bslmf::MovableRef<IcmpTimeout> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP timeout having the same value as the specified
    /// 'original' object.
    IcmpTimeout(const IcmpTimeout& original);

    /// Destroy this object.
    ~IcmpTimeout();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpTimeout& operator=(
        bslmf::MovableRef<IcmpTimeout> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpTimeout& operator=(const IcmpTimeout& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

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

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IcmpTimeout& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IcmpTimeout& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IcmpTimeout);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IcmpTimeout);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IcmpTimeout);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IcmpTimeout
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpTimeout& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpTimeout
bool operator==(const IcmpTimeout& lhs, const IcmpTimeout& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpTimeout
bool operator!=(const IcmpTimeout& lhs, const IcmpTimeout& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpTimeout
bool operator<(const IcmpTimeout& lhs, const IcmpTimeout& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpTimeout
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpTimeout& value);

NTSCFG_INLINE
IcmpTimeout::IcmpTimeout()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);

    d_unused = static_cast<bsl::uint32_t>(0);
}

NTSCFG_INLINE
IcmpTimeout::IcmpTimeout(
    bslmf::MovableRef<IcmpTimeout> original) NTSCFG_NOEXCEPT
: d_unused(NTSCFG_MOVE_FROM(original, d_unused))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IcmpTimeout::IcmpTimeout(const IcmpTimeout& original)
: d_unused(original.d_unused)
{
}

NTSCFG_INLINE
IcmpTimeout::~IcmpTimeout()
{
}

NTSCFG_INLINE
IcmpTimeout& IcmpTimeout::operator=(
    bslmf::MovableRef<IcmpTimeout> other) NTSCFG_NOEXCEPT
{
    d_unused = NTSCFG_MOVE_FROM(other, d_unused);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IcmpTimeout& IcmpTimeout::operator=(const IcmpTimeout& other)
{
    d_unused = other.d_unused;

    return *this;
}

NTSCFG_INLINE
void IcmpTimeout::reset()
{
    d_unused = static_cast<bsl::uint32_t>(0);
}

NTSCFG_INLINE
bool IcmpTimeout::equals(const IcmpTimeout& other) const
{
    return d_unused == other.d_unused;
}

NTSCFG_INLINE
bool IcmpTimeout::less(const IcmpTimeout& other) const
{
    return static_cast<bsl::uint32_t>(d_unused) <
           static_cast<bsl::uint32_t>(other.d_unused);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpTimeout::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, static_cast<bsl::uint32_t>(d_unused));
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpTimeout& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpTimeout& lhs, const IcmpTimeout& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpTimeout& lhs, const IcmpTimeout& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IcmpTimeout& lhs, const IcmpTimeout& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&    algorithm,
                              const IcmpTimeout& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
