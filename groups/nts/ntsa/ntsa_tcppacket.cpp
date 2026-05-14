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

#include <ntsa_tcppacket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcppacket_cpp, "$Id$ $CSID$")

#include <ntsa_tcpchecksum.h>
#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error TcpPacket::decode(ntsa::PacketDecoder*     decoder,
                              const ntsa::Ipv4Address& sourceAddress,
                              const ntsa::Ipv4Address& destinationAddress)
{
    ntsa::Error error;

    const bsl::size_t headerPosition = decoder->position();

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    const bsl::size_t extensionLength =
        d_header.dataOffset() -
        static_cast<bsl::size_t>(ntsa::TcpHeader::k_MIN_HEADER_LENGTH);

    if (extensionLength > 0) {
        error = d_extension.decode(decoder);
        if (error) {
            return error;
        }
    }

    const bsl::size_t payloadSize = decoder->size() - decoder->position();

    if (payloadSize > 0) {
        error = decoder->decodeRaw(&d_payload, payloadSize);
        if (error) {
            return error;
        }
    }

    const bsl::size_t finalPosition = decoder->position();

    const bsl::size_t packetLength =
        static_cast<bsl::size_t>(finalPosition - headerPosition);

    error = decoder->seek(headerPosition);
    if (error) {
        return error;
    }

    ntsa::TcpChecksum checksum;
    checksum.add(sourceAddress, destinationAddress, packetLength);
    checksum.add(decoder->next(), packetLength);

    const bsl::uint16_t checksumValue = checksum.value();

    if (checksumValue != 0xFFFF) {
        BSLS_LOG_WARN("Invalid checksum: expected %zu but found %zu",
                      static_cast<bsl::size_t>(d_header.checksum()),
                      static_cast<bsl::size_t>(checksumValue));
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = decoder->seek(finalPosition);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TcpPacket::decode(ntsa::PacketDecoder*     decoder,
                              const ntsa::Ipv6Address& sourceAddress,
                              const ntsa::Ipv6Address& destinationAddress)
{
    NTSCFG_WARNING_UNUSED(decoder);
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TcpPacket::encode(
    ntsa::PacketEncoder*     encoder,
    const ntsa::Ipv4Address& sourceAddress,
    const ntsa::Ipv4Address& destinationAddress) const
{
    ntsa::Error error;

    ntsa::TcpHeader header = d_header;
    header.setChecksum(0);

    const bsl::size_t headerPosition = encoder->position();

    const bsl::size_t headerLength =
        static_cast<bsl::size_t>(ntsa::TcpHeader::k_MIN_HEADER_LENGTH);

    error = encoder->advance(headerLength);
    if (error) {
        return error;
    }

    const bsl::size_t extensionPosition = encoder->position();

    error = d_extension.encode(encoder);
    if (error) {
        return error;
    }

    const bsl::size_t extensionLength =
        static_cast<bsl::size_t>(encoder->position() - extensionPosition);

    const bsl::size_t payloadLength =
        static_cast<bsl::size_t>(d_payload.size());

    if (payloadLength > 0) {
        error = encoder->encodeRaw(d_payload, d_payload.size());
        if (error) {
            return error;
        }
    }

    const bsl::size_t finalPosition = encoder->position();

    const bsl::size_t packetLength =
        headerLength + extensionLength + payloadLength;

    error = encoder->seek(headerPosition);
    if (error) {
        return error;
    }

    error = header.encode(encoder);
    if (error) {
        return error;
    }

    error = encoder->seek(headerPosition);
    if (error) {
        return error;
    }

    ntsa::TcpChecksum checksum;
    checksum.add(sourceAddress, destinationAddress, packetLength);
    checksum.add(encoder->next(), packetLength);

    header.setChecksum(checksum.value());

    error = header.encode(encoder);
    if (error) {
        return error;
    }

    error = encoder->seek(finalPosition);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TcpPacket::encode(
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

bool TcpPacket::equals(const TcpPacket& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_extension != other.d_extension) {
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

bsl::ostream& TcpPacket::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    d_header.print(&printer);
    d_extension.print(&printer);

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
