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

#include <ntsa_igmpjoin.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_igmpjoin_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IgmpJoin::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    bsl::uint32_t groupAddressValue;
    error = decoder->decodeRaw(&groupAddressValue, sizeof groupAddressValue);
    if (error) {
        return error;
    }

    d_groupAddress = ntsa::Ipv4Address(groupAddressValue);

    return ntsa::Error();
}

ntsa::Error IgmpJoin::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    const bsl::uint32_t groupAddressValue = d_groupAddress.value();

    error = encoder->encodeRaw(&groupAddressValue, sizeof groupAddressValue);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& IgmpJoin::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IgmpJoin::print(bslim::Printer* printer) const
{
    printer->printAttribute("groupAddress", d_groupAddress);
}

}  // close package namespace
}  // close enterprise namespace
