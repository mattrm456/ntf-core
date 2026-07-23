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

#ifndef INCLUDED_NTSA_ICMPROUTERRESPONSE
#define INCLUDED_NTSA_ICMPROUTERRESPONSE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_icmprouterinfo.h>
#include <ntsa_ipv4header.h>
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

/// Provide the body of an ICMP type 9 router advertisement message.
///
/// @details
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpRouterResponse
{
    /// The vector of router addresses and their priorities.
    bsl::vector<ntsa::IcmpRouterInfo> d_infoVector;

    /// The maximum number of seconds the router addresses should be considered
    /// to be valid.
    bsl::uint16_t d_timeToLive;

  public:
    /// Enumerates the constants used by this implementation.
    enum Constant {
        /// The default number of seconds the router addresses should be
        /// considered to be valid.
        k_DEFAULT_TIME_TO_LIVE = 1800
    };

    /// Create a new ICMP router response having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit IcmpRouterResponse(bslma::Allocator* basicAllocator = 0);

    /// Create a new ICMP router response having the same value as the
    /// specified 'original' object. Assign an unspecified but valid value to
    /// the 'original' original.
    IcmpRouterResponse(bslmf::MovableRef<IcmpRouterResponse> original)
        NTSCFG_NOEXCEPT;

    /// Create a new ICMP router response having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    IcmpRouterResponse(const IcmpRouterResponse& original,
                       bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~IcmpRouterResponse();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpRouterResponse& operator=(bslmf::MovableRef<IcmpRouterResponse> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpRouterResponse& operator=(const IcmpRouterResponse& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Add the specified 'value' to the entry vector of available routers.
    void addInfo(const ntsa::IcmpRouterInfo& value);

    /// Set the entry vector of available routers to the specified 'value'.
    void setInfo(const bsl::vector<ntsa::IcmpRouterInfo>& value);

    /// Set the time-to-live to the specified 'value'.
    void setTimeToLive(bsl::uint16_t value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the entry vector of available routers.
    const bsl::vector<ntsa::IcmpRouterInfo>& info() const;

    /// Return the time-to-live.
    bsl::uint16_t timeToLive() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IcmpRouterResponse& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IcmpRouterResponse& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(IcmpRouterResponse);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IcmpRouterResponse
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const IcmpRouterResponse& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpRouterResponse
bool operator==(const IcmpRouterResponse& lhs, const IcmpRouterResponse& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpRouterResponse
bool operator!=(const IcmpRouterResponse& lhs, const IcmpRouterResponse& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpRouterResponse
bool operator<(const IcmpRouterResponse& lhs, const IcmpRouterResponse& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpRouterResponse
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpRouterResponse& value);

NTSCFG_INLINE
IcmpRouterResponse::IcmpRouterResponse(bslma::Allocator* basicAllocator)
: d_infoVector(basicAllocator)
, d_timeToLive(k_DEFAULT_TIME_TO_LIVE)
{
}

NTSCFG_INLINE
IcmpRouterResponse::IcmpRouterResponse(
    bslmf::MovableRef<IcmpRouterResponse> original) NTSCFG_NOEXCEPT
: d_infoVector(NTSCFG_MOVE_FROM(original, d_infoVector)),
  d_timeToLive(NTSCFG_MOVE_FROM(original, d_timeToLive))
{
}

NTSCFG_INLINE
IcmpRouterResponse::IcmpRouterResponse(const IcmpRouterResponse& original,
                                       bslma::Allocator* basicAllocator)
: d_infoVector(original.d_infoVector, basicAllocator)
, d_timeToLive(original.d_timeToLive)
{
}

NTSCFG_INLINE
IcmpRouterResponse::~IcmpRouterResponse()
{
}

NTSCFG_INLINE
IcmpRouterResponse& IcmpRouterResponse::operator=(
    bslmf::MovableRef<IcmpRouterResponse> other) NTSCFG_NOEXCEPT
{
    d_infoVector = NTSCFG_MOVE_FROM(other, d_infoVector);
    d_timeToLive = NTSCFG_MOVE_FROM(other, d_timeToLive);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IcmpRouterResponse& IcmpRouterResponse::operator=(
    const IcmpRouterResponse& other)
{
    d_infoVector = other.d_infoVector;
    d_timeToLive = other.d_timeToLive;

    return *this;
}

NTSCFG_INLINE
void IcmpRouterResponse::reset()
{
    d_infoVector.clear();
    d_timeToLive = k_DEFAULT_TIME_TO_LIVE;
}

NTSCFG_INLINE
void IcmpRouterResponse::addInfo(const ntsa::IcmpRouterInfo& value)
{
    d_infoVector.push_back(value);
}

NTSCFG_INLINE
void IcmpRouterResponse::setInfo(
    const bsl::vector<ntsa::IcmpRouterInfo>& value)
{
    d_infoVector = value;
}

NTSCFG_INLINE
void IcmpRouterResponse::setTimeToLive(bsl::uint16_t value)
{
    d_timeToLive = value;
}

NTSCFG_INLINE
const bsl::vector<ntsa::IcmpRouterInfo>& IcmpRouterResponse::info() const
{
    return d_infoVector;
}

NTSCFG_INLINE
bsl::uint16_t IcmpRouterResponse::timeToLive() const
{
    return d_timeToLive;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpRouterResponse::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_infoVector);
    hashAppend(algorithm, d_timeToLive);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const IcmpRouterResponse& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpRouterResponse& lhs, const IcmpRouterResponse& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpRouterResponse& lhs, const IcmpRouterResponse& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IcmpRouterResponse& lhs, const IcmpRouterResponse& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&           algorithm,
                              const IcmpRouterResponse& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
