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

#include <ntso_device.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_device_cpp, "$Id$ $CSID$")

#include <ntsa_adapter.h>
#include <ntsa_packet.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetdecodercontext.h>
#include <ntsa_packetdecoderoptions.h>
#include <ntsa_packetencoder.h>
#include <ntsa_packetencodercontext.h>
#include <ntsa_packetencoderoptions.h>
#include <ntsa_packetqueue.h>
#include <ntsa_shutdowntype.h>
#include <ntscfg_limits.h>
#include <ntscfg_platform.h>
#include <ntsu_adapterutil.h>
#include <ntsu_deviceutil.h>
#include <ntsu_packetutil.h>
#include <ntsu_routeutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

#include <bdlb_string.h>
#include <bdlbb_blob.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlcc_fixedqueue.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorguard.h>
#include <bslma_default.h>
#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

#define NTSO_DEVICE_LOG_PACKET_DECODER_ERROR(buffer, packet, error)           \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to decode packet " << (packet) << ": "    \
           << (error) << "\n"                                                 \
           << bdlb::PrintStringHexDumper((buffer).data(), (buffer).size());   \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSO_DEVICE_LOG_PACKET_ENCODER_ERROR(packet, error)                   \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to encode packet " << (packet) << ": "    \
           << (error);                                                        \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSO_DEVICE_LOG_PACKET_WRITER_ERROR(packet, error)                    \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to write packet " << (packet) << ": "     \
           << (error);                                                        \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSO_DEVICE_LOG_PACKET_WRITER_ERROR_EOF(packet)                       \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to write packet " << (packet) << ": EOF"; \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSO_DEVICE_LOG_PACKET_WRITER_UNEXPECTED_BYTES_SENT(packet,           \
                                                            buffer,           \
                                                            bytesSent)        \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to write packet " << (packet)             \
           << ": unexpected number of bytes sent: expected "                  \
           << (buffer).size() << " but found " << (bytesSent);                \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSO_DEVICE_LOG_PACKET_INCOMING_DROP(packet)                          \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Incoming packet dropped = " << (packet);                       \
                                                                              \
        BSLS_LOG_WARN("%s", ss.str().c_str());                                \
    } while (false)

#define NTSO_DEVICE_LOG_PACKET_INCOMING(packet)                               \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Incoming packet = " << (packet);                               \
                                                                              \
        BSLS_LOG_DEBUG("%s", ss.str().c_str());                               \
    } while (false)

#define NTSO_DEVICE_LOG_PACKET_OUTGOING(packet, buffer)                       \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Outgoing packet " << (packet) << ":\n"                         \
           << bdlb::PrintStringHexDumper((buffer).data(), (buffer).size());   \
                                                                              \
        BSLS_LOG_DEBUG("%s", ss.str().c_str());                               \
    } while (false)

