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

#ifndef INCLUDED_NTSA_IGMPPACKET
#define INCLUDED_NTSA_IGMPPACKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_igmpheader.h>
#include <ntsa_igmppayload.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Control Message Protocol (IGMP) packet.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IgmpPacket
{
    ntsa::IgmpHeader  d_header;
    ntsa::IgmpPayload d_payload;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new IGMP packet having a default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit IgmpPacket(bslma::Allocator* basicAllocator = 0);

    /// Create a new IGMP packet having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IgmpPacket(bslmf::MovableRef<IgmpPacket> original) NTSCFG_NOEXCEPT;

    /// Create a new IGMP packet having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    IgmpPacket(const IgmpPacket& original,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~IgmpPacket();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IgmpPacket& operator=(bslmf::MovableRef<IgmpPacket> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    IgmpPacket& operator=(const IgmpPacket& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the header to the specified 'value'.
    void setHeader(const ntsa::IgmpHeader& value);

    /// Set the payload to the specified 'value'.
    void setPayload(const ntsa::IgmpPayload& value);

    /// Return a reference to the modifiable header.
    ntsa::IgmpHeader& header();

    /// Return a reference to the modifiable payload.
    ntsa::IgmpPayload& payload();

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return a reference to the non-modifiable header.
    const ntsa::IgmpHeader& header() const;

    /// Return a reference to the non-modifiable payload.
    const ntsa::IgmpPayload& payload() const;

    /// Return the allocator.
    bslma::Allocator* allocator() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const IgmpPacket& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(IgmpPacket);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::IgmpPacket
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpPacket& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IgmpPacket
bool operator==(const IgmpPacket& lhs, const IgmpPacket& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IgmpPacket
bool operator!=(const IgmpPacket& lhs, const IgmpPacket& rhs);

NTSCFG_INLINE
IgmpPacket::IgmpPacket(bslma::Allocator* basicAllocator)
: d_header()
, d_payload(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
IgmpPacket::IgmpPacket(bslmf::MovableRef<IgmpPacket> original) NTSCFG_NOEXCEPT
: d_header(NTSCFG_MOVE_FROM(original, d_header)),
  d_payload(NTSCFG_MOVE_FROM(original, d_payload)),
  d_allocator_p(NTSCFG_MOVE_FROM(original, d_allocator_p))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IgmpPacket::IgmpPacket(const IgmpPacket& original,
                       bslma::Allocator* basicAllocator)
: d_header(original.d_header)
, d_payload(original.d_payload, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
IgmpPacket::~IgmpPacket()
{
}

NTSCFG_INLINE
IgmpPacket& IgmpPacket::operator=(bslmf::MovableRef<IgmpPacket> other)
    NTSCFG_NOEXCEPT
{
    d_header      = NTSCFG_MOVE_FROM(other, d_header);
    d_payload     = NTSCFG_MOVE_FROM(other, d_payload);
    d_allocator_p = NTSCFG_MOVE_FROM(other, d_allocator_p);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IgmpPacket& IgmpPacket::operator=(const IgmpPacket& other)
{
    d_header  = other.d_header;
    d_payload = other.d_payload;
    return *this;
}

NTSCFG_INLINE
void IgmpPacket::reset()
{
    d_header.reset();
    d_payload.reset();
}

NTSCFG_INLINE
void IgmpPacket::setHeader(const ntsa::IgmpHeader& value)
{
    d_header = value;
}

NTSCFG_INLINE
void IgmpPacket::setPayload(const ntsa::IgmpPayload& value)
{
    d_payload = value;
}

NTSCFG_INLINE
ntsa::IgmpHeader& IgmpPacket::header()
{
    return d_header;
}

NTSCFG_INLINE
ntsa::IgmpPayload& IgmpPacket::payload()
{
    return d_payload;
}

NTSCFG_INLINE
const ntsa::IgmpHeader& IgmpPacket::header() const
{
    return d_header;
}

NTSCFG_INLINE
const ntsa::IgmpPayload& IgmpPacket::payload() const
{
    return d_payload;
}

NTSCFG_INLINE
bslma::Allocator* IgmpPacket::allocator() const
{
    return d_allocator_p;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpPacket& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IgmpPacket& lhs, const IgmpPacket& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IgmpPacket& lhs, const IgmpPacket& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
#endif
