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

#include <ntsa_arprequest.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_arprequest_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error ArpRequest::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    error = decoder->decodeRaw(&d_senderHardwareAddress,
                               sizeof d_senderHardwareAddress);
    if (error) {
        return error;
    }

    error = decoder->decodeRaw(&d_senderProtocolAddress,
                               sizeof d_senderProtocolAddress);
    if (error) {
        return error;
    }

    error = decoder->decodeRaw(&d_targetHardwareAddress,
                               sizeof d_targetHardwareAddress);
    if (error) {
        return error;
    }

    error = decoder->decodeRaw(&d_targetProtocolAddress,
                               sizeof d_targetProtocolAddress);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ArpRequest::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeRaw(&d_senderHardwareAddress,
                               sizeof d_senderHardwareAddress);
    if (error) {
        return error;
    }

    error = encoder->encodeRaw(&d_senderProtocolAddress,
                               sizeof d_senderProtocolAddress);
    if (error) {
        return error;
    }

    error = encoder->encodeRaw(&d_targetHardwareAddress,
                               sizeof d_targetHardwareAddress);
    if (error) {
        return error;
    }

    error = encoder->encodeRaw(&d_targetProtocolAddress,
                               sizeof d_targetProtocolAddress);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& ArpRequest::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void ArpRequest::print(bslim::Printer* printer) const
{
    printer->printAttribute("senderHardwareAddress", d_senderHardwareAddress);
    printer->printAttribute("senderProtocolAddress", d_senderProtocolAddress);
    printer->printAttribute("targetHardwareAddress", d_targetHardwareAddress);
    printer->printAttribute("targetProtocolAddress", d_targetProtocolAddress);
}

}  // close package namespace
}  // close enterprise namespace
