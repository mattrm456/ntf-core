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

#include <ntsa_icmpheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_icmpheader_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IcmpHeader::decode(const bdlbb::BlobBuffer& buffer,
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

ntsa::Error IcmpHeader::encode(bdlbb::BlobBuffer* buffer,
                              bsl::size_t        offset) const
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

    if (offset + static_cast<bsl::size_t>(k_LENGTH) > bufferCapacity) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::memcpy(reinterpret_cast<void*>(bufferData + offset),
                reinterpret_cast<const void*>(this),
                static_cast<bsl::size_t>(k_LENGTH));

    return ntsa::Error();
}

bool IcmpHeader::equals(const IcmpHeader& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) == 0;
}

bool IcmpHeader::less(const IcmpHeader& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) < 0;
}

bsl::ostream& IcmpHeader::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IcmpHeader::print(bslim::Printer* printer) const
{
    printer->printAttribute("type", static_cast<int>(this->type()));
    printer->printAttribute("code", static_cast<int>(this->code()));
    printer->printAttribute("checksum", this->checksum());
}

}  // close package namespace
}  // close enterprise namespace
