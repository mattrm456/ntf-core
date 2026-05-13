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

ntsa::Error IcmpPacket::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    if (d_header.code() == ntsa::IcmpType::e_ECHO) {
        ntsa::IcmpPing& ping = d_payload.makeEcho();

        error = ping.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_ECHO_REPLY) {
        ntsa::IcmpPong& pong = d_payload.makeEchoReply();

        error = pong.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_DESTINATION_UNREACHABLE) {
        ntsa::IcmpUnreachable& unreachable =
            d_payload.makeDestinationUnreachable();

        error = unreachable.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_REDIRECT) {
        ntsa::IcmpRedirect& redirect = d_payload.makeRedirect();

        error = redirect.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_TIME_EXCEEDED) {
        ntsa::IcmpTimeout& timeout = d_payload.makeTimeExceeded();

        error = timeout.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_PARAMETER_PROBLEM) {
        ntsa::IcmpProblem& problem = d_payload.makeParameterProblem();

        error = problem.decode(decoder);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

ntsa::Error IcmpPacket::encode(
    ntsa::PacketEncoder*     encoder,
    const ntsa::Ipv4Address& sourceAddress,
    const ntsa::Ipv4Address& destinationAddress) const
{
    NTSCFG_WARNING_UNUSED(encoder);
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
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
    ntsa::Error error;

    if (buffer.size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset > packetSize) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset > static_cast<bsl::size_t>(buffer.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_header.decode(buffer, offset, packetSize);
    if (error) {
        return error;
    }

    const bsl::size_t headerLength = ntsa::IcmpHeader::k_LENGTH;

    if (offset + headerLength > static_cast<bsl::size_t>(buffer.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset + headerLength == packetSize) {
        return ntsa::Error();
    }

    if (d_header.code() == ntsa::IcmpType::e_ECHO) {
        ntsa::IcmpPing& ping = d_payload.makeEcho();

        error = ping.decode(buffer, offset + headerLength, packetSize);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_ECHO_REPLY) {
        ntsa::IcmpPong& pong = d_payload.makeEchoReply();

        error = pong.decode(buffer, offset + headerLength, packetSize);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_DESTINATION_UNREACHABLE) {
        ntsa::IcmpUnreachable& unreachable =
            d_payload.makeDestinationUnreachable();

        error = unreachable.decode(buffer, offset + headerLength, packetSize);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_REDIRECT) {
        ntsa::IcmpRedirect& redirect = d_payload.makeRedirect();

        error = redirect.decode(buffer, offset + headerLength, packetSize);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_TIME_EXCEEDED) {
        ntsa::IcmpTimeout& timeout = d_payload.makeTimeExceeded();

        error = timeout.decode(buffer, offset + headerLength, packetSize);
        if (error) {
            return error;
        }
    }
    else if (d_header.code() == ntsa::IcmpType::e_PARAMETER_PROBLEM) {
        ntsa::IcmpProblem& problem = d_payload.makeParameterProblem();

        error = problem.decode(buffer, offset + headerLength, packetSize);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

ntsa::Error IcmpPacket::encode(
    bdlbb::BlobBuffer*       buffer,
    bsl::size_t              offset,
    const ntsa::Ipv4Address& sourceAddress,
    const ntsa::Ipv4Address& destinationAddress) const
{
    ntsa::Error error;

    if (buffer->data() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    char* bufferData = buffer->data();

    if (buffer->size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t bufferCapacity =
        static_cast<bsl::size_t>(buffer->size());

    if (offset > bufferCapacity) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // MRM
    NTSCFG_WARNING_UNUSED(sourceAddress);
    NTSCFG_WARNING_UNUSED(destinationAddress);
    NTSCFG_WARNING_UNUSED(bufferData);
#if 0
    const bsl::size_t headerLength = ntsa::IcmpHeader::k_LENGTH;

    const bsl::size_t packetLength =
        headerLength + static_cast<bsl::size_t>(d_payload.size());

    ntsa::IcmpHeader header = d_header;

    header.setChecksum(0);


    ntsa::IcmpChecksum checksum;
    checksum.add(sourceAddress, destinationAddress, packetLength);
    checksum.add(&header, header.headerLength());
    if (d_payload.size() > 0) {
        checksum.add(d_payload.data(),
                     static_cast<bsl::size_t>(d_payload.size()));
    }

    header.setChecksum(checksum.value());

    error = header.encode(buffer, offset);
    if (error) {
        return error;
    }

    if (d_payload.size() > 0) {
        if (offset + headerLength > bufferCapacity) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bsl::memcpy(bufferData + offset + headerLength,
                    d_payload.data(),
                    static_cast<bsl::size_t>(d_payload.size()));
    }
#endif

    return ntsa::Error();
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
