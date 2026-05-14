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

#include <ntsa_udpextension.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_udpextension_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

void UdpExtension::add(const ntsa::UdpOption& option)
{
    d_vector.push_back(option);
}

void UdpExtension::add(bslmf::MovableRef<ntsa::UdpOption> option)
{
    d_vector.push_back(NTSCFG_MOVE_ACCESS(option));
}

ntsa::Error UdpExtension::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    while (true) {
        ntsa::UdpOption option(d_allocator_p);
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

ntsa::Error UdpExtension::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    const bsl::size_t numOptions = d_vector.size();

    if (numOptions == 0) {
        return ntsa::Error();
    }

    const bsl::size_t initialPosition = encoder->position();

    if (reinterpret_cast<bsl::uintptr_t>(encoder->next()) % 2 != 0) {
        error = encoder->encodeUint8(0);
        if (error) {
            return error;
        }
    }

    const bsl::size_t checksumPosition = encoder->position();

    error = encoder->encodeUint16(0);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 0; i < numOptions; ++i) {
        const bool isFinal = i == numOptions - 1;
        d_vector[i].encode(encoder, isFinal);
    }

    const bsl::size_t finalPosition = encoder->position();

    bsl::uint16_t checksumValue = 0;

    return ntsa::Error();
}

void UdpExtension::load(ntsa::UdpOptionVector* result) const
{
    *result = d_vector;
}

bool UdpExtension::equals(const UdpExtension& other) const
{
    return d_vector == other.d_vector;
}

bool UdpExtension::less(const UdpExtension& other) const
{
    return d_vector < other.d_vector;
}

bsl::ostream& UdpExtension::print(bsl::ostream& stream,
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

void UdpExtension::print(bslim::Printer* printer) const
{
    for (bsl::size_t i = 0; i < d_vector.size(); ++i) {
        d_vector[i].print(printer);
    }
}

}  // close package namespace
}  // close enterprise namespace
