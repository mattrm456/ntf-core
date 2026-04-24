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

#include <ntsa_ipv4packet.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4packet_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error Ipv4Packet::decode(const bdlbb::BlobBuffer& source)
{
    ntsa::Error error;

    error = d_header.decode(source);
    if (error) {
        return error;
    }

    bdlbb::BlobBuffer payload(
        bsl::shared_ptr<char>(
            source.buffer(), source.data() + d_header.headerLength()),
        static_cast<int>(source.size() - d_header.headerLength()));

    if (d_header.protocol() == 6) {
        // d_payload.makeTcp();
        // error = d_payload.tcp().decode(payload);
    }
    else if (d_header.protocol() == 17) {
        d_payload.makeUdp();
        error = d_payload.udp().decode(payload);
        if (error) {
            return error;
        }
    }
    else {
        d_payload.makeRaw(payload);
    }

    return ntsa::Error();
}

ntsa::Error Ipv4Packet::encode(bdlbb::BlobBuffer* destination) const
{
    NTSCFG_WARNING_UNUSED(destination);

    ntsa::Error error;

    return ntsa::Error();
}

bool Ipv4Packet::equals(const Ipv4Packet& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_payload != other.d_payload) {
        return false;
    }

    return true;
}

bsl::ostream& Ipv4Packet::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("header", d_header);
    printer.printAttribute("payload", d_payload);
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
