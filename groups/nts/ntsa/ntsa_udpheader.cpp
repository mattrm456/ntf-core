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

#include <ntsa_udpheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_udpheader_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error UdpHeader::decode(const void* data, const bsl::size_t size)
{
    reset();

    if (size < static_cast<bsl::size_t>(k_LENGTH)) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bsl::memcpy(reinterpret_cast<void*>(this),
                data,
                static_cast<bsl::size_t>(k_LENGTH));

    return ntsa::Error();
}

ntsa::Error UdpHeader::decode(const bdlbb::BlobBuffer& source)
{
    reset();
    
    if (source.size() < static_cast<int>(k_LENGTH)) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bsl::memcpy(reinterpret_cast<void*>(this),
                source.data(),
                static_cast<bsl::size_t>(k_LENGTH));

    return ntsa::Error();
}

ntsa::Error UdpHeader::encode(bdlbb::BlobBuffer* destination) const
{
    NTSCFG_WARNING_UNUSED(destination);
    return ntsa::Error();
}

bool UdpHeader::equals(const UdpHeader& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) == 0;
}

bool UdpHeader::less(const UdpHeader& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) < 0;
}

bsl::ostream& UdpHeader::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("headerLength", this->headerLength());
    printer.printAttribute("packetLength", this->packetLength());
    printer.printAttribute("sourcePort", this->sourcePort());
    printer.printAttribute("destinationPort", this->destinationPort());
    printer.printAttribute("checksum", this->checksum());
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
