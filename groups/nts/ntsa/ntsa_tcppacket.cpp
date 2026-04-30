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

#include <ntsa_tcppacket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcppacket_cpp, "$Id$ $CSID$")

#include <ntsa_tcpchecksum.h>
#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {


ntsa::Error TcpPacket::decode(const bdlbb::BlobBuffer& buffer,
                              bsl::size_t              offset,
                              bsl::size_t              packetSize)
{
    ntsa::Error error;

    if (buffer.data() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const char* bufferData = buffer.data();

    if (buffer.size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t bufferSize = static_cast<bsl::size_t>(buffer.size());

    error = d_header.decode(buffer, offset, packetSize);
    if (error) {
        return error;
    }

    const bsl::size_t headerLength =
        static_cast<bsl::size_t>(ntsa::TcpHeader::k_MIN_HEADER_LENGTH);

    if (d_header.dataOffset() < headerLength) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t extensionLength = d_header.dataOffset() - headerLength;

    if (extensionLength > 0) {
        d_extension.makeValue();

        error = d_extension.value().decode(buffer, offset + headerLength, extensionLength);
        if (error) {
            return error;
        }
    }

    if (offset + headerLength + extensionLength >
        static_cast<bsl::size_t>(buffer.size()))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset + headerLength + extensionLength != packetSize) {
        const bsl::size_t payloadOffset =
            offset + headerLength + extensionLength;

        const bsl::size_t payloadSize =
            static_cast<bsl::size_t>(
                bufferSize - offset - headerLength - extensionLength);

        if (payloadOffset + payloadSize > bufferSize) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_payload.reset(
            bsl::shared_ptr<char>(
                buffer.buffer(),
                const_cast<char*>(bufferData + payloadOffset)),
            static_cast<int>(payloadSize));
    }

    return ntsa::Error();
}

ntsa::Error TcpPacket::encode(
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

    const bsl::size_t headerLength =
        static_cast<bsl::size_t>(ntsa::TcpHeader::k_MIN_HEADER_LENGTH);

    const bsl::size_t extensionLength =
        d_extension.has_value() ? d_extension.value().size() : 0;

    const bsl::size_t packetLength =
        headerLength + extensionLength +
        static_cast<bsl::size_t>(d_payload.size());

    ntsa::TcpHeader header = d_header;

    header.setChecksum(0);

    ntsa::TcpChecksum checksum;
    checksum.add(sourceAddress, destinationAddress, packetLength);
    checksum.add(&header, headerLength);
    if (extensionLength > 0) {
        checksum.add(d_extension.value().data(), extensionLength);
    }

    if (d_payload.size() > 0) {
        checksum.add(d_payload.data(),
                     static_cast<bsl::size_t>(d_payload.size()));
    }

    header.setChecksum(checksum.value());

    error = header.encode(buffer, offset);
    if (error) {
        return error;
    }

    if (extensionLength > 0) {
        error = d_extension.value().encode(buffer, offset + headerLength);
        if (error) {
            return error;
        }
    }

    if (d_payload.size() > 0) {
        const bsl::size_t payloadOffset =
            offset + headerLength + extensionLength;

        const bsl::size_t payloadSize =
            static_cast<bsl::size_t>(
                d_payload.size());

        if (payloadOffset + payloadSize > bufferCapacity) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bsl::memcpy(bufferData + payloadOffset,
                    d_payload.data(),
                    payloadSize);
    }

    return ntsa::Error();
}

ntsa::Error TcpPacket::encode(
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

bool TcpPacket::equals(const TcpPacket& other) const
{
    if (d_header != other.d_header) {
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

bsl::ostream& TcpPacket::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("header", d_header);
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
