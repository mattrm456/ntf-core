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

#include <ntsa_packetfilter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_packetfilter_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool PacketFilter::equals(const PacketFilter& other) const
{
    return d_packetType == other.d_packetType &&
           d_sourceEthernetAddress == other.d_sourceEthernetAddress &&
           d_sourceIpv4Address == other.d_sourceIpv4Address &&
           d_sourceIpv6Address == other.d_sourceIpv6Address &&
           d_sourceTcpPort == other.d_sourceTcpPort &&
           d_sourceUdpPort == other.d_sourceUdpPort &&
           d_destinationEthernetAddress ==
               other.d_destinationEthernetAddress &&
           d_destinationIpv4Address == other.d_destinationIpv4Address &&
           d_destinationIpv6Address == other.d_destinationIpv6Address &&
           d_destinationTcpPort == other.d_destinationTcpPort &&
           d_destinationUdpPort == other.d_destinationUdpPort &&
           d_flags == other.d_flags;
}

bool PacketFilter::less(const PacketFilter& other) const
{
    if (d_packetType < other.d_packetType) {
        return true;
    }

    if (other.d_packetType < d_packetType) {
        return false;
    }

    if (d_sourceEthernetAddress < other.d_sourceEthernetAddress) {
        return true;
    }

    if (other.d_sourceEthernetAddress < d_sourceEthernetAddress) {
        return false;
    }

    if (d_sourceIpv4Address < other.d_sourceIpv4Address) {
        return true;
    }

    if (other.d_sourceIpv4Address < d_sourceIpv4Address) {
        return false;
    }

    if (d_sourceIpv6Address < other.d_sourceIpv6Address) {
        return true;
    }

    if (other.d_sourceIpv6Address < d_sourceIpv6Address) {
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

    if (d_destinationIpv4Address < other.d_destinationIpv4Address) {
        return true;
    }

    if (other.d_destinationIpv4Address < d_destinationIpv4Address) {
        return false;
    }

    if (d_destinationIpv6Address < other.d_destinationIpv6Address) {
        return true;
    }

    if (other.d_destinationIpv6Address < d_destinationIpv6Address) {
        return false;
    }

    if (d_destinationTcpPort < other.d_destinationTcpPort) {
        return true;
    }

    if (other.d_destinationTcpPort < d_destinationTcpPort) {
        return false;
    }

    if (d_destinationUdpPort < other.d_destinationUdpPort) {
        return true;
    }

    if (other.d_destinationUdpPort < d_destinationUdpPort) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& PacketFilter::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_packetType.size() > 0) {
        printer.printAttribute("packetType", d_packetType);
    }

    if (d_sourceEthernetAddress.size() > 0) {
        printer.printAttribute("sourceEthernetAddress",
                               d_sourceEthernetAddress);
    }

    if (d_sourceIpv4Address.size() > 0) {
        printer.printAttribute("sourceIpv4Address",
                               d_sourceIpv4Address);
    }

    if (d_sourceIpv6Address.size() > 0) {
        printer.printAttribute("sourceIpv6Address",
                               d_sourceIpv6Address);
    }

    if (d_sourceTcpPort.size() > 0) {
        printer.printAttribute("sourceTcpPort", d_sourceTcpPort);
    }

    if (d_sourceUdpPort.size() > 9) {
        printer.printAttribute("sourceUdpPort", d_sourceUdpPort);
    }

    if (d_destinationEthernetAddress.size() > 0) {
        printer.printAttribute("destinationEthernetAddress",
                               d_destinationEthernetAddress);
    }

    if (d_destinationIpv4Address.size() > 0) {
        printer.printAttribute("destinationIpv4Address",
                               d_destinationIpv4Address);
    }

    if (d_destinationIpv6Address.size() > 0) {
        printer.printAttribute("destinationIpv6Address",
                               d_destinationIpv6Address);
    }

    if (d_destinationTcpPort.size() > 0) {
        printer.printAttribute("destinationTcpPort", d_destinationTcpPort);
    }

    if (d_destinationUdpPort.size() > 0) {
        printer.printAttribute("destinationUdpPort", d_destinationUdpPort);
    }

    if (d_flags != 0) {
        printer.printAttribute("flags", d_flags);
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
