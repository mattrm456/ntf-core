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

#include <ntsa_igmpquery.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_igmpquery_cpp, "$Id$ $CSID$")

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IgmpQuery::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    {
        bsl::uint32_t groupAddressValue;
        error =
            decoder->decodeRaw(&groupAddressValue, sizeof groupAddressValue);
        if (error) {
            return error;
        }

        d_groupAddress = ntsa::Ipv4Address(groupAddressValue);
    }

    error = decoder->decodeUint8(&d_flagsAndQrv);
    if (error) {
        return error;
    }

    error = decoder->decodeUint8(&d_qqic);
    if (error) {
        return error;
    }

    bsl::uint16_t numSources;
    error = decoder->decodeUint16(&numSources);
    if (error) {
        return error;
    }

    d_sourceAddresses.resize(static_cast<bsl::size_t>(numSources));

    for (bsl::uint16_t i = 0; i < numSources; ++i) {
        bsl::uint32_t addressValue;
        error = decoder->decodeRaw(&addressValue, sizeof addressValue);
        if (error) {
            return error;
        }

        d_sourceAddresses[i] = ntsa::Ipv4Address(addressValue);
    }

    return ntsa::Error();
}

ntsa::Error IgmpQuery::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    if (d_sourceAddresses.size() > 65535) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        const bsl::uint32_t groupAddressValue = d_groupAddress.value();

        error =
            encoder->encodeRaw(&groupAddressValue, sizeof groupAddressValue);
        if (error) {
            return error;
        }
    }

    error = encoder->encodeUint8(d_flagsAndQrv);
    if (error) {
        return error;
    }

    error = encoder->encodeUint8(d_qqic);
    if (error) {
        return error;
    }

    const bsl::uint16_t numSources =
        static_cast<bsl::uint16_t>(d_sourceAddresses.size());

    error = encoder->encodeUint16(numSources);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 0; i < d_sourceAddresses.size(); ++i) {
        const bsl::uint32_t addressValue = d_sourceAddresses[i].value();

        error = encoder->encodeRaw(&addressValue, sizeof addressValue);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

bool IgmpQuery::equals(const IgmpQuery& other) const
{
    return d_groupAddress == other.d_groupAddress &&
           d_flagsAndQrv == other.d_flagsAndQrv && d_qqic == other.d_qqic &&
           d_sourceAddresses == other.d_sourceAddresses;
}

bool IgmpQuery::less(const IgmpQuery& other) const
{
    if (d_groupAddress < other.d_groupAddress) {
        return true;
    }

    if (other.d_groupAddress < d_groupAddress) {
        return false;
    }

    if (d_flagsAndQrv < other.d_flagsAndQrv) {
        return true;
    }

    if (other.d_flagsAndQrv < d_flagsAndQrv) {
        return false;
    }

    if (d_qqic < other.d_qqic) {
        return true;
    }

    if (other.d_qqic < d_qqic) {
        return false;
    }

    return d_sourceAddresses < other.d_sourceAddresses;
}

bsl::ostream& IgmpQuery::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IgmpQuery::print(bslim::Printer* printer) const
{
    printer->printAttribute("groupAddress", d_groupAddress);
    printer->printAttribute("suppressFlag", this->suppressFlag());
    printer->printAttribute("qrv", static_cast<int>(this->qrv()));
    printer->printAttribute("qqic", static_cast<int>(this->qqic()));
    printer->printAttribute("sourceAddresses", d_sourceAddresses);
}

}  // close package namespace
}  // close enterprise namespace
