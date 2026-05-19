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

#include <ntsa_packet.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_packet_cpp, "$Id$ $CSID$")

#include <bdlb_print.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntsa {

Packet::Packet(bslmf::MovableRef<Packet> original) NTSCFG_NOEXCEPT
: d_type(NTSCFG_MOVE_FROM(original, d_type)),
  d_allocator_p(NTSCFG_MOVE_FROM(original, d_allocator_p))
{
    if (d_type == ntsa::PacketType::e_ETHERNET) {
        new (d_ethernet.buffer()) ntsa::EthernetPacket(
            NTSCFG_MOVE_FROM(original, d_ethernet.object()));
    }
    else if (d_type == ntsa::PacketType::e_IPV4) {
        new (d_ipv4.buffer())
            ntsa::Ipv4Packet(NTSCFG_MOVE_FROM(original, d_ipv4.object()));
    }
    else if (d_type == ntsa::PacketType::e_IPV6) {
        new (d_ipv6.buffer())
            ntsa::Ipv6Packet(NTSCFG_MOVE_FROM(original, d_ipv6.object()));
    }
    else if (d_type == ntsa::PacketType::e_ICMP) {
        new (d_icmp.buffer())
            ntsa::IcmpPacket(NTSCFG_MOVE_FROM(original, d_icmp.object()));
    }
    else if (d_type == ntsa::PacketType::e_IGMP) {
        new (d_igmp.buffer())
            ntsa::IgmpPacket(NTSCFG_MOVE_FROM(original, d_igmp.object()));
    }
    else if (d_type == ntsa::PacketType::e_TCP) {
        new (d_tcp.buffer())
            ntsa::TcpPacket(NTSCFG_MOVE_FROM(original, d_tcp.object()));
    }
    else if (d_type == ntsa::PacketType::e_UDP) {
        new (d_udp.buffer())
            ntsa::UdpPacket(NTSCFG_MOVE_FROM(original, d_udp.object()));
    }
    else {
        BSLS_ASSERT(d_type == ntsa::PacketType::e_UNDEFINED);
    }

    NTSCFG_MOVE_RESET(original);
}

