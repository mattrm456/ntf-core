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

#ifndef INCLUDED_NTSA_IGMPRECORD
#define INCLUDED_NTSA_IGMPRECORD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_igmprecordtype.h>
#include <ntsa_ipv4address.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsl_iosfwd.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Describe a group record within an IGMPv3 Membership Report message.
///
/// @details
/// Each group record contains information pertaining to the sender's
/// membership in a single multicast group on the interface from which the
/// report is sent, as described in RFC 9776.
///
/// The wire format of a group record is:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |  Record Type  |  Aux Data Len |     Number of Sources (N)     |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                       Multicast Address                       |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                       Source Address [1]                      |
/// +-                                                             -+
/// .                               .                               .
/// +-                                                             -+
/// |                       Source Address [N]                      |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// Record Type (8 bits): Identifies the type of group record:
///
///   1 - MODE_IS_INCLUDE (Current-State Record)
///   2 - MODE_IS_EXCLUDE (Current-State Record)
///   3 - CHANGE_TO_INCLUDE_MODE (Filter-Mode-Change Record)
///   4 - CHANGE_TO_EXCLUDE_MODE (Filter-Mode-Change Record)
///   5 - ALLOW_NEW_SOURCES (Source-List-Change Record)
///   6 - BLOCK_OLD_SOURCES (Source-List-Change Record)
///
/// Multicast Address (32 bits): The IP multicast group address to which this
/// group record pertains.
///
/// Source Address [i] (32 bits each): A vector of IP unicast source addresses.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IgmpRecord
{
    /// The record type.
    ntsa::IgmpRecordType::Value d_type;

    /// The multicast address.
    ntsa::Ipv4Address d_multicastAddress;

    /// The source addresses.
    bsl::vector<ntsa::Ipv4Address> d_sourceAddresses;

  public:
    /// Create a new IGMP group record having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit IgmpRecord(bslma::Allocator* basicAllocator = 0);

    /// Create a new IGMP group record having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IgmpRecord(bslmf::MovableRef<IgmpRecord> original) NTSCFG_NOEXCEPT;

    /// Create a new IGMP group record having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    IgmpRecord(const IgmpRecord& original,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~IgmpRecord();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IgmpRecord& operator=(bslmf::MovableRef<IgmpRecord> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IgmpRecord& operator=(const IgmpRecord& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the record type to the specified 'value'.
    void setType(ntsa::IgmpRecordType::Value value);

    /// Set the multicast address to the specified 'value'.
    void setMulticastAddress(const ntsa::Ipv4Address& value);

    /// Set the source addresses to the specified 'value'.
    void setSourceAddresses(const bsl::vector<ntsa::Ipv4Address>& value);

    /// Add the specified 'value' to the source address list.
    void addSourceAddress(const ntsa::Ipv4Address& value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the record type.
    ntsa::IgmpRecordType::Value type() const;

    /// Return the multicast address.
    const ntsa::Ipv4Address& multicastAddress() const;

    /// Return the source addresses.
    const bsl::vector<ntsa::Ipv4Address>& sourceAddresses() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IgmpRecord& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IgmpRecord& other) const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(IgmpRecord);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::IgmpRecord
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpRecord& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IgmpRecord
bool operator==(const IgmpRecord& lhs, const IgmpRecord& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IgmpRecord
bool operator!=(const IgmpRecord& lhs, const IgmpRecord& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IgmpRecord
bool operator<(const IgmpRecord& lhs, const IgmpRecord& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IgmpRecord
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IgmpRecord& value);

NTSCFG_INLINE
IgmpRecord::IgmpRecord(bslma::Allocator* basicAllocator)
: d_type(ntsa::IgmpRecordType::e_UNDEFINED)
, d_multicastAddress()
, d_sourceAddresses(basicAllocator)
{
}

NTSCFG_INLINE
IgmpRecord::IgmpRecord(bslmf::MovableRef<IgmpRecord> original) NTSCFG_NOEXCEPT
: d_type(NTSCFG_MOVE_FROM(original, d_type)),
  d_multicastAddress(NTSCFG_MOVE_FROM(original, d_multicastAddress)),
  d_sourceAddresses(NTSCFG_MOVE_FROM(original, d_sourceAddresses))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IgmpRecord::IgmpRecord(const IgmpRecord& original,
                       bslma::Allocator* basicAllocator)
: d_type(original.d_type)
, d_multicastAddress(original.d_multicastAddress)
, d_sourceAddresses(original.d_sourceAddresses, basicAllocator)
{
}

NTSCFG_INLINE
IgmpRecord::~IgmpRecord()
{
}

NTSCFG_INLINE
IgmpRecord& IgmpRecord::operator=(bslmf::MovableRef<IgmpRecord> other)
    NTSCFG_NOEXCEPT
{
    d_type             = NTSCFG_MOVE_FROM(other, d_type);
    d_multicastAddress = NTSCFG_MOVE_FROM(other, d_multicastAddress);
    d_sourceAddresses  = NTSCFG_MOVE_FROM(other, d_sourceAddresses);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IgmpRecord& IgmpRecord::operator=(const IgmpRecord& other)
{
    d_type             = other.d_type;
    d_multicastAddress = other.d_multicastAddress;
    d_sourceAddresses  = other.d_sourceAddresses;

    return *this;
}

NTSCFG_INLINE
void IgmpRecord::reset()
{
    d_type             = ntsa::IgmpRecordType::e_UNDEFINED;
    d_multicastAddress = ntsa::Ipv4Address();
    d_sourceAddresses.clear();
}

NTSCFG_INLINE
void IgmpRecord::setType(ntsa::IgmpRecordType::Value value)
{
    d_type = value;
}

NTSCFG_INLINE
void IgmpRecord::setMulticastAddress(const ntsa::Ipv4Address& value)
{
    d_multicastAddress = value;
}

NTSCFG_INLINE
void IgmpRecord::setSourceAddresses(
    const bsl::vector<ntsa::Ipv4Address>& value)
{
    d_sourceAddresses = value;
}

NTSCFG_INLINE
void IgmpRecord::addSourceAddress(const ntsa::Ipv4Address& value)
{
    d_sourceAddresses.push_back(value);
}

NTSCFG_INLINE
ntsa::IgmpRecordType::Value IgmpRecord::type() const
{
    return d_type;
}

NTSCFG_INLINE
const ntsa::Ipv4Address& IgmpRecord::multicastAddress() const
{
    return d_multicastAddress;
}

NTSCFG_INLINE
const bsl::vector<ntsa::Ipv4Address>& IgmpRecord::sourceAddresses() const
{
    return d_sourceAddresses;
}

NTSCFG_INLINE
bool IgmpRecord::equals(const IgmpRecord& other) const
{
    return d_type == other.d_type &&
           d_multicastAddress == other.d_multicastAddress &&
           d_sourceAddresses == other.d_sourceAddresses;
}

NTSCFG_INLINE
bool IgmpRecord::less(const IgmpRecord& other) const
{
    if (d_type < other.d_type) {
        return true;
    }

    if (other.d_type < d_type) {
        return false;
    }

    if (d_multicastAddress < other.d_multicastAddress) {
        return true;
    }

    if (other.d_multicastAddress < d_multicastAddress) {
        return false;
    }

    return d_sourceAddresses < other.d_sourceAddresses;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IgmpRecord::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_type);
    hashAppend(algorithm, d_multicastAddress);
    hashAppend(algorithm, d_sourceAddresses);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpRecord& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IgmpRecord& lhs, const IgmpRecord& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IgmpRecord& lhs, const IgmpRecord& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IgmpRecord& lhs, const IgmpRecord& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&   algorithm,
                              const IgmpRecord& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
