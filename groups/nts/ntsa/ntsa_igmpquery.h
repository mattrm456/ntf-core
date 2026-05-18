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

#ifndef INCLUDED_NTSA_IGMPQUERY
#define INCLUDED_NTSA_IGMPQUERY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
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
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide the body of an IGMPv3 Membership Query message.
///
/// @details
/// The IGMPv3 Membership Query is sent by a multicast router to query the
/// multicast reception state of neighboring interfaces, as described in
/// RFC 9776. This class represents only the fields that follow the common
/// 8-byte IGMP header (type, max response code, checksum, group address),
/// which is represented separately by 'ntsa::IgmpHeader'.
///
/// The body fields represented by this class are:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// | Flags |S| QRV |     QQIC      |     Number of Sources (N)     |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                       Source Address [1]                      |
/// +-                                                             -+
/// |                       Source Address [2]                      |
/// +-                              .                              -+
/// .                               .                               .
/// +-                                                             -+
/// |                       Source Address [N]                      |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// Flags (4 bits): Reserved flags managed by the IANA "IGMP Type Numbers"
/// registry.
///
/// S Flag (1 bit): When set, indicates to receiving multicast routers that
/// they should suppress normal timer updates upon receiving this Query.
///
/// QRV (3 bits): The Querier's Robustness Variable. If non-zero, routers
/// adopt this value as their own Robustness Variable.
///
/// QQIC (8 bits): The Querier's Query Interval Code. Encodes the querier's
/// query interval in seconds.
///
/// Number of Sources (16 bits): The number of source addresses present in
/// this Query. Zero in a General Query or Group-Specific Query; non-zero in
/// a Group-and-Source-Specific Query.
///
/// Source Address [i] (32 bits each): A vector of IP unicast source addresses.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IgmpQuery
{
    /// The group address.
    ntsa::Ipv4Address d_groupAddress;

    /// The combined flags/S/QRV byte.
    bsl::uint8_t d_flagsAndQrv;

    /// The Querier's Query Interval Code.
    bsl::uint8_t d_qqic;

    /// The source addresses.
    bsl::vector<ntsa::Ipv4Address> d_sourceAddresses;

  public:
    /// Create a new IGMP query body having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit IgmpQuery(bslma::Allocator* basicAllocator = 0);

    /// Create a new IGMP query body having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IgmpQuery(bslmf::MovableRef<IgmpQuery> original) NTSCFG_NOEXCEPT;

    /// Create a new IGMP query body having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    IgmpQuery(const IgmpQuery& original,
              bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~IgmpQuery();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IgmpQuery& operator=(bslmf::MovableRef<IgmpQuery> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IgmpQuery& operator=(const IgmpQuery& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the group address to the specified 'value'.
    void setGroupAddress(const ntsa::Ipv4Address& value);

    /// Set the suppress router-side processing flag to the specified 'value'.
    void setSuppressFlag(bool value);

    /// Set the Querier's Robustness Variable to the specified 'value'. The
    /// behavior is undefined unless 'value' is in the range [0, 7].
    void setQrv(bsl::uint8_t value);

    /// Set the Querier's Query Interval Code to the specified 'value'.
    void setQqic(bsl::uint8_t value);

    /// Set the source addresses to the specified 'value'.
    void setSourceAddresses(const bsl::vector<ntsa::Ipv4Address>& value);

    /// Add the specified 'value' to the source address list.
    void addSourceAddress(const ntsa::Ipv4Address& value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the group address.
    const ntsa::Ipv4Address& groupAddress() const;

    /// Return the suppress router-side processing flag.
    bool suppressFlag() const;

    /// Return the Querier's Robustness Variable.
    bsl::uint8_t qrv() const;

    /// Return the Querier's Query Interval Code.
    bsl::uint8_t qqic() const;

    /// Return the source addresses.
    const bsl::vector<ntsa::Ipv4Address>& sourceAddresses() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IgmpQuery& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IgmpQuery& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(IgmpQuery);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IgmpQuery
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpQuery& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IgmpQuery
bool operator==(const IgmpQuery& lhs, const IgmpQuery& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IgmpQuery
bool operator!=(const IgmpQuery& lhs, const IgmpQuery& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IgmpQuery
bool operator<(const IgmpQuery& lhs, const IgmpQuery& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IgmpQuery
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IgmpQuery& value);

NTSCFG_INLINE
IgmpQuery::IgmpQuery(bslma::Allocator* basicAllocator)
: d_groupAddress()
, d_flagsAndQrv(0)
, d_qqic(0)
, d_sourceAddresses(basicAllocator)
{
}

NTSCFG_INLINE
IgmpQuery::IgmpQuery(bslmf::MovableRef<IgmpQuery> original) NTSCFG_NOEXCEPT
: d_groupAddress(NTSCFG_MOVE_FROM(original, d_groupAddress))
, d_flagsAndQrv(NTSCFG_MOVE_FROM(original, d_flagsAndQrv))
, d_qqic(NTSCFG_MOVE_FROM(original, d_qqic))
, d_sourceAddresses(NTSCFG_MOVE_FROM(original, d_sourceAddresses))
{
}

NTSCFG_INLINE
IgmpQuery::IgmpQuery(const IgmpQuery&  original,
                     bslma::Allocator* basicAllocator)
: d_groupAddress(original.d_groupAddress)
, d_flagsAndQrv(original.d_flagsAndQrv)
, d_qqic(original.d_qqic)
, d_sourceAddresses(original.d_sourceAddresses, basicAllocator)
{
}

NTSCFG_INLINE
IgmpQuery::~IgmpQuery()
{
}

NTSCFG_INLINE
IgmpQuery& IgmpQuery::operator=(bslmf::MovableRef<IgmpQuery> other)
    NTSCFG_NOEXCEPT
{
    d_groupAddress    = NTSCFG_MOVE_FROM(other, d_groupAddress);
    d_flagsAndQrv     = NTSCFG_MOVE_FROM(other, d_flagsAndQrv);
    d_qqic            = NTSCFG_MOVE_FROM(other, d_qqic);
    d_sourceAddresses = NTSCFG_MOVE_FROM(other, d_sourceAddresses);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IgmpQuery& IgmpQuery::operator=(const IgmpQuery& other)
{
    d_groupAddress    = other.d_groupAddress;
    d_flagsAndQrv     = other.d_flagsAndQrv;
    d_qqic            = other.d_qqic;
    d_sourceAddresses = other.d_sourceAddresses;

    return *this;
}

NTSCFG_INLINE
void IgmpQuery::reset()
{
    d_groupAddress = ntsa::Ipv4Address();
    d_flagsAndQrv  = 0;
    d_qqic         = 0;
    d_sourceAddresses.clear();
}

NTSCFG_INLINE
void IgmpQuery::setGroupAddress(const ntsa::Ipv4Address& value)
{
    d_groupAddress = value;
}

NTSCFG_INLINE
void IgmpQuery::setSuppressFlag(bool value)
{
    if (value) {
        d_flagsAndQrv |= 0x08;
    }
    else {
        d_flagsAndQrv &= static_cast<bsl::uint8_t>(~0x08);
    }
}

NTSCFG_INLINE
void IgmpQuery::setQrv(bsl::uint8_t value)
{
    BSLS_ASSERT(value <= 7);
    d_flagsAndQrv = static_cast<bsl::uint8_t>(
        (d_flagsAndQrv & 0xF8) | (value & 0x07));
}

NTSCFG_INLINE
void IgmpQuery::setQqic(bsl::uint8_t value)
{
    d_qqic = value;
}

NTSCFG_INLINE
void IgmpQuery::setSourceAddresses(
    const bsl::vector<ntsa::Ipv4Address>& value)
{
    d_sourceAddresses = value;
}

NTSCFG_INLINE
void IgmpQuery::addSourceAddress(const ntsa::Ipv4Address& value)
{
    d_sourceAddresses.push_back(value);
}

NTSCFG_INLINE
const ntsa::Ipv4Address& IgmpQuery::groupAddress() const
{
    return d_groupAddress;
}

NTSCFG_INLINE
bool IgmpQuery::suppressFlag() const
{
    return (d_flagsAndQrv & 0x08) != 0;
}

NTSCFG_INLINE
bsl::uint8_t IgmpQuery::qrv() const
{
    return static_cast<bsl::uint8_t>(d_flagsAndQrv & 0x07);
}

NTSCFG_INLINE
bsl::uint8_t IgmpQuery::qqic() const
{
    return d_qqic;
}

NTSCFG_INLINE
const bsl::vector<ntsa::Ipv4Address>& IgmpQuery::sourceAddresses() const
{
    return d_sourceAddresses;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IgmpQuery::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_groupAddress);
    hashAppend(algorithm, d_flagsAndQrv);
    hashAppend(algorithm, d_qqic);
    hashAppend(algorithm, d_sourceAddresses);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpQuery& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IgmpQuery& lhs, const IgmpQuery& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IgmpQuery& lhs, const IgmpQuery& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IgmpQuery& lhs, const IgmpQuery& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM& algorithm,
                              const IgmpQuery& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
