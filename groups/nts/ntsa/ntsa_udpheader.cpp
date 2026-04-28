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

#include <ntsa_udpheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_udpheader_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

UdpChecksum::UdpChecksum()
: d_accumulator(0)
{
}

UdpChecksum::UdpChecksum(const UdpChecksum& original)
: d_accumulator(original.d_accumulator)
{
}

UdpChecksum::~UdpChecksum()
{
}

UdpChecksum& UdpChecksum::operator=(const UdpChecksum& other)
{
    if (this != &other) {
        d_accumulator = other.d_accumulator;
    }

    return *this;
}

void UdpChecksum::reset()
{
    d_accumulator = 0;
}

void UdpChecksum::add(const ntsa::Ipv4Address& sourceAddress,
                      const ntsa::Ipv4Address& destinationAddress,
                      bsl::size_t              length)
{
    // ntsa::Ipv4Address stores its bytes in network byte order, so casting
    // to uint8_t* gives the wire-order octets directly.
    const bsl::uint8_t* src =
        reinterpret_cast<const bsl::uint8_t*>(&sourceAddress);
    const bsl::uint8_t* dst =
        reinterpret_cast<const bsl::uint8_t*>(&destinationAddress);

    d_accumulator +=
        (static_cast<bsl::uint16_t>(src[0]) << 8) | src[1];
    d_accumulator +=
        (static_cast<bsl::uint16_t>(src[2]) << 8) | src[3];

    d_accumulator +=
        (static_cast<bsl::uint16_t>(dst[0]) << 8) | dst[1];
    d_accumulator +=
        (static_cast<bsl::uint16_t>(dst[2]) << 8) | dst[3];

    // Zero byte concatenated with protocol number forms one 16-bit word.
    d_accumulator += static_cast<bsl::uint16_t>(UdpHeader::k_PROTOCOL_UDP);

    d_accumulator += static_cast<bsl::uint16_t>(length);
}

void UdpChecksum::add(const void* data, bsl::size_t size)
{
    const bsl::uint8_t* p = static_cast<const bsl::uint8_t*>(data);
    bsl::size_t         n = size;

    while (n > 1) {
        d_accumulator +=
            (static_cast<bsl::uint16_t>(p[0]) << 8) |
             static_cast<bsl::uint16_t>(p[1]);
        p += 2;
        n -= 2;
    }

    if (n > 0) {
        // Pad the trailing byte with a zero on the right, as required by
        // RFC 768: the odd byte occupies the high byte of the 16-bit word.
        d_accumulator += static_cast<bsl::uint16_t>(p[0]) << 8;
    }
}

bsl::uint32_t UdpChecksum::accumulator() const
{
    return d_accumulator;
}

bsl::uint16_t UdpChecksum::value() const
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

ntsa::Error UdpHeader::decode(const void* data, const bsl::size_t size)
{
    reset();

    if (size < static_cast<bsl::size_t>(k_LENGTH)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::memcpy(reinterpret_cast<void*>(this),
                data,
                static_cast<bsl::size_t>(k_LENGTH));

    return ntsa::Error();
}

ntsa::Error UdpHeader::decode(const bdlbb::BlobBuffer& source)
{
    reset();

    if (source.size() < static_cast<int>(k_LENGTH)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::memcpy(reinterpret_cast<void*>(this),
                source.data(),
                static_cast<bsl::size_t>(k_LENGTH));

    return ntsa::Error();
}

ntsa::Error UdpHeader::encode(bdlbb::BlobBuffer* buffer,
                              bsl::size_t        offset) const
{
    ntsa::Error error;

    if (buffer->data() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    char* bufferData = buffer->data();

    if (buffer->size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t bufferCapacity =
        static_cast<bsl::size_t>(buffer->size());

    const bsl::size_t headerLength = this->headerLength();

    if (offset + headerLength > bufferCapacity) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::memcpy(reinterpret_cast<void*>(bufferData + offset),
                reinterpret_cast<const void*>(this),
                sizeof *this);

    return ntsa::Error();
}

bool UdpHeader::equals(const UdpHeader& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) == 0;
}

bool UdpHeader::less(const UdpHeader& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) < 0;
}

bsl::ostream& UdpHeader::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("headerLength", this->headerLength());
    printer.printAttribute("packetLength", this->packetLength());
    printer.printAttribute("sourcePort", this->sourcePort());
    printer.printAttribute("destinationPort", this->destinationPort());
    printer.printAttribute("checksum", this->checksum());
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
