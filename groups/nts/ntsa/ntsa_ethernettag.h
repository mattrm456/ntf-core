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

#ifndef INCLUDED_NTSA_ETHERNETTAG
#define INCLUDED_NTSA_ETHERNETTAG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ethernetpriority.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntsa {

/// Describes an IEEE 802.1Q Ethernet tag.
///
/// @ingroup module_ntsa_protocol
class EthernetTag
{
    bdlb::BigEndianUint16 d_tci;

  public:
    /// Enumerates the constants used by the implementation.
    enum Constant {
        /// The length of the tag on the wire.
        k_LENGTH = 2,

        /// The minimum virtual LAN (VLAN) group identifier, inclusive.
        k_MIN_GROUP = 0,

        /// The maximum virtual LAN (VLAN) group identifier, inclusive.
        k_MAX_GROUP = 4095
    };

    /// Create a new Ethernet tag having a default value.
    EthernetTag();

    /// Create a new Ethernet tag having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    EthernetTag(bslmf::MovableRef<EthernetTag> original) NTSCFG_NOEXCEPT;

    /// Create a new Ethernet tag having the same value as the specified
    /// 'original' object.
    EthernetTag(const EthernetTag& original);

    /// Destroy this object.
    ~EthernetTag();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    EthernetTag& operator=(bslmf::MovableRef<EthernetTag> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EthernetTag& operator=(const EthernetTag& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the virtual LAN (VLAN) identifier to the specified 'value'. The
    /// behavior is undefined unless 'value' is less than or equal to
    /// k_MAX_GROUP.
    void setGroup(bsl::uint16_t value);

    /// Set the priority to the specified 'value'.
    void setPriority(ntsa::EthernetPriority::Value value);

    /// Set the flag that indicates the packet is eligible to the dropped in
    /// the presence of congestion to the specified 'value'.
    void setDroppable(bool value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the virtual LAN (VLAN) identifier.
    bsl::uint16_t group() const;

    /// Return the priority.
    ntsa::EthernetPriority::Value priority() const;

    /// Return the flag that indicates the packet is eligible to the dropped.
    bool droppable() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const EthernetTag& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const EthernetTag& other) const;

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

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(EthernetTag);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(EthernetTag);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(EthernetTag);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::EthernetTag
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetTag& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::EthernetTag
bool operator==(const EthernetTag& lhs, const EthernetTag& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::EthernetTag
bool operator!=(const EthernetTag& lhs, const EthernetTag& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::EthernetTag
bool operator<(const EthernetTag& lhs, const EthernetTag& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::EthernetTag
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EthernetTag& value);

NTSCFG_INLINE
EthernetTag::EthernetTag()
{
    BSLMF_ASSERT(sizeof *this == k_LENGTH);

    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
EthernetTag::EthernetTag(bslmf::MovableRef<EthernetTag> original)
    NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(original)),
        sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
EthernetTag::EthernetTag(const EthernetTag& original)
{
    NTSCFG_MEMORY_COPY(this, &original, sizeof *this);
}

NTSCFG_INLINE
EthernetTag::~EthernetTag()
{
}

NTSCFG_INLINE
EthernetTag& EthernetTag::operator=(bslmf::MovableRef<EthernetTag> other)
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
EthernetTag& EthernetTag::operator=(const EthernetTag& other)
{
    NTSCFG_MEMORY_COPY(this, &other, sizeof *this);

    return *this;
}

NTSCFG_INLINE
void EthernetTag::reset()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
void EthernetTag::setGroup(bsl::uint16_t value)
{
    BSLS_ASSERT(value <= k_MAX_GROUP);

    bsl::uint16_t tci = static_cast<bsl::uint16_t>(d_tci);
    tci   = static_cast<bsl::uint16_t>((tci & 0xF000) | (value & 0x0FFF));
    d_tci = tci;
}

NTSCFG_INLINE
void EthernetTag::setPriority(ntsa::EthernetPriority::Value value)
{
    bsl::uint16_t tci = static_cast<bsl::uint16_t>(d_tci);
    tci               = static_cast<bsl::uint16_t>(
        (tci & 0x1FFF) | (static_cast<bsl::uint16_t>(value) << 13));
    d_tci = tci;
}

NTSCFG_INLINE
void EthernetTag::setDroppable(bool value)
{
    bsl::uint16_t tci = static_cast<bsl::uint16_t>(d_tci);
    if (value) {
        tci = static_cast<bsl::uint16_t>(tci | 0x1000);
    }
    else {
        tci = static_cast<bsl::uint16_t>(tci & ~0x1000);
    }
    d_tci = tci;
}

NTSCFG_INLINE
bsl::uint16_t EthernetTag::group() const
{
    return static_cast<bsl::uint16_t>(static_cast<bsl::uint16_t>(d_tci) &
                                      0x0FFF);
}

NTSCFG_INLINE
ntsa::EthernetPriority::Value EthernetTag::priority() const
{
    return static_cast<ntsa::EthernetPriority::Value>(
        (static_cast<bsl::uint16_t>(d_tci) >> 13) & 0x07);
}

NTSCFG_INLINE
bool EthernetTag::droppable() const
{
    return (static_cast<bsl::uint16_t>(d_tci) & 0x1000) != 0;
}

NTSCFG_INLINE
bool EthernetTag::equals(const EthernetTag& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool EthernetTag::less(const EthernetTag& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void EthernetTag::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const void*>(this), sizeof *this);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetTag& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const EthernetTag& lhs, const EthernetTag& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const EthernetTag& lhs, const EthernetTag& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const EthernetTag& lhs, const EthernetTag& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&    algorithm,
                              const EthernetTag& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
