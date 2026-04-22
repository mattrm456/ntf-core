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

#ifndef INCLUDED_NTSA_IPV6PAYLOAD
#define INCLUDED_NTSA_IPV6PAYLOAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_tcpsegment.h>
#include <ntsa_udpdatagram.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>

#include <bdlbb_blob.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a Internet Protocol version 6 (IPv6) payload.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class Ipv6Payload
{
    /// Enumerates the representations of the payload.
    enum Type {
        /// The payload type is not defined.
        e_UNDEFINED = 0,

        /// The payload type is represented as a raw sequence of bytes.
        e_RAW = 1,

        /// The payload type is a TCP segment.
        e_TCP = 2,

        /// The payload type is a UDP datagram.
        e_UDP = 3
    };

    Type d_type;

    union {
        bsls::ObjectBuffer<bdlbb::BlobBuffer>  d_raw;
        bsls::ObjectBuffer<ntsa::TcpSegment>   d_tcp;
        bsls::ObjectBuffer<ntsa::UdpDatagram>  d_udp;
    };

public:
    /// Create a new IPv6 payload having a default value.
    Ipv6Payload();

    /// Create a new IPv6 payload having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    Ipv6Payload(bslmf::MovableRef<Ipv6Payload> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv6 payload having the same value as the specified
    /// 'original' object.
    Ipv6Payload(const Ipv6Payload& original);

    /// Destroy this object.
    ~Ipv6Payload();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Ipv6Payload& operator=(bslmf::MovableRef<Ipv6Payload> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv6Payload& operator=(const Ipv6Payload& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Make the representation of the payload a raw sequence of bytes. Return
    /// a reference to the modifable respresentation.
    bdlbb::BlobBuffer& makeRaw();

    /// Make the representation of the payload a raw sequence of bytes having
    /// the specified 'value'. Return a reference to the modifable
    /// respresentation.
    bdlbb::BlobBuffer& makeRaw(const bdlbb::BlobBuffer& value);

    /// Make the representation of the payload a raw sequence of bytes having
    /// the specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable respresentation.
    bdlbb::BlobBuffer& makeRaw(bslmf::MovableRef<bdlbb::BlobBuffer> value) 
        NTSCFG_NOEXCEPT;

    /// Make the representation of the payload a TCP segment. Return a
    /// reference to the modifable respresentation.
    ntsa::TcpSegment& makeTcp();

    /// Make the representation of the payload a TCP segment having the
    /// specified 'value'. Return a reference to the modifable respresentation.
    ntsa::TcpSegment& makeTcp(const ntsa::TcpSegment& value);

    /// Make the representation of the payload a TCP segment having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable respresentation.
    ntsa::TcpSegment& makeTcp(bslmf::MovableRef<ntsa::TcpSegment> value) 
        NTSCFG_NOEXCEPT;

    /// Make the representation of the payload a UDP datagram. Return a
    /// reference to the modifable respresentation.
    ntsa::UdpDatagram& makeUdp();

    /// Make the representation of the payload a UDP datagram having the
    /// specified 'value'. Return a reference to the modifable respresentation.
    ntsa::UdpDatagram& makeUdp(const ntsa::UdpDatagram& value);

    /// Make the representation of the payload a UDP datagram having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable respresentation.
    ntsa::UdpDatagram& makeUdp(bslmf::MovableRef<ntsa::UdpDatagram> value) 
        NTSCFG_NOEXCEPT;

    /// Return a reference to the modifiable raw sequence of bytes. The
    /// behavior is undefined unless 'isRaw()' is true. 
    bdlbb::BlobBuffer& raw();

    /// Return a reference to the modifiable TCP segment. The behavior is
    /// undefined unless 'isTcp()' is true. 
    ntsa::TcpSegment& tcp();

    /// Return a reference to the modifiable UDP datagram. The behavior is
    /// undefined unless 'isUdp()' is true. 
    ntsa::UdpDatagram& udp();

    /// Return a reference to the non-modifiable raw sequence of bytes. The
    /// behavior is undefined unless 'isRaw()' is true. 
    const bdlbb::BlobBuffer& raw() const;

    /// Return a reference to the non-modifiable TCP segment. The behavior is
    /// undefined unless 'isTcp()' is true. 
    const ntsa::TcpSegment& tcp() const;

    /// Return a reference to the non-modifiable UDP datagram. The behavior is
    /// undefined unless 'isUdp()' is true. 
    const ntsa::UdpDatagram& udp() const;

    /// Return true if the representation is not defined, otherwise return
    /// false. 
    bool isUndefined() const;

    /// Return true if the representation is a raw sequence of bytes, otherwise
    /// return false. 
    bool isRaw() const;

    /// Return true if the representation is a TCP segment, otherwise return
    /// false. 
    bool isTcp() const;

    /// Return true if the representation is a UDP datagram, otherwise return
    /// false. 
    bool isUdp() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Ipv6Payload& other) const;

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
                        int           spacesPerLevel = 6) const;
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::Ipv6Payload
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Payload& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Payload
bool operator==(const Ipv6Payload& lhs, const Ipv6Payload& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Payload
bool operator!=(const Ipv6Payload& lhs, const Ipv6Payload& rhs);

NTSCFG_INLINE
Ipv6Payload::Ipv6Payload()
: d_type(e_UNDEFINED)
{
}

NTSCFG_INLINE
bdlbb::BlobBuffer& Ipv6Payload::raw()
{
    BSLS_ASSERT(isRaw());
    return d_raw.object();
}

NTSCFG_INLINE
ntsa::TcpSegment& Ipv6Payload::tcp()
{
    BSLS_ASSERT(isTcp());
    return d_tcp.object();
}

NTSCFG_INLINE
ntsa::UdpDatagram& Ipv6Payload::udp()
{
    BSLS_ASSERT(isUdp());
    return d_udp.object();
}

NTSCFG_INLINE
const bdlbb::BlobBuffer& Ipv6Payload::raw() const
{
    BSLS_ASSERT(isRaw());
    return d_raw.object();
}

NTSCFG_INLINE
const ntsa::TcpSegment& Ipv6Payload::tcp() const
{
    BSLS_ASSERT(isTcp());
    return d_tcp.object();
}

NTSCFG_INLINE
const ntsa::UdpDatagram& Ipv6Payload::udp() const
{
    BSLS_ASSERT(isUdp());
    return d_udp.object();
}

NTSCFG_INLINE
bool Ipv6Payload::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

NTSCFG_INLINE
bool Ipv6Payload::isRaw() const
{
    return d_type == e_RAW;
}

NTSCFG_INLINE
bool Ipv6Payload::isTcp() const
{
    return d_type == e_TCP;
}

NTSCFG_INLINE
bool Ipv6Payload::isUdp() const
{
    return d_type == e_UDP;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
