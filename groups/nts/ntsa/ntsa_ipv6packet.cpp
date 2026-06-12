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

#include <ntsa_ipv6packet.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv6packet_cpp, "$Id$ $CSID$")

#include <ntsa_ipv6checksum.h>
#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error Ipv6Packet::decode(ntsa::PacketDecoderContext*       context,
                               ntsa::PacketDecoder*              decoder,
                               const ntsa::PacketDecoderOptions& options)
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(decoder);
    NTSCFG_WARNING_UNUSED(options);

    ntsa::Error error;

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Ipv6Packet::encode(ntsa::PacketEncoderContext*       context,
                               ntsa::PacketEncoder*              encoder,
                               const ntsa::PacketEncoderOptions& options) const
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(encoder);
    NTSCFG_WARNING_UNUSED(options);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bool Ipv6Packet::equals(const Ipv6Packet& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_payload != other.d_payload) {
        return false;
    }

    return true;
}

bsl::ostream& Ipv6Packet::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("ipv6", d_header);

    if (d_payload.isTcp()) {
        printer.printAttribute("tcp", d_payload.tcp().header());
    }
    else if (d_payload.isUdp()) {
        printer.printAttribute("udp", d_payload.udp().header());
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
