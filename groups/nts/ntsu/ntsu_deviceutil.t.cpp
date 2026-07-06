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

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_deviceutil_t_cpp, "$Id$ $CSID$")

#include <ntsu_deviceutil.h>

#include <ntsa_packet.h>
#include <ntsa_packetpool.h>
#include <ntsa_packetqueue.h>
#include <ntsu_adapterutil.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>
#include <bsls_review.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsu {

// Provide tests for 'ntsu::DeviceUtil'.
class DeviceUtilTest
{
    /// Discover the loopback device and load its adapter into the specified
    /// 'result'. Return true if such a loopback device is found, and false
    /// otherwise.
    static bool discoverLoopback(ntsa::Adapter* result);

    /// Discover the default device and load its adapter into the specified
    /// 'result'. Return true if such a default device is found, and false
    /// otherwise.
    static bool discoverDefault(ntsa::Adapter* result);

    // Return a packet created through the specified 'packetFactory' from the
    // specified 'adapter' to that same 'adapter'.
    static bsl::shared_ptr<ntsa::Packet> createPacket(
        const ntsa::Adapter&                        adapter,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory);

    // Execute the reader loop.
    static void reader(
        ntsa::Handle                                device,
        ntsa::DeviceType::Value                     deviceType,
        const bsl::shared_ptr<ntsa::PacketQueue>&   packetQueue,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
        const bsls::TimeInterval&                   duration);

    // Execute the writer loop.
    static void writer(
        ntsa::Handle                                device,
        ntsa::DeviceType::Value                     deviceType,
        const bsl::shared_ptr<ntsa::PacketQueue>&   packetQueue,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
        const bsls::TimeInterval&                   duration);

    // Verify the specified 'adapter'.
    static void verifyAdapter(const ntsa::Adapter& adapter);

    // The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.DEVICEUTIL.TEST");

  public:
    // Concern: Verify raw packet transmission and reception from the loopback
    // network interface.
    static void verifyLoopback();

    // Concern: Verify raw packet transmission and reception from the principle
    // network interface.
    static void verifyDefault();
};

bool DeviceUtilTest::discoverLoopback(ntsa::Adapter* result)
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

bool DeviceUtilTest::discoverDefault(ntsa::Adapter* result)
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

bsl::shared_ptr<ntsa::Packet> DeviceUtilTest::createPacket(
    const ntsa::Adapter&                        adapter,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory)
{
    bsl::shared_ptr<ntsa::Packet> packet;
    packetFactory->createOutgoingPacket(&packet);

    ntsa::EthernetPacket& ethernet = packet->makeEthernet();

    ntsa::EthernetAddress sourceEthernetAddress;
    ntsa::EthernetAddress destinationEthernetAddress;

    sourceEthernetAddress.parse(adapter.ethernetAddress());
    destinationEthernetAddress.parse(adapter.ethernetAddress());

    ethernet.header().setSource(sourceEthernetAddress);
    ethernet.header().setDestination(destinationEthernetAddress);

    ethernet.header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet& ipv4 = ethernet.payload().makeIpv4();

    ntsa::Ipv4Address sourceIpv4Address      = adapter.ipv4Address().value();
    ntsa::Ipv4Address destinationIpv4Address = adapter.ipv4Address().value();

    ipv4.header().setSourceAddress(sourceIpv4Address);
    ipv4.header().setDestinationAddress(destinationIpv4Address);

    ipv4.header().setProtocol(ntsa::Ipv4Header::k_PROTOCOL_UDP);
    ipv4.header().setId(1);
    ipv4.header().setPreserve(true);

    ntsa::UdpPacket& udp = ipv4.payload().makeUdp();

    const ntsa::Port sourceUdpPort      = 3001;
    const ntsa::Port destinationUdpPort = 4001;

    udp.header().setSourcePort(sourceUdpPort);
    udp.header().setDestinationPort(destinationUdpPort);

    bdlbb::BlobBuffer payload;
    packetFactory->createOutgoingBlobBuffer(&payload);

    NTSCFG_MEMORY_COPY(payload.data(), "Hello, world!", 13);
    payload.setSize(13);

    udp.setPayload(payload);

    return packet;
}

