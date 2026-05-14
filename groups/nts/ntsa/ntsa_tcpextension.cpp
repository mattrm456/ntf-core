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

#include <ntsa_tcpextension.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcpextension_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

void TcpExtension::add(const ntsa::TcpOption& option)
{
    d_vector.push_back(option);
}

void TcpExtension::add(bslmf::MovableRef<ntsa::TcpOption> option)
{
    d_vector.push_back(NTSCFG_MOVE_ACCESS(option));
}

ntsa::Error TcpExtension::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    while (true) {
        ntsa::TcpOption option(d_allocator_p);
        error = option.decode(decoder);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                return ntsa::Error();
            }
            else {
                return error;
            }
        }

        d_vector.push_back(NTSCFG_MOVE(option));
    }

    return ntsa::Error();
}

ntsa::Error TcpExtension::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    const bsl::size_t numOptions = d_vector.size();

    for (bsl::size_t i = 0; i < numOptions; ++i) {
        const bool isFinal = i == numOptions - 1;
        d_vector[i].encode(encoder, isFinal);
    }

    return ntsa::Error();
}

void TcpExtension::load(ntsa::TcpOptionVector* result) const
{
    *result = d_vector;
}

bool TcpExtension::equals(const TcpExtension& other) const
{
    return d_vector == other.d_vector;
}

bool TcpExtension::less(const TcpExtension& other) const
{
    return d_vector < other.d_vector;
}

bsl::ostream& TcpExtension::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    ntsa::Error error;

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void TcpExtension::print(bslim::Printer* printer) const
{
    for (bsl::size_t i = 0; i < d_vector.size(); ++i) {
        d_vector[i].print(printer);
    }
}

}  // close package namespace
}  // close enterprise namespace
