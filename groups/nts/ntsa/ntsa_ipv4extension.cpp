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

#include <ntsa_ipv4extension.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4extension_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

void Ipv4Extension::add(const ntsa::Ipv4Option& option)
{
    d_vector.push_back(option);
}

void Ipv4Extension::add(bslmf::MovableRef<ntsa::Ipv4Option> option)
{
    d_vector.push_back(NTSCFG_MOVE_ACCESS(option));
}

ntsa::Error Ipv4Extension::decode(ntsa::PacketDecoder* decoder,
                                  bsl::size_t          size)
{
    ntsa::Error error;

    reset();

    bsl::size_t n = 0;

    while (true) {
        const bsl::size_t p0 = decoder->position();

        ntsa::Ipv4Option option(d_allocator_p);
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

        const bsl::size_t p1 = decoder->position();

        n += static_cast<bsl::size_t>(p1 - p0);

        if (n == size) {
            break;
        }
        else if (n > size) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error Ipv4Extension::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    const bsl::size_t numOptions = d_vector.size();

    for (bsl::size_t i = 0; i < numOptions; ++i) {
        const bool isFinal = i == numOptions - 1;
        d_vector[i].encode(encoder, isFinal);
    }

    return ntsa::Error();
}

void Ipv4Extension::load(ntsa::Ipv4OptionVector* result) const
{
    *result = d_vector;
}

bool Ipv4Extension::equals(const Ipv4Extension& other) const
{
    return d_vector == other.d_vector;
}

bool Ipv4Extension::less(const Ipv4Extension& other) const
{
    return d_vector < other.d_vector;
}

bsl::ostream& Ipv4Extension::print(bsl::ostream& stream,
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

void Ipv4Extension::print(bslim::Printer* printer) const
{
    for (bsl::size_t i = 0; i < d_vector.size(); ++i) {
        d_vector[i].print(printer);
    }
}

}  // close package namespace
}  // close enterprise namespace
