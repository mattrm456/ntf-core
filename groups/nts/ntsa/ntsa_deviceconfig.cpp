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

#include <ntsa_deviceconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_deviceconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

DeviceConfig::DeviceConfig(bslma::Allocator* basicAllocator)
: d_driverName(basicAllocator)
, d_adapterName(basicAllocator)
, d_outgoingEnabled()
, d_outgoingPacketFilter()
, d_outgoingMinThreads()
, d_outgoingMaxThreads()
, d_outgoingMaxPackets()
, d_incomingEnabled()
, d_incomingPacketFilter()
, d_incomingMinThreads()
, d_incomingMaxThreads()
, d_incomingMaxPackets()
, d_promiscuous()
{
}

DeviceConfig::DeviceConfig(const DeviceConfig& original,
                           bslma::Allocator*   basicAllocator)
: d_driverName(original.d_driverName, basicAllocator)
, d_adapterName(original.d_adapterName, basicAllocator)
, d_outgoingEnabled(original.d_outgoingEnabled)
, d_outgoingPacketFilter(original.d_outgoingPacketFilter)
, d_outgoingMinThreads(original.d_outgoingMinThreads)
, d_outgoingMaxThreads(original.d_outgoingMaxThreads)
, d_outgoingMaxPackets(original.d_outgoingMaxPackets)
, d_incomingEnabled(original.d_incomingEnabled)
, d_incomingPacketFilter(original.d_incomingPacketFilter)
, d_incomingMinThreads(original.d_incomingMinThreads)
, d_incomingMaxThreads(original.d_incomingMaxThreads)
, d_incomingMaxPackets(original.d_incomingMaxPackets)
, d_promiscuous(original.d_promiscuous)
{
}

DeviceConfig::~DeviceConfig()
{
}

DeviceConfig& DeviceConfig::operator=(const DeviceConfig& other)
{
    if (this != &other) {
        d_driverName            = other.d_driverName;
        d_adapterName           = other.d_adapterName;
        d_outgoingEnabled       = other.d_outgoingEnabled;
        d_outgoingPacketFilter  = other.d_outgoingPacketFilter;
        d_outgoingMinThreads    = other.d_outgoingMinThreads;
        d_outgoingMaxThreads    = other.d_outgoingMaxThreads;
        d_outgoingMaxPackets    = other.d_outgoingMaxPackets;
        d_incomingEnabled       = other.d_incomingEnabled;
        d_incomingPacketFilter  = other.d_incomingPacketFilter;
        d_incomingMinThreads    = other.d_incomingMinThreads;
        d_incomingMaxThreads    = other.d_incomingMaxThreads;
        d_incomingMaxPackets    = other.d_incomingMaxPackets;
        d_promiscuous = other.d_promiscuous;
    }

    return *this;
}

void DeviceConfig::reset()
{
    d_driverName.reset();
    d_adapterName.reset();
    d_outgoingEnabled.reset();
    d_outgoingPacketFilter.reset();
    d_outgoingMinThreads.reset();
    d_outgoingMaxThreads.reset();
    d_outgoingMaxPackets.reset();
    d_incomingEnabled.reset();
    d_incomingPacketFilter.reset();
    d_incomingMinThreads.reset();
    d_incomingMaxThreads.reset();
    d_incomingMaxPackets.reset();
    d_promiscuous.reset();
}

void DeviceConfig::setDriverName(const bsl::string& value)
{
    d_driverName = value;
}

void DeviceConfig::setAdapterName(const bsl::string& value)
{
    d_adapterName = value;
}

void DeviceConfig::setOutgoingEnabled(bool value)
{
    d_outgoingEnabled = value;
}

void DeviceConfig::setOutgoingPacketFilter(const ntsa::PacketFilter& value)
{
    d_outgoingPacketFilter = value;
}

void DeviceConfig::setOutgoingMinThreads(bsl::size_t value)
{
    d_outgoingMinThreads = value;
}

void DeviceConfig::setOutgoingMaxThreads(bsl::size_t value)
{
    d_outgoingMaxThreads = value;
}

