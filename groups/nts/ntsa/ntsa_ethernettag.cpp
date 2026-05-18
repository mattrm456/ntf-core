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

#include <ntsa_ethernettag.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernettag_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error EthernetTag::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeRaw(this, sizeof *this);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EthernetTag::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeRaw(this, sizeof *this);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& EthernetTag::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    const bsl::uint16_t                 group =     this->group();
    const ntsa::EthernetPriority::Value priority =  this->priority();
    const bool                          droppable = this->droppable();

    printer.printAttribute("group", group);
    printer.printAttribute("priority", priority);
    if (droppable) {
        printer.printAttribute("droppable", true);
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
