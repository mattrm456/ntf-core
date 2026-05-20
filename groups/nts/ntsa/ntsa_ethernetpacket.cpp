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

#include <ntsa_ethernetpacket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetpacket_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error EthernetPacket::decode(ntsa::PacketDecoderContext*       context,
                                   ntsa::PacketDecoder*              decoder,
                                   const ntsa::PacketDecoderOptions& options)
{
    ntsa::Error error;

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    if (options.sourceEthernetAddress().has_value()) {
        if (d_header.source() != options.sourceEthernetAddress().value()) {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setSourceEthernetAddress(d_header.source());

    if (options.destinationEthernetAddress().has_value()) {
        if (d_header.destination() !=
            options.destinationEthernetAddress().value())
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setDestinationEthernetAddress(d_header.destination());

    if (d_header.protocol() == ntsa::EthernetProtocol::e_IPV4) {
        ntsa::Ipv4Packet& ipv4 = d_payload.makeIpv4();

        error = ipv4.decode(context, decoder, options);
        if (error) {
            return error;
        }
    }
    else if (d_header.protocol() == ntsa::EthernetProtocol::e_IPV6) {
        ntsa::Ipv6Packet& ipv6 = d_payload.makeIpv6();

        error = ipv6.decode(context, decoder, options);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

ntsa::Error EthernetPacket::encode(
    ntsa::PacketEncoderContext*       context,
    ntsa::PacketEncoder*              encoder,
    const ntsa::PacketEncoderOptions& options) const
{
    ntsa::Error error;

    if (options.sourceEthernetAddress().has_value()) {
        if (d_header.source() != options.sourceEthernetAddress()) {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setSourceEthernetAddress(d_header.source());

    if (options.destinationEthernetAddress().has_value()) {
        if (d_header.destination() != options.destinationEthernetAddress()) {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
    }

    context->setDestinationEthernetAddress(d_header.destination());

    error = d_header.encode(encoder);
    if (error) {
        return error;
    }

    if (d_payload.isIpv4()) {
        if (d_header.protocol() != ntsa::EthernetProtocol::e_IPV4) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::Ipv4Packet& ipv4 = d_payload.ipv4();

        error = ipv4.encode(context, encoder, options);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isIpv6()) {
        if (d_header.protocol() != ntsa::EthernetProtocol::e_IPV6) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::Ipv6Packet& ipv6 = d_payload.ipv6();

        error = ipv6.encode(context, encoder, options);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

bool EthernetPacket::equals(const EthernetPacket& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_payload != other.d_payload) {
        return false;
    }

    return true;
}

bsl::ostream& EthernetPacket::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("header", d_header);
    printer.printAttribute("payload", d_payload);
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
