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

#include <ntsa_igmppacket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_igmppacket_cpp, "$Id$ $CSID$")

#include <ntsa_igmpchecksum.h>
#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IgmpPacket::decode(ntsa::PacketDecoder*     decoder,
                               const ntsa::Ipv4Address& sourceAddress,
                               const ntsa::Ipv4Address& destinationAddress)
{
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    ntsa::Error error;

    const bsl::uint8_t* packetData = decoder->next();

    const bsl::size_t headerPosition = decoder->position();

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    if (d_header.type() == ntsa::IgmpType::e_REPORT_V2) {
        ntsa::IgmpJoin& join = d_payload.makeJoin();

        error = join.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IgmpType::e_LEAVE) {
        ntsa::IgmpLeave& leave = d_payload.makeLeave();

        error = leave.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IgmpType::e_QUERY) {
        ntsa::IgmpQuery& query = d_payload.makeQuery();

        error = query.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IgmpType::e_REPORT_V3) {
        ntsa::IgmpReport& report = d_payload.makeReport();

        error = report.decode(decoder);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    const bsl::size_t finalPosition = decoder->position();

    const bsl::size_t packetLength =
        static_cast<bsl::size_t>(finalPosition - headerPosition);

    ntsa::IgmpChecksum checksum;
    checksum.add(packetData, packetLength);

    const bsl::uint16_t checksumValue = checksum.value();

    if (checksumValue != 0xFFFF) {
        BSLS_LOG_WARN("Invalid checksum: expected %zu but found %zu",
                      static_cast<bsl::size_t>(d_header.checksum()),
                      static_cast<bsl::size_t>(checksumValue));
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error IgmpPacket::decode(ntsa::PacketDecoder*     decoder,
                               const ntsa::Ipv6Address& sourceAddress,
                               const ntsa::Ipv6Address& destinationAddress)
{
    NTSCFG_WARNING_UNUSED(decoder);
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error IgmpPacket::encode(
    ntsa::PacketEncoder*     encoder,
    const ntsa::Ipv4Address& sourceAddress,
    const ntsa::Ipv4Address& destinationAddress) const
{
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    ntsa::Error error;

    ntsa::IgmpHeader header = d_header;
    header.setChecksum(0);

    const bsl::uint8_t* packetData = encoder->next();

    const bsl::size_t headerPosition = encoder->position();

    error = header.encode(encoder);
    if (error) {
        return error;
    }

    if (d_payload.isJoin()) {
        if (d_header.type() != ntsa::IgmpType::e_REPORT_V2) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IgmpJoin& join = d_payload.join();

        error = join.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isLeave()) {
        if (d_header.type() != ntsa::IgmpType::e_LEAVE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IgmpLeave& leave = d_payload.leave();

        error = leave.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isQuery()) {
        if (d_header.type() != ntsa::IgmpType::e_QUERY) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IgmpQuery& query = d_payload.query();

        error = query.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isReport()) {
        if (d_header.type() != ntsa::IgmpType::e_REPORT_V3) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IgmpReport& report = d_payload.report();

        error = report.encode(encoder);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    const bsl::size_t finalPosition = encoder->position();

    const bsl::size_t packetLength =
        static_cast<bsl::size_t>(finalPosition - headerPosition);

    ntsa::IgmpChecksum checksum;
    checksum.add(packetData, packetLength);

    header.setChecksum(checksum.value());

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

ntsa::Error IgmpPacket::encode(
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

bool IgmpPacket::equals(const IgmpPacket& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_payload != other.d_payload) {
        return false;
    }

    return true;
}

bsl::ostream& IgmpPacket::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    d_header.print(&printer);
    d_payload.print(&printer);
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
