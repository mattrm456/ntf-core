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

#include <ntsa_icmppong.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_icmppong_cpp, "$Id$ $CSID$")

#include <bdlb_print.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IcmpPong::decode(const bdlbb::BlobBuffer& buffer,
                             bsl::size_t              offset,
                             bsl::size_t              packetSize)
{
    NTSCFG_WARNING_UNUSED(packetSize);

    reset();

    if (buffer.data() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (buffer.size() < offset) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const char* bufferData = buffer.data() + offset;

    const bsl::size_t bufferSize =
        static_cast<bsl::size_t>(buffer.size() - offset);

    if (bufferSize < sizeof(bdlb::BigEndianUint16)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTSCFG_MEMORY_COPY(&d_identifier,
                       bufferData + offset,
                       sizeof(bdlb::BigEndianUint16));

    bufferData += sizeof(bdlb::BigEndianUint16);
    bufferSize -= sizeof(bdlb::BigEndianUint16);

    if (bufferSize < sizeof(bdlb::BigEndianUint16)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTSCFG_MEMORY_COPY(&d_sequenceNumber,
                       bufferData + offset + sizeof(bdlb::BigEndianUint16),
                       sizeof(bdlb::BigEndianUint16));

    bufferData += sizeof(bdlb::BigEndianUint16);
    bufferSize -= sizeof(bdlb::BigEndianUint16);

    if (offset + static_cast<bsl::size_t>(k_LENGTH) < packetSize) {
        const bsl::size_t dataOffset =
            static_cast<bsl::size_t>(offset + k_LENGTH);

        const bsl::size_t dataSize =
            static_cast<bsl::size_t>(packetSize - offset - k_LENGTH);

        if (dataOffset + dataSize > bufferSize) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_data.reset(
            bsl::shared_ptr<char>(buffer.buffer(),
                                  const_cast<char*>(bufferData + dataOffset)),
            static_cast<int>(dataSize));
    }

    return ntsa::Error();
}

ntsa::Error IcmpPong::encode(bdlbb::BlobBuffer* buffer,
                             bsl::size_t        offset) const
{
    if (buffer->data() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    char* bufferData = buffer->data();

    if (buffer->size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t bufferCapacity =
        static_cast<bsl::size_t>(buffer->size());

    if (offset + static_cast<bsl::size_t>(k_LENGTH) > bufferCapacity) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTSCFG_MEMORY_COPY(bufferData + offset,
                       &d_identifier,
                       sizeof(bdlb::BigEndianUint16));

    NTSCFG_MEMORY_COPY(bufferData + offset + sizeof(bdlb::BigEndianUint16),
                       &d_sequenceNumber,
                       sizeof(bdlb::BigEndianUint16));

    if (d_data.size() > 0) {
        const bsl::size_t dataOffset =
            static_cast<bsl::size_t>(offset + k_LENGTH);

        const bsl::size_t dataSize =
            static_cast<bsl::size_t>(d_data.size());

        if (dataOffset + dataSize > bufferCapacity) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        NTSCFG_MEMORY_COPY(bufferData + dataOffset, d_data.data(), dataSize);
    }

    return ntsa::Error();
}

bsl::ostream& IcmpPong::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IcmpPong::print(bslim::Printer* printer) const
{
    printer->printAttribute("identifier", this->identifier());
    printer->printAttribute("sequenceNumber", this->sequenceNumber());
    if (d_data.size() > 0) {
        printer->printForeign(d_data, &IcmpPong::printData, "data");
    }
}

bsl::ostream& IcmpPong::printData(bsl::ostream&            stream,
                                  const bdlbb::BlobBuffer& data,
                                  int                      level,
                                  int                      spacesPerLevel)
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    if (data.size() > 0) {
        return bdlb::Print::singleLineHexDump(stream,
                                              data.data(),
                                              data.size());
    }
    else {
        return stream;
    }
}

}  // close package namespace
}  // close enterprise namespace
