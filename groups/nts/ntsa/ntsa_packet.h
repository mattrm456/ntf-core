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

#ifndef INCLUDED_NTSA_PACKET
#define INCLUDED_NTSA_PACKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ethernetpacket.h>
#include <ntsa_icmppacket.h>
#include <ntsa_igmppacket.h>
#include <ntsa_ipv4packet.h>
#include <ntsa_ipv6packet.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntsa_packettype.h>
#include <ntsa_tcppacket.h>
#include <ntsa_udppacket.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a raw packet.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class Packet
{
    /// The packet type.
    ntsa::PacketType::Value d_type;

    union {
        /// The Ethernet packet.
        bsls::ObjectBuffer<ntsa::EthernetPacket> d_ethernet;

        /// The IPv4 packet.
        bsls::ObjectBuffer<ntsa::Ipv4Packet> d_ipv4;

        /// The IPv6 packet.
        bsls::ObjectBuffer<ntsa::Ipv6Packet> d_ipv6;

        /// The ICMP packet.
        bsls::ObjectBuffer<ntsa::IcmpPacket> d_icmp;

        /// The IGMP packet.
        bsls::ObjectBuffer<ntsa::IgmpPacket> d_igmp;

        /// The TCP packet.
        bsls::ObjectBuffer<ntsa::TcpPacket> d_tcp;

        /// The UDP packet.
        bsls::ObjectBuffer<ntsa::UdpPacket> d_udp;
    };

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new packet having a default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit Packet(bslma::Allocator* basicAllocator = 0);

    /// Create a new packet having the same value as the specified 'original'
    /// object. Assign an unspecified but valid value to the 'original'
    /// original.
    Packet(bslmf::MovableRef<Packet> original) NTSCFG_NOEXCEPT;

    /// Create a new packet having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    Packet(const Packet& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Packet();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Packet& operator=(bslmf::MovableRef<Packet> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Packet& operator=(const Packet& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Make the representation of the specified 'type'.
    void makeType(ntsa::PacketType::Value type);

    /// Make the representation of the payload an Ethernet packet. Return a
    /// reference to the modifable representation.
    ntsa::EthernetPacket& makeEthernet();

    /// Make the representation of the payload an Ethernet packet having the
    /// specified 'value'. Return a reference to the modifable representation.
    ntsa::EthernetPacket& makeEthernet(const ntsa::EthernetPacket& value);

    /// Make the representation of the payload an Ethernet packet having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    ntsa::EthernetPacket& makeEthernet(
        bslmf::MovableRef<ntsa::EthernetPacket> value) NTSCFG_NOEXCEPT;

    /// Make the representation of the payload an IPv4 packet. Return a
    /// reference to the modifable representation.
    ntsa::Ipv4Packet& makeIpv4();

    /// Make the representation of the payload an IPv4 packet having the
    /// specified 'value'. Return a reference to the modifable representation.
    ntsa::Ipv4Packet& makeIpv4(const ntsa::Ipv4Packet& value);

    /// Make the representation of the payload an IPv4 packet having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    ntsa::Ipv4Packet& makeIpv4(bslmf::MovableRef<ntsa::Ipv4Packet> value)
        NTSCFG_NOEXCEPT;

    /// Make the representation of the payload an IPv6 packet. Return a
    /// reference to the modifable representation.
    ntsa::Ipv6Packet& makeIpv6();

    /// Make the representation of the payload an IPv6 packet having the
    /// specified 'value'. Return a reference to the modifable representation.
    ntsa::Ipv6Packet& makeIpv6(const ntsa::Ipv6Packet& value);

    /// Make the representation of the payload an IPv6 packet having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    ntsa::Ipv6Packet& makeIpv6(bslmf::MovableRef<ntsa::Ipv6Packet> value)
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

    /// Return a reference to the modifiable Ethernet packet. The behavior is
    /// undefined unless 'isEthernet()' is true.
    ntsa::EthernetPacket& ethernet();

    /// Return a reference to the modifiable IPv4 packet. The behavior is
    /// undefined unless 'isIpv4()' is true.
    ntsa::Ipv4Packet& ipv4();

    /// Return a reference to the modifiable IPv6 packet. The behavior is
    /// undefined unless 'isIpv6()' is true.
    ntsa::Ipv6Packet& ipv6();

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

    /// Decode the packet from the specified 'buffer'. Return the error.
    /// Note that decoding will fail unless the packet unless the packet type
    /// is not undefined.
    ntsa::Error decode(ntsa::PacketDecoderContext*       context,
                       const bdlbb::BlobBuffer&          buffer,
                       const ntsa::PacketDecoderOptions& options);

    /// Encode the packet to the specified 'buffer'. Return the error. Note
    /// that decoding will fail unless the packet unless the packet type is not
    /// undefined.
    ntsa::Error encode(ntsa::PacketEncoderContext*       context,
                       bdlbb::BlobBuffer*                buffer,
                       const ntsa::PacketEncoderOptions& options) const;

    /// Return a reference to the non-modifiable Ethernet packet. The behavior
    /// is undefined unless 'isEthernet()' is true.
    const ntsa::EthernetPacket& ethernet() const;

    /// Return a reference to the non-modifiable IPv4 packet. The behavior is
    /// undefined unless 'isIpv4()' is true.
    const ntsa::Ipv4Packet& ipv4() const;

    /// Return a reference to the non-modifiable IPv6 packet. The behavior is
    /// undefined unless 'isIpv6()' is true.
    const ntsa::Ipv6Packet& ipv6() const;

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

    /// Return the packet type.
    ntsa::PacketType::Value type() const;

    /// Return the allocator.
    bslma::Allocator* allocator() const;

    /// Return true if the representation is not defined, otherwise return
    /// false.
    bool isUndefined() const;

    /// Return true if the representation is an Ethernet packet, otherwise
    /// return false.
    bool isEthernet() const;

    /// Return true if the representation is an IPv4 packet, otherwise return
    /// false.
    bool isIpv4() const;

    /// Return true if the representation is an IPv6 packet, otherwise return
    /// false.
    bool isIpv6() const;

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
    bool equals(const Packet& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Packet);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::Packet
