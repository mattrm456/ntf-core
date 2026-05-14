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

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error UdpHeader::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeRaw(this, sizeof *this);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error UdpHeader::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeRaw(this, sizeof *this);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& UdpHeader::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void UdpHeader::print(bslim::Printer* printer) const
{
    printer->printAttribute("headerLength", this->headerLength());
    printer->printAttribute("packetLength", this->packetLength());
    printer->printAttribute("sourcePort", this->sourcePort());
    printer->printAttribute("destinationPort", this->destinationPort());
    printer->printAttribute("checksum", this->checksum());
}

}  // close package namespace
}  // close enterprise namespace
