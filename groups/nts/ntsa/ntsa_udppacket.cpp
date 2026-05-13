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

#include <ntsa_udppacket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_udppacket_cpp, "$Id$ $CSID$")

#include <ntsa_udpchecksum.h>
#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error UdpPacket::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    const bsl::size_t payloadSize = decoder->size() - decoder->position();

    if (payloadSize > 0) {
        error = decoder->decodeRaw(&d_payload, payloadSize);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error UdpPacket::encode(
    ntsa::PacketEncoder*     encoder,
    const ntsa::Ipv4Address& sourceAddress,
    const ntsa::Ipv4Address& destinationAddress) const
{
    ntsa::Error error;

    const bsl::size_t headerLength = d_header.headerLength();
    const bsl::size_t packetLength =
        headerLength + static_cast<bsl::size_t>(d_payload.size());

    ntsa::UdpHeader header = d_header;

    header.setChecksum(0);

    ntsa::UdpChecksum checksum;
    checksum.add(sourceAddress, destinationAddress, packetLength);
    checksum.add(&header, header.headerLength());
    if (d_payload.size() > 0) {
        checksum.add(d_payload.data(),
                     static_cast<bsl::size_t>(d_payload.size()));
    }

    header.setChecksum(checksum.value());

    error = header.encode(encoder);
    if (error) {
        return error;
    }

    if (d_payload.size() > 0) {
        error = encoder->encodeRaw(d_payload, d_payload.size());
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error UdpPacket::encode(
    ntsa::PacketEncoder*     encoder,
    const ntsa::Ipv6Address& sourceAddress,
    const ntsa::Ipv6Address& destinationAddress) const
{
    NTSCFG_WARNING_UNUSED(encoder);
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error UdpPacket::decode(const bdlbb::BlobBuffer& buffer,
                              bsl::size_t              offset,
                              bsl::size_t              packetSize)
{
    ntsa::Error error;

    if (buffer.size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset > packetSize) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset > static_cast<bsl::size_t>(buffer.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_header.decode(buffer, offset, packetSize);
    if (error) {
        return error;
    }

    const bsl::size_t headerLength = d_header.headerLength();

    if (offset + headerLength > static_cast<bsl::size_t>(buffer.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset + headerLength != packetSize) {
        d_payload.reset(
            bsl::shared_ptr<char>(buffer.buffer(),
                                  buffer.data() + offset + headerLength),
            static_cast<int>(buffer.size() - offset - headerLength));
    }

    return ntsa::Error();
}

ntsa::Error UdpPacket::encode(
    bdlbb::BlobBuffer*       buffer,
    bsl::size_t              offset,
    const ntsa::Ipv4Address& sourceAddress,
    const ntsa::Ipv4Address& destinationAddress) const
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

    if (offset > bufferCapacity) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t headerLength = d_header.headerLength();
    const bsl::size_t packetLength =
        headerLength + static_cast<bsl::size_t>(d_payload.size());

    ntsa::UdpHeader header = d_header;

    header.setChecksum(0);

    ntsa::UdpChecksum checksum;
    checksum.add(sourceAddress, destinationAddress, packetLength);
    checksum.add(&header, header.headerLength());
    if (d_payload.size() > 0) {
        checksum.add(d_payload.data(),
                     static_cast<bsl::size_t>(d_payload.size()));
    }

    header.setChecksum(checksum.value());

    error = header.encode(buffer, offset);
    if (error) {
        return error;
    }

    if (d_payload.size() > 0) {
        if (offset + headerLength > bufferCapacity) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bsl::memcpy(bufferData + offset + headerLength,
                    d_payload.data(),
                    static_cast<bsl::size_t>(d_payload.size()));
    }

    return ntsa::Error();
}

ntsa::Error UdpPacket::encode(
    bdlbb::BlobBuffer*       buffer,
    bsl::size_t              offset,
    const ntsa::Ipv6Address& sourceAddress,
    const ntsa::Ipv6Address& destinationAddress) const
{
    NTSCFG_WARNING_UNUSED(buffer);
    NTSCFG_WARNING_UNUSED(offset);
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bool UdpPacket::equals(const UdpPacket& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_payload.size() != other.d_payload.size()) {
        return false;
    }

    const int compare = bsl::memcmp(d_payload.data(),
                                    other.d_payload.data(),
                                    d_payload.size());
    if (compare != 0) {
        return false;
    }

    return true;
}

bsl::ostream& UdpPacket::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    d_header.print(&printer);

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
