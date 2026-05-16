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

#ifndef INCLUDED_NTSA_ICMPROUTERINFO
#define INCLUDED_NTSA_ICMPROUTERINFO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv4address.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Describe a router advertised through the Internet Control Message Protocol
/// (ICMP).
///
/// @ingroup module_ntsa_protocol
class IcmpRouterInfo
{
    ntsa::Ipv4Address d_address;
    bsl::int32_t      d_priority;

  public:
    /// Create a new TCP time point interval having a default value.
    IcmpRouterInfo();

    /// Create a new TCP time point interval having the same value as the
    /// specified 'original' object. Assign an unspecified but valid value to
    /// the 'original' original.
    IcmpRouterInfo(bslmf::MovableRef<IcmpRouterInfo> original) NTSCFG_NOEXCEPT;

    /// Create a new TCP time point interval having the same value as the
    /// specified 'original' object.
    IcmpRouterInfo(const IcmpRouterInfo& original);

    /// Destroy this object.
    ~IcmpRouterInfo();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpRouterInfo& operator=(bslmf::MovableRef<IcmpRouterInfo> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpRouterInfo& operator=(const IcmpRouterInfo& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the address to the specified 'value'.
    void setAddress(const ntsa::Ipv4Address& value);

    /// Set the priority to the specified 'value'.
    void setPriority(bsl::int32_t value);

    /// Return the address.
    const ntsa::Ipv4Address& address() const;

    /// Return the priority.
    bsl::int32_t priority() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IcmpRouterInfo& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IcmpRouterInfo& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IcmpRouterInfo);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IcmpRouterInfo);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IcmpRouterInfo);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::IcmpRouterInfo
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpRouterInfo& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpRouterInfo
bool operator==(const IcmpRouterInfo& lhs, const IcmpRouterInfo& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpRouterInfo
bool operator!=(const IcmpRouterInfo& lhs, const IcmpRouterInfo& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpRouterInfo
bool operator<(const IcmpRouterInfo& lhs, const IcmpRouterInfo& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpRouterInfo
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpRouterInfo& value);

NTSCFG_INLINE
IcmpRouterInfo::IcmpRouterInfo()
: d_address()
, d_priority(0)
{
}

NTSCFG_INLINE
IcmpRouterInfo::IcmpRouterInfo(bslmf::MovableRef<IcmpRouterInfo> original)
    NTSCFG_NOEXCEPT : d_address(NTSCFG_MOVE_FROM(original, d_address)),
                      d_priority(NTSCFG_MOVE_FROM(original, d_priority))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IcmpRouterInfo::IcmpRouterInfo(const IcmpRouterInfo& original)
: d_address(original.d_address)
, d_priority(original.d_priority)
{
}

NTSCFG_INLINE
IcmpRouterInfo::~IcmpRouterInfo()
{
}

NTSCFG_INLINE
IcmpRouterInfo& IcmpRouterInfo::operator=(
    bslmf::MovableRef<IcmpRouterInfo> other) NTSCFG_NOEXCEPT
{
    d_address  = NTSCFG_MOVE_FROM(other, d_address);
    d_priority = NTSCFG_MOVE_FROM(other, d_priority);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IcmpRouterInfo& IcmpRouterInfo::operator=(const IcmpRouterInfo& other)
{
    d_address  = other.d_address;
    d_priority = other.d_priority;

    return *this;
}

NTSCFG_INLINE
void IcmpRouterInfo::reset()
{
    d_address  = 0;
    d_priority = 0;
}

NTSCFG_INLINE
void IcmpRouterInfo::setAddress(const ntsa::Ipv4Address& value)
{
    d_address = value;
}

NTSCFG_INLINE
void IcmpRouterInfo::setPriority(bsl::int32_t value)
{
    d_priority = value;
}

NTSCFG_INLINE
const ntsa::Ipv4Address& IcmpRouterInfo::address() const
{
    return d_address;
}

NTSCFG_INLINE
bsl::int32_t IcmpRouterInfo::priority() const
{
    return d_priority;
}

NTSCFG_INLINE
bool IcmpRouterInfo::equals(const IcmpRouterInfo& other) const
{
    return d_address == other.d_address && d_priority == other.d_priority;
}

NTSCFG_INLINE
bool IcmpRouterInfo::less(const IcmpRouterInfo& other) const
{
    if (d_address < other.d_address) {
        return true;
    }

    if (other.d_address < d_address) {
        return false;
    }

    return d_priority < other.d_priority;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpRouterInfo::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_address);
    hashAppend(algorithm, d_priority);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpRouterInfo& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpRouterInfo& lhs, const IcmpRouterInfo& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpRouterInfo& lhs, const IcmpRouterInfo& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IcmpRouterInfo& lhs, const IcmpRouterInfo& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&       algorithm,
                              const IcmpRouterInfo& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
