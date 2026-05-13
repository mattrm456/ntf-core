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

ntsa::Error Ipv4Packet::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    error = d_header.decode(decoder);
    if (error) {
        return error;
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
        static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_TCP))
    {
        ntsa::TcpPacket& tcp = d_payload.makeTcp();

        error = tcp.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.protocol() ==
             static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_UDP))
    {
        ntsa::UdpPacket& udp = d_payload.makeUdp();

        error = udp.decode(decoder);
        if (error) {
            return error;
        }
    }
    else {
        error = decoder->decodeRaw(
            &d_payload.makeRaw(), decoder->size() - decoder->position());
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Ipv4Packet::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    ntsa::Ipv4Header header = d_header;

    header.setChecksum(0);

    ntsa::Ipv4Checksum checksum;
    checksum.add(&header, header.headerLength());

    header.setChecksum(checksum.value());

    error = d_header.encode(encoder);
    if (error) {
        return error;
    }

    if (d_payload.isIcmp()) {
        if (d_header.protocol() !=
            static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_ICMP))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpPacket& icmp = d_payload.icmp();

        error = icmp.encode(encoder,
                            d_header.sourceAddress(),
                            d_header.destinationAddress());
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

        error = tcp.encode(encoder,
                           d_header.sourceAddress(),
                           d_header.destinationAddress());
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

        error = udp.encode(encoder,
                           d_header.sourceAddress(),
                           d_header.destinationAddress());
        if (error) {
            return error;
        }
    }
    else if (d_payload.isRaw()) {
        if (d_payload.raw().size() > 0) {
            error = encoder->encodeRaw(d_payload.raw(), d_payload.raw().size());
            if (error) {
                return error;
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Ipv4Packet::decode(const bdlbb::BlobBuffer& buffer,
                               bsl::size_t              offset)
{
    ntsa::Error error;

    error = d_header.decode(buffer, offset);
    if (error) {
        return error;
    }

    const bsl::size_t headerLength =
        static_cast<bsl::size_t>(d_header.headerLength());

    const bsl::size_t packetSize =
        static_cast<bsl::size_t>(d_header.packetLength());

    if (d_header.protocol() ==
        static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_ICMP))
    {
        ntsa::IcmpPacket& icmp = d_payload.makeIcmp();

        error = icmp.decode(buffer, offset + headerLength, packetSize);
        if (error) {
            return error;
        }
    }
    else if (d_header.protocol() ==
        static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_TCP))
    {
        ntsa::TcpPacket& tcp = d_payload.makeTcp();

        error = tcp.decode(buffer, offset + headerLength, packetSize);
        if (error) {
            return error;
        }
    }
    else if (d_header.protocol() ==
             static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_UDP))
    {
        ntsa::UdpPacket& udp = d_payload.makeUdp();

        error = udp.decode(buffer, offset + headerLength, packetSize);
        if (error) {
            return error;
        }
    }
    else {
        bdlbb::BlobBuffer& blobBuffer = d_payload.makeRaw();
        blobBuffer.reset(
            bsl::shared_ptr<char>(buffer.buffer(),
                                  buffer.data() + offset + headerLength),
            static_cast<int>(buffer.size() - offset - headerLength));
    }

    return ntsa::Error();
}

ntsa::Error Ipv4Packet::encode(bdlbb::BlobBuffer* buffer,
                               bsl::size_t        offset) const
{
    ntsa::Error error;

    ntsa::Ipv4Header header = d_header;

    header.setChecksum(0);

    ntsa::Ipv4Checksum checksum;
    checksum.add(&header, header.headerLength());

    header.setChecksum(checksum.value());

    error = d_header.encode(buffer, offset);
    if (error) {
        return error;
    }

    if (d_payload.isIcmp()) {
        if (d_header.protocol() !=
            static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_ICMP))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpPacket& icmp = d_payload.icmp();

        error = icmp.encode(buffer,
                            offset + d_header.headerLength(),
                            d_header.sourceAddress(),
                            d_header.destinationAddress());
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

        error = tcp.encode(buffer,
                           offset + d_header.headerLength(),
                           d_header.sourceAddress(),
                           d_header.destinationAddress());
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

        error = udp.encode(buffer,
                           offset + d_header.headerLength(),
                           d_header.sourceAddress(),
                           d_header.destinationAddress());
        if (error) {
            return error;
        }
    }
    else if (d_payload.isRaw()) {
        if (d_payload.raw().size() > 0) {
            if (offset + d_header.headerLength() + d_payload.raw().size() >
                static_cast<bsl::size_t>(buffer->size()))
            {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::memcpy(buffer->data() + offset + d_header.headerLength(),
                        d_payload.raw().data(),
                        static_cast<bsl::size_t>(d_payload.raw().size()));
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
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
