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

#include <ntsa_ipv4header.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4header_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsls_byteorder.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error Ipv4Header::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    const bsl::uint8_t control = *decoder->next();
    const bsl::uint8_t version = (control & 0xF0) >> 4;

    const bsl::uint8_t headerLength = (control & 0x0F) * sizeof(bsl::uint32_t);

    if (version != 4) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (headerLength < k_MIN_HEADER_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (headerLength > k_MAX_HEADER_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = decoder->decodeRaw(this, sizeof *this);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Ipv4Header::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeRaw(this, sizeof *this);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& Ipv4Header::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("version", static_cast<bsl::size_t>(d_version));

    const bsl::size_t headerLength =
        static_cast<bsl::size_t>(this->headerLength());

    printer.printAttribute("headerLength", headerLength);

    const bsl::size_t packetLength =
        static_cast<bsl::size_t>(this->packetLength());

    printer.printAttribute("packetLength", packetLength);

    printer.printAttribute("sourceAddress", d_sourceAddress);
    printer.printAttribute("destinationAddress", d_destinationAddress);

    const bsl::size_t id = static_cast<bsl::size_t>(this->id());
    if (id != 0) {
        printer.printAttribute("id", id);
    }

    const bool preserve = this->preserve();
    if (preserve) {
        printer.printAttribute("preserve", 1);
    }

    const bool more = this->more();
    if (more) {
        printer.printAttribute("more", 1);
    }

    const bsl::size_t fragmentOffset =
        static_cast<bsl::size_t>(this->fragmentOffset());
    if (fragmentOffset != 0) {
        printer.printAttribute("fragmentOffset", fragmentOffset);
    }

    const bsl::size_t timeToLive =
        static_cast<bsl::size_t>(this->timeToLive());

    printer.printAttribute("timeToLive", timeToLive);

    printer.printForeign(this->protocol(),
                         &Ipv4Header::printProtocol,
                         "protocol");

    const bsl::size_t checksum = static_cast<bsl::size_t>(this->checksum());

    printer.printAttribute("checksum", checksum);

    printer.end();

    return stream;
}

bsl::ostream& Ipv4Header::printProtocol(bsl::ostream& stream,
                                        bsl::uint8_t  protocol,
                                        int           level,
                                        int           spacesPerLevel)
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    switch (protocol) {
    case k_PROTOCOL_TCP:
        stream << "TCP";
        break;
    case k_PROTOCOL_UDP:
        stream << "UDP";
        break;
    case k_PROTOCOL_ICMP:
        stream << "ICMP";
        break;
    case k_PROTOCOL_IGMP:
        stream << "IGMP";
        break;
    default:
        stream << protocol;
    }

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
