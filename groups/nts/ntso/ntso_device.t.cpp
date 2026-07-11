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

#include <ntscfg_test.h>
#include <ntso_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_device_t_cpp, "$Id$ $CSID$")

#include <ntsa_adapter.h>
#include <ntso_device.h>
#include <ntsu_adapterutil.h>

#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>

#include <ball_log.h>

#include <bsls_review.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntso {

// Provide tests for 'ntso::Device'.
class DeviceTest
{
    /// Discover the loopback device and load its adapter into the specified
    /// 'result'. Return true if such a loopback device is found, and false
    /// otherwise.
    static bool discoverLoopback(ntsa::Adapter* result);

    /// Discover the default device and load its adapter into the specified
    /// 'result'. Return true if such a default device is found, and false
    /// otherwise.
    static bool discoverDefault(ntsa::Adapter* result);

    // Execute the reader loop.
    static void reader(const bsl::shared_ptr<ntsi::Device>& device,
                       const bsls::TimeInterval&            duration);

    // Execute the writer loop.
    static void writer(const bsl::shared_ptr<ntsi::Device>& device,
                       const bsls::TimeInterval&            duration);

    // Verify the specified 'adapter'.
    static void verifyAdapter(const ntsa::Adapter& adapter);

    // The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSO.DEVICE.TEST");

  public:
    // Concern: Verify raw packet transmission and reception from the loopback
    // network interface.
    static void verifyLoopback();

    // Concern: Verify raw packet transmission and reception from the principle
    // network interface.
    static void verifyDefault();
};

bool DeviceTest::discoverLoopback(ntsa::Adapter* result)
{
    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);

    for (bsl::size_t i = 0; i < adapterList.size(); ++i) {
        const ntsa::Adapter& candidateAdapter = adapterList[i];
        if (candidateAdapter.ipv4Address().has_value()) {
            if (candidateAdapter.ipv4Address().value().isLoopback()) {
                *result = candidateAdapter;
                return true;
            }
        }
    }

    return false;
}

bool DeviceTest::discoverDefault(ntsa::Adapter* result)
{
    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);

    for (bsl::size_t i = 0; i < adapterList.size(); ++i) {
        const ntsa::Adapter& candidateAdapter = adapterList[i];
        if (candidateAdapter.ipv4Address().has_value()) {
            if (!candidateAdapter.ipv4Address().value().isLoopback()) {
                *result = candidateAdapter;
                return true;
            }
        }
    }

    return false;
}

void DeviceTest::reader(const bsl::shared_ptr<ntsi::Device>& device,
                        const bsls::TimeInterval&            duration)
{
    ntsa::Error error;

    BALL_LOG_INFO << "Test reader thread starting" << BALL_LOG_END;

    bsls::TimeInterval now      = bdlt::CurrentTime::now();
    bsls::TimeInterval deadline = now + duration;

    while (true) {
        bsl::shared_ptr<ntsa::Packet> packet;
        error = device->dequeuePacket(&packet);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                BALL_LOG_INFO << "Device dequeued EOF" << BALL_LOG_END;
                break;
            }

            NTSCFG_TEST_TRUE(packet);
            NTSCFG_TEST_FALSE(packet->isUndefined());
        }
    }

    BALL_LOG_INFO << "Test reader thread complete" << BALL_LOG_END;
}

void DeviceTest::writer(const bsl::shared_ptr<ntsi::Device>& device,
                        const bsls::TimeInterval&            duration)
{
    ntsa::Error error;

    BALL_LOG_INFO << "Test writer thread starting" << BALL_LOG_END;

    bsls::TimeInterval now      = bdlt::CurrentTime::now();
    bsls::TimeInterval deadline = now + duration;

    bsl::uint16_t nextId = 1;

    while (true) {
        now = bdlt::CurrentTime::now();
        if (now >= deadline) {
            break;
        }

        bsl::shared_ptr<ntsa::Packet> packet;
        device->createOutgoingPacket(&packet);

        ntsa::EthernetPacket& ethernet = packet->makeEthernet();

        ntsa::EthernetAddress sourceEthernetAddress;
        ntsa::EthernetAddress destinationEthernetAddress;

        sourceEthernetAddress.parse(device->adapter().ethernetAddress());
        destinationEthernetAddress.parse(device->adapter().ethernetAddress());

        ethernet.header().setSource(sourceEthernetAddress);
        ethernet.header().setDestination(destinationEthernetAddress);

        ethernet.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

        ntsa::Ipv4Packet& ipv4 = ethernet.payload().makeIpv4();

        ntsa::Ipv4Address sourceIpv4Address = ntsa::Ipv4Address::loopback();
        ntsa::Ipv4Address destinationIpv4Address =
            device->adapter().ipv4Address().value();

        ipv4.header().setSourceAddress(sourceIpv4Address);
        ipv4.header().setDestinationAddress(destinationIpv4Address);

        ipv4.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_UDP);
        ipv4.header().setId(nextId++);
        ipv4.header().setPreserve(true);

        ntsa::UdpPacket& udp = ipv4.payload().makeUdp();

        const ntsa::Port sourceUdpPort = 3001;
        const ntsa::Port destinationUdpPort = 4001;

        udp.header().setSourcePort(sourceUdpPort);
        udp.header().setDestinationPort(destinationUdpPort);

        bdlbb::BlobBuffer payload;
        device->createOutgoingBlobBuffer(&payload);

        NTSCFG_MEMORY_COPY(payload.data(), "Hello, world!", 13);
        payload.setSize(13);

        udp.setPayload(payload);

        error = device->enqueuePacket(packet);
        NTSCFG_TEST_OK(error);

        bsls::TimeInterval interval;
        interval.setTotalMilliseconds(250);

        bslmt::ThreadUtil::sleep(interval);
    }

    BALL_LOG_INFO << "Test writer thread complete" << BALL_LOG_END;
}

