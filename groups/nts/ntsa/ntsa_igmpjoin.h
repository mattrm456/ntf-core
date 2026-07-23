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

#ifndef INCLUDED_NTSA_IGMPJOIN
#define INCLUDED_NTSA_IGMPJOIN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide the body of an IGMPv2 Membership Report message.
///
/// @details
/// The IGMPv2 Membership Report message is sent by a host to inform
/// neighboring multicast routers that the host is joining, has joined, or is
/// otherwise a member of a multicast group, as described in RFC 2236. The
/// IGMPv2 Membership Report message is addressed to the all-routers multicast
/// group (224.0.0.2).
///
/// The full join wire format, carried inside an IP datagram, is:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |  Type = 0x17  | Max Resp Time |          Checksum             |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                         Group Address                         |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// The type (0x17), max response time (set to zero), and checksum fields are
/// represented by 'ntsa::IgmpHeader'. This class represents the
/// message-specific body that immediately follows the header: the group
/// address being left.
///
/// Group Address (32 bits): The IP multicast group address of the group being
/// joined.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IgmpJoin
{
    /// The group address.
    ntsa::Ipv4Address d_groupAddress;

  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed length of the IgmpJoin body in octets.
        k_LENGTH = sizeof(ntsa::Ipv4Address)
    };

    /// Create a new IGMP join having a default value.
    IgmpJoin();

    /// Create a new IGMP join having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IgmpJoin(bslmf::MovableRef<IgmpJoin> original) NTSCFG_NOEXCEPT;

    /// Create a new IGMP join having the same value as the specified
    /// 'original' object.
    IgmpJoin(const IgmpJoin& original);

    /// Destroy this object.
    ~IgmpJoin();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IgmpJoin& operator=(bslmf::MovableRef<IgmpJoin> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IgmpJoin& operator=(const IgmpJoin& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the group address to the specified 'value'.
    void setGroupAddress(const ntsa::Ipv4Address& value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the group address.
    const ntsa::Ipv4Address& groupAddress() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IgmpJoin& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IgmpJoin& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IgmpJoin);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IgmpJoin);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IgmpJoin);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IgmpJoin
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpJoin& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IgmpJoin
bool operator==(const IgmpJoin& lhs, const IgmpJoin& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IgmpJoin
bool operator!=(const IgmpJoin& lhs, const IgmpJoin& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IgmpJoin
bool operator<(const IgmpJoin& lhs, const IgmpJoin& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IgmpJoin
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IgmpJoin& value);

NTSCFG_INLINE
IgmpJoin::IgmpJoin()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);

    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
IgmpJoin::IgmpJoin(bslmf::MovableRef<IgmpJoin> original) NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(original)),
        sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IgmpJoin::IgmpJoin(const IgmpJoin& original)
{
    NTSCFG_MEMORY_COPY(this, &original, sizeof *this);
}

NTSCFG_INLINE
IgmpJoin::~IgmpJoin()
{
}

NTSCFG_INLINE
IgmpJoin& IgmpJoin::operator=(bslmf::MovableRef<IgmpJoin> other)
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
IgmpJoin& IgmpJoin::operator=(const IgmpJoin& other)
{
    NTSCFG_MEMORY_COPY(this, &other, sizeof *this);

    return *this;
}

NTSCFG_INLINE
void IgmpJoin::reset()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
void IgmpJoin::setGroupAddress(const ntsa::Ipv4Address& value)
{
    d_groupAddress = value;
}

NTSCFG_INLINE
const ntsa::Ipv4Address& IgmpJoin::groupAddress() const
{
    return d_groupAddress;
}

NTSCFG_INLINE
bool IgmpJoin::equals(const IgmpJoin& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool IgmpJoin::less(const IgmpJoin& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IgmpJoin::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_groupAddress);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpJoin& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IgmpJoin& lhs, const IgmpJoin& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IgmpJoin& lhs, const IgmpJoin& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IgmpJoin& lhs, const IgmpJoin& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM& algorithm, const IgmpJoin& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
