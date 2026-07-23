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
    ntsa::Error error;

    reset();

    if (size < 2) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::uint8_t pointer = 0;
    error                = decoder->decodeUint8(&pointer);
    if (error) {
        return error;
    }

    const bsl::size_t entryVectorBytes = size - 2;

    if (entryVectorBytes == 0) {
        return ntsa::Error();
    }

    const bsl::size_t entrySize = sizeof(bsl::uint32_t);

    if (pointer % entrySize != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    this->setIndex(pointer / entrySize);

    if (entryVectorBytes % entrySize != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t entryCount = entryVectorBytes / entrySize;

    this->setCount(entryCount);

    for (bsl::size_t i = 0; i < entryCount; ++i) {
        ntsa::Ipv4Address address;
        error = decoder->decodeRaw(&d_vector[i], sizeof d_vector[i]);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Ipv4RouteSequence::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    if (d_index > 9) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_vector.size() > 9) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t entrySize = sizeof(bsl::uint32_t);

    const bsl::uint8_t pointer =
        static_cast<bsl::uint8_t>(d_index * entrySize);

    error = encoder->encodeUint8(pointer);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 0; i < d_vector.size(); ++i) {
        error = encoder->encodeRaw(&d_vector[i], sizeof d_vector[i]);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

bsl::size_t Ipv4RouteSequence::payloadSize() const
{
    return d_vector.size() * sizeof(ntsa::Ipv4Address);
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
