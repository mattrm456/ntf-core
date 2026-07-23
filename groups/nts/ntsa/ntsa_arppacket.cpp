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

#include <ntsa_arppacket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_arppacket_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error ArpPacket::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    if (d_header.operation() == ntsa::ArpType::e_REQUEST) {
        ntsa::ArpRequest& request = d_payload.makeRequest();

        error = request.decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_header.operation() == ntsa::ArpType::e_RESPONSE) {
        ntsa::ArpResponse& response = d_payload.makeResponse();

        error = response.decode(decoder);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

ntsa::Error ArpPacket::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = d_header.encode(encoder);
    if (error) {
        return error;
    }

    if (d_payload.isRequest()) {
        if (d_header.operation() != ntsa::ArpType::e_REQUEST) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::ArpRequest& request = d_payload.request();

        error = request.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_payload.isResponse()) {
        if (d_header.operation() != ntsa::ArpType::e_RESPONSE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const ntsa::ArpResponse& response = d_payload.response();

        error = response.encode(encoder);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

bool ArpPacket::equals(const ArpPacket& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_payload != other.d_payload) {
        return false;
    }

    return true;
}

bsl::ostream& ArpPacket::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    d_header.print(&printer);
    d_payload.print(&printer);
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