namespace BloombergLP {
namespace ntso {

#if defined(BSLS_PLATFORM_OS_DARWIN)

/// @brief @internal
/// Provide an implementation of the 'ntso::Device' interface to send and
/// receive packets through a network device.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class Device : public ntsi::Device
{
    /// Define a type alias for a mutex.
    typedef bslmt::Mutex Mutex;

    /// Define a type alias for a condition variable.
    typedef bslmt::Condition Condition;

    /// Define a type alias for a lock guard.
    typedef bslmt::LockGuard<bslmt::Mutex> LockGuard;

    /// Enumerate the operation state.
    enum State {
        /// The object is in the default state.
        e_DEFAULT,

        /// The object is open.
        e_OPEN,

        /// The object is closing.
        e_CLOSING,

        /// The object is closed.
        e_CLOSED
    };

    /// The mutex.
    Mutex d_mutex;

    /// The outgoing device handle.
    ntsa::Handle d_outgoingDeviceHandle;

    /// The outgoing device type.
    ntsa::DeviceType::Value d_outgoingDeviceType;

    /// The outgoing packet factory.
    bsl::shared_ptr<ntsa::PacketFactory> d_outgoingPacketFactory;

    /// The outgoing packet queue.
    bsl::shared_ptr<ntsa::PacketQueue> d_outgoingPacketQueue;

    /// The thread group processing outgoing packets.
    bslmt::ThreadGroup d_outgoingThreadGroup;

    /// The outgoing state.
    bsls::AtomicInt d_outgoingState;

    /// The incoming device handle.
    ntsa::Handle d_incomingDeviceHandle;

    /// The outgoing device type.
    ntsa::DeviceType::Value d_incomingDeviceType;

    /// The incoming packet factory.
    bsl::shared_ptr<ntsa::PacketFactory> d_incomingPacketFactory;

    /// The incoming packet queue.
    bsl::shared_ptr<ntsa::PacketQueue> d_incomingPacketQueue;

    /// The thread group processing incoming packets.
    bslmt::ThreadGroup d_incomingThreadGroup;

    /// The incoming state.
    bsls::AtomicInt d_incomingState;

    /// The adapter.
    ntsa::Adapter d_adapter;

    /// The device configuration.
    ntsa::DeviceConfig d_config;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSO.DEVICE");

  private:
    Device(const Device&) BSLS_KEYWORD_DELETED;
    Device& operator=(const Device&) BSLS_KEYWORD_DELETED;

  private:
    /// Open the driver. Return the error.
    ntsa::Error openDriver();

    /// Open the outgoing packet queue. Return the error.
    ntsa::Error openOutgoingPacketQueue();

    /// Open the incoming packet queue. Return the error.
    ntsa::Error openIncomingPacketQueue();

    /// Process outgoing packets.
    void processOutgoingPacketQueue();

    /// Process incoming packets.
    void processIncomingPacketQueue();

    /// Close the outgoing packet queue.
    void closeOutgoingPacketQueue();

    /// Close the incoming packet queue.
    void closeIncomingPacketQueue();

    /// Close the incoming packet device.
    void closeDriver();

  public:
    /// Create a new device for the specified 'adapter' having the specified
    /// 'configuration'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Device(const ntsa::DeviceConfig& configuration,
                    const ntsa::Adapter&      adapter,
                    bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~Device() BSLS_KEYWORD_OVERRIDE;

    /// Open the device.
    ntsa::Error open() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a packet suitable to enqueuePacket to to the
    /// associated device.
    void createOutgoingPacket(bsl::shared_ptr<ntsa::Packet>* result)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a packet suitable to dequeuePacket from the
    /// associated device.
    void createIncomingPacket(bsl::shared_ptr<ntsa::Packet>* result)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a blob buffer suitable to enqueuePacket to
    /// to the associated device.
    void createOutgoingBlobBuffer(bdlbb::BlobBuffer* result)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a blob buffer suitable to dequeuePacket from
    /// the associated device.
    void createIncomingBlobBuffer(bdlbb::BlobBuffer* result)
        BSLS_KEYWORD_OVERRIDE;

    /// Apply the specified packet 'filter' to incoming packets. Return the
    /// error.
    ntsa::Error applyFilter(const ntsa::PacketFilter& filter) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueuePacket(const bsl::shared_ptr<ntsa::Packet>& packet)
        BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueuePacket(bslmf::MovableRef<bsl::shared_ptr<ntsa::Packet> >
                            packet) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    ntsa::Error dequeuePacket(bsl::shared_ptr<ntsa::Packet>* result)
        BSLS_KEYWORD_OVERRIDE;

    /// Close the device. Return the error.
    ntsa::Error close() BSLS_KEYWORD_OVERRIDE;

    /// Return the adapter for this device.
    const ntsa::Adapter& adapter() const BSLS_KEYWORD_OVERRIDE;
};

ntsa::Error Device::openDriver()
{
    ntsa::Error error;

    if (d_config.outgoingEnabled().value_or(true)) {
        ntsa::DeviceConfig deviceConfig = d_config;
        deviceConfig.setIncomingEnabled(false);

        bsl::size_t outgoingTxBufferSize = 0;
        bsl::size_t outgoingRxBufferSize = 0;

        error = ntsu::DeviceUtil::open(
            &d_outgoingDeviceHandle,
            &d_outgoingDeviceType,
            &outgoingTxBufferSize,
            &outgoingRxBufferSize,
            d_adapter,
            deviceConfig);

        if (error) {
            return error;
        }

        bsl::shared_ptr<ntsa::PacketPool> outgoingPacketPool;
        outgoingPacketPool.createInplace(
            d_allocator_p,
            outgoingTxBufferSize,
            outgoingRxBufferSize,
            d_allocator_p);

        d_outgoingPacketFactory = outgoingPacketPool;
    }

    if (d_config.incomingEnabled().value_or(true)) {
        ntsa::DeviceConfig deviceConfig = d_config;
        deviceConfig.setOutgoingEnabled(false);

        bsl::size_t incomingTxBufferSize = 0;
        bsl::size_t incomingRxBufferSize = 0;

        error = ntsu::DeviceUtil::open(
            &d_incomingDeviceHandle,
            &d_incomingDeviceType,
            &incomingTxBufferSize,
            &incomingRxBufferSize,
            d_adapter,
            deviceConfig);

        if (error) {
            return error;
        }

        bsl::shared_ptr<ntsa::PacketPool> incomingPacketPool;
        incomingPacketPool.createInplace(
            d_allocator_p,
            incomingTxBufferSize,
            incomingRxBufferSize,
            d_allocator_p);

        d_incomingPacketFactory = incomingPacketPool;
    }

    return ntsa::Error();
}

ntsa::Error Device::openOutgoingPacketQueue()
{
    ntsa::Error error;
    int         rc;

    if (!d_config.outgoingEnabled().value()) {
        return ntsa::Error();
    }

    if (d_outgoingState != e_DEFAULT) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_outgoingState = e_OPEN;

    BSLS_ASSERT(d_config.outgoingMinThreads().has_value());
    BSLS_ASSERT(d_config.outgoingMaxThreads().has_value());

    for (bsl::size_t i = 0; i < d_config.outgoingMaxThreads().value(); ++i) {
        bsl::string threadName = d_adapter.name();
        threadName.append("-tx", 3);
        if (d_config.outgoingMaxThreads().value() > 1) {
            threadName.append(1, '-');
            threadName.append(bsl::to_string(i));
        }

        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName(threadName);
        threadAttributes.setStackSize(1024 * 1024 * 16);

        rc = d_outgoingThreadGroup.addThread(
            bdlf::MemFnUtil::memFn(&Device::processOutgoingPacketQueue, this),
            threadAttributes);
        if (rc != 0) {
            return ntsa::Error::last();
        }
    }

    return ntsa::Error();
}

ntsa::Error Device::openIncomingPacketQueue()
{
    ntsa::Error error;
    int         rc;

    if (!d_config.incomingEnabled().value()) {
        return ntsa::Error();
    }

    if (d_incomingState != e_DEFAULT) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_incomingState = e_OPEN;

    BSLS_ASSERT(d_config.incomingMinThreads().has_value());
    BSLS_ASSERT(d_config.incomingMaxThreads().has_value());

    for (bsl::size_t i = 0; i < d_config.incomingMaxThreads().value(); ++i) {
        bsl::string threadName = d_adapter.name();
        threadName.append("-rx", 3);
        if (d_config.incomingMaxThreads().value() > 1) {
            threadName.append(1, '-');
            threadName.append(bsl::to_string(i));
        }

        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName(threadName);
        threadAttributes.setStackSize(1024 * 1024 * 16);

        rc = d_incomingThreadGroup.addThread(
            bdlf::MemFnUtil::memFn(&Device::processIncomingPacketQueue, this),
            threadAttributes);
        if (rc != 0) {
            return ntsa::Error::last();
        }
    }

    return ntsa::Error();
}

void Device::processOutgoingPacketQueue()
{
    ntsa::Error error;

    BSLS_LOG_INFO("Outgoing packet queue thread starting");

    while (true) {
        bsl::shared_ptr<ntsa::Packet> packet;
        error = d_outgoingPacketQueue->dequeuePacket(&packet);
        if (!packet || packet->isUndefined()) {
            break;
        }

        error = ntsu::DeviceUtil::enqueuePacket(
            d_outgoingDeviceHandle,
            d_outgoingDeviceType,
            packet,
            d_outgoingPacketFactory);

        if (error) {
            BSLS_LOG_ERROR("Failed to enqueuePacket packet: %s",
                           error.text().c_str());
            break;
        }
    }

    BSLS_LOG_INFO("Outgoing packet queue thread complete");
}

void Device::processIncomingPacketQueue()
{
    ntsa::Error error;
    int         rc;

    BSLS_LOG_INFO("Incoming packet queue thread starting");

    while (true) {
        error = ntsu::DeviceUtil::dequeuePacket(
            d_incomingDeviceHandle,
            d_incomingDeviceType,
            d_incomingPacketQueue,
            d_incomingPacketFactory);

        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                break;
            }
            else {
                BSLS_LOG_ERROR("Failed to dequeuePacket packet: %s",
                               error.text().c_str());
            }
        }
    }