void DeviceConfig::setOutgoingMaxPackets(bsl::size_t value)
{
    d_outgoingMaxPackets = value;
}

void DeviceConfig::setIncomingEnabled(bool value)
{
    d_incomingEnabled = value;
}

void DeviceConfig::setIncomingPacketFilter(const ntsa::PacketFilter& value)
{
    d_incomingPacketFilter = value;
}

void DeviceConfig::setIncomingMinThreads(bsl::size_t value)
{
    d_incomingMinThreads = value;
}

void DeviceConfig::setIncomingMaxThreads(bsl::size_t value)
{
    d_incomingMaxThreads = value;
}

void DeviceConfig::setIncomingMaxPackets(bsl::size_t value)
{
    d_incomingMaxPackets = value;
}

void DeviceConfig::setPromiscuous(bool value)
{
    d_promiscuous = value;
}

const bdlb::NullableValue<bsl::string>& DeviceConfig::driverName() const
{
    return d_driverName;
}

const bdlb::NullableValue<bsl::string>& DeviceConfig::adapterName() const
{
    return d_adapterName;
}

const bdlb::NullableValue<bool>& DeviceConfig::outgoingEnabled() const
{
    return d_outgoingEnabled;
}

const bdlb::NullableValue<ntsa::PacketFilter>&
DeviceConfig::outgoingPacketFilter() const
{
    return d_outgoingPacketFilter;
}

const bdlb::NullableValue<bsl::size_t>&
DeviceConfig::outgoingMinThreads() const
{
    return d_outgoingMinThreads;
}

const bdlb::NullableValue<bsl::size_t>&
DeviceConfig::outgoingMaxThreads() const
{
    return d_outgoingMaxThreads;
}

const bdlb::NullableValue<bsl::size_t>&
DeviceConfig::outgoingMaxPackets() const
{
    return d_outgoingMaxPackets;
}

const bdlb::NullableValue<bool>& DeviceConfig::incomingEnabled() const
{
    return d_incomingEnabled;
}

const bdlb::NullableValue<ntsa::PacketFilter>&
DeviceConfig::incomingPacketFilter() const
{
    return d_incomingPacketFilter;
}

const bdlb::NullableValue<bsl::size_t>&
DeviceConfig::incomingMinThreads() const
{
    return d_incomingMinThreads;
}

const bdlb::NullableValue<bsl::size_t>&
DeviceConfig::incomingMaxThreads() const
{
    return d_incomingMaxThreads;
}

const bdlb::NullableValue<bsl::size_t>&
DeviceConfig::incomingMaxPackets() const
{
    return d_incomingMaxPackets;
}

const bdlb::NullableValue<bool>& DeviceConfig::promiscuous() const
{
    return d_promiscuous;
}

bool DeviceConfig::equals(const DeviceConfig& other) const
{
    return d_driverName           == other.d_driverName         &&
           d_adapterName          == other.d_adapterName        &&
           d_outgoingEnabled      == other.d_outgoingEnabled    &&
           d_outgoingPacketFilter == other.d_outgoingPacketFilter &&
           d_outgoingMinThreads   == other.d_outgoingMinThreads &&
           d_outgoingMaxThreads   == other.d_outgoingMaxThreads &&
           d_outgoingMaxPackets   == other.d_outgoingMaxPackets &&
           d_incomingEnabled      == other.d_incomingEnabled    &&
           d_incomingPacketFilter == other.d_incomingPacketFilter &&
           d_incomingMinThreads   == other.d_incomingMinThreads &&
           d_incomingMaxThreads   == other.d_incomingMaxThreads &&
           d_incomingMaxPackets   == other.d_incomingMaxPackets &&
           d_promiscuous == other.d_promiscuous;
}

