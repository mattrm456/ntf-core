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

#include <ntsa_igmpreport.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_igmpreport_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IgmpReport::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    error = decoder->decodeUint16(&d_flags);
    if (error) {
        return error;
    }

    bsl::uint16_t numRecords;
    error = decoder->decodeUint16(&numRecords);
    if (error) {
        return error;
    }

    d_records.resize(static_cast<bsl::size_t>(numRecords));

    for (bsl::uint16_t i = 0; i < numRecords; ++i) {
        error = d_records[i].decode(decoder);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error IgmpReport::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    if (d_records.size() > bsl::numeric_limits<bsl::uint16_t>::max()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = encoder->encodeUint16(d_flags);
    if (error) {
        return error;
    }

    const bsl::uint16_t numRecords =
        static_cast<bsl::uint16_t>(d_records.size());

    error = encoder->encodeUint16(numRecords);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 0; i < d_records.size(); ++i) {
        error = d_records[i].encode(encoder);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

bool IgmpReport::equals(const IgmpReport& other) const
{
    return d_flags == other.d_flags && d_records == other.d_records;
}

bool IgmpReport::less(const IgmpReport& other) const
{
    if (d_flags < other.d_flags) {
        return true;
    }

    if (other.d_flags < d_flags) {
        return false;
    }

    return d_records < other.d_records;
}

bsl::ostream& IgmpReport::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IgmpReport::print(bslim::Printer* printer) const
{
    printer->printAttribute("flags", d_flags);
    printer->printAttribute("records", d_records);
}

}  // close package namespace
}  // close enterprise namespace