    BSLS_LOG_INFO("Incoming packet queue thread complete");
}

void Device::closeOutgoingPacketQueue()
{
    ntsa::Error error;

    if (!d_config.outgoingEnabled().value()) {
        return;
    }

    if (d_outgoingState != e_OPEN) {
        return;
    }

    d_outgoingState = e_CLOSING;

    error = ntsu::DeviceUtil::shutdown(d_outgoingDeviceHandle);
    if (error) {
        BALL_LOG_WARN << "Failed to shutdown: " << error << BALL_LOG_END;
    }

    d_outgoingPacketQueue->enqueuePacket(bsl::shared_ptr<ntsa::Packet>());
    d_outgoingPacketQueue->shutdown();
    d_outgoingThreadGroup.joinAll();

    d_outgoingState = e_CLOSED;
}

void Device::closeIncomingPacketQueue()
{
    ntsa::Error error;

    if (!d_config.incomingEnabled().value()) {
        return;
    }

    if (d_incomingState != e_OPEN) {
        return;
    }

    d_incomingState = e_CLOSING;

    error = ntsu::DeviceUtil::shutdown(d_incomingDeviceHandle);
    if (error) {
        BALL_LOG_WARN << "Failed to shutdown: " << error << BALL_LOG_END;
    }

    d_incomingPacketQueue->enqueuePacket(bsl::shared_ptr<ntsa::Packet>());
    d_incomingPacketQueue->shutdown();
    d_incomingThreadGroup.joinAll();

    d_incomingState = e_CLOSED;
}

void Device::closeDriver()
{
    ntsa::Error error;

    if (d_outgoingDeviceHandle != ntsa::k_INVALID_HANDLE) {
        error = ntsu::DeviceUtil::close(d_outgoingDeviceHandle);
        if (error) {
            BALL_LOG_WARN << "Failed to close: " << error << BALL_LOG_END;
        }

        d_outgoingDeviceHandle = ntsa::k_INVALID_HANDLE;
        d_outgoingDeviceType   = ntsa::DeviceType::e_UNDEFINED;
    }

    if (d_incomingDeviceHandle != ntsa::k_INVALID_HANDLE) {
        error = ntsu::DeviceUtil::close(d_incomingDeviceHandle);
        if (error) {
            BALL_LOG_WARN << "Failed to close: " << error << BALL_LOG_END;
        }

        d_incomingDeviceHandle = ntsa::k_INVALID_HANDLE;
        d_incomingDeviceType   = ntsa::DeviceType::e_UNDEFINED;
    }
}

Device::Device(const ntsa::DeviceConfig& configuration,
               const ntsa::Adapter&      adapter,
               bslma::Allocator*         basicAllocator)
: d_mutex()
, d_outgoingDeviceHandle(ntsa::k_INVALID_HANDLE)
, d_outgoingDeviceType(ntsa::DeviceType::e_UNDEFINED)
, d_outgoingPacketFactory()
, d_outgoingPacketQueue()
, d_outgoingThreadGroup(basicAllocator)
, d_outgoingState(e_DEFAULT)
, d_incomingDeviceHandle(ntsa::k_INVALID_HANDLE)
, d_incomingDeviceType(ntsa::DeviceType::e_UNDEFINED)
, d_incomingPacketFactory()
, d_incomingPacketQueue()
, d_incomingThreadGroup(basicAllocator)
, d_incomingState(e_DEFAULT)
, d_adapter(adapter, basicAllocator)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    const bsl::size_t outgoingMaxPackets =
        configuration.outgoingMaxPackets().value_or(
            static_cast<bsl::size_t>(
                ntso::DeviceUtil::k_DEFAULT_OUTGOING_MAX_PACKETS));

    d_outgoingPacketQueue.createInplace(
        d_allocator_p, outgoingMaxPackets, d_allocator_p);

    const bsl::size_t incomingMaxPackets =
        configuration.incomingMaxPackets().value_or(
            static_cast<bsl::size_t>(
                ntso::DeviceUtil::k_DEFAULT_INCOMING_MAX_PACKETS));

    d_incomingPacketQueue.createInplace(
        d_allocator_p, incomingMaxPackets, d_allocator_p);
}

