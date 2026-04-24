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

#include <ntsa_tcpheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcpheader_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error TcpHeader::decode(const bdlbb::BlobBuffer& source)
{
    NTSCFG_WARNING_UNUSED(source);
    return ntsa::Error();
}

ntsa::Error TcpHeader::encode(bdlbb::BlobBuffer* destination) const
{
    NTSCFG_WARNING_UNUSED(destination);
    return ntsa::Error();
}

bool TcpHeader::equals(const TcpHeader& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) == 0;
}

bool TcpHeader::less(const TcpHeader& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) < 0;
}

bsl::ostream& TcpHeader::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("sourcePort", this->sourcePort());
    printer.printAttribute("destinationPort", this->destinationPort());
    printer.printAttribute("sequenceNumber", this->sequenceNumber());
    printer.printAttribute("acknowledgmentNumber",
                           this->acknowledgmentNumber());
    printer.printAttribute("headerLength", this->headerLength());
    printer.printAttribute("flags", static_cast<bsl::size_t>(this->flags()));
    printer.printAttribute("windowSize", this->windowSize());
    printer.printAttribute("urgentPointer", this->urgentPointer());

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