void DeviceUtilTest::reader(
    ntsa::Handle                                device,
    ntsa::DeviceType::Value                     deviceType,
    const bsl::shared_ptr<ntsa::PacketQueue>&   packetQueue,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const bsls::TimeInterval&                   duration)
{
    ntsa::Error error;

    BALL_LOG_INFO << "Test reader thread starting" << BALL_LOG_END;

    bsls::TimeInterval now      = bdlt::CurrentTime::now();
    bsls::TimeInterval deadline = now + duration;

    while (true) {
        now = bdlt::CurrentTime::now();
        if (now >= deadline) {
            break;
        }

        // MRM
#if 0
        BALL_LOG_TRACE << "Device descriptor " << device
                       << " wait until readable starting: "
                       << BALL_LOG_END;
#endif

        error = ntsu::DeviceUtil::waitUntilReadable(device, deadline);

// MRM
#if 0
        BALL_LOG_TRACE << "Device descriptor " << device
                       << " wait until readable complete: "
                       << error
                       << BALL_LOG_END;
#endif

        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                bsls::TimeInterval interval;
                interval.setTotalMilliseconds(200);
                bslmt::ThreadUtil::sleep(interval);
                continue;
            }
            else if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                break;
            }
            else {
                BALL_LOG_ERROR << "Device descriptor " << device
                               << " failed to wait until readable: " << error
                               << BALL_LOG_END;
                break;
            }
        }

        error = ntsu::DeviceUtil::dequeuePacket(device,
                                                deviceType,
                                                packetQueue,
                                                packetFactory);
        if (error) {
            BALL_LOG_ERROR << "Device descriptor " << device
                           << " failed to dequeue packet: " << error
                           << BALL_LOG_END;
            break;
        }

        bsls::TimeInterval interval;
        interval.setTotalMilliseconds(100);

        bslmt::ThreadUtil::sleep(interval);
    }

    BALL_LOG_INFO << "Test reader thread complete" << BALL_LOG_END;
}

void DeviceUtilTest::writer(
    ntsa::Handle                                device,
    ntsa::DeviceType::Value                     deviceType,
    const bsl::shared_ptr<ntsa::PacketQueue>&   packetQueue,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory,
    const bsls::TimeInterval&                   duration)
{
    ntsa::Error error;

    BALL_LOG_INFO << "Test writer thread starting" << BALL_LOG_END;

    bsls::TimeInterval now      = bdlt::CurrentTime::now();
    bsls::TimeInterval deadline = now + duration;

    while (true) {
        now = bdlt::CurrentTime::now();
        if (now >= deadline) {
            break;
        }

        bsl::shared_ptr<ntsa::Packet> packet;
        error = packetQueue->dequeue(&packet);
        if (error) {
            if (error != ntsa::Error(ntsa::Error::e_EOF)) {
                BALL_LOG_ERROR
                    << "Failed to dequeue packet from packet queue: "
                    << BALL_LOG_END;
            }
            break;
        }

        NTSCFG_TEST_TRUE(packet);

        error = ntsu::DeviceUtil::waitUntilWritable(device, deadline);
        if (error) {
            BALL_LOG_ERROR << "Device descriptor " << device
                           << " failed to wait until writable: " << error
                           << BALL_LOG_END;
            // break;
        }

        error = ntsu::DeviceUtil::enqueuePacket(device,
                                                deviceType,
                                                packet,
                                                packetFactory);
        if (error) {
            BALL_LOG_ERROR << "Device descriptor " << device
                           << " failed to enqueue packet: " << error
                           << BALL_LOG_END;
            break;
        }
    }

    BALL_LOG_INFO << "Test writer thread complete" << BALL_LOG_END;
}

