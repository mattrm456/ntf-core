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

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error Ipv4Packet::decode(const bdlbb::BlobBuffer& source)
{
    ntsa::Error error;

    if (source.size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_header.decode(source);
    if (error) {
        return error;
    }

    const bsl::size_t offset =
        static_cast<bsl::size_t>(d_header.headerLength());

    const bsl::size_t packetSize =
        static_cast<bsl::size_t>(d_header.packetLength());

    if (offset > packetSize) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset > static_cast<bsl::size_t>(source.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_header.protocol() ==
        static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_TCP))
    {
        ntsa::TcpPacket& tcp = d_payload.makeTcp();

        error = tcp.decode(source, offset, packetSize);
        if (error) {
            return error;
        }
    }
    else if (d_header.protocol() ==
             static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_UDP))
    {
        ntsa::UdpPacket& udp = d_payload.makeUdp();

        error = udp.decode(source, offset, packetSize);
        if (error) {
            return error;
        }
    }
    else {
        bdlbb::BlobBuffer& blobBuffer = d_payload.makeRaw();
        blobBuffer.reset(
            bsl::shared_ptr<char>(source.buffer(), source.data() + offset),
            static_cast<int>(source.size() - offset));
    }

    return ntsa::Error();
}

ntsa::Error Ipv4Packet::encode(bdlbb::BlobBuffer* destination) const
{
    NTSCFG_WARNING_UNUSED(destination);

    ntsa::Error error;

    error = d_header.encode(destination, 0);
    if (error) {
        return error;
    }

    if (d_payload.isTcp()) {
        if (d_header.protocol() !=
            static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_TCP))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        // TODO
    }
    else if (d_payload.isUdp()) {
        if (d_header.protocol() !=
            static_cast<bsl::uint8_t>(ntsa::Ipv4Header::k_PROTOCOL_UDP))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::UdpPacket& udp = d_payload.udp();

        error = udp.encode(destination,
                           d_header.headerLength(),
                           d_header.packetLength(),
                           d_header.sourceAddress(),
                           d_header.destinationAddress());
        if (error) {
            return error;
        }
    }
    else if (d_payload.isRaw()) {
        if (d_payload.raw().size() > 0) {
            if (d_header.headerLength() + d_payload.raw().size() >
                static_cast<bsl::size_t>(destination->size()))
            {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::memcpy(destination->data() + d_header.headerLength(),
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

    if (d_payload.isTcp()) {
        printer.printAttribute("tcp", d_payload.tcp().header());
    }
    else if (d_payload.isUdp()) {
        printer.printAttribute("udp", d_payload.udp().header());
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
