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

#include <ntsa_ethernetpayload.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetpayload_cpp, "$Id$ $CSID$")

#include <bdlb_print.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntsa {

EthernetPayload::EthernetPayload(bslmf::MovableRef<EthernetPayload> original)
    NTSCFG_NOEXCEPT : d_type(NTSCFG_MOVE_FROM(original, d_type)),
                      d_allocator_p(NTSCFG_MOVE_FROM(original, d_allocator_p))
{
    if (d_type == e_RAW) {
        new (d_raw.buffer())
            bdlbb::BlobBuffer(NTSCFG_MOVE_FROM(original, d_raw.object()));
    }
    else if (d_type == e_IPV4) {
        new (d_ipv4.buffer())
            ntsa::Ipv4Packet(NTSCFG_MOVE_FROM(original, d_ipv4.object()));
    }
    else if (d_type == e_IPV6) {
        new (d_ipv6.buffer())
            ntsa::Ipv6Packet(NTSCFG_MOVE_FROM(original, d_ipv6.object()));
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    NTSCFG_MOVE_RESET(original);
}

EthernetPayload::EthernetPayload(const EthernetPayload& original,
                                 bslma::Allocator*      basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_RAW) {
        new (d_raw.buffer()) bdlbb::BlobBuffer(original.d_raw.object());
    }
    else if (d_type == e_IPV4) {
        new (d_ipv4.buffer())
            ntsa::Ipv4Packet(original.d_ipv4.object(), d_allocator_p);
    }
    else if (d_type == e_IPV6) {
        new (d_ipv6.buffer())
            ntsa::Ipv6Packet(original.d_ipv6.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EthernetPayload::~EthernetPayload()
{
    if (d_type == e_RAW) {
        typedef bdlbb::BlobBuffer Type;
        d_raw.object().~Type();
    }
    else if (d_type == e_IPV4) {
        typedef ntsa::Ipv4Packet Type;
        d_ipv4.object().~Type();
    }
    else if (d_type == e_IPV6) {
        typedef ntsa::Ipv6Packet Type;
        d_ipv6.object().~Type();
    }
}

EthernetPayload& EthernetPayload::operator=(
    bslmf::MovableRef<EthernetPayload> other) NTSCFG_NOEXCEPT
{
    reset();

    d_type        = NTSCFG_MOVE_FROM(other, d_type);
    d_allocator_p = NTSCFG_MOVE_FROM(other, d_allocator_p);

    if (d_type == e_RAW) {
        new (d_raw.buffer())
            bdlbb::BlobBuffer(NTSCFG_MOVE_FROM(other, d_raw.object()));
    }
    else if (d_type == e_IPV4) {
        new (d_ipv4.buffer())
            ntsa::Ipv4Packet(NTSCFG_MOVE_FROM(other, d_ipv4.object()));
    }
    else if (d_type == e_IPV6) {
        new (d_ipv6.buffer())
            ntsa::Ipv6Packet(NTSCFG_MOVE_FROM(other, d_ipv6.object()));
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    NTSCFG_MOVE_RESET(other);

    return *this;
}

EthernetPayload& EthernetPayload::operator=(const EthernetPayload& other)
{
    if (this == &other) {
        return *this;
    }

    reset();

    d_type = other.d_type;

    if (d_type == e_RAW) {
        new (d_raw.buffer()) bdlbb::BlobBuffer(other.d_raw.object());
    }
    else if (d_type == e_IPV4) {
        new (d_ipv4.buffer())
            ntsa::Ipv4Packet(other.d_ipv4.object(), d_allocator_p);
    }
    else if (d_type == e_IPV6) {
        new (d_ipv6.buffer())
            ntsa::Ipv6Packet(other.d_ipv6.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    return *this;
}

void EthernetPayload::reset()
{
    if (d_type == e_RAW) {
        typedef bdlbb::BlobBuffer Type;
        d_raw.object().~Type();
    }
    else if (d_type == e_IPV4) {
        typedef ntsa::Ipv4Packet Type;
        d_ipv4.object().~Type();
    }
    else if (d_type == e_IPV6) {
        typedef ntsa::Ipv6Packet Type;
        d_ipv6.object().~Type();
    }

    d_type = e_UNDEFINED;
}

bdlbb::BlobBuffer& EthernetPayload::makeRaw()
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

bdlbb::BlobBuffer& EthernetPayload::makeRaw(const bdlbb::BlobBuffer& value)
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

bdlbb::BlobBuffer& EthernetPayload::makeRaw(
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

ntsa::Ipv4Packet& EthernetPayload::makeIpv4()
{
    if (d_type == e_IPV4) {
        d_ipv4.object().reset();
    }
    else {
        reset();
        new (d_ipv4.buffer()) ntsa::Ipv4Packet(d_allocator_p);
        d_type = e_IPV4;
    }

    return d_ipv4.object();
}

ntsa::Ipv4Packet& EthernetPayload::makeIpv4(const ntsa::Ipv4Packet& value)
{
    if (d_type == e_IPV4) {
        d_ipv4.object() = value;
    }
    else {
        reset();
        new (d_ipv4.buffer()) ntsa::Ipv4Packet(value, d_allocator_p);
        d_type = e_IPV4;
    }

    return d_ipv4.object();
}

ntsa::Ipv4Packet& EthernetPayload::makeIpv4(
    bslmf::MovableRef<ntsa::Ipv4Packet> value) NTSCFG_NOEXCEPT
{
    if (d_type == e_IPV4) {
        d_ipv4.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_ipv4.buffer()) ntsa::Ipv4Packet(NTSCFG_MOVE(value));
        d_type = e_IPV4;
    }

    NTSCFG_MOVE_RESET(value);

    return d_ipv4.object();
}

ntsa::Ipv6Packet& EthernetPayload::makeIpv6()
{
    if (d_type == e_IPV6) {
        d_ipv6.object().reset();
    }
    else {
        reset();
        new (d_ipv6.buffer()) ntsa::Ipv6Packet(d_allocator_p);
        d_type = e_IPV6;
    }

    return d_ipv6.object();
}

ntsa::Ipv6Packet& EthernetPayload::makeIpv6(const ntsa::Ipv6Packet& value)
{
    if (d_type == e_IPV6) {
        d_ipv6.object() = value;
    }
    else {
        reset();
        new (d_ipv6.buffer()) ntsa::Ipv6Packet(value, d_allocator_p);
        d_type = e_IPV6;
    }

    return d_ipv6.object();
}

ntsa::Ipv6Packet& EthernetPayload::makeIpv6(
    bslmf::MovableRef<ntsa::Ipv6Packet> value) NTSCFG_NOEXCEPT
{
    if (d_type == e_IPV6) {
        d_ipv6.object() = NTSCFG_MOVE(value);
    }
    else {
        reset();
        new (d_ipv6.buffer()) ntsa::Ipv6Packet(NTSCFG_MOVE(value));
        d_type = e_IPV6;
    }

    NTSCFG_MOVE_RESET(value);

    return d_ipv6.object();
}

bool EthernetPayload::equals(const EthernetPayload& other) const
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
    else if (d_type == e_IPV4) {
        if (!d_ipv4.object().equals(other.d_ipv4.object())) {
            return false;
        }
    }
    else if (d_type == e_IPV6) {
        if (!d_ipv6.object().equals(other.d_ipv6.object())) {
            return false;
        }
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    return true;
}

bsl::ostream& EthernetPayload::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    if (d_type == e_RAW) {
        return bdlb::Print::hexDump(stream,
                                    d_raw.object().data(),
                                    d_raw.object().size());
    }
    else if (d_type == e_IPV4) {
        return d_ipv4.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_IPV6) {
        return d_ipv6.object().print(stream, level, spacesPerLevel);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
        stream << "UNDEFINED";
        return stream;
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const EthernetPayload& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EthernetPayload& lhs, const EthernetPayload& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EthernetPayload& lhs, const EthernetPayload& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
