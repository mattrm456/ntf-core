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

ntsa::Error UdpPacket::decode(ntsa::PacketDecoderContext*       context,
                              ntsa::PacketDecoder*              decoder,
                              const ntsa::PacketDecoderOptions& options)
{
    ntsa::Error error;

    const bsl::size_t headerPosition = decoder->position();

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    if (options.sourceUdpPort().has_value()) {
        if (d_header.sourcePort() != options.sourceUdpPort().value()) {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setSourceUdpPort(d_header.sourcePort());

    if (options.destinationUdpPort().has_value()) {
        if (d_header.destinationPort() != options.destinationUdpPort().value())
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setDestinationUdpPort(d_header.destinationPort());

    const bsl::size_t payloadSize = static_cast<bsl::size_t>(
        d_header.packetLength() - d_header.headerLength());

    if (payloadSize > 0) {
        error = decoder->decodeRaw(&d_payload, payloadSize);
        if (error) {
            return error;
        }
    }

    const bsl::size_t extensionLength = decoder->size() - decoder->position();

    if (extensionLength > 0) {
        error = d_extension.decode(decoder, extensionLength);
        if (error) {
            return error;
        }
    }

    if (d_header.checksum() != 0 && !options.ignoreChecksum()) {
        const bsl::size_t finalPosition = decoder->position();

        const bsl::size_t packetLength = d_header.packetLength();

        error = decoder->seek(headerPosition);
        if (error) {
            return error;
        }

        ntsa::UdpChecksum checksum;

        if (!context->sourceIpAddress().has_value()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!context->destinationIpAddress().has_value()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (context->sourceIpAddress().value().isV4()) {
            if (!context->destinationIpAddress().value().isV4()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            checksum.add(context->sourceIpAddress().value().v4(),
                        context->destinationIpAddress().value().v4(),
                        packetLength,
                        ntsa::UdpHeader::k_PROTOCOL_UDP);
        }
        else if (context->sourceIpAddress().value().isV6()) {
            if (!context->destinationIpAddress().value().isV6()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            checksum.add(context->sourceIpAddress().value().v6(),
                        context->destinationIpAddress().value().v6(),
                        packetLength,
                        ntsa::UdpHeader::k_PROTOCOL_UDP);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

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
    }

    return ntsa::Error();
}

ntsa::Error UdpPacket::encode(ntsa::PacketEncoderContext*       context,
                              ntsa::PacketEncoder*              encoder,
                              const ntsa::PacketEncoderOptions& options) const
{
    ntsa::Error error;

    if (options.sourceUdpPort().has_value()) {
        if (d_header.sourcePort() != options.sourceUdpPort().value()) {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setSourceUdpPort(d_header.sourcePort());

    if (options.destinationUdpPort().has_value()) {
        if (d_header.destinationPort() != options.destinationUdpPort().value())
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setDestinationUdpPort(d_header.destinationPort());

    const bsl::size_t headerLength = d_header.headerLength();
    const bsl::size_t packetLength =
        headerLength + static_cast<bsl::size_t>(d_payload.size());

    ntsa::UdpHeader header = d_header;
    header.setChecksum(0);
    header.setPacketLength(packetLength);

    ntsa::UdpChecksum checksum;

    if (!context->sourceIpAddress().has_value()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!context->destinationIpAddress().has_value()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context->sourceIpAddress().value().isV4()) {
        if (!context->destinationIpAddress().value().isV4()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        checksum.add(context->sourceIpAddress().value().v4(),
                     context->destinationIpAddress().value().v4(),
                     packetLength,
                     ntsa::UdpHeader::k_PROTOCOL_UDP);
    }
    else if (context->sourceIpAddress().value().isV6()) {
        if (!context->destinationIpAddress().value().isV6()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        checksum.add(context->sourceIpAddress().value().v6(),
                     context->destinationIpAddress().value().v6(),
                     packetLength,
                     ntsa::UdpHeader::k_PROTOCOL_UDP);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

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

    if (!d_extension.empty()) {
        error = d_extension.encode(encoder);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

bool UdpPacket::equals(const UdpPacket& other) const
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

bsl::ostream& UdpPacket::print(bsl::ostream& stream,
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
