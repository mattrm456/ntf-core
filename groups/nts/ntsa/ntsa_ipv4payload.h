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

#ifndef INCLUDED_NTSA_IPV4PAYLOAD
#define INCLUDED_NTSA_IPV4PAYLOAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_icmppacket.h>
#include <ntsa_igmppacket.h>
#include <ntsa_tcppacket.h>
#include <ntsa_udppacket.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a Internet Protocol version 4 (IPv4) payload.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class Ipv4Payload
{
    /// Enumerates the representations of the payload.
    enum Type {
        /// The payload type is not defined.
        e_UNDEFINED = 0,

        /// The payload type is represented as a raw sequence of bytes.
        e_RAW = 255,

        /// The payload type is an ICMP packet.
        e_ICMP = 1,

        /// The payload type is an IGMP packet.
        e_IGMP = 2,

        /// The payload type is a TCP packet.
        e_TCP = 6,

        /// The payload type is a UDP packet.
        e_UDP = 17
    };

    Type d_type;

    union {
        bsls::ObjectBuffer<bdlbb::BlobBuffer> d_raw;
        bsls::ObjectBuffer<ntsa::IcmpPacket>  d_icmp;
        bsls::ObjectBuffer<ntsa::IgmpPacket>  d_igmp;
        bsls::ObjectBuffer<ntsa::TcpPacket>   d_tcp;
        bsls::ObjectBuffer<ntsa::UdpPacket>   d_udp;
    };

    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new IPv4 payload having a default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit Ipv4Payload(bslma::Allocator* basicAllocator = 0);

    /// Create a new IPv4 payload having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    Ipv4Payload(bslmf::MovableRef<Ipv4Payload> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv4 payload having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Ipv4Payload(const Ipv4Payload& original,
                bslma::Allocator*  basicAllocator = 0);

    /// Destroy this object.
    ~Ipv4Payload();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Ipv4Payload& operator=(bslmf::MovableRef<Ipv4Payload> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv4Payload& operator=(const Ipv4Payload& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Make the representation of the payload a raw sequence of bytes. Return
    /// a reference to the modifable representation.
    bdlbb::BlobBuffer& makeRaw();

    /// Make the representation of the payload a raw sequence of bytes having
    /// the specified 'value'. Return a reference to the modifable
    /// representation.
    bdlbb::BlobBuffer& makeRaw(const bdlbb::BlobBuffer& value);

    /// Make the representation of the payload a raw sequence of bytes having
    /// the specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    bdlbb::BlobBuffer& makeRaw(bslmf::MovableRef<bdlbb::BlobBuffer> value)
        NTSCFG_NOEXCEPT;

    /// Make the representation of the payload an ICMP packet. Return a
    /// reference to the modifable representation.
    ntsa::IcmpPacket& makeIcmp();

    /// Make the representation of the payload an ICMP packet having the
    /// specified 'value'. Return a reference to the modifable representation.
    ntsa::IcmpPacket& makeIcmp(const ntsa::IcmpPacket& value);

    /// Make the representation of the payload an ICMP packet having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    ntsa::IcmpPacket& makeIcmp(bslmf::MovableRef<ntsa::IcmpPacket> value)
        NTSCFG_NOEXCEPT;

    /// Make the representation of the payload an IGMP packet. Return a
    /// reference to the modifable representation.
    ntsa::IgmpPacket& makeIgmp();

    /// Make the representation of the payload an IGMP packet having the
    /// specified 'value'. Return a reference to the modifable representation.
    ntsa::IgmpPacket& makeIgmp(const ntsa::IgmpPacket& value);

    /// Make the representation of the payload an IGMP packet having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    ntsa::IgmpPacket& makeIgmp(bslmf::MovableRef<ntsa::IgmpPacket> value)
        NTSCFG_NOEXCEPT;

    /// Make the representation of the payload a TCP packet. Return a reference
    /// to the modifable representation.
    ntsa::TcpPacket& makeTcp();

    /// Make the representation of the payload a TCP packet having the
    /// specified 'value'. Return a reference to the modifable representation.
    ntsa::TcpPacket& makeTcp(const ntsa::TcpPacket& value);

    /// Make the representation of the payload a TCP packet having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    ntsa::TcpPacket& makeTcp(bslmf::MovableRef<ntsa::TcpPacket> value)
        NTSCFG_NOEXCEPT;

    /// Make the representation of the payload a UDP packet. Return a reference
    /// to the modifable representation.
    ntsa::UdpPacket& makeUdp();

    /// Make the representation of the payload a UDP packet having the
    /// specified 'value'. Return a reference to the modifable representation.
    ntsa::UdpPacket& makeUdp(const ntsa::UdpPacket& value);

    /// Make the representation of the payload a UDP packet having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    ntsa::UdpPacket& makeUdp(bslmf::MovableRef<ntsa::UdpPacket> value)
        NTSCFG_NOEXCEPT;

    /// Return a reference to the modifiable raw sequence of bytes. The
    /// behavior is undefined unless 'isRaw()' is true.
    bdlbb::BlobBuffer& raw();

    /// Return a reference to the modifiable ICMP packet. The behavior is
    /// undefined unless 'isIcmp()' is true.
    ntsa::IcmpPacket& icmp();

    /// Return a reference to the modifiable IGMP packet. The behavior is
    /// undefined unless 'isIgmp()' is true.
    ntsa::IgmpPacket& igmp();

    /// Return a reference to the modifiable TCP packet. The behavior is
    /// undefined unless 'isTcp()' is true.
    ntsa::TcpPacket& tcp();

    /// Return a reference to the modifiable UDP packet. The behavior is
    /// undefined unless 'isUdp()' is true.
    ntsa::UdpPacket& udp();

    /// Return a reference to the non-modifiable raw sequence of bytes. The
    /// behavior is undefined unless 'isRaw()' is true.
    const bdlbb::BlobBuffer& raw() const;

    /// Return a reference to the non-modifiable ICMP packet. The behavior is
    /// undefined unless 'isIcmp()' is true.
    const ntsa::IcmpPacket& icmp() const;

    /// Return a reference to the non-modifiable IGMP packet. The behavior is
    /// undefined unless 'isIgmp()' is true.
    const ntsa::IgmpPacket& igmp() const;

    /// Return a reference to the non-modifiable TCP packet. The behavior is
    /// undefined unless 'isTcp()' is true.
    const ntsa::TcpPacket& tcp() const;

    /// Return a reference to the non-modifiable UDP packet. The behavior is
    /// undefined unless 'isUdp()' is true.
    const ntsa::UdpPacket& udp() const;

    /// Return the allocator.
    bslma::Allocator* allocator() const;

    /// Return true if the representation is not defined, otherwise return
    /// false.
    bool isUndefined() const;

    /// Return true if the representation is a raw sequence of bytes, otherwise
    /// return false.
    bool isRaw() const;

    /// Return true if the representation is an ICMP packet, otherwise return
    /// false.
    bool isIcmp() const;

    /// Return true if the representation is an IGMP packet, otherwise return
    /// false.
    bool isIgmp() const;

    /// Return true if the representation is a TCP packet, otherwise return
    /// false.
    bool isTcp() const;

    /// Return true if the representation is a UDP packet, otherwise return
    /// false.
    bool isUdp() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Ipv4Payload& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Ipv4Payload);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::Ipv4Payload
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Payload& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Payload
bool operator==(const Ipv4Payload& lhs, const Ipv4Payload& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Payload
bool operator!=(const Ipv4Payload& lhs, const Ipv4Payload& rhs);

NTSCFG_INLINE
Ipv4Payload::Ipv4Payload(bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
bdlbb::BlobBuffer& Ipv4Payload::raw()
{
    BSLS_ASSERT(isRaw());
    return d_raw.object();
}

NTSCFG_INLINE
ntsa::IcmpPacket& Ipv4Payload::icmp()
{
    BSLS_ASSERT(isIcmp());
    return d_icmp.object();
}

NTSCFG_INLINE
ntsa::IgmpPacket& Ipv4Payload::igmp()
{
    BSLS_ASSERT(isIgmp());
    return d_igmp.object();
}

NTSCFG_INLINE
ntsa::TcpPacket& Ipv4Payload::tcp()
{
    BSLS_ASSERT(isTcp());
    return d_tcp.object();
}

NTSCFG_INLINE
ntsa::UdpPacket& Ipv4Payload::udp()
{
    BSLS_ASSERT(isUdp());
    return d_udp.object();
}

NTSCFG_INLINE
const bdlbb::BlobBuffer& Ipv4Payload::raw() const
{
    BSLS_ASSERT(isRaw());
    return d_raw.object();
}

NTSCFG_INLINE
const ntsa::IcmpPacket& Ipv4Payload::icmp() const
{
    BSLS_ASSERT(isIcmp());
    return d_icmp.object();
}

NTSCFG_INLINE
const ntsa::IgmpPacket& Ipv4Payload::igmp() const
{
    BSLS_ASSERT(isIgmp());
    return d_igmp.object();
}

NTSCFG_INLINE
const ntsa::TcpPacket& Ipv4Payload::tcp() const
{
    BSLS_ASSERT(isTcp());
    return d_tcp.object();
}

NTSCFG_INLINE
const ntsa::UdpPacket& Ipv4Payload::udp() const
{
    BSLS_ASSERT(isUdp());
    return d_udp.object();
}

NTSCFG_INLINE
bslma::Allocator* Ipv4Payload::allocator() const
{
    return d_allocator_p;
}

NTSCFG_INLINE
bool Ipv4Payload::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

NTSCFG_INLINE
bool Ipv4Payload::isRaw() const
{
    return d_type == e_RAW;
}

NTSCFG_INLINE
bool Ipv4Payload::isIcmp() const
{
    return d_type == e_ICMP;
}

NTSCFG_INLINE
bool Ipv4Payload::isIgmp() const
{
    return d_type == e_IGMP;
}

NTSCFG_INLINE
bool Ipv4Payload::isTcp() const
{
    return d_type == e_TCP;
}

NTSCFG_INLINE
bool Ipv4Payload::isUdp() const
{
    return d_type == e_UDP;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
