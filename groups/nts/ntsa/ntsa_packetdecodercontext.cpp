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

#include <ntsa_packetdecodercontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_packetdecodercontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool PacketDecoderContext::equals(const PacketDecoderContext& other) const
{
    return d_sourceEthernetAddress == other.d_sourceEthernetAddress &&
           d_sourceIpAddress == other.d_sourceIpAddress &&
           d_sourceTcpPort == other.d_sourceTcpPort &&
           d_sourceUdpPort == other.d_sourceUdpPort &&
           d_destinationEthernetAddress ==
               other.d_destinationEthernetAddress &&
           d_destinationIpAddress == other.d_destinationIpAddress &&
           d_destinationTcpPort == other.d_destinationTcpPort &&
           d_destinationUdpPort == other.d_destinationUdpPort;
}

bool PacketDecoderContext::less(const PacketDecoderContext& other) const
{
    if (d_sourceEthernetAddress < other.d_sourceEthernetAddress) {
        return true;
    }

    if (other.d_sourceEthernetAddress < d_sourceEthernetAddress) {
        return false;
    }

    if (d_sourceIpAddress < other.d_sourceIpAddress) {
        return true;
    }

    if (other.d_sourceIpAddress < d_sourceIpAddress) {
        return false;
    }

    if (d_sourceTcpPort < other.d_sourceTcpPort) {
        return true;
    }

    if (other.d_sourceTcpPort < d_sourceTcpPort) {
        return false;
    }

    if (d_sourceUdpPort < other.d_sourceUdpPort) {
        return true;
    }

    if (other.d_sourceUdpPort < d_sourceUdpPort) {
        return false;
    }

    if (d_destinationEthernetAddress < other.d_destinationEthernetAddress) {
        return true;
    }

    if (other.d_destinationEthernetAddress < d_destinationEthernetAddress) {
        return false;
    }

    if (d_destinationIpAddress < other.d_destinationIpAddress) {
        return true;
    }

    if (other.d_destinationIpAddress < d_destinationIpAddress) {
        return false;
    }

    if (d_destinationTcpPort < other.d_destinationTcpPort) {
        return true;
    }

    if (other.d_destinationTcpPort < d_destinationTcpPort) {
        return false;
    }

    return d_destinationUdpPort < other.d_destinationUdpPort;
}

bsl::ostream& PacketDecoderContext::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_sourceEthernetAddress.has_value()) {
        printer.printAttribute("sourceEthernetAddress",
                               d_sourceEthernetAddress.value());
    }

    if (d_sourceIpAddress.has_value()) {
        printer.printAttribute("sourceIpAddress", d_sourceIpAddress.value());
    }

    if (d_sourceTcpPort.has_value()) {
        printer.printAttribute("sourceTcpPort", d_sourceTcpPort.value());
    }

    if (d_sourceUdpPort.has_value()) {
        printer.printAttribute("sourceUdpPort", d_sourceUdpPort.value());
    }

    if (d_destinationEthernetAddress.has_value()) {
        printer.printAttribute("destinationEthernetAddress",
                               d_destinationEthernetAddress.value());
    }

    if (d_destinationIpAddress.has_value()) {
        printer.printAttribute("destinationIpAddress",
                               d_destinationIpAddress.value());
    }

    if (d_destinationTcpPort.has_value()) {
        printer.printAttribute("destinationTcpPort",
                               d_destinationTcpPort.value());
    }

    if (d_destinationUdpPort.has_value()) {
        printer.printAttribute("destinationUdpPort",
                               d_destinationUdpPort.value());
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
