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

#include <ntsa_icmppacket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_icmppacket_cpp, "$Id$ $CSID$")

#include <ntsa_icmpchecksum.h>
#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IcmpPacket::decode(ntsa::PacketDecoder*     decoder,
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

    if (d_header.type() == ntsa::IcmpType::e_ECHO_REQUEST) {
        ntsa::IcmpEchoRequest& echoRequest = d_payload.makeEchoRequest();

        error = echoRequest.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IcmpType::e_ECHO_RESPONSE) {
        ntsa::IcmpEchoResponse& echoResponse = d_payload.makeEchoResponse();

        error = echoResponse.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IcmpType::e_ROUTER_REQUEST) {
        ntsa::IcmpRouterRequest& routerRequest = d_payload.makeRouterRequest();

        error = routerRequest.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IcmpType::e_ROUTER_RESPONSE) {
        ntsa::IcmpRouterResponse& routerResponse =
            d_payload.makeRouterResponse();

        error = routerResponse.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IcmpType::e_REDIRECT) {
        ntsa::IcmpRedirect& redirect = d_payload.makeRedirect();

        error = redirect.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IcmpType::e_UNREACHABLE) {
        ntsa::IcmpUnreachable& unreachable = d_payload.makeUnreachable();

        error = unreachable.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IcmpType::e_TIMEOUT) {
        ntsa::IcmpTimeout& timeout = d_payload.makeTimeout();

        error = timeout.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.type() == ntsa::IcmpType::e_PROBLEM) {
        ntsa::IcmpProblem& problem = d_payload.makeProblem();

        error = problem.decode(decoder);
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

    ntsa::IcmpChecksum checksum;
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

ntsa::Error IcmpPacket::decode(ntsa::PacketDecoder*     decoder,
                               const ntsa::Ipv6Address& sourceAddress,
                               const ntsa::Ipv6Address& destinationAddress)
{
    NTSCFG_WARNING_UNUSED(decoder);
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error IcmpPacket::encode(
    ntsa::PacketEncoder*     encoder,
    const ntsa::Ipv4Address& sourceAddress,
    const ntsa::Ipv4Address& destinationAddress) const
{
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    ntsa::Error error;

    ntsa::IcmpHeader header = d_header;
    header.setChecksum(0);

    const bsl::uint8_t* packetData = encoder->next();

    const bsl::size_t headerPosition = encoder->position();

    error = header.encode(encoder);
    if (error) {
        return error;
    }

    if (d_payload.isEchoRequest()) {
        if (d_header.type() != ntsa::IcmpType::e_ECHO_REQUEST) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpEchoRequest& echoRequest = d_payload.echoRequest();

        error = echoRequest.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isEchoResponse()) {
        if (d_header.type() != ntsa::IcmpType::e_ECHO_RESPONSE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpEchoResponse& echoResponse = d_payload.echoResponse();

        error = echoResponse.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isRouterRequest()) {
        if (d_header.type() != ntsa::IcmpType::e_ROUTER_REQUEST) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpRouterRequest& routerRequest =
            d_payload.routerRequest();

        error = routerRequest.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isRouterResponse()) {
        if (d_header.type() != ntsa::IcmpType::e_ROUTER_RESPONSE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpRouterResponse& routerResponse =
            d_payload.routerResponse();

        error = routerResponse.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isRedirect()) {
        if (d_header.type() != ntsa::IcmpType::e_REDIRECT) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpRedirect& redirect = d_payload.redirect();

        error = redirect.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isUnreachable()) {
        if (d_header.type() != ntsa::IcmpType::e_UNREACHABLE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpUnreachable& unreachable = d_payload.unreachable();

        error = unreachable.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isTimeout()) {
        if (d_header.type() != ntsa::IcmpType::e_TIMEOUT) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpTimeout& timeout = d_payload.timeout();

        error = timeout.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isProblem()) {
        if (d_header.type() != ntsa::IcmpType::e_PROBLEM) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::IcmpProblem& problem = d_payload.problem();

        error = problem.encode(encoder);
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

    ntsa::IcmpChecksum checksum;
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

ntsa::Error IcmpPacket::encode(
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

ntsa::Error IcmpPacket::decode(const bdlbb::BlobBuffer& buffer,
                               bsl::size_t              offset,
                               bsl::size_t              packetSize)
{
    NTSCFG_WARNING_UNUSED(buffer);
    NTSCFG_WARNING_UNUSED(offset);
    NTSCFG_WARNING_UNUSED(packetSize);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error IcmpPacket::encode(
    bdlbb::BlobBuffer*       buffer,
    bsl::size_t              offset,
    const ntsa::Ipv4Address& sourceAddress,
    const ntsa::Ipv4Address& destinationAddress) const
{
    NTSCFG_WARNING_UNUSED(buffer);
    NTSCFG_WARNING_UNUSED(offset);
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error IcmpPacket::encode(
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

bool IcmpPacket::equals(const IcmpPacket& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_payload != other.d_payload) {
        return false;
    }

    return true;
}

bsl::ostream& IcmpPacket::print(bsl::ostream& stream,
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
