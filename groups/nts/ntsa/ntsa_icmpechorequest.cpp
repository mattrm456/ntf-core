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

#include <ntsa_icmpechorequest.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_icmpechorequest_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IcmpEchoRequest::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    error = decoder->decodeUint16(&d_identifier);
    if (error) {
        return error;
    }

    error = decoder->decodeUint16(&d_sequenceNumber);
    if (error) {
        return error;
    }

    const bsl::size_t dataSize = decoder->size() - decoder->position();

    if (dataSize > 0) {
        error = decoder->decodeRaw(&d_data, dataSize);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error IcmpEchoRequest::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeUint16(d_identifier);
    if (error) {
        return error;
    }

    error = encoder->encodeUint16(d_sequenceNumber);
    if (error) {
        return error;
    }

    if (d_data.size() > 0) {
        error = encoder->encodeRaw(d_data, d_data.size());
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

bool IcmpEchoRequest::equals(const IcmpEchoRequest& other) const
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

bool IcmpEchoRequest::less(const IcmpEchoRequest& other) const
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

bsl::ostream& IcmpEchoRequest::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IcmpEchoRequest::print(bslim::Printer* printer) const
{
    printer->printAttribute("identifier",     this->identifier());
    printer->printAttribute("sequenceNumber", this->sequenceNumber());
    if (d_data.size() > 0) {
        printer->printForeign(d_data, &IcmpEchoRequest::printData, "data");
    }
}

bsl::ostream& IcmpEchoRequest::printData(bsl::ostream&            stream,
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
