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

#include <ntsa_igmprecord.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_igmprecord_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error IgmpRecord::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    reset();

    bsl::uint8_t recordType;
    error = decoder->decodeUint8(&recordType);
    if (error) {
        return error;
    }

    if (ntsa::IgmpRecordType::fromInt(&d_type,
                                      static_cast<int>(recordType)) != 0)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::uint8_t auxiliaryDataLength;
    error = decoder->decodeUint8(&auxiliaryDataLength);
    if (error) {
        return error;
    }

    bsl::uint16_t numSources;
    error = decoder->decodeUint16(&numSources);
    if (error) {
        return error;
    }

    error = decoder->decodeRaw(&d_multicastAddress, sizeof d_multicastAddress);
    if (error) {
        return error;
    }

    d_sourceAddresses.resize(static_cast<bsl::size_t>(numSources));

    for (bsl::uint16_t i = 0; i < numSources; ++i) {
        error = decoder->decodeRaw(&d_sourceAddresses[i],
                                   sizeof d_sourceAddresses[i]);
        if (error) {
            return error;
        }
    }

    if (auxiliaryDataLength > 0) {
        error = decoder->advance(auxiliaryDataLength);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error IgmpRecord::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeUint8(static_cast<bsl::uint8_t>(d_type));
    if (error) {
        return error;
    }

    error = encoder->encodeUint8(0);
    if (error) {
        return error;
    }

    if (d_sourceAddresses.size() > bsl::numeric_limits<bsl::uint16_t>::max()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = encoder->encodeUint16(
        static_cast<bsl::uint16_t>(d_sourceAddresses.size()));
    if (error) {
        return error;
    }

    error = encoder->encodeRaw(&d_multicastAddress, sizeof d_multicastAddress);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 0; i < d_sourceAddresses.size(); ++i) {
        error = encoder->encodeRaw(&d_sourceAddresses[i],
                                   sizeof d_sourceAddresses[i]);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

bsl::ostream& IgmpRecord::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("type", d_type);
    printer.printAttribute("multicastAddress", d_multicastAddress);
    printer.printAttribute("sourceAddresses", d_sourceAddresses);

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
