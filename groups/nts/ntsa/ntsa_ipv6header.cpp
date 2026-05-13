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

#include <ntsa_ipv6header.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv6header_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error Ipv6Header::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    error = decoder->decodeRaw(this, 8);
    if (error) {
        return error;
    }

    bsl::uint8_t sourceAddress[16];
    error = decoder->decodeRaw(sourceAddress, sizeof sourceAddress);
    if (error) {
        return error;
    }

    d_sourceAddress.copyFrom(sourceAddress, sizeof sourceAddress);

    bsl::uint8_t destinationAddress[16];
    error = decoder->decodeRaw(destinationAddress, sizeof destinationAddress);
    if (error) {
        return error;
    }

    d_destinationAddress.copyFrom(destinationAddress, sizeof destinationAddress);

    return ntsa::Error();
}

ntsa::Error Ipv6Header::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeRaw(this, 8);
    if (error) {
        return error;
    }

    bsl::uint8_t sourceAddress[16];
    d_sourceAddress.copyTo(sourceAddress, sizeof sourceAddress);

    error = encoder->encodeRaw(sourceAddress, sizeof sourceAddress);
    if (error) {
        return error;
    }

    bsl::uint8_t destinationAddress[16];
    d_destinationAddress.copyTo(destinationAddress, sizeof destinationAddress);

    error = encoder->encodeRaw(destinationAddress, sizeof destinationAddress);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& Ipv6Header::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("version", static_cast<bsl::size_t>(d_version));

    printer.printAttribute("sourceAddress", d_sourceAddress);
    printer.printAttribute("destinationAddress", d_destinationAddress);

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
