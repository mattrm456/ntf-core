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

#include <ntsa_ipv4header.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4header_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

bool Ipv4Header::equals(const Ipv4Header& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) == 0;
}

bool Ipv4Header::less(const Ipv4Header& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) < 0;
}

bsl::ostream& Ipv4Header::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("version", static_cast<bsl::size_t>(d_version));

    const bsl::size_t headerLength =
        static_cast<bsl::size_t>(this->headerLength());

    printer.printAttribute("headerLength", headerLength);

    const bsl::size_t totalLength =
        static_cast<bsl::size_t>(this->totalLength());

    printer.printAttribute("totalLength", totalLength);

    const bsl::size_t id = static_cast<bsl::size_t>(this->id());
    if (id != 0) {
        printer.printAttribute("id", id);
    }

    const bool preserve = this->preserve();
    if (preserve) {
        printer.printAttribute("preserve", 1);
    }

    const bool more = this->more();
    if (more) {
        printer.printAttribute("more", 1);
    }

    const bsl::size_t fragmentOffset =
        static_cast<bsl::size_t>(this->fragmentOffset());
    if (fragmentOffset != 0) {
        printer.printAttribute("fragmentOffset", fragmentOffset);
    }

    const bsl::size_t timeToLive =
        static_cast<bsl::size_t>(this->timeToLive());

    printer.printAttribute("timeToLive", timeToLive);

    const bsl::size_t protocol = static_cast<bsl::size_t>(this->protocol());

    printer.printAttribute("protocol", protocol);

    const bsl::size_t checksum = static_cast<bsl::size_t>(this->checksum());

    printer.printAttribute("checksum", checksum);

    printer.printAttribute("sourceAddress", d_sourceAddress);
    printer.printAttribute("destinationAddress", d_destinationAddress);
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
