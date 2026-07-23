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

#include <ntsa_ipv4packet.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4packet_cpp, "$Id$ $CSID$")

#include <ntsa_ipv4checksum.h>
#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error Ipv4Packet::decode(ntsa::PacketDecoderContext*       context,
                               ntsa::PacketDecoder*              decoder,
                               const ntsa::PacketDecoderOptions& options)
{
    ntsa::Error error;

    const bsl::size_t headerPosition = decoder->position();

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    if (options.sourceIpAddress().has_value()) {
        if (options.sourceIpAddress().value().isV4()) {
            if (d_header.sourceAddress() !=
                options.sourceIpAddress().value().v4())
            {
                return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setSourceIpAddress(ntsa::IpAddress(d_header.sourceAddress()));

    if (options.destinationIpAddress().has_value()) {
        if (options.destinationIpAddress().value().isV4()) {
            if (d_header.destinationAddress() !=
                options.destinationIpAddress().value().v4())
            {
                return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setDestinationIpAddress(
        ntsa::IpAddress(d_header.destinationAddress()));

    const bsl::size_t extensionPosition = decoder->position();

    const bsl::size_t extensionLength =
        d_header.headerLength() - ntsa::Ipv4Header::k_MIN_HEADER_LENGTH;

    if (extensionLength > ntsa::Ipv4Extension::k_MAX_OPTIONS_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (extensionLength > 0) {
        error = d_extension.decode(decoder, extensionLength);
        if (error) {
            return error;
        }
    }

    const bsl::size_t payloadPosition = decoder->position();

    const bsl::size_t extensionLengthDecoded =
        payloadPosition - extensionPosition;

    if (extensionLengthDecoded != extensionLength) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_header.checksum() != 0 && !options.ignoreChecksum()) {
        error = decoder->seek(headerPosition);
        if (error) {
            return error;
        }

        ntsa::Ipv4Checksum checksum;
        checksum.add(decoder->next(), d_header.headerLength());

        const bsl::uint16_t checksumValue = checksum.value();

        if (checksumValue != 0xFFFF) {
            BSLS_LOG_WARN("Invalid checksum: expected %zu but found %zu",
                          static_cast<bsl::size_t>(d_header.checksum()),
                          static_cast<bsl::size_t>(checksumValue));
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = decoder->seek(payloadPosition);
        if (error) {
            return error;
        }
    }

    const bsl::size_t remaining = decoder->size() - decoder->position();
    const bsl::size_t payloadLength =
        d_header.packetLength() - d_header.headerLength();

    if (remaining > payloadLength) {
        error = decoder->truncate(remaining - payloadLength);
        if (error) {
            return error;
        }
    }

    if (d_header.protocol() ==
        static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_ICMP))
    {
        ntsa::IcmpPacket& icmp = d_payload.makeIcmp();

        error = icmp.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.protocol() ==
             static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_IGMP))
    {
        ntsa::IgmpPacket& igmp = d_payload.makeIgmp();

        error = igmp.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.protocol() ==
             static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_TCP))
    {
        ntsa::TcpPacket& tcp = d_payload.makeTcp();

        error = tcp.decode(context, decoder, options);
        if (error) {
            return error;
        }
    }
    else if (d_header.protocol() ==
             static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_UDP))
    {
        ntsa::UdpPacket& udp = d_payload.makeUdp();

        error = udp.decode(context, decoder, options);
        if (error) {
            return error;
        }
    }
    else {
        error = decoder->decodeRaw(&d_payload.makeRaw(),
                                   decoder->size() - decoder->position());
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Ipv4Packet::encode(ntsa::PacketEncoderContext*       context,
                               ntsa::PacketEncoder*              encoder,
                               const ntsa::PacketEncoderOptions& options) const
{
    ntsa::Error error;

    if (options.sourceIpAddress().has_value()) {
        if (options.sourceIpAddress().value().isV4()) {
            if (d_header.sourceAddress() !=
                options.sourceIpAddress().value().v4())
            {
                return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setSourceIpAddress(ntsa::IpAddress(d_header.sourceAddress()));

    if (options.destinationIpAddress().has_value()) {
        if (options.destinationIpAddress().value().isV4()) {
            if (d_header.destinationAddress() !=
                options.destinationIpAddress().value().v4())
            {
                return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setDestinationIpAddress(
        ntsa::IpAddress(d_header.destinationAddress()));

    const bsl::size_t headerPosition = encoder->position();

    const bsl::size_t headerLength =
        static_cast<bsl::size_t>(ntsa::Ipv4Header::k_MIN_HEADER_LENGTH);

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

    const bsl::size_t payloadPosition = encoder->position();

    if (d_payload.isIcmp()) {
        if (d_header.protocol() !=
            static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_ICMP))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpPacket& icmp = d_payload.icmp();

        error = icmp.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isIgmp()) {
        if (d_header.protocol() !=
            static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_IGMP))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IgmpPacket& igmp = d_payload.igmp();

        error = igmp.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isTcp()) {
        if (d_header.protocol() !=
            static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_TCP))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::TcpPacket& tcp = d_payload.tcp();

        error = tcp.encode(context, encoder, options);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isUdp()) {
        if (d_header.protocol() !=
            static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_UDP))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::UdpPacket& udp = d_payload.udp();

        error = udp.encode(context, encoder, options);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isRaw()) {
        if (d_payload.raw().size() > 0) {
            error =
                encoder->encodeRaw(d_payload.raw(), d_payload.raw().size());
            if (error) {
                return error;
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    const bsl::size_t finalPosition = encoder->position();

    const bsl::size_t payloadLength = finalPosition - payloadPosition;

    const bsl::size_t packetLength =
        headerLength + extensionLength + payloadLength;

    ntsa::Ipv4Header header = d_header;
    header.setHeaderLength(headerLength + extensionLength);
    header.setPacketLength(packetLength);
    header.setChecksum(0);

    if (!options.ignoreChecksum()) {
        ntsa::Ipv4Checksum checksum;
        checksum.add(&header, header.headerLength());

        error = encoder->seek(extensionPosition);
        if (error) {
            return error;
        }

        checksum.add(encoder->next(), extensionLength);

        header.setChecksum(checksum.value());
    }

    error = encoder->seek(headerPosition);
    if (error) {
        return error;
    }

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

bool Ipv4Packet::equals(const Ipv4Packet& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_payload != other.d_payload) {
        return false;
    }

    return true;
}

bsl::ostream& Ipv4Packet::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("ipv4", d_header);

    if (d_payload.isIcmp()) {
        printer.printAttribute("icmp", d_payload.icmp());
    }
    else if (d_payload.isTcp()) {
        printer.printAttribute("tcp", d_payload.tcp());
    }
    else if (d_payload.isUdp()) {
        printer.printAttribute("udp", d_payload.udp());
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
