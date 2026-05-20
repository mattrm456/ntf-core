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

#ifndef INCLUDED_NTSA_ETHERNETPACKET
#define INCLUDED_NTSA_ETHERNETPACKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ethernetheader.h>
#include <ntsa_ethernetpayload.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a Ethernet packet.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class EthernetPacket
{
    ntsa::EthernetHeader  d_header;
    ntsa::EthernetPayload d_payload;

  public:
    /// Create a new Ethernet packet having a default value. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EthernetPacket(bslma::Allocator* basicAllocator = 0);

    /// Create a new Ethernet packet having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    EthernetPacket(bslmf::MovableRef<EthernetPacket> original) NTSCFG_NOEXCEPT;

    /// Create a new Ethernet packet having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    EthernetPacket(const EthernetPacket& original,
                   bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~EthernetPacket();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    EthernetPacket& operator=(bslmf::MovableRef<EthernetPacket> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    EthernetPacket& operator=(const EthernetPacket& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the header to the specified 'value'.
    void setHeader(const ntsa::EthernetHeader& value);

    /// Set the payload to the specified 'value'.
    void setPayload(const ntsa::EthernetPayload& value);

    /// Return a reference to the modifiable header.
    ntsa::EthernetHeader& header();

    /// Return a reference to the modifiable payload.
    ntsa::EthernetPayload& payload();

    /// Decode the object from the specified 'decoder' according to the
    /// specified 'options'. Populate the specified 'context' with the
    /// consequences of encoding the packet. Return the error.
    ntsa::Error decode(ntsa::PacketDecoderContext*       context,
                       ntsa::PacketDecoder*              decoder,
                       const ntsa::PacketDecoderOptions& options);

    /// Encode the object through the specified 'encoder' according to the
    /// specified 'options'. Populate the specified 'context' with consequences
    /// of decoding the packet. Return the error.
    ntsa::Error encode(ntsa::PacketEncoderContext*       context,
                       ntsa::PacketEncoder*              encoder,
                       const ntsa::PacketEncoderOptions& options) const;

    /// Return a reference to the non-modifiable header.
    const ntsa::EthernetHeader& header() const;

    /// Return a reference to the non-modifiable payload.
    const ntsa::EthernetPayload& payload() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EthernetPacket& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(EthernetPacket);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::EthernetPacket
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetPacket& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::EthernetPacket
bool operator==(const EthernetPacket& lhs, const EthernetPacket& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::EthernetPacket
bool operator!=(const EthernetPacket& lhs, const EthernetPacket& rhs);

NTSCFG_INLINE
EthernetPacket::EthernetPacket(bslma::Allocator* basicAllocator)
: d_header()
, d_payload(basicAllocator)
{
}

NTSCFG_INLINE
EthernetPacket::EthernetPacket(bslmf::MovableRef<EthernetPacket> original)
    NTSCFG_NOEXCEPT : d_header(NTSCFG_MOVE_FROM(original, d_header)),
                      d_payload(NTSCFG_MOVE_FROM(original, d_payload))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
EthernetPacket::EthernetPacket(const EthernetPacket& original,
                               bslma::Allocator*     basicAllocator)
: d_header(original.d_header)
, d_payload(original.d_payload, basicAllocator)
{
}

NTSCFG_INLINE
EthernetPacket::~EthernetPacket()
{
}

NTSCFG_INLINE
EthernetPacket& EthernetPacket::operator=(
    bslmf::MovableRef<EthernetPacket> other) NTSCFG_NOEXCEPT
{
    d_header  = NTSCFG_MOVE_FROM(other, d_header);
    d_payload = NTSCFG_MOVE_FROM(other, d_payload);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
EthernetPacket& EthernetPacket::operator=(const EthernetPacket& other)
{
    d_header  = other.d_header;
    d_payload = other.d_payload;
    return *this;
}

NTSCFG_INLINE
void EthernetPacket::reset()
{
    d_header.reset();
    d_payload.reset();
}

NTSCFG_INLINE
void EthernetPacket::setHeader(const ntsa::EthernetHeader& value)
{
    d_header = value;
}

NTSCFG_INLINE
void EthernetPacket::setPayload(const ntsa::EthernetPayload& value)
{
    d_payload = value;
}

NTSCFG_INLINE
ntsa::EthernetHeader& EthernetPacket::header()
{
    return d_header;
}

NTSCFG_INLINE
ntsa::EthernetPayload& EthernetPacket::payload()
{
    return d_payload;
}

NTSCFG_INLINE
const ntsa::EthernetHeader& EthernetPacket::header() const
{
    return d_header;
}

NTSCFG_INLINE
const ntsa::EthernetPayload& EthernetPacket::payload() const
{
    return d_payload;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetPacket& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const EthernetPacket& lhs, const EthernetPacket& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const EthernetPacket& lhs, const EthernetPacket& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
#endif
