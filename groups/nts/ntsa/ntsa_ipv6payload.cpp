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

#include <ntsa_ipv6payload.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv6payload_cpp, "$Id$ $CSID$")

#include <bdlb_print.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntsa {

Ipv6Payload::Ipv6Payload(bslmf::MovableRef<Ipv6Payload> original)
    NTSCFG_NOEXCEPT : d_type(NTSCFG_MOVE_FROM(original, d_type))
{
    if (d_type == e_RAW) {
        new (d_raw.buffer())
            bdlbb::BlobBuffer(NTSCFG_MOVE_FROM(original, d_raw.object()));
    }
    else if (d_type == e_TCP) {
        new (d_tcp.buffer())
            ntsa::TcpPacket(NTSCFG_MOVE_FROM(original, d_tcp.object()));
    }
    else if (d_type == e_UDP) {
        new (d_udp.buffer())
            ntsa::UdpPacket(NTSCFG_MOVE_FROM(original, d_udp.object()));
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    NTSCFG_MOVE_RESET(original);
}

Ipv6Payload::Ipv6Payload(const Ipv6Payload& original)
: d_type(original.d_type)
{
    if (d_type == e_RAW) {
        new (d_raw.buffer()) bdlbb::BlobBuffer(original.d_raw.object());
    }
    else if (d_type == e_TCP) {
        new (d_tcp.buffer()) ntsa::TcpPacket(original.d_tcp.object());
    }
    else if (d_type == e_UDP) {
        new (d_udp.buffer()) ntsa::UdpPacket(original.d_udp.object());
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

Ipv6Payload::~Ipv6Payload()
{
    if (d_type == e_RAW) {
        typedef bdlbb::BlobBuffer Type;
        d_raw.object().~Type();
    }
    else if (d_type == e_TCP) {
        typedef ntsa::TcpPacket Type;
        d_tcp.object().~Type();
    }
    else if (d_type == e_UDP) {
        typedef ntsa::UdpPacket Type;
        d_udp.object().~Type();
    }
}

Ipv6Payload& Ipv6Payload::operator=(bslmf::MovableRef<Ipv6Payload> other)
    NTSCFG_NOEXCEPT
{
    reset();

    d_type = NTSCFG_MOVE_FROM(other, d_type);

    if (d_type == e_RAW) {
        new (d_raw.buffer())
            bdlbb::BlobBuffer(NTSCFG_MOVE_FROM(other, d_raw.object()));
    }
    else if (d_type == e_TCP) {
        new (d_tcp.buffer())
            ntsa::TcpPacket(NTSCFG_MOVE_FROM(other, d_tcp.object()));
    }
    else if (d_type == e_UDP) {
        new (d_udp.buffer())
            ntsa::UdpPacket(NTSCFG_MOVE_FROM(other, d_udp.object()));
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    NTSCFG_MOVE_RESET(other);

    return *this;
}

Ipv6Payload& Ipv6Payload::operator=(const Ipv6Payload& other)
{
    if (this == &other) {
        return *this;
    }

    reset();

    d_type = other.d_type;

    if (d_type == e_RAW) {
        new (d_raw.buffer()) bdlbb::BlobBuffer(other.d_raw.object());
    }
    else if (d_type == e_TCP) {
        new (d_tcp.buffer()) ntsa::TcpPacket(other.d_tcp.object());
    }
    else if (d_type == e_UDP) {
        new (d_udp.buffer()) ntsa::UdpPacket(other.d_udp.object());
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    return *this;
}

void Ipv6Payload::reset()
{
    if (d_type == e_RAW) {
        typedef bdlbb::BlobBuffer Type;
        d_raw.object().~Type();
    }
    else if (d_type == e_TCP) {
        typedef ntsa::TcpPacket Type;
        d_tcp.object().~Type();
    }
    else if (d_type == e_UDP) {
        typedef ntsa::UdpPacket Type;
        d_udp.object().~Type();
    }

    d_type = e_UNDEFINED;
}

bdlbb::BlobBuffer& Ipv6Payload::makeRaw()
{
    if (d_type == e_RAW) {
        d_raw.object().reset();
    }
    else {
        reset();
        new (d_raw.buffer()) bdlbb::BlobBuffer();
        d_type = e_RAW;
    }

    return d_raw.object();
}

bdlbb::BlobBuffer& Ipv6Payload::makeRaw(const bdlbb::BlobBuffer& value)
{
    if (d_type == e_RAW) {
        d_raw.object() = value;
    }
    else {
        reset();
        new (d_raw.buffer()) bdlbb::BlobBuffer(value);
        d_type = e_RAW;
    }

    return d_raw.object();
}

bdlbb::BlobBuffer& Ipv6Payload::makeRaw(
    bslmf::MovableRef<bdlbb::BlobBuffer> value) NTSCFG_NOEXCEPT
{
    if (d_type == e_RAW) {
        d_raw.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_raw.buffer()) bdlbb::BlobBuffer(NTSCFG_MOVE(value));
        d_type = e_RAW;
    }

    NTSCFG_MOVE_RESET(value);

    return d_raw.object();
}

ntsa::TcpPacket& Ipv6Payload::makeTcp()
{
    if (d_type == e_TCP) {
        d_tcp.object().reset();
    }
    else {
        reset();
        new (d_tcp.buffer()) ntsa::TcpPacket();
        d_type = e_TCP;
    }

    return d_tcp.object();
}

ntsa::TcpPacket& Ipv6Payload::makeTcp(const ntsa::TcpPacket& value)
{
    if (d_type == e_TCP) {
        d_tcp.object() = value;
    }
    else {
        reset();
        new (d_tcp.buffer()) ntsa::TcpPacket(value);
        d_type = e_TCP;
    }

    return d_tcp.object();
}

ntsa::TcpPacket& Ipv6Payload::makeTcp(bslmf::MovableRef<ntsa::TcpPacket> value)
    NTSCFG_NOEXCEPT
{
    if (d_type == e_TCP) {
        d_tcp.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_tcp.buffer()) ntsa::TcpPacket(NTSCFG_MOVE(value));
        d_type = e_TCP;
    }

    NTSCFG_MOVE_RESET(value);

    return d_tcp.object();
}

ntsa::UdpPacket& Ipv6Payload::makeUdp()
{
    if (d_type == e_UDP) {
        d_udp.object().reset();
    }
    else {
        reset();
        new (d_udp.buffer()) ntsa::UdpPacket();
        d_type = e_UDP;
    }

    return d_udp.object();
}

ntsa::UdpPacket& Ipv6Payload::makeUdp(const ntsa::UdpPacket& value)
{
    if (d_type == e_UDP) {
        d_udp.object() = value;
    }
    else {
        reset();
        new (d_udp.buffer()) ntsa::UdpPacket(value);
        d_type = e_UDP;
    }

    return d_udp.object();
}

ntsa::UdpPacket& Ipv6Payload::makeUdp(bslmf::MovableRef<ntsa::UdpPacket> value)
    NTSCFG_NOEXCEPT
{
    if (d_type == e_UDP) {
        d_udp.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_udp.buffer()) ntsa::UdpPacket(NTSCFG_MOVE(value));
        d_type = e_UDP;
    }

    NTSCFG_MOVE_RESET(value);

    return d_udp.object();
}

bool Ipv6Payload::equals(const Ipv6Payload& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_RAW) {
        if (d_raw.object().size() != other.d_raw.object().size()) {
            return false;
        }

        const int compare = bsl::memcmp(d_raw.object().data(),
                                        other.d_raw.object().data(),
                                        d_raw.object().size());

        if (compare != 0) {
            return false;
        }
    }
    else if (d_type == e_TCP) {
        if (!d_tcp.object().equals(other.d_tcp.object())) {
            return false;
        }
    }
    else if (d_type == e_UDP) {
        if (!d_udp.object().equals(other.d_udp.object())) {
            return false;
        }
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    return true;
}

bsl::ostream& Ipv6Payload::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    if (d_type == e_RAW) {
        return bdlb::Print::hexDump(stream,
                                    d_raw.object().data(),
                                    d_raw.object().size());
    }
    else if (d_type == e_TCP) {
        return d_tcp.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_UDP) {
        return d_udp.object().print(stream, level, spacesPerLevel);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
        stream << "UNDEFINED";
        return stream;
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Payload& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Ipv6Payload& lhs, const Ipv6Payload& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Ipv6Payload& lhs, const Ipv6Payload& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
