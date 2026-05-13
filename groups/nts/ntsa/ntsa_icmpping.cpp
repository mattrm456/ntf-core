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

#include <ntsa_icmpping.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_icmpping_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IcmpPing::decode(ntsa::PacketDecoder* decoder)
{
    NTSCFG_WARNING_UNUSED(decoder);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error IcmpPing::encode(ntsa::PacketEncoder* encoder) const
{
    NTSCFG_WARNING_UNUSED(encoder);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error IcmpPing::decode(const bdlbb::BlobBuffer& buffer,
                             bsl::size_t              offset,
                             bsl::size_t              packetSize)
{
    NTSCFG_WARNING_UNUSED(packetSize);

    reset();

    if (offset > static_cast<bsl::size_t>(buffer.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const char* bufferData = buffer.data() + offset;

    bsl::size_t bufferSize =
        static_cast<bsl::size_t>(buffer.size() - offset);

    if (bufferSize < sizeof(bdlb::BigEndianUint16)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTSCFG_MEMORY_COPY(&d_identifier,
                       bufferData,
                       sizeof(bdlb::BigEndianUint16));

    bufferData += sizeof(bdlb::BigEndianUint16);
    bufferSize -= sizeof(bdlb::BigEndianUint16);

    if (bufferSize < sizeof(bdlb::BigEndianUint16)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTSCFG_MEMORY_COPY(&d_sequenceNumber,
                       bufferData,
                       sizeof(bdlb::BigEndianUint16));

    bufferData += sizeof(bdlb::BigEndianUint16);
    bufferSize -= sizeof(bdlb::BigEndianUint16);

    if (bufferSize > 0) {
        d_data.reset(
            bsl::shared_ptr<char>(buffer.buffer(),
                                  const_cast<char*>(bufferData)),
            static_cast<int>(bufferSize));
    }

    return ntsa::Error();
}

ntsa::Error IcmpPing::encode(bdlbb::BlobBuffer* buffer,
                             bsl::size_t        offset) const
{
    if (offset > static_cast<bsl::size_t>(buffer->size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    char* bufferData = buffer->data() + offset;

    bsl::size_t bufferCapacity =
        static_cast<bsl::size_t>(buffer->size() - offset);

    if (bufferCapacity < sizeof(bdlb::BigEndianUint16)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTSCFG_MEMORY_COPY(bufferData,
                       &d_identifier,
                       sizeof(bdlb::BigEndianUint16));

    bufferData     += sizeof(bdlb::BigEndianUint16);
    bufferCapacity -= sizeof(bdlb::BigEndianUint16);

    if (bufferCapacity < sizeof(bdlb::BigEndianUint16)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTSCFG_MEMORY_COPY(bufferData,
                       &d_sequenceNumber,
                       sizeof(bdlb::BigEndianUint16));

    bufferData     += sizeof(bdlb::BigEndianUint16);
    bufferCapacity -= sizeof(bdlb::BigEndianUint16);

    if (d_data.size() > 0) {
        if (bufferCapacity < static_cast<bsl::size_t>(d_data.size())) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        NTSCFG_MEMORY_COPY(bufferData, d_data.data(), d_data.size());
    }

    return ntsa::Error();
}

bool IcmpPing::equals(const IcmpPing& other) const
{
    if (d_identifier != other.d_identifier) {
        return false;
    }

    if (d_sequenceNumber != other.d_sequenceNumber) {
        return false;
    }

    if (d_data.size() != other.d_data.size()) {
        return false;
    }

    const int compare =
        bsl::memcmp(d_data.data(), other.d_data.data(), d_data.size());
    if (compare != 0) {
        return false;
    }

    return true;
}

bool IcmpPing::less(const IcmpPing& other) const
{
    if (static_cast<bsl::uint16_t>(d_identifier) <
        static_cast<bsl::uint16_t>(other.d_identifier))
    {
        return true;
    }

    if (static_cast<bsl::uint16_t>(other.d_identifier) <
        static_cast<bsl::uint16_t>(d_identifier))
    {
        return false;
    }

    if (static_cast<bsl::uint16_t>(d_sequenceNumber) <
        static_cast<bsl::uint16_t>(other.d_sequenceNumber))
    {
        return true;
    }

    if (static_cast<bsl::uint16_t>(other.d_sequenceNumber) <
        static_cast<bsl::uint16_t>(d_sequenceNumber))
    {
        return false;
    }

    const int compare =
        bsl::memcmp(d_data.data(), other.d_data.data(), d_data.size());
    if (compare >= 0) {
        return false;
    }

    return true;
}

bsl::ostream& IcmpPing::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IcmpPing::print(bslim::Printer* printer) const
{
    printer->printAttribute("identifier",     this->identifier());
    printer->printAttribute("sequenceNumber", this->sequenceNumber());
    if (d_data.size() > 0) {
        printer->printForeign(d_data, &IcmpPing::printData, "data");
    }
}

bsl::ostream& IcmpPing::printData(bsl::ostream&            stream,
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