void DeviceTest::verifyAdapter(const ntsa::Adapter& adapter)
{
    BALL_LOG_INFO << "Verifying adapter " << adapter << BALL_LOG_END;

    ntsa::Error error;
    int         rc;

    ntsa::PacketFilter incomingPacketFilter;

    incomingPacketFilter.addPacketType(ntsa::PacketType::e_ETHERNET);
    incomingPacketFilter.addPacketType(ntsa::PacketType::e_IPV4);
    incomingPacketFilter.addPacketType(ntsa::PacketType::e_UDP);
    incomingPacketFilter.addPacketType(ntsa::PacketType::e_ICMP);

    incomingPacketFilter.addDestinationEthernetAddress(
        ntsa::EthernetAddress(adapter.ethernetAddress()));

    incomingPacketFilter.addDestinationIpv4Address(
        adapter.ipv4Address().value());
    incomingPacketFilter.addDestinationUdpPort(4001);

    ntsa::DeviceConfig deviceConfig(NTSCFG_TEST_ALLOCATOR);
    deviceConfig.setAdapterName(adapter.name());
    deviceConfig.setIncomingEnabled(true);
    deviceConfig.setOutgoingEnabled(true);
    deviceConfig.setIncomingPacketFilter(incomingPacketFilter);

    bsl::shared_ptr<ntsi::Device> device =
        ntso::DeviceUtil::createDevice(
            deviceConfig, NTSCFG_TEST_ALLOCATOR);

    error = device->open();
    NTSCFG_TEST_OK(error);

    bsls::TimeInterval duration = bsls::TimeInterval(3, 0);

    bslmt::ThreadGroup incomingThreadGroup(NTSCFG_TEST_ALLOCATOR);
    bslmt::ThreadGroup outgoingThreadGroup(NTSCFG_TEST_ALLOCATOR);

    {
        bslmt::ThreadAttributes incomingThreadAttributes;
        incomingThreadAttributes.setThreadName("test-incoming");

        rc = incomingThreadGroup.addThread(
            bdlf::BindUtil::bind(
                &DeviceTest::reader, device, duration),
                incomingThreadAttributes);
        NTSCFG_TEST_EQ(rc, 0);
    }

    {
        bslmt::ThreadAttributes outgoingThreadAttributes;
        outgoingThreadAttributes.setThreadName("test-outgoing");

        rc = outgoingThreadGroup.addThread(
            bdlf::BindUtil::bind(
                &DeviceTest::writer, device, duration),
                outgoingThreadAttributes);
        NTSCFG_TEST_EQ(rc, 0);
    }

    bslmt::ThreadUtil::sleep(duration);

    BALL_LOG_WARN << "Join application outgoing thread group starting" << BALL_LOG_END;

    outgoingThreadGroup.joinAll();

    BALL_LOG_WARN << "Join application outgoing thread group complete" << BALL_LOG_END;

    BALL_LOG_WARN << "Closing device" << BALL_LOG_END;

    error = device->close();
    NTSCFG_TEST_OK(error);

    BALL_LOG_WARN << "Join application incoming thread group starting" << BALL_LOG_END;

    incomingThreadGroup.joinAll();

    BALL_LOG_WARN << "Join application incoming thread group complete" << BALL_LOG_END;
}

NTSCFG_TEST_FUNCTION(ntso::DeviceTest::verifyLoopback)
{
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    if (!ntso::DeviceUtil::isSupported()) {
        return;
    }

    ntsa::Adapter adapter;
    if (!DeviceTest::discoverLoopback(&adapter)) {
        return;
    }

    DeviceTest::verifyAdapter(adapter);
}

NTSCFG_TEST_FUNCTION(ntso::DeviceTest::verifyDefault)
{
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    if (!ntso::DeviceUtil::isSupported()) {
        return;
    }

    ntsa::Adapter adapter;
    if (!DeviceTest::discoverDefault(&adapter)) {
        return;
    }

    DeviceTest::verifyAdapter(adapter);
}

}  // close namespace ntso
}  // close namespace BloombergLP

