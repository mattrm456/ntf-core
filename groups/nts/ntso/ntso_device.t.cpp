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

#include <ntso_device.h>

#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>

#include <ball_log.h>

using namespace BloombergLP;

#if NTSO_DEVICE_ENABLED
namespace BloombergLP {
namespace ntso {

// Provide tests for 'ntso::Device'.
class DeviceTest
{
    // Execute the reader loop.
    static void reader(const bsl::shared_ptr<ntsi::Device>& device);

    // Execute the writer loop.
    static void writer(const bsl::shared_ptr<ntsi::Device>& device,
                       const bsls::TimeInterval&            duration);

    // The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSO.DEVICE.TEST");

  public:
    // Concern: TODO.
    static void verifyLoopback();

    // Concern: TODO.
    static void verifyEn0();
};

void DeviceTest::reader(const bsl::shared_ptr<ntsi::Device>& device)
{
    ntsa::Error error;

    BALL_LOG_INFO << "Test reader thread starting" << BALL_LOG_END;

    while (true) {
        ntsa::Packet packet;
        error = device->dequeue(&packet);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                BALL_LOG_INFO << "Device dequeued EOF" << BALL_LOG_END;
                break;
            }

            if (packet.isUndefined()) {
                BALL_LOG_INFO << "Device dequeued undefined packet"
                              << BALL_LOG_END;
                break;
            }
        }

        BALL_LOG_INFO << "Incoming packet = " << packet << BALL_LOG_END;
    }

    BALL_LOG_INFO << "Test reader thread complete" << BALL_LOG_END;
}

void DeviceTest::writer(const bsl::shared_ptr<ntsi::Device>& device,
                        const bsls::TimeInterval&            duration)
{
    ntsa::Error error;

    BALL_LOG_INFO << "Test writer thread starting" << BALL_LOG_END;

    bsls::TimeInterval deadline = bdlt::CurrentTime::now() + duration;

    while (true) {
        ntsa::Packet packet(NTSCFG_TEST_ALLOCATOR);

        ntsa::EthernetPacket& ethernet = packet.makeEthernet();

        ntsa::EthernetAddress sourceEthernetAddress;
        ntsa::EthernetAddress destinationEthernetAddress;

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
        ipv4.header().setId(1);
        ipv4.header().setPreserve(true);

        ntsa::UdpPacket& udp = ipv4.payload().makeUdp();

        const ntsa::Port sourceUdpPort = 3001;
        const ntsa::Port destinationUdpPort = 4001;

        udp.header().setSourcePort(sourceUdpPort);
        udp.header().setDestinationPort(destinationUdpPort);

        bdlbb::BlobBuffer payload;
        error = device->allocate(&payload);
        NTSCFG_TEST_OK(error);

        NTSCFG_MEMORY_COPY(payload.data(), "Hello, world!", 13);
        payload.setSize(13);

        BALL_LOG_ERROR << "Enqueuing packet " << packet << BALL_LOG_END;

        error = device->enqueue(packet);
        NTSCFG_TEST_OK(error);

        const bsls::TimeInterval now = bdlt::CurrentTime::now();
        if (now >= deadline) {
            break;
        }

        bsls::TimeInterval interval;
        interval.setTotalMilliseconds(100);

        bslmt::ThreadUtil::sleep(interval);
    }

    NTSCFG_TEST_LOG_DEBUG << "Closing device" << NTSCFG_TEST_LOG_END;

    error = device->close();
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Test writer thread complete" << BALL_LOG_END;
}

NTSCFG_TEST_FUNCTION(ntso::DeviceTest::verifyLoopback)
{
    if (!ntso::DeviceUtil::isSupported()) {
        return;
    }

    ntsa::Error error;
    int         rc;

    ntsa::DeviceConfig deviceConfig(NTSCFG_TEST_ALLOCATOR);
    deviceConfig.setAdapterName("lo0");

    bsl::shared_ptr<ntsi::Device> device = ntso::DeviceUtil::createDevice(
        deviceConfig, NTSCFG_TEST_ALLOCATOR);

    error = device->open();
    NTSCFG_TEST_OK(error);

    bsls::TimeInterval duration = bsls::TimeInterval(10, 0);

    bslmt::ThreadGroup threadGroup(NTSCFG_TEST_ALLOCATOR);

    {
        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName("reader");

        rc = threadGroup.addThread(
            bdlf::BindUtil::bind(
                &DeviceTest::reader, device), threadAttributes);
        NTSCFG_TEST_EQ(rc, 0);
    }

    bslmt::ThreadUtil::sleep(duration);

    NTSCFG_TEST_LOG_DEBUG << "Closing device" << NTSCFG_TEST_LOG_END;

    error = device->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(ntso::DeviceTest::verifyEn0)
{
    if (!ntso::DeviceUtil::isSupported()) {
        return;
    }

    ntsa::Error error;
    int         rc;

    ntsa::DeviceConfig deviceConfig(NTSCFG_TEST_ALLOCATOR);
    deviceConfig.setAdapterName("en0");

    bsl::shared_ptr<ntsi::Device> device = ntso::DeviceUtil::createDevice(
        deviceConfig, NTSCFG_TEST_ALLOCATOR);

    error = device->open();
    NTSCFG_TEST_OK(error);

    bsls::TimeInterval duration = bsls::TimeInterval(10, 0);
    bsls::TimeInterval deadline = bdlt::CurrentTime::now() + duration;

    bslmt::ThreadGroup threadGroup(NTSCFG_TEST_ALLOCATOR);

    {
        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName("reader");

        rc = threadGroup.addThread(
            bdlf::BindUtil::bind(
                &DeviceTest::reader, device), threadAttributes);
        NTSCFG_TEST_EQ(rc, 0);
    }

    {
        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName("writer");

        rc = threadGroup.addThread(
            bdlf::BindUtil::bind(
                &DeviceTest::writer, device, duration), threadAttributes);
        NTSCFG_TEST_EQ(rc, 0);
    }

    threadGroup.joinAll();
}

}  // close namespace ntso
}  // close namespace BloombergLP
#endif
