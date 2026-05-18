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

#include <ntsa_ethernetheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetheader_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error EthernetHeader::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;
    int         rc;

    error = decoder->decodeRaw(&d_destination, sizeof d_destination);
    if (error) {
        return error;
    }

    error = decoder->decodeRaw(&d_source, sizeof d_source);
    if (error) {
        return error;
    }

    bsl::uint16_t type;
    error = decoder->decodeUint16(&type);
    if (error) {
        return error;
    }

    if (type == 0x8100) {
        error = decoder->decodeRaw(
            &d_tag.makeValue(),
            static_cast<bsl::size_t>(ntsa::EthernetTag::k_LENGTH));
        if (error) {
            return error;
        }

        error = decoder->decodeUint16(&type);
        if (error) {
            return error;
        }
    }

    rc = ntsa::EthernetProtocol::fromInt(&d_protocol, static_cast<int>(type));
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error EthernetHeader::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeRaw(&d_destination, sizeof d_destination);
    if (error) {
        return error;
    }

    error = encoder->encodeRaw(&d_source, sizeof d_source);
    if (error) {
        return error;
    }

    if (d_tag.has_value()) {
        error = encoder->encodeUint16(0x8100);
        if (error) {
            return error;
        }

        error = encoder->encodeRaw(
            &d_tag.value(),
            static_cast<bsl::size_t>(ntsa::EthernetTag::k_LENGTH));
        if (error) {
            return error;
        }
    }

    const bsl::uint16_t type = static_cast<bsl::uint16_t>(d_protocol);

    error = encoder->encodeUint16(type);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bool EthernetHeader::equals(const EthernetHeader& other) const
{
    return d_source == other.d_source &&
           d_destination == other.d_destination && d_tag == other.d_tag &&
           d_protocol == other.d_protocol;
}

bool EthernetHeader::less(const EthernetHeader& other) const
{
    if (d_source < other.d_source) {
        return true;
    }

    if (other.d_source < d_source) {
        return false;
    }

    if (d_destination < other.d_destination) {
        return true;
    }

    if (other.d_destination < d_destination) {
        return false;
    }

    if (d_tag < other.d_tag) {
        return true;
    }

    if (other.d_tag < d_tag) {
        return false;
    }

    return d_protocol < other.d_protocol;
}

bsl::ostream& EthernetHeader::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("source", d_source);
    printer.printAttribute("destination", d_destination);
    printer.printAttribute("protocol", d_protocol);
    if (d_tag.has_value()) {
        printer.printAttribute("tag", d_tag.value());
    }
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