Device::~Device()
{
}

ntsa::Error Device::open()
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    error = openDriver();
    if (error) {
        return error;
    }

    error = openOutgoingPacketQueue();
    if (error) {
        return error;
    }

    error = openIncomingPacketQueue();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

void Device::createOutgoingPacket(bsl::shared_ptr<ntsa::Packet>* result)
{
    d_outgoingPacketFactory->createOutgoingPacket(result);
}

void Device::createIncomingPacket(bsl::shared_ptr<ntsa::Packet>* result)
{
    d_incomingPacketFactory->createIncomingPacket(result);
}

void Device::createOutgoingBlobBuffer(bdlbb::BlobBuffer* result)
{
    d_outgoingPacketFactory->createOutgoingBlobBuffer(result);
}

void Device::createIncomingBlobBuffer(bdlbb::BlobBuffer* result)
{
    d_incomingPacketFactory->createIncomingBlobBuffer(result);
}

ntsa::Error Device::applyFilter(const ntsa::PacketFilter& filter)
{
    LockGuard lock(&d_mutex);

    if (d_incomingDeviceHandle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsu::DeviceUtil::applyFilter(
        d_incomingDeviceHandle, d_incomingDeviceType, d_adapter, filter);
}

ntsa::Error Device::enqueuePacket(const bsl::shared_ptr<ntsa::Packet>& packet)
{
    return d_outgoingPacketQueue->enqueuePacket(packet);
}

ntsa::Error Device::enqueuePacket(
    bslmf::MovableRef<bsl::shared_ptr<ntsa::Packet> > packet)
{
    return d_outgoingPacketQueue->enqueuePacket(NTSCFG_MOVE(packet));
}

ntsa::Error Device::dequeuePacket(bsl::shared_ptr<ntsa::Packet>* result)
{
    ntsa::Error error;

    result->reset();

    error = d_incomingPacketQueue->dequeuePacket(result);
    if (error) {
        return error;
    }

    if (!result->get() || (*result)->isUndefined()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    return ntsa::Error();
}

ntsa::Error Device::close()
{
    LockGuard lock(&d_mutex);

    closeOutgoingPacketQueue();
    closeIncomingPacketQueue();
    closeDriver();

    return ntsa::Error();
}

const ntsa::Adapter& Device::adapter() const
{
    return d_adapter;
}

#else

/// @brief @internal
/// Provide an implementation of the 'ntso::Device' interface to send and
/// receive packets through a network device.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class Device : public ntsi::Device
{
    /// The device adapter.
    ntsa::Adapter d_adapter;

    /// The device configuration.
    ntsa::DeviceConfig d_config;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

  private:
    Device(const Device&) BSLS_KEYWORD_DELETED;
    Device& operator=(const Device&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new device having the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit Device(const ntsa::DeviceConfig& configuration,
                    bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~Device() BSLS_KEYWORD_OVERRIDE;

    /// Open the device.
    ntsa::Error open() BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueuePacket(const ntsa::Packet& packet) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueuePacket(bslmf::MovableRef<ntsa::Packet> packet)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    ntsa::Error dequeuePacket(ntsa::Packet* result) BSLS_KEYWORD_OVERRIDE;

    /// Close the device. Return the error.
    ntsa::Error close() BSLS_KEYWORD_OVERRIDE;

    /// Return the adapter for this device.
    const ntsa::Adapter& adapter() const BSLS_KEYWORD_OVERRIDE;
};

Device::Device(const ntsa::DeviceConfig& configuration,
               bslma::Allocator*         basicAllocator)
: d_adapter(basicAllocator)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Device::~Device()
{
}

ntsa::Error Device::open()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Device::enqueuePacket(const ntsa::Packet& packet)
{
    NTSCFG_WARNING_UNUSED(packet);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Device::enqueuePacket(bslmf::MovableRef<ntsa::Packet> packet)
{
    NTSCFG_WARNING_UNUSED(packet);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Device::dequeuePacket(ntsa::Packet* result)
{
    result->reset();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Device::close()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const ntsa::Adapter& Device::adapter() const
{
    return d_adapter;
}

#endif

bsl::shared_ptr<ntsi::Device> DeviceUtil::createDevice(
    const ntsa::DeviceConfig& configuration,
    bslma::Allocator*         basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Device> device;

    ntsa::DeviceConfig effectiveConfig = configuration;
    ntso::DeviceUtil::sanitizeConfig(&effectiveConfig);

    ntsa::Adapter adapter;
    error = ntso::DeviceUtil::resolveAdapter(&adapter, effectiveConfig);
    if (error) {
        NTSCFG_ABORT();
    }

    bsl::string driverName;
    if (effectiveConfig.driverName().isNull() ||
        effectiveConfig.driverName().value().empty())
    {
        driverName = "native";
    }

    if (bdlb::String::areEqualCaseless(driverName, "native")) {
        device.createInplace(allocator, effectiveConfig, adapter, allocator);
    }
    else {
        NTSCFG_ABORT();
    }

    return device;
}

void DeviceUtil::sanitizeConfig(ntsa::DeviceConfig* configuration)
{
    if (configuration->outgoingEnabled().isNull()) {
        configuration->setOutgoingEnabled(true);
    }

    if (configuration->outgoingMinThreads().isNull() ||
        configuration->outgoingMinThreads().value() == 0)
    {
        configuration->setOutgoingMinThreads(
            static_cast<bsl::size_t>(k_DEFAULT_OUTGOING_MIN_THREADS));
    }

    BSLS_ASSERT(configuration->outgoingMinThreads().has_value());
    BSLS_ASSERT(configuration->outgoingMinThreads().value() > 0);

    if (configuration->outgoingMaxThreads().isNull() ||
        configuration->outgoingMaxThreads().value() == 0)
    {
        configuration->setOutgoingMaxThreads(k_DEFAULT_OUTGOING_MAX_THREADS);
    }

    BSLS_ASSERT(configuration->outgoingMaxThreads().has_value());
    BSLS_ASSERT(configuration->outgoingMaxThreads().value() > 0);

    if (configuration->outgoingMaxThreads().value() >
        k_DEFAULT_OUTGOING_MAX_THREADS)
    {
        configuration->setOutgoingMaxThreads(k_DEFAULT_OUTGOING_MAX_THREADS);
    }

    if (configuration->outgoingMinThreads().value() >
        configuration->outgoingMaxThreads().value())
    {
        configuration->setOutgoingMinThreads(
            configuration->outgoingMaxThreads().value());
    }

    BSLS_ASSERT(configuration->outgoingMinThreads().value() <=
                configuration->outgoingMaxThreads().value());

    BSLS_ASSERT(configuration->outgoingMaxThreads().value() <=
                k_DEFAULT_OUTGOING_MAX_THREADS);

    // Sanitize the incoming configuration.

    if (configuration->incomingEnabled().isNull()) {
        configuration->setIncomingEnabled(true);
    }

    if (configuration->incomingMinThreads().isNull() ||
        configuration->incomingMinThreads().value() == 0)
    {
        configuration->setIncomingMinThreads(
            static_cast<bsl::size_t>(k_DEFAULT_OUTGOING_MIN_THREADS));
    }

    BSLS_ASSERT(configuration->incomingMinThreads().has_value());
    BSLS_ASSERT(configuration->incomingMinThreads().value() > 0);

    if (configuration->incomingMaxThreads().isNull() ||
        configuration->incomingMaxThreads().value() == 0)
    {
        configuration->setIncomingMaxThreads(k_DEFAULT_OUTGOING_MAX_THREADS);
    }

    BSLS_ASSERT(configuration->incomingMaxThreads().has_value());
    BSLS_ASSERT(configuration->incomingMaxThreads().value() > 0);

    if (configuration->incomingMaxThreads().value() >
        k_DEFAULT_OUTGOING_MAX_THREADS)
    {
        configuration->setIncomingMaxThreads(k_DEFAULT_OUTGOING_MAX_THREADS);
    }

    if (configuration->incomingMinThreads().value() >
        configuration->incomingMaxThreads().value())
    {
        configuration->setIncomingMinThreads(
            configuration->incomingMaxThreads().value());
    }

    BSLS_ASSERT(configuration->incomingMinThreads().value() <=
                configuration->incomingMaxThreads().value());

    BSLS_ASSERT(configuration->incomingMaxThreads().value() <=
                k_DEFAULT_OUTGOING_MAX_THREADS);
}

ntsa::Error DeviceUtil::resolveAdapter(ntsa::Adapter*            result,
                                       const ntsa::DeviceConfig& configuration)
{
    ntsa::Error error;

    result->reset();

    if (configuration.adapterName().has_value()) {
        bsl::vector<ntsa::Adapter> adapterList;
        ntsu::AdapterUtil::discoverAdapterList(&adapterList);

        error = ntsu::AdapterUtil::resolveAdapter(
                result,
                adapterList,
                configuration.adapterName().value());
        if (error) {
            return error;
        }
    }
    else {
        bsl::vector<ntsa::Adapter> adapterList;
        ntsu::AdapterUtil::discoverAdapterList(&adapterList);

        for (bsl::size_t i = 0; i < adapterList.size(); ++i) {
            const ntsa::Adapter& candidateAdapter = adapterList[i];
            if (candidateAdapter.ipv4Address().has_value()) {
                if (!candidateAdapter.ipv4Address().value().isLoopback()) {
                    *result = candidateAdapter;
                    break;
                }
            }
        }

        if (result->name().empty()) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
    }

    error = DeviceUtil::validateAdapter(*result, configuration);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::validateAdapter(
    const ntsa::Adapter&      adapter,
    const ntsa::DeviceConfig& configuration)
{
    ntsa::EthernetAddress adapterEthernetAddress;
    if (!adapterEthernetAddress.parse(adapter.ethernetAddress())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (configuration.adapterName().has_value()) {
        if (adapter.name() != configuration.adapterName().value()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

bool DeviceUtil::isSupported()
{
    return ntsu::DeviceUtil::isSupported();
}

ntsa::Error Network::ensureTxDevice(
    bsl::shared_ptr<ntsi::Device>* device,
    const ntsa::EthernetAddress&   ethernetAddress)
{
    ntsa::Error error;

    device->reset();

    DeviceByEthernetAddress::iterator it =
        d_txDeviceByEthernetAddress.find(ethernetAddress);
    if (it != d_txDeviceByEthernetAddress.end()) {
        *device = it->second;
        return ntsa::Error();
    }

    ntsa::Adapter adapter;
    error = ntsu::AdapterUtil::resolveAdapter(&adapter, d_adapterVector, ethernetAddress);
    if (error) {
        return error;
    }

    ntsa::DeviceConfig deviceConfig;
    deviceConfig.setAdapterName(adapter.name());
    deviceConfig.setIncomingEnabled(false);
    deviceConfig.setOutgoingEnabled(true);

    *device = ntso::DeviceUtil::createDevice(deviceConfig, d_allocator_p);

    {
        bsl::pair<DeviceByEthernetAddress::iterator, bool> emplaceResult =
            d_txDeviceByEthernetAddress.emplace(ethernetAddress, *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv4Address().has_value()) {
        bsl::pair<DeviceByIpv4Address::iterator, bool> emplaceResult =
            d_txDeviceByIpv4Address.emplace(adapter.ipv4Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv6Address().has_value()) {
        bsl::pair<DeviceByIpv6Address::iterator, bool> emplaceResult =
            d_txDeviceByIpv6Address.emplace(adapter.ipv6Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    return ntsa::Error();
}

ntsa::Error Network::ensureTxDevice(bsl::shared_ptr<ntsi::Device>* device,
                                    const ntsa::Ipv4Address&       ipv4Address)
{
    ntsa::Error error;

    device->reset();

    DeviceByIpv4Address::iterator it =
        d_txDeviceByIpv4Address.find(ipv4Address);
    if (it != d_txDeviceByIpv4Address.end()) {
        *device = it->second;
        return ntsa::Error();
    }

    ntsa::Adapter adapter;
    error = ntsu::AdapterUtil::resolveAdapter(&adapter, d_adapterVector, ipv4Address);
    if (error) {
        return error;
    }

    ntsa::EthernetAddress ethernetAddress;
    if (!ethernetAddress.parse(adapter.ethernetAddress())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::DeviceConfig deviceConfig;
    deviceConfig.setAdapterName(adapter.name());
    deviceConfig.setIncomingEnabled(false);
    deviceConfig.setOutgoingEnabled(true);

    *device = ntso::DeviceUtil::createDevice(deviceConfig, d_allocator_p);

    {
        bsl::pair<DeviceByEthernetAddress::iterator, bool> emplaceResult =
            d_txDeviceByEthernetAddress.emplace(ethernetAddress, *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv4Address().has_value()) {
        bsl::pair<DeviceByIpv4Address::iterator, bool> emplaceResult =
            d_txDeviceByIpv4Address.emplace(adapter.ipv4Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv6Address().has_value()) {
        bsl::pair<DeviceByIpv6Address::iterator, bool> emplaceResult =
            d_txDeviceByIpv6Address.emplace(adapter.ipv6Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    return ntsa::Error();
}

ntsa::Error Network::ensureTxDevice(bsl::shared_ptr<ntsi::Device>* device,
                                    const ntsa::Ipv6Address&       ipv6Address)
{
    ntsa::Error error;

    device->reset();

    DeviceByIpv6Address::iterator it =
        d_txDeviceByIpv6Address.find(ipv6Address);
    if (it != d_txDeviceByIpv6Address.end()) {
        *device = it->second;
        return ntsa::Error();
    }

    ntsa::Adapter adapter;
    error = ntsu::AdapterUtil::resolveAdapter(&adapter, d_adapterVector, ipv6Address);
    if (error) {
        return error;
    }

    ntsa::EthernetAddress ethernetAddress;
    if (!ethernetAddress.parse(adapter.ethernetAddress())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::DeviceConfig deviceConfig;
    deviceConfig.setAdapterName(adapter.name());
    deviceConfig.setIncomingEnabled(false);
    deviceConfig.setOutgoingEnabled(true);

    *device = ntso::DeviceUtil::createDevice(deviceConfig, d_allocator_p);

    {
        bsl::pair<DeviceByEthernetAddress::iterator, bool> emplaceResult =
            d_txDeviceByEthernetAddress.emplace(ethernetAddress, *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv4Address().has_value()) {
        bsl::pair<DeviceByIpv4Address::iterator, bool> emplaceResult =
            d_txDeviceByIpv4Address.emplace(adapter.ipv4Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv6Address().has_value()) {
        bsl::pair<DeviceByIpv6Address::iterator, bool> emplaceResult =
            d_txDeviceByIpv6Address.emplace(adapter.ipv6Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    return ntsa::Error();
}

ntsa::Error Network::ensureRxDevice(
    bsl::shared_ptr<ntsi::Device>* device,
    const ntsa::EthernetAddress&   ethernetAddress)
{
    ntsa::Error error;

    device->reset();

    DeviceByEthernetAddress::iterator it =
        d_rxDeviceByEthernetAddress.find(ethernetAddress);
    if (it != d_rxDeviceByEthernetAddress.end()) {
        *device = it->second;
        return ntsa::Error();
    }

    ntsa::Adapter adapter;
    error = ntsu::AdapterUtil::resolveAdapter(&adapter, d_adapterVector, ethernetAddress);
    if (error) {
        return error;
    }

    ntsa::DeviceConfig deviceConfig;
    deviceConfig.setAdapterName(adapter.name());
    deviceConfig.setIncomingEnabled(true);
    deviceConfig.setOutgoingEnabled(false);

    *device = ntso::DeviceUtil::createDevice(deviceConfig, d_allocator_p);

    {
        bsl::pair<DeviceByEthernetAddress::iterator, bool> emplaceResult =
            d_rxDeviceByEthernetAddress.emplace(ethernetAddress, *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv4Address().has_value()) {
        bsl::pair<DeviceByIpv4Address::iterator, bool> emplaceResult =
            d_rxDeviceByIpv4Address.emplace(adapter.ipv4Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv6Address().has_value()) {
        bsl::pair<DeviceByIpv6Address::iterator, bool> emplaceResult =
            d_rxDeviceByIpv6Address.emplace(adapter.ipv6Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    return ntsa::Error();
}

ntsa::Error Network::ensureRxDevice(bsl::shared_ptr<ntsi::Device>* device,
                                    const ntsa::Ipv4Address&       ipv4Address)
{
    ntsa::Error error;

    device->reset();

    DeviceByIpv4Address::iterator it =
        d_rxDeviceByIpv4Address.find(ipv4Address);
    if (it != d_rxDeviceByIpv4Address.end()) {
        *device = it->second;
        return ntsa::Error();
    }

    ntsa::Adapter adapter;
    error = ntsu::AdapterUtil::resolveAdapter(&adapter, d_adapterVector, ipv4Address);
    if (error) {
        return error;
    }

    ntsa::EthernetAddress ethernetAddress;
    if (!ethernetAddress.parse(adapter.ethernetAddress())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::DeviceConfig deviceConfig;
    deviceConfig.setAdapterName(adapter.name());
    deviceConfig.setIncomingEnabled(true);
    deviceConfig.setOutgoingEnabled(false);

    *device = ntso::DeviceUtil::createDevice(deviceConfig, d_allocator_p);

    {
        bsl::pair<DeviceByEthernetAddress::iterator, bool> emplaceResult =
            d_rxDeviceByEthernetAddress.emplace(ethernetAddress, *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv4Address().has_value()) {
        bsl::pair<DeviceByIpv4Address::iterator, bool> emplaceResult =
            d_rxDeviceByIpv4Address.emplace(adapter.ipv4Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv6Address().has_value()) {
        bsl::pair<DeviceByIpv6Address::iterator, bool> emplaceResult =
            d_rxDeviceByIpv6Address.emplace(adapter.ipv6Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    return ntsa::Error();
}

ntsa::Error Network::ensureRxDevice(bsl::shared_ptr<ntsi::Device>* device,
                                    const ntsa::Ipv6Address&       ipv6Address)
{
    ntsa::Error error;

    device->reset();

    DeviceByIpv6Address::iterator it =
        d_rxDeviceByIpv6Address.find(ipv6Address);
    if (it != d_rxDeviceByIpv6Address.end()) {
        *device = it->second;
        return ntsa::Error();
    }

    ntsa::Adapter adapter;
    error = ntsu::AdapterUtil::resolveAdapter(&adapter, d_adapterVector, ipv6Address);
    if (error) {
        return error;
    }

    ntsa::EthernetAddress ethernetAddress;
    if (!ethernetAddress.parse(adapter.ethernetAddress())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::DeviceConfig deviceConfig;
    deviceConfig.setAdapterName(adapter.name());
    deviceConfig.setIncomingEnabled(true);
    deviceConfig.setOutgoingEnabled(false);

    *device = ntso::DeviceUtil::createDevice(deviceConfig, d_allocator_p);

    {
        bsl::pair<DeviceByEthernetAddress::iterator, bool> emplaceResult =
            d_rxDeviceByEthernetAddress.emplace(ethernetAddress, *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv4Address().has_value()) {
        bsl::pair<DeviceByIpv4Address::iterator, bool> emplaceResult =
            d_rxDeviceByIpv4Address.emplace(adapter.ipv4Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    if (adapter.ipv6Address().has_value()) {
        bsl::pair<DeviceByIpv6Address::iterator, bool> emplaceResult =
            d_rxDeviceByIpv6Address.emplace(adapter.ipv6Address().value(),
                                            *device);
        BSLS_ASSERT_OPT(emplaceResult.second);
    }

    return ntsa::Error();
}

Network::Network(bslma::Allocator* basicAllocator)
: d_mutex()
, d_blobBufferFactory()
, d_adapterVector(basicAllocator)
, d_txDeviceByEthernetAddress(basicAllocator)
, d_txDeviceByIpv4Address(basicAllocator)
, d_txDeviceByIpv6Address(basicAllocator)
, d_rxDeviceByEthernetAddress(basicAllocator)
, d_rxDeviceByIpv4Address(basicAllocator)
, d_rxDeviceByIpv6Address(basicAllocator)
, d_packetFilter(basicAllocator)
, d_ethernetRouteTable(basicAllocator)
, d_ipv4RouteTable(basicAllocator)
, d_ipv6RouteTable(basicAllocator)
, d_allocator_p(basicAllocator)
{
    bsl::shared_ptr<bdlbb::PooledBlobBufferFactory> blobBufferFactory;
    blobBufferFactory.createInplace(d_allocator_p,
                                    static_cast<int>(k_MTU),
                                    d_allocator_p);

    d_blobBufferFactory = blobBufferFactory;
}

Network::~Network()
{
}

ntsa::Error Network::open()
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    ntsu::AdapterUtil::discoverAdapterList(&d_adapterVector);

    error = ntsu::RouteUtil::load(&d_ethernetRouteTable, d_adapterVector);
    if (error) {
        return error;
    }

    error = ntsu::RouteUtil::load(&d_ipv4RouteTable,
                                   d_adapterVector,
                                   d_ethernetRouteTable);
    if (error) {
        return error;
    }

    error = ntsu::RouteUtil::load(&d_ipv6RouteTable,
                                   d_adapterVector,
                                   d_ethernetRouteTable);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Network::allocate(bdlbb::BlobBuffer* buffer)
{
    d_blobBufferFactory->allocate(buffer);
    return ntsa::Error();
}

ntsa::Error Network::allocate(bsl::shared_ptr<ntsa::Packet>*       packet,
                              bsl::shared_ptr<ntsi::PacketSender>* sender,
                              const ntsa::Ipv4Address& destinationIpv4Address)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    packet->reset();

    ntsa::EthernetAddress sourceEthernetAddress;
    ntsa::EthernetAddress destinationEthernetAddress;
    ntsa::Ipv4Address     sourceIpv4Address;

    error = d_ipv4RouteTable.find(&sourceEthernetAddress,
                                  &destinationEthernetAddress,
                                  &sourceIpv4Address,
                                  destinationIpv4Address);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntsi::Device> device;
    error = this->ensureTxDevice(&device, sourceEthernetAddress);
    if (error) {
        return error;
    }

    device->createOutgoingPacket(packet);

    ntsa::EthernetPacket* ethernet = &(*packet)->makeEthernet();

    ethernet->header().setSource(sourceEthernetAddress);
    ethernet->header().setDestination(destinationEthernetAddress);
    ethernet->header().setProtocol(ntsa::EthernetProtocol::e_IPV4);

    ntsa::Ipv4Packet* ipv4 = &ethernet->payload().makeIpv4();

    ipv4->header().setSourceAddress(sourceIpv4Address);
    ipv4->header().setDestinationAddress(destinationIpv4Address);

    *sender = device;

    return ntsa::Error();
}

ntsa::Error Network::allocate(bsl::shared_ptr<ntsa::Packet>*       packet,
                              bsl::shared_ptr<ntsi::PacketSender>* sender,
                              const ntsa::Ipv6Address& destinationIpv6Address)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    packet->reset();

    ntsa::EthernetAddress sourceEthernetAddress;
    ntsa::EthernetAddress destinationEthernetAddress;
    ntsa::Ipv6Address     sourceIpv6Address;

    error = d_ipv6RouteTable.find(&sourceEthernetAddress,
                                  &destinationEthernetAddress,
                                  &sourceIpv6Address,
                                  destinationIpv6Address);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntsi::Device> device;
    error = this->ensureTxDevice(&device, sourceEthernetAddress);
    if (error) {
        return error;
    }

    device->createOutgoingPacket(packet);

    ntsa::EthernetPacket* ethernet = &(*packet)->makeEthernet();

    ethernet->header().setSource(sourceEthernetAddress);
    ethernet->header().setDestination(destinationEthernetAddress);
    ethernet->header().setProtocol(ntsa::EthernetProtocol::e_IPV6);

    ntsa::Ipv6Packet* ipv6 = &ethernet->payload().makeIpv6();

    ipv6->header().setSourceAddress(sourceIpv6Address);
    ipv6->header().setDestinationAddress(destinationIpv6Address);

    *sender = device;

    return ntsa::Error();
}

ntsa::Error Network::bind(bsl::shared_ptr<ntsi::PacketReceiver>* receiver,
                     const ntsa::EthernetAddress& sourceEthernetAddress)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<ntsi::Device> device;
    error = this->ensureRxDevice(&device, sourceEthernetAddress);
    if (error) {
        return error;
    }

    *receiver = device;

    return ntsa::Error();
}

ntsa::Error Network::bind(bsl::shared_ptr<ntsi::PacketReceiver>* receiver,
                          const ntsa::Ipv4Address& sourceIpv4Address)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<ntsi::Device> device;
    error = this->ensureRxDevice(&device, sourceIpv4Address);
    if (error) {
        return error;
    }

    *receiver = device;

    return ntsa::Error();
}

ntsa::Error Network::bind(bsl::shared_ptr<ntsi::PacketReceiver>* receiver,
                          const ntsa::Ipv6Address& sourceIpv6Address)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<ntsi::Device> device;
    error = this->ensureRxDevice(&device, sourceIpv6Address);
    if (error) {
        return error;
    }

    *receiver = device;

    return ntsa::Error();
}

ntsa::Error Network::close()
{
    LockGuard lock(&d_mutex);

    {
        DeviceByEthernetAddress::iterator it =
            d_txDeviceByEthernetAddress.begin();

        DeviceByEthernetAddress::iterator et =
            d_txDeviceByEthernetAddress.end();

        for (; it != et; ++it) {
            bsl::shared_ptr<ntsi::Device>& device = it->second;
            device->close();
            device.reset();
        }

        d_txDeviceByEthernetAddress.clear();
        d_txDeviceByIpv4Address.clear();
        d_txDeviceByIpv6Address.clear();
    }

    {
        DeviceByEthernetAddress::iterator it =
            d_rxDeviceByEthernetAddress.begin();

        DeviceByEthernetAddress::iterator et =
            d_rxDeviceByEthernetAddress.end();

        for (; it != et; ++it) {
            bsl::shared_ptr<ntsi::Device>& device = it->second;
            device->close();
            device.reset();
        }

        d_rxDeviceByEthernetAddress.clear();
        d_rxDeviceByIpv4Address.clear();
        d_rxDeviceByIpv6Address.clear();
    }

    return ntsa::Error();
}

bsl::shared_ptr<ntsi::Network> NetworkUtil::createNetwork(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Network> network;
    network.createInplace(allocator, allocator);

    return network;
}

}  // close package namespace
}  // close enterprise namespace
