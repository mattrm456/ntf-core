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

#ifndef INCLUDED_NTSA_IGMPREPORT
#define INCLUDED_NTSA_IGMPREPORT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_igmprecord.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide the body of an IGMPv3 Membership Report message.
///
/// @details
/// The IGMPv3 Membership Report is sent by IP systems to report (to
/// neighboring routers) the current multicast reception state, or changes in
/// the multicast reception state, of their interfaces, as described in
/// RFC 9776. This class represents only the fields that follow the common
/// 4-byte IGMP header (type, reserved, checksum), which is represented
/// separately by 'ntsa::IgmpHeader'.
///
/// The body fields represented by this class are:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |             Flags             |  Number of Group Records (M)  |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                        Group Record [1]                       |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                        Group Record [2]                       |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// .                               .                               .
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                        Group Record [M]                       |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// Flags (16 bits): Reserved flags managed by the IANA "IGMP Type Numbers"
/// registry.
///
/// Number of Group Records (16 bits): The number of group records present in
/// this report.
///
/// Group Record [i]: Each group record is a block of fields containing
/// information pertaining to the sender's membership in a single multicast
/// group, represented by 'ntsa::IgmpRecord'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IgmpReport
{
    /// The flags.
    bsl::uint16_t d_flags;

    /// The group records.
    bsl::vector<ntsa::IgmpRecord> d_records;

  public:
    /// Create a new IGMP report body having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit IgmpReport(bslma::Allocator* basicAllocator = 0);

    /// Create a new IGMP report body having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IgmpReport(bslmf::MovableRef<IgmpReport> original) NTSCFG_NOEXCEPT;

    /// Create a new IGMP report body having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    IgmpReport(const IgmpReport& original,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~IgmpReport();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IgmpReport& operator=(bslmf::MovableRef<IgmpReport> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IgmpReport& operator=(const IgmpReport& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the flags to the specified 'value'.
    void setFlags(bsl::uint16_t value);

    /// Set the group records to the specified 'value'.
    void setRecords(const bsl::vector<ntsa::IgmpRecord>& value);

    /// Add the specified 'value' to the group record list.
    void addRecord(const ntsa::IgmpRecord& value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the flags.
    bsl::uint16_t flags() const;

    /// Return the group records.
    const bsl::vector<ntsa::IgmpRecord>& records() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IgmpReport& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IgmpReport& other) const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(IgmpReport);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IgmpReport
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpReport& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IgmpReport
bool operator==(const IgmpReport& lhs, const IgmpReport& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IgmpReport
bool operator!=(const IgmpReport& lhs, const IgmpReport& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IgmpReport
bool operator<(const IgmpReport& lhs, const IgmpReport& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IgmpReport
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IgmpReport& value);

NTSCFG_INLINE
IgmpReport::IgmpReport(bslma::Allocator* basicAllocator)
: d_flags(0)
, d_records(basicAllocator)
{
}

NTSCFG_INLINE
IgmpReport::IgmpReport(bslmf::MovableRef<IgmpReport> original) NTSCFG_NOEXCEPT
: d_flags(NTSCFG_MOVE_FROM(original, d_flags))
, d_records(NTSCFG_MOVE_FROM(original, d_records))
{
}

NTSCFG_INLINE
IgmpReport::IgmpReport(const IgmpReport& original,
                       bslma::Allocator* basicAllocator)
: d_flags(original.d_flags)
, d_records(original.d_records, basicAllocator)
{
}

NTSCFG_INLINE
IgmpReport::~IgmpReport()
{
}

NTSCFG_INLINE
IgmpReport& IgmpReport::operator=(bslmf::MovableRef<IgmpReport> other)
    NTSCFG_NOEXCEPT
{
    d_flags   = NTSCFG_MOVE_FROM(other, d_flags);
    d_records = NTSCFG_MOVE_FROM(other, d_records);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IgmpReport& IgmpReport::operator=(const IgmpReport& other)
{
    d_flags   = other.d_flags;
    d_records = other.d_records;

    return *this;
}

NTSCFG_INLINE
void IgmpReport::reset()
{
    d_flags = 0;
    d_records.clear();
}

NTSCFG_INLINE
void IgmpReport::setFlags(bsl::uint16_t value)
{
    d_flags = value;
}

NTSCFG_INLINE
void IgmpReport::setRecords(const bsl::vector<ntsa::IgmpRecord>& value)
{
    d_records = value;
}

NTSCFG_INLINE
void IgmpReport::addRecord(const ntsa::IgmpRecord& value)
{
    d_records.push_back(value);
}

NTSCFG_INLINE
bsl::uint16_t IgmpReport::flags() const
{
    return d_flags;
}

NTSCFG_INLINE
const bsl::vector<ntsa::IgmpRecord>& IgmpReport::records() const
{
    return d_records;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IgmpReport::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_flags);
    hashAppend(algorithm, d_records);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpReport& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IgmpReport& lhs, const IgmpReport& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IgmpReport& lhs, const IgmpReport& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IgmpReport& lhs, const IgmpReport& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&   algorithm,
                              const IgmpReport& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
