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

#include <ntsa_tcppacket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcppacket_cpp, "$Id$ $CSID$")

#include <ntsa_tcpchecksum.h>
#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {


ntsa::Error TcpPacket::decode(const bdlbb::BlobBuffer& source,
                              bsl::size_t              offset,
                              bsl::size_t              packetSize)
{
    ntsa::Error error;

    if (source.size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset > packetSize) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset > static_cast<bsl::size_t>(source.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_header.decode(source.data() + offset, packetSize - offset);
    if (error) {
        return error;
    }

    const bsl::size_t headerLength = d_header.headerLength();

    if (offset + headerLength >
        static_cast<bsl::size_t>(source.size()))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (offset + headerLength != packetSize) {
        d_payload.reset(
            bsl::shared_ptr<char>(
                source.buffer(),
                source.data() + offset + headerLength),
            static_cast<int>(source.size() - offset - headerLength));
    }

    return ntsa::Error();
}

ntsa::Error TcpPacket::encode(bdlbb::BlobBuffer* destination) const
{
    NTSCFG_WARNING_UNUSED(destination);

    return ntsa::Error();
}

bool TcpPacket::equals(const TcpPacket& other) const
{
    if (d_header != other.d_header) {
        return false;
    }

    if (d_payload.size() != other.d_payload.size()) {
        return false;
    }

    const int compare = bsl::memcmp(d_payload.data(),
                                    other.d_payload.data(),
                                    d_payload.size());
    if (compare != 0) {
        return false;
    }

    return true;
}

bsl::ostream& TcpPacket::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("header", d_header);
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
