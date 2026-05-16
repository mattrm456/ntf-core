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

#include <ntsa_icmpunreachable.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_icmpunreachable_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IcmpUnreachable::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    error = decoder->decodeRaw(&d_unused, sizeof d_unused);
    if (error) {
        return error;
    }

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    const bsl::size_t payloadSize = decoder->size() - decoder->position();

    if (payloadSize > 0) {
        d_payloadSize = bsl::min(payloadSize, sizeof d_payloadData);
        error = decoder->decodeRaw(d_payloadData, d_payloadSize);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error IcmpUnreachable::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeRaw(&d_unused, sizeof d_unused);
    if (error) {
        return error;
    }

    error = d_header.encode(encoder);
    if (error) {
        return error;
    }

    if (d_payloadSize > 0) {
        error = encoder->encodeRaw(d_payloadData, d_payloadSize);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error IcmpUnreachable::decode(const bdlbb::BlobBuffer& buffer,
                                    bsl::size_t              offset,
                                    bsl::size_t              packetSize)
{
    NTSCFG_WARNING_UNUSED(packetSize);

    reset();

    if (buffer.data() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const char* bufferData = buffer.data();

    if (buffer.size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t bufferSize = static_cast<bsl::size_t>(buffer.size());

    if (offset + static_cast<bsl::size_t>(k_LENGTH) > bufferSize) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::memcpy(reinterpret_cast<void*>(this),
                bufferData + offset,
                static_cast<bsl::size_t>(k_LENGTH));

    return ntsa::Error();
}

ntsa::Error IcmpUnreachable::encode(bdlbb::BlobBuffer* buffer,
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

    bsl::memcpy(reinterpret_cast<void*>(bufferData + offset),
                reinterpret_cast<const void*>(this),
                static_cast<bsl::size_t>(k_LENGTH));

    return ntsa::Error();
}

bsl::ostream& IcmpUnreachable::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IcmpUnreachable::print(bslim::Printer* printer) const
{
    printer->printAttribute("header", d_header);
    if (d_payloadSize > 0) {
        printer->printForeign(
            bslstl::StringRef(reinterpret_cast<const char*>(d_payloadData),
                              d_payloadSize),
            &IcmpUnreachable::printData,
            "payload");
    }
}

bsl::ostream& IcmpUnreachable::printData(bsl::ostream&            stream,
                                     const bslstl::StringRef& data,
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