void DeviceUtilTest::verifyAdapter(const ntsa::Adapter& adapter)
{
    BALL_LOG_INFO << "Verifying adapter " << adapter << BALL_LOG_END;

    ntsa::Error error;
    int         rc;

    const bsl::size_t k_MAX_PACKETS = 1024;

    ntsa::DeviceConfig incomingDeviceConfig(NTSCFG_TEST_ALLOCATOR);
    incomingDeviceConfig.setAdapterName(adapter.name());
    incomingDeviceConfig.setIncomingEnabled(true);
    incomingDeviceConfig.setOutgoingEnabled(false);

    ntsa::Handle incomingDevice = ntsa::k_INVALID_HANDLE;

    ntsa::DeviceType::Value incomingDeviceType = ntsa::DeviceType::e_UNDEFINED;

    bsl::size_t incomingTxBufferSize = 0;
    bsl::size_t incomingRxBufferSize = 0;

    error = ntsu::DeviceUtil::open(&incomingDevice,
                                   &incomingDeviceType,
                                   &incomingTxBufferSize,
                                   &incomingRxBufferSize,
                                   adapter,
                                   incomingDeviceConfig);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_NE(incomingDevice, ntsa::k_INVALID_HANDLE);
    NTSCFG_TEST_NE(incomingDeviceType, ntsa::DeviceType::e_UNDEFINED);
    NTSCFG_TEST_GT(incomingTxBufferSize, 0);
    NTSCFG_TEST_GT(incomingRxBufferSize, 0);

    error = ntsu::DeviceUtil::setBlocking(incomingDevice, false);
    NTSCFG_TEST_OK(error);

    ntsa::PacketFilter incomingPacketFilter;

    incomingPacketFilter.addPacketType(ntsa::PacketType::e_ETHERNET);
    incomingPacketFilter.addPacketType(ntsa::PacketType::e_IPV4);
    incomingPacketFilter.addPacketType(ntsa::PacketType::e_UDP);

    incomingPacketFilter.addDestinationEthernetAddress(
        ntsa::EthernetAddress(adapter.ethernetAddress()));
    incomingPacketFilter.addDestinationIpv4Address(
        adapter.ipv4Address().value());
    incomingPacketFilter.addDestinationUdpPort(4001);

    error = ntsu::DeviceUtil::applyFilter(incomingDevice,
                                          incomingDeviceType,
                                          adapter,
                                          incomingPacketFilter);
    NTSCFG_TEST_OK(error);

    bsl::shared_ptr<ntsa::PacketPool> incomingPacketPool;
    incomingPacketPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                                     incomingTxBufferSize,
                                     incomingRxBufferSize,
                                     NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntsa::PacketQueue> incomingPacketQueue;
    incomingPacketQueue.createInplace(NTSCFG_TEST_ALLOCATOR,
                                      k_MAX_PACKETS,
                                      NTSCFG_TEST_ALLOCATOR);

    ntsa::DeviceConfig outgoingDeviceConfig(NTSCFG_TEST_ALLOCATOR);
    outgoingDeviceConfig.setAdapterName(adapter.name());
    outgoingDeviceConfig.setIncomingEnabled(false);
    outgoingDeviceConfig.setOutgoingEnabled(true);

    ntsa::Handle outgoingDevice = ntsa::k_INVALID_HANDLE;

    ntsa::DeviceType::Value outgoingDeviceType = ntsa::DeviceType::e_UNDEFINED;

    bsl::size_t outgoingTxBufferSize = 0;
    bsl::size_t outgoingRxBufferSize = 0;

    error = ntsu::DeviceUtil::open(&outgoingDevice,
                                   &outgoingDeviceType,
                                   &outgoingTxBufferSize,
                                   &outgoingRxBufferSize,
                                   adapter,
                                   outgoingDeviceConfig);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_NE(outgoingDevice, ntsa::k_INVALID_HANDLE);
    NTSCFG_TEST_NE(outgoingDeviceType, ntsa::DeviceType::e_UNDEFINED);
    NTSCFG_TEST_GT(outgoingTxBufferSize, 0);
    NTSCFG_TEST_GT(outgoingRxBufferSize, 0);

    error = ntsu::DeviceUtil::setBlocking(outgoingDevice, false);
    NTSCFG_TEST_OK(error);

    bsl::shared_ptr<ntsa::PacketPool> outgoingPacketPool;
    outgoingPacketPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                                     outgoingTxBufferSize,
                                     outgoingRxBufferSize,
                                     NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntsa::PacketQueue> outgoingPacketQueue;
    outgoingPacketQueue.createInplace(NTSCFG_TEST_ALLOCATOR,
                                      k_MAX_PACKETS,
                                      NTSCFG_TEST_ALLOCATOR);

    bsls::TimeInterval duration = bsls::TimeInterval(1, 0);
    bsls::TimeInterval deadline = bdlt::CurrentTime::now() + duration;

    bslmt::ThreadGroup incomingThreadGroup(NTSCFG_TEST_ALLOCATOR);
    bslmt::ThreadGroup outgoingThreadGroup(NTSCFG_TEST_ALLOCATOR);

    {
        bslmt::ThreadAttributes incomingThreadAttributes;
        incomingThreadAttributes.setThreadName("test-incoming");

        rc = incomingThreadGroup.addThread(
            bdlf::BindUtil::bind(&DeviceUtilTest::reader,
                                 incomingDevice,
                                 incomingDeviceType,
                                 incomingPacketQueue,
                                 incomingPacketPool,
                                 duration),
            incomingThreadAttributes);
        NTSCFG_TEST_EQ(rc, 0);
    }

    {
        bslmt::ThreadAttributes outgoingThreadAttributes;
        outgoingThreadAttributes.setThreadName("test-outgoing");

        rc = outgoingThreadGroup.addThread(
            bdlf::BindUtil::bind(&DeviceUtilTest::writer,
                                 outgoingDevice,
                                 outgoingDeviceType,
                                 outgoingPacketQueue,
                                 outgoingPacketPool,
                                 duration),
            outgoingThreadAttributes);
        NTSCFG_TEST_EQ(rc, 0);
    }

    bsl::shared_ptr<ntsa::Packet> packet =
        DeviceUtilTest::createPacket(adapter, outgoingPacketPool);

    error = outgoingPacketQueue->enqueue(packet);
    NTSCFG_TEST_OK(error);

    bslmt::ThreadUtil::sleep(duration);

    outgoingPacketQueue->shutdown();

    // MRM
#if 0
    error = ntsu::DeviceUtil::shutdown(outgoingDevice);
    NTSCFG_TEST_OK(error);

    error = ntsu::DeviceUtil::shutdown(incomingDevice);
    NTSCFG_TEST_OK(error);
#endif

    outgoingThreadGroup.joinAll();
    incomingThreadGroup.joinAll();

    incomingPacketQueue->shutdown();

    error = ntsu::DeviceUtil::close(incomingDevice);
    NTSCFG_TEST_OK(error);

    error = ntsu::DeviceUtil::close(outgoingDevice);
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(ntsu::DeviceUtilTest::verifyLoopback)
{
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    if (!ntsu::DeviceUtil::isSupported()) {
        return;
    }

    ntsa::Adapter adapter;
    if (!DeviceUtilTest::discoverLoopback(&adapter)) {
        return;
    }

    DeviceUtilTest::verifyAdapter(adapter);
}

NTSCFG_TEST_FUNCTION(ntsu::DeviceUtilTest::verifyDefault)
{
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    if (!ntsu::DeviceUtil::isSupported()) {
        return;
    }

    ntsa::Adapter adapter;
    if (!DeviceUtilTest::discoverDefault(&adapter)) {
        return;
    }

    DeviceUtilTest::verifyAdapter(adapter);
}

}  // close namespace ntsu
}  // close namespace BloombergLP
