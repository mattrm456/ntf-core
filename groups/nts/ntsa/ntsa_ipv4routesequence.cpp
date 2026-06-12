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

#include <ntsa_ipv4routesequence.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4routesequence_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error Ipv4RouteSequence::decode(ntsa::PacketDecoder* decoder,
                                      bsl::size_t          size)
{
    NTSCFG_WARNING_UNUSED(decoder);

    reset();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Ipv4RouteSequence::encode(ntsa::PacketEncoder* encoder) const
{
    NTSCFG_WARNING_UNUSED(encoder);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t Ipv4RouteSequence::payloadSize() const
{
    return 0; // TODO
}

bool Ipv4RouteSequence::equals(const Ipv4RouteSequence& other) const
{
    return d_index == other.d_index && d_vector == other.d_vector;
}

bool Ipv4RouteSequence::less(const Ipv4RouteSequence& other) const
{
    if (d_index < other.d_index) {
        return true;
    }

    if (other.d_index < d_index) {
        return false;
    }

    return d_vector < other.d_vector;
}

bsl::ostream& Ipv4RouteSequence::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void Ipv4RouteSequence::print(bslim::Printer* printer) const
{
    printer->printAttribute("index", d_index);
    printer->printAttribute("vector", d_vector);
}

}  // close package namespace
}  // close enterprise namespace
