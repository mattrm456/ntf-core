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

#include <ntsa_igmpchecksum.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_igmpchecksum_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsa {

IgmpChecksum::IgmpChecksum()
: d_accumulator(0)
{
}

IgmpChecksum::IgmpChecksum(const IgmpChecksum& original)
: d_accumulator(original.d_accumulator)
{
}

IgmpChecksum::~IgmpChecksum()
{
}

IgmpChecksum& IgmpChecksum::operator=(const IgmpChecksum& other)
{
    if (this != &other) {
        d_accumulator = other.d_accumulator;
    }

    return *this;
}

void IgmpChecksum::reset()
{
    d_accumulator = 0;
}

void IgmpChecksum::add(const ntsa::Ipv4Address& sourceAddress,
                       const ntsa::Ipv4Address& destinationAddress,
                       bsl::size_t              length)
{
    const bsl::uint8_t* src =
        reinterpret_cast<const bsl::uint8_t*>(&sourceAddress);

    const bsl::uint8_t* dst =
        reinterpret_cast<const bsl::uint8_t*>(&destinationAddress);

    d_accumulator += (static_cast<bsl::uint16_t>(src[0]) << 8) | src[1];

    d_accumulator += (static_cast<bsl::uint16_t>(src[2]) << 8) | src[3];

    d_accumulator += (static_cast<bsl::uint16_t>(dst[0]) << 8) | dst[1];

    d_accumulator += (static_cast<bsl::uint16_t>(dst[2]) << 8) | dst[3];

    d_accumulator += static_cast<bsl::uint16_t>(IgmpChecksum::k_PROTOCOL_IGMP);
    d_accumulator += static_cast<bsl::uint16_t>(length);
}

void IgmpChecksum::add(const void* data, bsl::size_t size)
{
    const bsl::uint8_t* p = static_cast<const bsl::uint8_t*>(data);
    bsl::size_t         n = size;

    while (n > 1) {
        d_accumulator += (static_cast<bsl::uint16_t>(p[0]) << 8) |
                         static_cast<bsl::uint16_t>(p[1]);
        p += 2;
        n -= 2;
    }

    if (n > 0) {
        d_accumulator += static_cast<bsl::uint16_t>(p[0]) << 8;
    }
}

bsl::uint32_t IgmpChecksum::accumulator() const
{
    return d_accumulator;
}

bsl::uint16_t IgmpChecksum::value() const
{
    bsl::uint32_t accumulator = d_accumulator;

    while (accumulator >> 16) {
        accumulator = (accumulator & 0xFFFF) + (accumulator >> 16);
    }

    const bsl::uint16_t result = static_cast<bsl::uint16_t>(~accumulator);

    if (result == 0) {
        return 0xFFFF;
    }

    return result;
}

}  // close package namespace
}  // close enterprise namespace
