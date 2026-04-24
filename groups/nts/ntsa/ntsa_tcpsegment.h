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

#ifndef INCLUDED_NTSA_TCPSEGMENT
#define INCLUDED_NTSA_TCPSEGMENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_tcpheader.h>
#include <ntsa_tcppayload.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a Transmission Control Protocol (TCP) segment.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class TcpSegment
{
    ntsa::TcpHeader  d_header;
    ntsa::TcpPayload d_payload;

  public:
    /// Create a new TCP segment having a default value.
    TcpSegment();

    /// Create a new TCP segment having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    TcpSegment(bslmf::MovableRef<TcpSegment> original) NTSCFG_NOEXCEPT;

    /// Create a new TCP segment having the same value as the specified
    /// 'original' object.
    TcpSegment(const TcpSegment& original);

    /// Destroy this object.
    ~TcpSegment();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    TcpSegment& operator=(bslmf::MovableRef<TcpSegment> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    TcpSegment& operator=(const TcpSegment& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the header to the specified 'value'.
    void setHeader(const ntsa::TcpHeader& value);

    /// Set the payload to the specified 'value'.
    void setPayload(const ntsa::TcpPayload& value);

    /// Return a reference to the modifiable header.
    ntsa::TcpHeader& header();

    /// Return a reference to the modifiable payload.
    ntsa::TcpPayload& payload();

    /// Decode the packet from the specified 'source'. Return the error.
    ntsa::Error decode(const bdlbb::BlobBuffer& source);

    /// Encode the packet to the specified 'destination'. Return the error. 
    ntsa::Error encode(bdlbb::BlobBuffer* destination) const;

    /// Return a reference to the non-modifiable header.
    const ntsa::TcpHeader& header() const;

    /// Return a reference to the non-modifiable payload.
    const ntsa::TcpPayload& payload() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const TcpSegment& other) const;

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

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(TcpSegment);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::TcpSegment
bsl::ostream& operator<<(bsl::ostream& stream, const TcpSegment& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::TcpSegment
bool operator==(const TcpSegment& lhs, const TcpSegment& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::TcpSegment
bool operator!=(const TcpSegment& lhs, const TcpSegment& rhs);

NTSCFG_INLINE
TcpSegment::TcpSegment()
: d_header()
, d_payload()
{
}

NTSCFG_INLINE
TcpSegment::TcpSegment(bslmf::MovableRef<TcpSegment> original) NTSCFG_NOEXCEPT
: d_header(NTSCFG_MOVE_FROM(original, d_header)),
  d_payload(NTSCFG_MOVE_FROM(original, d_payload))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
TcpSegment::TcpSegment(const TcpSegment& original)
: d_header(original.d_header)
, d_payload(original.d_payload)
{
}

NTSCFG_INLINE
TcpSegment::~TcpSegment()
{
}

NTSCFG_INLINE
TcpSegment& TcpSegment::operator=(bslmf::MovableRef<TcpSegment> other)
    NTSCFG_NOEXCEPT
{
    d_header  = NTSCFG_MOVE_FROM(other, d_header);
    d_payload = NTSCFG_MOVE_FROM(other, d_payload);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
TcpSegment& TcpSegment::operator=(const TcpSegment& other)
{
    d_header  = other.d_header;
    d_payload = other.d_payload;
    return *this;
}

NTSCFG_INLINE
void TcpSegment::reset()
{
    d_header.reset();
    d_payload.reset();
}

NTSCFG_INLINE
void TcpSegment::setHeader(const ntsa::TcpHeader& value)
{
    d_header = value;
}

NTSCFG_INLINE
void TcpSegment::setPayload(const ntsa::TcpPayload& value)
{
    d_payload = value;
}

NTSCFG_INLINE
ntsa::TcpHeader& TcpSegment::header()
{
    return d_header;
}

NTSCFG_INLINE
ntsa::TcpPayload& TcpSegment::payload()
{
    return d_payload;
}

NTSCFG_INLINE
const ntsa::TcpHeader& TcpSegment::header() const
{
    return d_header;
}

NTSCFG_INLINE
const ntsa::TcpPayload& TcpSegment::payload() const
{
    return d_payload;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const TcpSegment& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const TcpSegment& lhs, const TcpSegment& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const TcpSegment& lhs, const TcpSegment& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
#endif