bsl::ostream& operator<<(bsl::ostream& stream, const Packet& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Packet
bool operator==(const Packet& lhs, const Packet& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Packet
bool operator!=(const Packet& lhs, const Packet& rhs);

NTSCFG_INLINE
Packet::Packet(bslma::Allocator* basicAllocator)
: d_type(ntsa::PacketType::e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
ntsa::EthernetPacket& Packet::ethernet()
{
    BSLS_ASSERT(isEthernet());
    return d_ethernet.object();
}

NTSCFG_INLINE
ntsa::Ipv4Packet& Packet::ipv4()
{
    BSLS_ASSERT(isIpv4());
    return d_ipv4.object();
}

NTSCFG_INLINE
ntsa::Ipv6Packet& Packet::ipv6()
{
    BSLS_ASSERT(isIpv6());
    return d_ipv6.object();
}

NTSCFG_INLINE
ntsa::IcmpPacket& Packet::icmp()
{
    BSLS_ASSERT(isIcmp());
    return d_icmp.object();
}

NTSCFG_INLINE
ntsa::IgmpPacket& Packet::igmp()
{
    BSLS_ASSERT(isIgmp());
    return d_igmp.object();
}

NTSCFG_INLINE
ntsa::TcpPacket& Packet::tcp()
{
    BSLS_ASSERT(isTcp());
    return d_tcp.object();
}

NTSCFG_INLINE
ntsa::UdpPacket& Packet::udp()
{
    BSLS_ASSERT(isUdp());
    return d_udp.object();
}

NTSCFG_INLINE
const ntsa::EthernetPacket& Packet::ethernet() const
{
    BSLS_ASSERT(isEthernet());
    return d_ethernet.object();
}

NTSCFG_INLINE
const ntsa::Ipv4Packet& Packet::ipv4() const
{
    BSLS_ASSERT(isIpv4());
    return d_ipv4.object();
}

NTSCFG_INLINE
const ntsa::Ipv6Packet& Packet::ipv6() const
{
    BSLS_ASSERT(isIpv6());
    return d_ipv6.object();
}

NTSCFG_INLINE
const ntsa::IcmpPacket& Packet::icmp() const
{
    BSLS_ASSERT(isIcmp());
    return d_icmp.object();
}

NTSCFG_INLINE
const ntsa::IgmpPacket& Packet::igmp() const
{
    BSLS_ASSERT(isIgmp());
    return d_igmp.object();
}

NTSCFG_INLINE
const ntsa::TcpPacket& Packet::tcp() const
{
    BSLS_ASSERT(isTcp());
    return d_tcp.object();
}

NTSCFG_INLINE
const ntsa::UdpPacket& Packet::udp() const
{
    BSLS_ASSERT(isUdp());
    return d_udp.object();
}

NTSCFG_INLINE
ntsa::PacketType::Value Packet::type() const
{
    return d_type;
}

NTSCFG_INLINE
bslma::Allocator* Packet::allocator() const
{
    return d_allocator_p;
}

NTSCFG_INLINE
bool Packet::isUndefined() const
{
    return d_type == ntsa::PacketType::e_UNDEFINED;
}

NTSCFG_INLINE
bool Packet::isEthernet() const
{
    return d_type == ntsa::PacketType::e_ETHERNET;
}

NTSCFG_INLINE
bool Packet::isIpv4() const
{
    return d_type == ntsa::PacketType::e_IPV4;
}

NTSCFG_INLINE
bool Packet::isIpv6() const
{
    return d_type == ntsa::PacketType::e_IPV6;
}

NTSCFG_INLINE
bool Packet::isIcmp() const
{
    return d_type == ntsa::PacketType::e_ICMP;
}

NTSCFG_INLINE
bool Packet::isIgmp() const
{
    return d_type == ntsa::PacketType::e_IGMP;
}

NTSCFG_INLINE
bool Packet::isTcp() const
{
    return d_type == ntsa::PacketType::e_TCP;
}

NTSCFG_INLINE
bool Packet::isUdp() const
{
    return d_type == ntsa::PacketType::e_UDP;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
