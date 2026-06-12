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

#include <ntsa_arpheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_arpheader_cpp, "$Id$ $CSID$")

#include <ntsa_arptype.h>
#include <ntsa_ethernetprotocol.h>
#include <ntsa_ethernetaddress.h>
#include <ntsa_ipv4address.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

ntsa::Error ArpHeader::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeRaw(this,
                               static_cast<bsl::size_t>(k_LENGTH));
    if (error) {
        return error;
    }

    const bsl::uint16_t hardwareType = this->hardwareType();
    if (hardwareType != k_HARDWARE_TYPE_ETHERNET) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t hardwareAddressLength = this->hardwareAddressLength();
    if (hardwareAddressLength != sizeof(ntsa::EthernetAddress)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint16_t protocolType = this->protocolType();
    if (protocolType != k_PROTOCOL_TYPE_IPV4) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t protocolAddressLength = this->protocolAddressLength();
    if (protocolAddressLength != sizeof(ntsa::Ipv4Address)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint16_t operation = this->operation();
    if (operation != ntsa::ArpType::e_REQUEST &&
        operation != ntsa::ArpType::e_RESPONSE)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ArpHeader::encode(ntsa::PacketEncoder* encoder) const
{
    ntsa::Error error;

    const bsl::uint16_t hardwareType = this->hardwareType();
    if (hardwareType != k_HARDWARE_TYPE_ETHERNET) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t hardwareAddressLength = this->hardwareAddressLength();
    if (hardwareAddressLength != sizeof(ntsa::EthernetAddress)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint16_t protocolType = this->protocolType();
    if (protocolType != k_PROTOCOL_TYPE_IPV4) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t protocolAddressLength = this->protocolAddressLength();
    if (protocolAddressLength != sizeof(ntsa::Ipv4Address)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint16_t operation = this->operation();
    if (operation != ntsa::ArpType::e_REQUEST &&
        operation != ntsa::ArpType::e_RESPONSE)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = encoder->encodeRaw(this,
                               static_cast<bsl::size_t>(k_LENGTH));
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& ArpHeader::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void ArpHeader::print(bslim::Printer* printer) const
{
    printer->printForeign(this->hardwareType(),
                          &ArpHeader::printAddressType,
                          "hardwareType");

    printer->printAttribute(
        "hardwareAddressLength",
        static_cast<int>(this->hardwareAddressLength()));

    printer->printForeign(this->protocolType(),
                          &ArpHeader::printAddressType,
                          "protocolType");

    printer->printAttribute(
        "protocolAddressLength",
        static_cast<int>(this->protocolAddressLength()));

    const bsl::uint16_t operation = this->operation();

    ntsa::ArpType::Value type = ntsa::ArpType::e_UNDEFINED;
    if (ntsa::ArpType::fromInt(&type, static_cast<int>(operation)) != 0) {
        printer->printAttribute("operation", type);
    }
    else {
        printer->printAttribute(
            "operation", static_cast<int>(this->operation()));
    }
}

bsl::ostream& ArpHeader::printAddressType(bsl::ostream& stream,
                                          bsl::uint16_t type,
                                          int           level,
                                          int           spacesPerLevel)
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    if (type == k_HARDWARE_TYPE_ETHERNET) {
        stream << "ETHERNET";
    }
    else if (type == k_PROTOCOL_TYPE_IPV4) {
        stream << "IPV4";
    }
    else {
        stream << type;
    }

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
