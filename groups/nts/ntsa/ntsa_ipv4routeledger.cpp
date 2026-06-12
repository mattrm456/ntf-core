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

#include <ntsa_ipv4routeledger.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4routeledger_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error Ipv4RouteLedger::decode(ntsa::PacketDecoder* decoder,
                                    bsl::size_t size)
{
    ntsa::Error error;

    reset();

    if (size < 2) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::uint8_t pointer = 0;
    error = decoder->decodeUint8(&pointer);
    if (error) {
        return error;
    }

    bsl::uint8_t overflowAndFlags = 0;
    error = decoder->decodeUint8(&overflowAndFlags);
    if (error) {
        return error;
    }

    const bsl::uint8_t overflow = overflowAndFlags & 0x0F;
    const bsl::uint8_t flags = overflowAndFlags >> 4;

    const bsl::size_t entryVectorBytes = size - 2;

    this->setIndex(pointer);
    this->setOverflow(overflow);
    this->setFlags(flags);

    if (entryVectorBytes == 0) {
        return ntsa::Error();
    }

    if (flags == k_TIMESTAMP_ONLY) {
        const bsl::size_t entrySize = sizeof(bsl::uint32_t);

        if (entryVectorBytes % entrySize != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const bsl::size_t entryCount = entryVectorBytes / entrySize;

        this->setCount(entryCount);

        for (bsl::size_t i = 0; i < entryCount; ++i) {
            bdlb::BigEndianUint32 timestamp;
            error = decoder->decodeUint32(&timestamp);
            if (error) {
                return error;
            }

            this->entry(i).setTimestamp(static_cast<bsl::uint32_t>(timestamp));
        }
    }
    else if (flags == k_TIMESTAMP_AND_ADDRESS ||
             flags == k_TIMESTAMP_AND_ADDRESS_PRESPECIFIED)
    {
        const bsl::size_t entrySize =
            sizeof(bsl::uint32_t) + sizeof(ntsa::Ipv4Address);

        if (entryVectorBytes % entrySize != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const bsl::size_t entryCount = entryVectorBytes / entrySize;

        this->setCount(entryCount);

        for (bsl::size_t i = 0; i < entryCount; ++i) {
            bdlb::BigEndianUint32 timestamp;
            error = decoder->decodeUint32(&timestamp);
            if (error) {
                return error;
            }

            ntsa::Ipv4Address address;
            error = decoder->decodeRaw(&address, sizeof address);
            if (error) {
                return error;
            }

            this->entry(i).setTimestamp(static_cast<bsl::uint32_t>(timestamp));
            this->entry(i).setAddress(address);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Ipv4RouteLedger::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    if (d_index > 255) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t pointer = static_cast<bsl::uint8_t>(d_index);

    error = encoder->encodeUint8(pointer);
    if (error) {
        return error;
    }

    if (d_flags != k_TIMESTAMP_ONLY &&
        d_flags != k_TIMESTAMP_AND_ADDRESS &&
        d_flags != k_TIMESTAMP_AND_ADDRESS_PRESPECIFIED)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_overflow > 15) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::uint8_t overflowAndFlags = static_cast<bsl::uint8_t>(d_flags);
    overflowAndFlags <<= 4;
    overflowAndFlags |= d_overflow;

    error = encoder->encodeUint8(overflowAndFlags);
    if (error) {
        return error;
    }

    if (d_flags == k_TIMESTAMP_ONLY) {
        for (bsl::size_t i = 0; i < d_vector.size(); ++i) {
            bdlb::BigEndianUint32 timestamp;
            if (d_vector[i].timestamp().has_value()) {
                timestamp = d_vector[i].timestamp().value();
            }

            error = encoder->encodeUint32(timestamp);
            if (error) {
                return error;
            }
        }
    }
    else if (d_flags == k_TIMESTAMP_AND_ADDRESS ||
             d_flags == k_TIMESTAMP_AND_ADDRESS_PRESPECIFIED)
    {
        for (bsl::size_t i = 0; i < d_vector.size(); ++i) {
            bdlb::BigEndianUint32 timestamp;
            if (d_vector[i].timestamp().has_value()) {
                timestamp = d_vector[i].timestamp().value();
            }

            ntsa::Ipv4Address address;
            if (d_vector[i].address().has_value()) {
                address = d_vector[i].address().value();
            }

            error = encoder->encodeUint32(timestamp);
            if (error) {
                return error;
            }

            error = encoder->encodeRaw(&address, sizeof address);
            if (error) {
                return error;
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

bsl::size_t Ipv4RouteLedger::payloadSize() const
{
    bsl::size_t result = 2;
    if (d_flags == ntsa::Ipv4RouteLedger::k_TIMESTAMP_ONLY) {
        result += d_vector.size() * sizeof(bsl::uint32_t);
    }
    else if (d_flags == k_TIMESTAMP_AND_ADDRESS ||
             d_flags == k_TIMESTAMP_AND_ADDRESS_PRESPECIFIED)
    {
        result += d_vector.size() * (
            sizeof(bsl::uint32_t) + sizeof(ntsa::Ipv4Address));
    }

    return result;
}

bool Ipv4RouteLedger::equals(const Ipv4RouteLedger& other) const
{
    return d_index == other.d_index && d_overflow == other.d_overflow &&
           d_flags == other.d_flags && d_vector == other.d_vector;
}

bool Ipv4RouteLedger::less(const Ipv4RouteLedger& other) const
{
    if (d_index < other.d_index) {
        return true;
    }

    if (other.d_index < d_index) {
        return false;
    }

    if (d_overflow < other.d_overflow) {
        return true;
    }

    if (other.d_overflow < d_overflow) {
        return false;
    }

    if (d_flags < other.d_flags) {
        return true;
    }

    if (other.d_flags < d_flags) {
        return false;
    }

    return d_vector < other.d_vector;
}

bsl::ostream& Ipv4RouteLedger::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void Ipv4RouteLedger::print(bslim::Printer* printer) const
{
    printer->printAttribute("index", d_index);
    printer->printAttribute("overflow", d_overflow);
    printer->printAttribute("flags", d_flags);
    printer->printAttribute("vector", d_vector);
}

}  // close package namespace
}  // close enterprise namespace
