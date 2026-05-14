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

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error TcpHeader::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeRaw(this, sizeof *this);
    if (error) {
        return error;
    }

    const bsl::size_t dataOffset = this->dataOffset();

    if (dataOffset < static_cast<bsl::size_t>(k_MIN_HEADER_LENGTH)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error TcpHeader::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeRaw(this, sizeof *this);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& TcpHeader::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void TcpHeader::print(bslim::Printer* printer) const
{
    printer->printAttribute("sourcePort", this->sourcePort());
    printer->printAttribute("destinationPort", this->destinationPort());
    printer->printAttribute("sequenceNumber", this->sequenceNumber());
    printer->printAttribute("acknowledgmentNumber",
                            this->acknowledgmentNumber());
    printer->printAttribute("dataOffset", this->dataOffset());
    printer->printForeign(this->flags(), &TcpHeader::printFlags, "flags");
    printer->printAttribute("windowSize", this->windowSize());
    printer->printAttribute("checksum", this->checksum());
    printer->printAttribute("urgentPointer", this->urgentPointer());
}

bsl::ostream& TcpHeader::printFlags(bsl::ostream& stream,
                                    bsl::uint8_t  flags,
                                    int           level,
                                    int           spacesPerLevel)
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

#if 0
        k_SYN = 1 << 1,
        k_ACK = 1 << 2,
        k_PSH = 1 << 3,
        k_FIN = 1 << 4,
        k_RST = 1 << 5,
        k_ECE = 1 << 6,
        k_CWR = 1 << 7,
        k_URG = 1 << 8
#endif

    bool found = false;

    if ((flags & static_cast<bsl::uint8_t>(k_SYN)) != 0) {
        if (found) {
            stream << '+';
        }

        stream << "SYN";
        found = true;
    }

    if ((flags & static_cast<bsl::uint8_t>(k_PSH)) != 0) {
        if (found) {
            stream << '+';
        }

        stream << "PSH";
        found = true;
    }

    if ((flags & static_cast<bsl::uint8_t>(k_FIN)) != 0) {
        if (found) {
            stream << '+';
        }

        stream << "FIN";
        found = true;
    }

    if ((flags & static_cast<bsl::uint8_t>(k_RST)) != 0) {
        if (found) {
            stream << '+';
        }

        stream << "RST";
        found = true;
    }

    if ((flags & static_cast<bsl::uint8_t>(k_URG)) != 0) {
        if (found) {
            stream << '+';
        }

        stream << "URG";
        found = true;
    }

    if ((flags & static_cast<bsl::uint8_t>(k_ECE)) != 0) {
        if (found) {
            stream << '+';
        }

        stream << "ECE";
        found = true;
    }

    if ((flags & static_cast<bsl::uint8_t>(k_CWR)) != 0) {
        if (found) {
            stream << '+';
        }

        stream << "CWR";
        found = true;
    }

    if ((flags & static_cast<bsl::uint8_t>(k_ACK)) != 0) {
        if (found) {
            stream << '+';
        }

        stream << "ACK";
        found = true;
    }

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