bool DeviceConfig::less(const DeviceConfig& other) const
{
    if (d_driverName < other.d_driverName) {
        return true;
    }

    if (other.d_driverName < d_driverName) {
        return false;
    }

    if (d_adapterName < other.d_adapterName) {
        return true;
    }

    if (other.d_adapterName < d_adapterName) {
        return false;
    }

    if (d_outgoingEnabled < other.d_outgoingEnabled) {
        return true;
    }

    if (other.d_outgoingEnabled < d_outgoingEnabled) {
        return false;
    }

    if (d_outgoingPacketFilter < other.d_outgoingPacketFilter) {
        return true;
    }

    if (other.d_outgoingPacketFilter < d_outgoingPacketFilter) {
        return false;
    }

    if (d_outgoingMinThreads < other.d_outgoingMinThreads) {
        return true;
    }

    if (other.d_outgoingMinThreads < d_outgoingMinThreads) {
        return false;
    }

    if (d_outgoingMaxThreads < other.d_outgoingMaxThreads) {
        return true;
    }

    if (other.d_outgoingMaxThreads < d_outgoingMaxThreads) {
        return false;
    }

    if (d_outgoingMaxPackets < other.d_outgoingMaxPackets) {
        return true;
    }

    if (other.d_outgoingMaxPackets < d_outgoingMaxPackets) {
        return false;
    }

    if (d_incomingEnabled < other.d_incomingEnabled) {
        return true;
    }

    if (other.d_incomingEnabled < d_incomingEnabled) {
        return false;
    }

    if (d_incomingPacketFilter < other.d_incomingPacketFilter) {
        return true;
    }

    if (other.d_incomingPacketFilter < d_incomingPacketFilter) {
        return false;
    }

    if (d_incomingMinThreads < other.d_incomingMinThreads) {
        return true;
    }

    if (other.d_incomingMinThreads < d_incomingMinThreads) {
        return false;
    }

    if (d_incomingMaxThreads < other.d_incomingMaxThreads) {
        return true;
    }

    if (other.d_incomingMaxThreads < d_incomingMaxThreads) {
        return false;
    }

    if (d_incomingMaxPackets < other.d_incomingMaxPackets) {
        return true;
    }

    if (other.d_incomingMaxPackets < d_incomingMaxPackets) {
        return false;
    }

    return d_promiscuous < other.d_promiscuous;
}

bsl::ostream& DeviceConfig::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (!d_driverName.isNull()) {
        printer.printAttribute("driverName", d_driverName.value());
    }

    if (!d_adapterName.isNull()) {
        printer.printAttribute("adapterName", d_adapterName.value());
    }

    if (!d_outgoingEnabled.isNull()) {
        printer.printAttribute("outgoingEnabled", d_outgoingEnabled.value());
    }

    if (!d_outgoingPacketFilter.isNull()) {
        printer.printAttribute(
            "outgoingPacketFilter",
            d_outgoingPacketFilter.value());
    }

    if (!d_outgoingMinThreads.isNull()) {
        printer.printAttribute(
            "outgoingMinThreads", d_outgoingMinThreads.value());
    }

    if (!d_outgoingMaxThreads.isNull()) {
        printer.printAttribute(
            "outgoingMaxThreads", d_outgoingMaxThreads.value());
    }

    if (!d_outgoingMaxPackets.isNull()) {
        printer.printAttribute(
            "outgoingMaxPackets", d_outgoingMaxPackets.value());
    }

    if (!d_incomingEnabled.isNull()) {
        printer.printAttribute("incomingEnabled", d_incomingEnabled.value());
    }

    if (!d_incomingPacketFilter.isNull()) {
        printer.printAttribute(
            "incomingPacketFilter",
            d_incomingPacketFilter.value());
    }

    if (!d_incomingMinThreads.isNull()) {
        printer.printAttribute(
            "incomingMinThreads", d_incomingMinThreads.value());
    }

    if (!d_incomingMaxThreads.isNull()) {
        printer.printAttribute(
            "incomingMaxThreads", d_incomingMaxThreads.value());
    }

    if (!d_incomingMaxPackets.isNull()) {
        printer.printAttribute(
            "incomingMaxPackets", d_incomingMaxPackets.value());
    }

    if (!d_promiscuous.isNull()) {
        printer.printAttribute(
            "promiscuous", d_promiscuous.value());
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const DeviceConfig& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const DeviceConfig& lhs, const DeviceConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const DeviceConfig& lhs, const DeviceConfig& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const DeviceConfig& lhs, const DeviceConfig& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace

