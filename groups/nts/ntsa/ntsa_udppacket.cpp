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

#include <ntsa_udppacket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_udppacket_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error UdpPacket::decode(const bdlbb::BlobBuffer& source,
                              bsl::size_t              offset,
                              bsl::size_t              packetSize)
{
    ntsa::Error error;

    if (source.size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset > packetSize) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset > static_cast<bsl::size_t>(source.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_header.decode(source.data() + offset, packetSize - offset);
    if (error) {
        return error;
    }

    const bsl::size_t headerLength = d_header.headerLength();

    if (offset + headerLength > static_cast<bsl::size_t>(source.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset + headerLength != packetSize) {
        d_payload.reset(
            bsl::shared_ptr<char>(source.buffer(),
                                  source.data() + offset + headerLength),
            static_cast<int>(source.size() - offset - headerLength));
    }

    return ntsa::Error();
}

ntsa::Error UdpPacket::encode(
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

    UdpHeader header = d_header;

    header.setChecksum(0);

    struct PseudoHeader {
        bsl::uint32_t         d_sourceAddress;
        bsl::uint32_t         d_destinationAddress;

        #if 0
        bsl::uint8_t          d_protocol;
        bsl::uint8_t          d_placeholder;
        #endif
        bdlb::BigEndianUint16 d_protocol;
        
        bdlb::BigEndianUint16 d_length;
    };

    BSLMF_ASSERT(sizeof(PseudoHeader) == 12);

    PseudoHeader psh;
    psh.d_sourceAddress = sourceAddress.value();
    psh.d_destinationAddress = destinationAddress.value();
    // psh.d_placeholder = 0;
    psh.d_protocol = static_cast<bsl::uint8_t>(UdpHeader::k_PROTOCOL_UDP);
    psh.d_length = static_cast<bsl::uint16_t>(d_header.packetLength());

    UdpChecksum checksum;
    checksum.add(&psh, sizeof psh);
    checksum.add(&header, sizeof header);
    if (d_payload.size() > 0) {
        checksum.add(d_payload.data(), 
                     static_cast<bsl::size_t>(d_payload.size()));
    }

    const bsl::uint16_t checksumValue = checksum.value();

    BSLS_LOG_DEBUG("Checksum = %d", (int)(checksumValue));

    header.setChecksum(checksumValue);

    error = header.encode(buffer, offset);
    if (error) {
        return error;
    }

    if (d_payload.size() > 0) {
        const bsl::size_t headerLength = d_header.headerLength();

        if (offset + headerLength > bufferCapacity) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bsl::memcpy(bufferData + offset + headerLength,
                    d_payload.data(),
                    static_cast<bsl::size_t>(d_payload.size()));
    }

    return ntsa::Error();
}

ntsa::Error UdpPacket::encode(
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

bool UdpPacket::equals(const UdpPacket& other) const
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

bsl::ostream& UdpPacket::print(bsl::ostream& stream,
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
