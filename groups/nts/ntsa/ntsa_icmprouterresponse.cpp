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

#include <ntsa_icmprouterresponse.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_icmprouterresponse_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IcmpRouterResponse::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    bsl::uint8_t entryCount;
    error = decoder->decodeUint8(&entryCount);
    if (error) {
        return error;
    }

    bsl::uint8_t entrySize;
    error = decoder->decodeUint8(&entrySize);
    if (error) {
        return error;
    }

    const bsl::uint8_t entrySizeRequired =
        (sizeof(bsl::uint32_t) + sizeof(bsl::int32_t)) / sizeof(bsl::uint32_t);

    if (entrySize != entrySizeRequired) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = decoder->decodeUint16(&d_timeToLive);
    if (error) {
        return error;
    }

    d_infoVector.resize(static_cast<bsl::size_t>(entryCount));

    for (bsl::uint8_t i = 0; i < entryCount; ++i) {
        bsl::uint32_t ipv4AddressValue;
        error = decoder->decodeRaw(&ipv4AddressValue, sizeof ipv4AddressValue);
        if (error) {
            return error;
        }

        bsl::int32_t ipv4AddressPriority;
        error = decoder->decodeInt32(&ipv4AddressPriority);
        if (error) {
            return error;
        }

        d_infoVector[i].setAddress(ntsa::Ipv4Address(ipv4AddressValue));
        d_infoVector[i].setPriority(ipv4AddressPriority);
    }

    return ntsa::Error();
}

ntsa::Error IcmpRouterResponse::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    if (d_infoVector.size() > 255) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t entryCount =
        static_cast<bsl::uint8_t>(d_infoVector.size());

    const bsl::uint8_t entrySize =
        (sizeof(bsl::uint32_t) + sizeof(bsl::int32_t)) / sizeof(bsl::uint32_t);

    error = encoder->encodeUint8(entryCount);
    if (error) {
        return error;
    }

    error = encoder->encodeUint8(entrySize);
    if (error) {
        return error;
    }

    error = encoder->encodeUint16(d_timeToLive);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 0; i < d_infoVector.size(); ++i) {
        const bsl::uint32_t ipv4AddressValue =
            d_infoVector[i].address().value();

        const bsl::int32_t ipv4AddressPriority = d_infoVector[i].priority();

        error = encoder->encodeRaw(&ipv4AddressValue, sizeof ipv4AddressValue);
        if (error) {
            return error;
        }

        error = encoder->encodeInt32(ipv4AddressPriority);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

bool IcmpRouterResponse::equals(const IcmpRouterResponse& other) const
{
    return d_infoVector == other.d_infoVector &&
           d_timeToLive == other.d_timeToLive;
}

bool IcmpRouterResponse::less(const IcmpRouterResponse& other) const
{
    if (d_infoVector < other.d_infoVector) {
        return true;
    }

    if (other.d_infoVector < d_infoVector) {
        return false;
    }

    return d_timeToLive < other.d_timeToLive;
}

bsl::ostream& IcmpRouterResponse::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IcmpRouterResponse::print(bslim::Printer* printer) const
{
    printer->printAttribute("info", d_infoVector);
    printer->printAttribute("timeToLive", d_timeToLive);
}

}  // close package namespace
}  // close enterprise namespace