Packet::Packet(const Packet& original, bslma::Allocator* basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == ntsa::PacketType::e_ETHERNET) {
        new (d_ethernet.buffer())
            ntsa::EthernetPacket(original.d_ethernet.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_IPV4) {
        new (d_ipv4.buffer())
            ntsa::Ipv4Packet(original.d_ipv4.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_IPV6) {
        new (d_ipv6.buffer())
            ntsa::Ipv6Packet(original.d_ipv6.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_ICMP) {
        new (d_icmp.buffer())
            ntsa::IcmpPacket(original.d_icmp.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_IGMP) {
        new (d_igmp.buffer())
            ntsa::IgmpPacket(original.d_igmp.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_TCP) {
        new (d_tcp.buffer())
            ntsa::TcpPacket(original.d_tcp.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_UDP) {
        new (d_udp.buffer())
            ntsa::UdpPacket(original.d_udp.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == ntsa::PacketType::e_UNDEFINED);
    }
}

Packet::~Packet()
{
    if (d_type == ntsa::PacketType::e_ETHERNET) {
        typedef ntsa::EthernetPacket Type;
        d_ethernet.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_IPV4) {
        typedef ntsa::Ipv4Packet Type;
        d_ipv4.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_IPV6) {
        typedef ntsa::Ipv6Packet Type;
        d_ipv6.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_ICMP) {
        typedef ntsa::IcmpPacket Type;
        d_icmp.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_IGMP) {
        typedef ntsa::IgmpPacket Type;
        d_igmp.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_TCP) {
        typedef ntsa::TcpPacket Type;
        d_tcp.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_UDP) {
        typedef ntsa::UdpPacket Type;
        d_udp.object().~Type();
    }
}

Packet& Packet::operator=(bslmf::MovableRef<Packet> other) NTSCFG_NOEXCEPT
{
    reset();

    d_type        = NTSCFG_MOVE_FROM(other, d_type);
    d_allocator_p = NTSCFG_MOVE_FROM(other, d_allocator_p);

    if (d_type == ntsa::PacketType::e_ETHERNET) {
        new (d_ethernet.buffer())
            ntsa::EthernetPacket(NTSCFG_MOVE_FROM(other, d_ethernet.object()));
    }
    else if (d_type == ntsa::PacketType::e_IPV4) {
        new (d_ipv4.buffer())
            ntsa::Ipv4Packet(NTSCFG_MOVE_FROM(other, d_ipv4.object()));
    }
    else if (d_type == ntsa::PacketType::e_IPV6) {
        new (d_ipv6.buffer())
            ntsa::Ipv6Packet(NTSCFG_MOVE_FROM(other, d_ipv6.object()));
    }
    else if (d_type == ntsa::PacketType::e_ICMP) {
        new (d_icmp.buffer())
            ntsa::IcmpPacket(NTSCFG_MOVE_FROM(other, d_icmp.object()));
    }
    else if (d_type == ntsa::PacketType::e_IGMP) {
        new (d_igmp.buffer())
            ntsa::IgmpPacket(NTSCFG_MOVE_FROM(other, d_igmp.object()));
    }
    else if (d_type == ntsa::PacketType::e_TCP) {
        new (d_tcp.buffer())
            ntsa::TcpPacket(NTSCFG_MOVE_FROM(other, d_tcp.object()));
    }
    else if (d_type == ntsa::PacketType::e_UDP) {
        new (d_udp.buffer())
            ntsa::UdpPacket(NTSCFG_MOVE_FROM(other, d_udp.object()));
    }
    else {
        BSLS_ASSERT(d_type == ntsa::PacketType::e_UNDEFINED);
    }

    NTSCFG_MOVE_RESET(other);

    return *this;
}

Packet& Packet::operator=(const Packet& other)
{
    if (this == &other) {
        return *this;
    }

    reset();

    d_type = other.d_type;

    if (d_type == ntsa::PacketType::e_ETHERNET) {
        new (d_ethernet.buffer())
            ntsa::EthernetPacket(other.d_ethernet.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_IPV4) {
        new (d_ipv4.buffer())
            ntsa::Ipv4Packet(other.d_ipv4.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_IPV6) {
        new (d_ipv6.buffer())
            ntsa::Ipv6Packet(other.d_ipv6.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_ICMP) {
        new (d_icmp.buffer())
            ntsa::IcmpPacket(other.d_icmp.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_IGMP) {
        new (d_igmp.buffer())
            ntsa::IgmpPacket(other.d_igmp.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_TCP) {
        new (d_tcp.buffer())
            ntsa::TcpPacket(other.d_tcp.object(), d_allocator_p);
    }
    else if (d_type == ntsa::PacketType::e_UDP) {
        new (d_udp.buffer())
            ntsa::UdpPacket(other.d_udp.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == ntsa::PacketType::e_UNDEFINED);
    }

    return *this;
}

void Packet::reset()
{
    if (d_type == ntsa::PacketType::e_ETHERNET) {
        typedef ntsa::EthernetPacket Type;
        d_ethernet.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_IPV4) {
        typedef ntsa::Ipv4Packet Type;
        d_ipv4.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_IPV6) {
        typedef ntsa::Ipv6Packet Type;
        d_ipv6.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_ICMP) {
        typedef ntsa::IcmpPacket Type;
        d_icmp.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_IGMP) {
        typedef ntsa::IgmpPacket Type;
        d_igmp.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_TCP) {
        typedef ntsa::TcpPacket Type;
        d_tcp.object().~Type();
    }
    else if (d_type == ntsa::PacketType::e_UDP) {
        typedef ntsa::UdpPacket Type;
        d_udp.object().~Type();
    }

    d_type = ntsa::PacketType::e_UNDEFINED;
}


void Packet::makeType(ntsa::PacketType::Value type)
{
    if (type == ntsa::PacketType::e_ETHERNET) {
        makeEthernet();
    }
    else if (type == ntsa::PacketType::e_IPV4) {
        makeIpv4();
    }
    else if (type == ntsa::PacketType::e_IPV6) {
        makeIpv6();
    }
    else if (type == ntsa::PacketType::e_ICMP) {
        makeIcmp();
    }
    else if (type == ntsa::PacketType::e_IGMP) {
        makeIgmp();
    }
    else if (type == ntsa::PacketType::e_TCP) {
        makeTcp();
    }
    else if (type == ntsa::PacketType::e_UDP) {
        makeUdp();
    }
    else {
        BSLS_ASSERT(type == ntsa::PacketType::e_UNDEFINED);
        reset();
    }
}

ntsa::EthernetPacket& Packet::makeEthernet()
{
    if (d_type == ntsa::PacketType::e_ETHERNET) {
        d_ethernet.object().reset();
    }
    else {
        reset();
        new (d_ethernet.buffer()) ntsa::EthernetPacket(d_allocator_p);
        d_type = ntsa::PacketType::e_ETHERNET;
    }

    return d_ethernet.object();
}

ntsa::EthernetPacket& Packet::makeEthernet(const ntsa::EthernetPacket& value)
{
    if (d_type == ntsa::PacketType::e_ETHERNET) {
        d_ethernet.object() = value;
    }
    else {
        reset();
        new (d_ethernet.buffer()) ntsa::EthernetPacket(value, d_allocator_p);
        d_type = ntsa::PacketType::e_ETHERNET;
    }

    return d_ethernet.object();
}

ntsa::EthernetPacket& Packet::makeEthernet(
    bslmf::MovableRef<ntsa::EthernetPacket> value) NTSCFG_NOEXCEPT
{
    if (d_type == ntsa::PacketType::e_ETHERNET) {
        d_ethernet.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_ethernet.buffer()) ntsa::EthernetPacket(NTSCFG_MOVE(value));
        d_type = ntsa::PacketType::e_ETHERNET;
    }

    NTSCFG_MOVE_RESET(value);

    return d_ethernet.object();
}

ntsa::Ipv4Packet& Packet::makeIpv4()
{
    if (d_type == ntsa::PacketType::e_IPV4) {
        d_ipv4.object().reset();
    }
    else {
        reset();
        new (d_ipv4.buffer()) ntsa::Ipv4Packet(d_allocator_p);
        d_type = ntsa::PacketType::e_IPV4;
    }

    return d_ipv4.object();
}

ntsa::Ipv4Packet& Packet::makeIpv4(const ntsa::Ipv4Packet& value)
{
    if (d_type == ntsa::PacketType::e_IPV4) {
        d_ipv4.object() = value;
    }
    else {
        reset();
        new (d_ipv4.buffer()) ntsa::Ipv4Packet(value, d_allocator_p);
        d_type = ntsa::PacketType::e_IPV4;
    }

    return d_ipv4.object();
}

ntsa::Ipv4Packet& Packet::makeIpv4(bslmf::MovableRef<ntsa::Ipv4Packet> value)
    NTSCFG_NOEXCEPT
{
    if (d_type == ntsa::PacketType::e_IPV4) {
        d_ipv4.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_ipv4.buffer()) ntsa::Ipv4Packet(NTSCFG_MOVE(value));
        d_type = ntsa::PacketType::e_IPV4;
    }

    NTSCFG_MOVE_RESET(value);

    return d_ipv4.object();
}

ntsa::Ipv6Packet& Packet::makeIpv6()
{
    if (d_type == ntsa::PacketType::e_IPV6) {
        d_ipv6.object().reset();
    }
    else {
        reset();
        new (d_ipv6.buffer()) ntsa::Ipv6Packet(d_allocator_p);
        d_type = ntsa::PacketType::e_IPV6;
    }

    return d_ipv6.object();
}

ntsa::Ipv6Packet& Packet::makeIpv6(const ntsa::Ipv6Packet& value)
{
    if (d_type == ntsa::PacketType::e_IPV6) {
        d_ipv6.object() = value;
    }
    else {
        reset();
        new (d_ipv6.buffer()) ntsa::Ipv6Packet(value, d_allocator_p);
        d_type = ntsa::PacketType::e_IPV6;
    }

    return d_ipv6.object();
}

ntsa::Ipv6Packet& Packet::makeIpv6(bslmf::MovableRef<ntsa::Ipv6Packet> value)
    NTSCFG_NOEXCEPT
{
    if (d_type == ntsa::PacketType::e_IPV6) {
        d_ipv6.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_ipv6.buffer()) ntsa::Ipv6Packet(NTSCFG_MOVE(value));
        d_type = ntsa::PacketType::e_IPV6;
    }

    NTSCFG_MOVE_RESET(value);

    return d_ipv6.object();
}

ntsa::IcmpPacket& Packet::makeIcmp()
{
    if (d_type == ntsa::PacketType::e_ICMP) {
        d_icmp.object().reset();
    }
    else {
        reset();
        new (d_icmp.buffer()) ntsa::IcmpPacket(d_allocator_p);
        d_type = ntsa::PacketType::e_ICMP;
    }

    return d_icmp.object();
}

ntsa::IcmpPacket& Packet::makeIcmp(const ntsa::IcmpPacket& value)
{
    if (d_type == ntsa::PacketType::e_ICMP) {
        d_icmp.object() = value;
    }
    else {
        reset();
        new (d_icmp.buffer()) ntsa::IcmpPacket(value, d_allocator_p);
        d_type = ntsa::PacketType::e_ICMP;
    }

    return d_icmp.object();
}

ntsa::IcmpPacket& Packet::makeIcmp(bslmf::MovableRef<ntsa::IcmpPacket> value)
    NTSCFG_NOEXCEPT
{
    if (d_type == ntsa::PacketType::e_ICMP) {
        d_icmp.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_icmp.buffer()) ntsa::IcmpPacket(NTSCFG_MOVE(value));
        d_type = ntsa::PacketType::e_ICMP;
    }

    NTSCFG_MOVE_RESET(value);

    return d_icmp.object();
}

ntsa::IgmpPacket& Packet::makeIgmp()
{
    if (d_type == ntsa::PacketType::e_IGMP) {
        d_igmp.object().reset();
    }
    else {
        reset();
        new (d_igmp.buffer()) ntsa::IgmpPacket(d_allocator_p);
        d_type = ntsa::PacketType::e_IGMP;
    }

    return d_igmp.object();
}

ntsa::IgmpPacket& Packet::makeIgmp(const ntsa::IgmpPacket& value)
{
    if (d_type == ntsa::PacketType::e_IGMP) {
        d_igmp.object() = value;
    }
    else {
        reset();
        new (d_igmp.buffer()) ntsa::IgmpPacket(value, d_allocator_p);
        d_type = ntsa::PacketType::e_IGMP;
    }

    return d_igmp.object();
}

ntsa::IgmpPacket& Packet::makeIgmp(bslmf::MovableRef<ntsa::IgmpPacket> value)
    NTSCFG_NOEXCEPT
{
    if (d_type == ntsa::PacketType::e_IGMP) {
        d_igmp.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_igmp.buffer()) ntsa::IgmpPacket(NTSCFG_MOVE(value));
        d_type = ntsa::PacketType::e_IGMP;
    }

    NTSCFG_MOVE_RESET(value);

    return d_igmp.object();
}

ntsa::TcpPacket& Packet::makeTcp()
{
    if (d_type == ntsa::PacketType::e_TCP) {
        d_tcp.object().reset();
    }
    else {
        reset();
        new (d_tcp.buffer()) ntsa::TcpPacket(d_allocator_p);
        d_type = ntsa::PacketType::e_TCP;
    }

    return d_tcp.object();
}

ntsa::TcpPacket& Packet::makeTcp(const ntsa::TcpPacket& value)
{
    if (d_type == ntsa::PacketType::e_TCP) {
        d_tcp.object() = value;
    }
    else {
        reset();
        new (d_tcp.buffer()) ntsa::TcpPacket(value, d_allocator_p);
        d_type = ntsa::PacketType::e_TCP;
    }

    return d_tcp.object();
}

ntsa::TcpPacket& Packet::makeTcp(bslmf::MovableRef<ntsa::TcpPacket> value)
    NTSCFG_NOEXCEPT
{
    if (d_type == ntsa::PacketType::e_TCP) {
        d_tcp.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_tcp.buffer()) ntsa::TcpPacket(NTSCFG_MOVE(value));
        d_type = ntsa::PacketType::e_TCP;
    }

    NTSCFG_MOVE_RESET(value);

    return d_tcp.object();
}

ntsa::UdpPacket& Packet::makeUdp()
{
    if (d_type == ntsa::PacketType::e_UDP) {
        d_udp.object().reset();
    }
    else {
        reset();
        new (d_udp.buffer()) ntsa::UdpPacket(d_allocator_p);
        d_type = ntsa::PacketType::e_UDP;
    }

    return d_udp.object();
}

ntsa::UdpPacket& Packet::makeUdp(const ntsa::UdpPacket& value)
{
    if (d_type == ntsa::PacketType::e_UDP) {
        d_udp.object() = value;
    }
    else {
        reset();
        new (d_udp.buffer()) ntsa::UdpPacket(value, d_allocator_p);
        d_type = ntsa::PacketType::e_UDP;
    }

    return d_udp.object();
}

ntsa::UdpPacket& Packet::makeUdp(bslmf::MovableRef<ntsa::UdpPacket> value)
    NTSCFG_NOEXCEPT
{
    if (d_type == ntsa::PacketType::e_UDP) {
        d_udp.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_udp.buffer()) ntsa::UdpPacket(NTSCFG_MOVE(value));
        d_type = ntsa::PacketType::e_UDP;
    }

    NTSCFG_MOVE_RESET(value);

    return d_udp.object();
}

ntsa::Error Packet::decode(ntsa::PacketDecoderContext*       context,
                       const bdlbb::BlobBuffer&          buffer,
                       const ntsa::PacketDecoderOptions& options)
{
    ntsa::Error error;

    ntsa::PacketDecoder decoder(buffer);

    if (d_type == ntsa::PacketType::e_ETHERNET) {
        error = d_ethernet.object().decode(&decoder);
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::PacketType::e_IPV4) {
        error = d_ipv4.object().decode(&decoder);
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::PacketType::e_IPV6) {
        error = d_ipv6.object().decode(&decoder);
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::PacketType::e_ICMP) {
        error = d_icmp.object().decode(&decoder);
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::PacketType::e_IGMP) {
        error = d_igmp.object().decode(&decoder);
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::PacketType::e_TCP) {
        error = d_tcp.object().decode(&decoder, ntsa::Ipv4Address(), ntsa::Ipv4Address());
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::PacketType::e_UDP) {
        error = d_udp.object().decode(&decoder, ntsa::Ipv4Address(), ntsa::Ipv4Address());
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Packet::encode(ntsa::PacketEncoderContext*       context,
                       bdlbb::BlobBuffer*                buffer,
                       const ntsa::PacketEncoderOptions& options) const
{
    NTSCFG_WARNING_UNUSED(buffer);

    return ntsa::Error();
}

bool Packet::equals(const Packet& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == ntsa::PacketType::e_ETHERNET) {
        if (!d_ethernet.object().equals(other.d_ethernet.object())) {
            return false;
        }
    }
    else if (d_type == ntsa::PacketType::e_IPV4) {
        if (!d_ipv4.object().equals(other.d_ipv4.object())) {
            return false;
        }
    }
    else if (d_type == ntsa::PacketType::e_IPV6) {
        if (!d_ipv6.object().equals(other.d_ipv6.object())) {
            return false;
        }
    }
    else if (d_type == ntsa::PacketType::e_ICMP) {
        if (!d_icmp.object().equals(other.d_icmp.object())) {
            return false;
        }
    }
    else if (d_type == ntsa::PacketType::e_IGMP) {
        if (!d_igmp.object().equals(other.d_igmp.object())) {
            return false;
        }
    }
    else if (d_type == ntsa::PacketType::e_TCP) {
        if (!d_tcp.object().equals(other.d_tcp.object())) {
            return false;
        }
    }
    else if (d_type == ntsa::PacketType::e_UDP) {
        if (!d_udp.object().equals(other.d_udp.object())) {
            return false;
        }
    }
    else {
        BSLS_ASSERT(d_type == ntsa::PacketType::e_UNDEFINED);
    }

    return true;
}

bsl::ostream& Packet::print(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
    if (d_type == ntsa::PacketType::e_ETHERNET) {
        return d_ethernet.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == ntsa::PacketType::e_IPV4) {
        return d_ipv4.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == ntsa::PacketType::e_IPV6) {
        return d_ipv6.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == ntsa::PacketType::e_ICMP) {
        return d_icmp.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == ntsa::PacketType::e_IGMP) {
        return d_igmp.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == ntsa::PacketType::e_TCP) {
        return d_tcp.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == ntsa::PacketType::e_UDP) {
        return d_udp.object().print(stream, level, spacesPerLevel);
    }
    else {
        BSLS_ASSERT(d_type == ntsa::PacketType::e_UNDEFINED);
        stream << "UNDEFINED";
        return stream;
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const Packet& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Packet& lhs, const Packet& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Packet& lhs, const Packet& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
