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

#if NTSO_DEVICE_ENABLED

#include <ntsa_adapter.h>
#include <ntsa_shutdowntype.h>
#include <ntsa_packet.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetdecodercontext.h>
#include <ntsa_packetdecoderoptions.h>
#include <ntsa_packetencoder.h>
#include <ntsa_packetencodercontext.h>
#include <ntsa_packetencoderoptions.h>
#include <ntscfg_limits.h>
#include <ntscfg_platform.h>
#include <ntsu_adapterutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

#include <bdlb_string.h>
#include <bdlbb_blob.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlcc_fixedqueue.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bslmt_mutex.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_set.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_DARWIN)
#include <net/if.h>
#include <net/bpf.h>
#include <net/ethernet.h>
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <netinet/ip.h>
#include <netpacket/packet.h>
#include <linux/if_ether.h>
#include <linux/errqueue.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <winerror.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
// clang-format on
#ifdef interface
#undef interface
#endif
#pragma comment(lib, "ws2_32")
#endif

namespace BloombergLP {
namespace ntso {

/// @brief @internal
/// Provide an implementation of the 'ntsi::Reactor' interface to poll for
/// socket events using the 'poll' API on all platforms.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class DeviceDriver : public bdlbb::BlobBufferFactory
{
public:
    /// Destroy this object.
    virtual ~DeviceDriver();

    /// Open the device driver. Return the error.
    virtual ntsa::Error open(const ntsa::Adapter& adapter) = 0;

    /// Allocate a blob buffer from this blob buffer factory, and load it
    /// into the specified `buffer`.
    virtual void allocate(bdlbb::BlobBuffer *buffer) = 0;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueue(const ntsa::Packet& packet) = 0;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    virtual ntsa::Error enqueue(bslmf::MovableRef<ntsa::Packet> packet) = 0;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    virtual ntsa::Error dequeue(ntsa::Packet* result) = 0;

    /// Shutdown transmission and reception according to the specified 'mode'.
    /// Return the error.
    virtual ntsa::Error shutdown(ntsa::ShutdownType::Value mode) = 0;

    /// Close the device driver. Return the error.
    virtual ntsa::Error close() = 0;

    /// Return the device driver handle.
    virtual ntsa::Handle handle() const = 0;

    /// Return the adapter.
    virtual const ntsa::Adapter& adapter() const = 0;
};

DeviceDriver::~DeviceDriver()
{
}

#if defined(BSLS_PLATFORM_OS_DARWIN)

/// @brief @internal
/// Provide an implementation of the 'ntso::DeviceDriver' interface to send
/// and receive packets through a network device using BPF on Darwin.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class Bpf : public ntso::DeviceDriver
{
    /// Define a type alias for a mutex.
    typedef bslmt::Mutex Mutex;

    /// Define a type alias for a condition variable.
    typedef bslmt::Condition Condition;

    /// Define a type alias for a lock guard.
    typedef bslmt::LockGuard<bslmt::Mutex> LockGuard;

    /// The device name.
    bsl::string d_deviceName;

    /// The device handle.
    ntsa::Handle d_deviceHandle;

    /// The device buffer size.
    bsl::size_t d_deviceBufferSize;

    /// The blob buffer factory.
    bsl::shared_ptr<bdlbb::BlobBufferFactory> d_deviceBufferFactory;

    /// The adapter.
    ntsa::Adapter d_adapter;

    /// The device configuration.
    ntsa::DeviceConfig d_config;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

public:
    /// Create a new device driver having the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Bpf(const ntsa::DeviceConfig& configuration,
                 bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~Bpf() BSLS_KEYWORD_OVERRIDE;

    /// Open the device driver. Return the error.
    ntsa::Error open(const ntsa::Adapter& adapter) BSLS_KEYWORD_OVERRIDE;

    /// Allocate a blob buffer from this blob buffer factory, and load it
    /// into the specified `buffer`.
    void allocate(bdlbb::BlobBuffer *buffer) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueue(const ntsa::Packet& packet) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueue(bslmf::MovableRef<ntsa::Packet> packet)
                        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    ntsa::Error dequeue(ntsa::Packet* result) BSLS_KEYWORD_OVERRIDE;

    /// Shutdown transmission and reception according to the specified 'mode'.
    /// Return the error.
    ntsa::Error shutdown(ntsa::ShutdownType::Value mode) BSLS_KEYWORD_OVERRIDE;

    /// Close the device driver. Return the error.
    ntsa::Error close();

    /// Return the device driver handle.
    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the adapter.
    const ntsa::Adapter& adapter() const BSLS_KEYWORD_OVERRIDE;
};

Bpf::Bpf(const ntsa::DeviceConfig& configuration,
         bslma::Allocator*         basicAllocator)
: d_deviceName(basicAllocator)
, d_deviceHandle(ntsa::k_INVALID_HANDLE)
, d_deviceBufferSize(1024 * 64)
, d_deviceBufferFactory()
, d_adapter(basicAllocator)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Bpf::~Bpf()
{
    close();
}

ntsa::Error Bpf::open(const ntsa::Adapter& adapter)
{
    ntsa::Error error;
    int         rc;

    if (d_deviceHandle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_adapter = adapter;

    // Open the device handle.

    char devicePath[PATH_MAX];
    bsl::memset(devicePath, 0, sizeof devicePath);

    int openFlags = 0;
    if (d_config.outgoingEnabled().value() &&
        d_config.incomingEnabled().value())
    {
        openFlags = O_RDWR;
    }
    else if (d_config.outgoingEnabled().value()) {
        openFlags = O_WRONLY;
    }
    else if (d_config.incomingEnabled().value()) {
        openFlags = O_RDONLY;
    }
    else {
        BSLS_LOG_ERROR("BPF device driver failed to open: "
                       "not enabled for either reading or writing");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    for (int i = 0; i < 99; i++) {
        bsl::snprintf(devicePath, sizeof devicePath - 1, "/dev/bpf%d", i);
        int fd = ::open(devicePath, O_RDWR);
        if (fd >= 0) {
            d_deviceName = devicePath;
            d_deviceHandle = fd;
            break;
        }
    }

    if (d_deviceHandle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    // Set the internal buffer length.

    unsigned int deviceBufferSize =
        static_cast<unsigned int>(d_deviceBufferSize);

    rc = ioctl(d_deviceHandle, BIOCSBLEN, &deviceBufferSize);
    if (rc < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to open: "
                       "failed to set device buffer size: %s",
                       error.text().c_str());
        return error;
    }

    d_deviceBufferSize = static_cast<bsl::size_t>(deviceBufferSize);

    // Create the device buffer factory.

    bsl::shared_ptr<bdlbb::PooledBlobBufferFactory> blobBufferFactory;
    blobBufferFactory.createInplace(
        d_allocator_p, d_deviceBufferSize, d_allocator_p);

    d_deviceBufferFactory = blobBufferFactory;

    // Bind the BPF device to a network interface.

    struct ifreq ifr;
    bsl::memset(&ifr, 0, sizeof ifr);

    bsl::strncpy(
        ifr.ifr_name, d_adapter.name().c_str(), sizeof ifr.ifr_name - 1);

    rc = ioctl(d_deviceHandle, BIOCSETIF, &ifr);
    if (rc < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to open: "
                       "failed to set interface '%s': %s",
                       d_adapter.name().c_str(),
                       error.text().c_str());
        return error;
    }

    // Configure the direction.

#if defined(BSLS_PLATFORM_OS_DARWIN)

    unsigned int readSent = 0;
    rc = ioctl(d_deviceHandle, BIOCGSEESENT, &readSent);
    if (rc < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to open: "
                       "failed to set direction: %s",
                       error.text().c_str());
        return error;
    }

#endif

#if defined(BSLS_PLATFORM_OS_FREEBSD)
    unsigned int direction = BPF_D_IN;
    rc = ioctl(d_deviceHandle, BIOCGDIRECTION, &direction);
    if (rc < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to open: "
                       "failed to set direction: %s",
                       error.text().c_str());
        return error;
    }
#endif

    // Configure immediate mode to return data as soon as it arrives.

    unsigned int immediateMode = 1;
    rc = ioctl(d_deviceHandle, BIOCIMMEDIATE, &immediateMode);
    if (rc < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to open: "
                       "failed to set immediate mode: %s",
                       error.text().c_str());
        return error;
    }

    // Configure the output path to expect a fully-defined Ethernet header.

    unsigned int headerComplete = 1;
    rc = ioctl(d_deviceHandle, BIOCSHDRCMPLT, &headerComplete);
    if (rc < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to open: "
                       "failed to set header complete flag: %s",
                       error.text().c_str());
        return error;
    }

    // Configure the read timeout.

    struct timeval tv;
    bsl::memset(&tv, 0, sizeof tv);

    tv.tv_sec  = 1;
    tv.tv_usec = 0;

    rc = ioctl(d_deviceHandle, BIOCGRTIMEOUT, &tv);
    if (rc < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to open: "
                       "failed to set read timeout: %s",
                       error.text().c_str());
        return error;
    }

    // Enable promiscuous mode to read all traffic on the link.

#if 0
    rc = ioctl(d_deviceHandle, BIOCPROMISC, NULL);
    if (rc < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to open: "
                       "failed to set immediate mode: %s",
                       error.text().c_str());
        return error;
    }
#endif

    BSLS_LOG_INFO("BPF device driver opened "
                  "[ interface = %s device = %s "
                  "handle = %d bufferSize = %zu ]",
                  d_adapter.name().c_str(),
                  d_deviceName.c_str(),
                  d_deviceHandle,
                  d_deviceBufferSize);

    return ntsa::Error();
}

void Bpf::allocate(bdlbb::BlobBuffer *buffer)
{
    d_deviceBufferFactory->allocate(buffer);
}

ntsa::Error Bpf::enqueue(const ntsa::Packet& packet)
{
    ntsa::Error error;

    bdlbb::BlobBuffer buffer;
    d_deviceBufferFactory->allocate(&buffer);

    ntsa::PacketEncoderContext encoderContext;
    ntsa::PacketEncoderOptions encoderOptions;

    error = packet.encode(&encoderContext, &buffer, encoderOptions);
    if (error) {
        BSLS_LOG_ERROR("BPF device driver failed to encode packet: %s",
                       error.text().c_str());
        return error;
    }

    ssize_t bytesSent = ::write(d_deviceHandle,
                                buffer.data(),
                                static_cast<bsl::size_t>(buffer.size()));
    if (bytesSent < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to write packet: %s",
                       error.text().c_str());
        return error;
    }
    else if (bytesSent == 0) {
        BSLS_LOG_DEBUG("BPF device driver failed to write packet: EOF");
        return ntsa::Error(ntsa::Error::e_EOF);
    }
    else if (bytesSent < static_cast<ssize_t>(buffer.size())) {
        BSLS_LOG_DEBUG("BPF device driver failed to write packet: too short");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (bytesSent > static_cast<ssize_t>(buffer.size())) {
        BSLS_LOG_DEBUG("BPF device driver failed to write packet: too long");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    BSLS_ASSERT(bytesSent == static_cast<ssize_t>(buffer.size()));

    return ntsa::Error();
}

ntsa::Error Bpf::enqueue(bslmf::MovableRef<ntsa::Packet> packet)
{
    ntsa::Error error;

    bdlbb::BlobBuffer buffer;
    d_deviceBufferFactory->allocate(&buffer);

    ntsa::PacketEncoderContext encoderContext;
    ntsa::PacketEncoderOptions encoderOptions;

    error = packet.encode(&encoderContext, &buffer, encoderOptions);
    if (error) {
        BSLS_LOG_ERROR("BPF device driver failed to encode packet: %s",
                       error.text().c_str());
        return error;
    }

    ssize_t bytesSent = ::write(d_deviceHandle,
                                buffer.data(),
                                static_cast<bsl::size_t>(buffer.size()));
    if (bytesSent < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to write packet: %s",
                       error.text().c_str());
        return error;
    }
    else if (bytesSent == 0) {
        BSLS_LOG_DEBUG("BPF device driver failed to write packet: EOF");
        return ntsa::Error(ntsa::Error::e_EOF);
    }
    else if (bytesSent < static_cast<ssize_t>(buffer.size())) {
        BSLS_LOG_DEBUG("BPF device driver failed to write packet: too short");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (bytesSent > static_cast<ssize_t>(buffer.size())) {
        BSLS_LOG_DEBUG("BPF device driver failed to write packet: too long");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    BSLS_ASSERT(bytesSent == static_cast<ssize_t>(buffer.size()));

    bslmf::MovableRefUtil::access(packet).reset();

    return ntsa::Error();
}

ntsa::Error Bpf::dequeue(ntsa::Packet* result)
{
    ntsa::Error error;

    bdlbb::BlobBuffer buffer;
    d_deviceBufferFactory->allocate(&buffer);

    bsl::memset(buffer.data(), 0, static_cast<bsl::size_t>(buffer.size()));

    ssize_t bytesRead = ::read(d_deviceHandle,
                               buffer.data(),
                               static_cast<bsl::size_t>(buffer.size()));
    if (bytesRead < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to read packet: %s",
                       error.text().c_str());
        return error;
    }
    else if (bytesRead == 0) {
        BSLS_LOG_DEBUG("BPF device driver failed to read packet: EOF");
        return ntsa::Error(ntsa::Error::e_EOF);
    }
    else if (bytesRead > static_cast<ssize_t>(buffer.size())) {
        BSLS_LOG_DEBUG("BPF device driver failed to read packet: too long");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    BSLS_ASSERT(bytesRead > 0);
    BSLS_ASSERT(bytesRead <= static_cast<ssize_t>(buffer.size()));

    buffer.setSize(static_cast<int>(bytesRead));

    // MRM
#if 0
    {
        bsl::stringstream ss;
        ss << "BPF device driver read data =\n" << bdlb::PrintStringHexDumper(
            buffer.data(), buffer.size());

        BSLS_LOG_DEBUG("%s", ss.str().c_str());
    }
#endif

    char *metaFrame = buffer.data();
    char *metaFrameEnd = metaFrame + buffer.size();

    while (metaFrame < metaFrameEnd) {
        struct bpf_hdr *bpf = reinterpret_cast<struct bpf_hdr*>(buffer.data());

        BSLS_LOG_DEBUG("BPF device read packet meta-data "
                       "[ caplen = %zu datalen = %zu hdrlen = %zu ]",
                       static_cast<bsl::size_t>(bpf->bh_caplen),
                       static_cast<bsl::size_t>(bpf->bh_datalen),
                       static_cast<bsl::size_t>(bpf->bh_hdrlen));

        if (bpf->bh_caplen != bpf->bh_datalen) {
            BSLS_LOG_ERROR("BPF device driver failed to read packet: "
                           "the captured length %zu "
                           "does not match the data length %zu",
                           static_cast<bsl::size_t>(bpf->bh_caplen),
                           static_cast<bsl::size_t>(bpf->bh_datalen));

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bdlbb::BlobBuffer ethernetBuffer(
            bsl::shared_ptr<char>(
                buffer.buffer(),
                metaFrame + bpf->bh_hdrlen),
            static_cast<int>(bpf->bh_datalen));

        {
            bsl::stringstream ss;
            ss << "BPF device driver read packet =\n"
               << bdlb::PrintStringHexDumper(
                    ethernetBuffer.data(), ethernetBuffer.size());

            BSLS_LOG_DEBUG("%s", ss.str().c_str());
        }

        ntsa::PacketDecoderContext decoderContext;
        ntsa::PacketDecoderOptions decoderOptions;

        result->makeEthernet();

        error = result->decode(
            &decoderContext, ethernetBuffer, decoderOptions);
        if (error) {
            {
                // MRM
                bsl::stringstream ss;
                ss << "BPF device driver failed to decode packet = "
                   << *result;

                BSLS_LOG_ERROR(ss.str().c_str());
            }
            BSLS_LOG_ERROR("BPF device driver failed to decode packet: %s",
                           error.text().c_str());
            return error;
        }
        else
        {
            bsl::stringstream ss;
            ss << "Incoming packet = " << *result;

            BSLS_LOG_DEBUG("%s", ss.str().c_str());
        }

        metaFrame += BPF_WORDALIGN(bpf->bh_hdrlen + bpf->bh_caplen);
    }

    return ntsa::Error();
}

ntsa::Error Bpf::shutdown(ntsa::ShutdownType::Value mode)
{
    NTSCFG_WARNING_UNUSED(mode);

    return ntsa::Error();
}

ntsa::Error Bpf::close()
{
    ntsa::Error error;
    int         rc;

    if (d_deviceHandle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    BSLS_LOG_INFO("BPF device driver closing "
                  "[ interface = %s device = %s "
                  "handle = %d bufferSize = %zu ]",
                  d_adapter.name().c_str(),
                  d_deviceName.c_str(),
                  d_deviceHandle,
                  d_deviceBufferSize);

    rc = ::close(d_deviceHandle);
    if (rc != 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("Failed to close device: %s",
                       error.text().c_str());
        return error;
    }

    BSLS_LOG_INFO("BPF device driver closed "
                  "[ interface = %s device = %s "
                  "handle = %d bufferSize = %zu ]",
                  d_adapter.name().c_str(),
                  d_deviceName.c_str(),
                  d_deviceHandle,
                  d_deviceBufferSize);

    d_deviceName.clear();
    d_deviceHandle = ntsa::k_INVALID_HANDLE;

    return ntsa::Error();
}

ntsa::Handle Bpf::handle() const
{
    return d_deviceHandle;
}

const ntsa::Adapter& Bpf::adapter() const
{
    return d_adapter;
}

#endif

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
    enum State
    {
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

    /// The device driver.
    bsl::shared_ptr<ntso::DeviceDriver> d_driver;

    /// The outgoing packet queue.
    bdlcc::FixedQueue<ntsa::Packet> d_outgoingPacketQueue;

    /// The thread group processing outgoing packets.
    bslmt::ThreadGroup d_outgoingThreadGroup;

    /// The outgoing state.
    bsls::AtomicInt d_outgoingState;

    /// The incoming packet queue.
    bdlcc::FixedQueue<ntsa::Packet> d_incomingPacketQueue;

    /// The thread group processing incoming packets.
    bslmt::ThreadGroup d_incomingThreadGroup;

    /// The incoming state.
    bsls::AtomicInt d_incomingState;

    /// The device adapter.
    ntsa::Adapter d_adapter;

    /// The device configuration.
    ntsa::DeviceConfig d_config;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

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
    /// Create a new reactor having the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit Device(const ntsa::DeviceConfig& configuration,
                    bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~Device() BSLS_KEYWORD_OVERRIDE;

    /// Open the device.
    ntsa::Error open() BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueue(const ntsa::Packet& packet) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueue(bslmf::MovableRef<ntsa::Packet> packet)
                        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    ntsa::Error dequeue(ntsa::Packet* result) BSLS_KEYWORD_OVERRIDE;

    /// Close the device. Return the error.
    ntsa::Error close() BSLS_KEYWORD_OVERRIDE;

    /// Return the adapter for this device.
    const ntsa::Adapter& adapter() const BSLS_KEYWORD_OVERRIDE;
};

ntsa::Error Device::openDriver()
{
    ntsa::Error error;

    if (d_driver) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#if defined(BSLS_PLATFORM_OS_DARWIN)

    if (d_config.driverName().isNull() ||
        d_config.driverName().value().empty() ||
        bdlb::String::areEqualCaseless(d_config.driverName().value(), "bpf"))
    {
        bsl::shared_ptr<ntso::Bpf> driver;
        driver.createInplace(d_allocator_p, d_config, d_allocator_p);

        ntsa::Adapter adapter;
        error = ntso::DeviceUtil::resolveAdapter(&adapter, d_config);
        if (error) {
            return error;
        }

        error = driver->open(adapter);
        if (error) {
            return error;
        }

        d_driver = driver;
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

#else
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
#endif


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

    d_outgoingPacketQueue.enable();

    BSLS_ASSERT(d_config.outgoingMinThreads().has_value());
    BSLS_ASSERT(d_config.outgoingMaxThreads().has_value());

    for (bsl::size_t i = 0; i < d_config.outgoingMaxThreads().value(); ++i) {
        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName(
            bsl::string("device-tx-") + bsl::to_string(i));
        threadAttributes.setStackSize(1024 * 1024 * 16);

        rc = d_outgoingThreadGroup.addThread(
            bdlf::MemFnUtil::memFn(
                &Device::processOutgoingPacketQueue, this), threadAttributes);
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

    d_incomingPacketQueue.enable();

    BSLS_ASSERT(d_config.incomingMinThreads().has_value());
    BSLS_ASSERT(d_config.incomingMaxThreads().has_value());

    for (bsl::size_t i = 0; i < d_config.incomingMaxThreads().value(); ++i) {
        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName(
            bsl::string("device-rx-") + bsl::to_string(i));
        threadAttributes.setStackSize(1024 * 1024 * 16);

        rc = d_incomingThreadGroup.addThread(
            bdlf::MemFnUtil::memFn(
                &Device::processIncomingPacketQueue, this), threadAttributes);
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
        ntsa::Packet packet;
        d_outgoingPacketQueue.popFront(&packet);

        if (packet.isUndefined()) {
            break;
        }

        error = d_driver->enqueue(bslmf::MovableRefUtil::move(packet));
        if (error) {
            BSLS_LOG_ERROR("Failed to enqueue packet: %s",
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
        ntsa::Packet packet;
        error = d_driver->dequeue(&packet);
        if (error) {
            BSLS_LOG_ERROR("Failed to dequeue packet: %s",
                           error.text().c_str());
            break;
        }

        if (packet.isUndefined()) {
            break;
        }

        rc = d_incomingPacketQueue.pushBack(
            bslmf::MovableRefUtil::move(packet));
        if (rc != 0) {
            BSLS_LOG_INFO("Dropping incoming packet");
            break;
        }
    }

    BSLS_LOG_INFO("Incoming packet queue thread complete");
}

void Device::closeOutgoingPacketQueue()
{
    int rc;

    if (!d_config.outgoingEnabled().value()) {
        return;
    }

    if (d_outgoingState != e_OPEN) {
        return;
    }

    d_outgoingState = e_CLOSING;

    d_driver->shutdown(ntsa::ShutdownType::e_SEND);

    d_outgoingPacketQueue.pushBack(ntsa::Packet());
    d_outgoingPacketQueue.disable();
    d_outgoingThreadGroup.joinAll();

    d_outgoingState = e_CLOSED;
}

void Device::closeIncomingPacketQueue()
{
    int rc;

    if (!d_config.incomingEnabled().value()) {
        return;
    }

    if (d_incomingState != e_OPEN) {
        return;
    }

    d_incomingState = e_CLOSING;

    d_driver->shutdown(ntsa::ShutdownType::e_RECEIVE);

    d_incomingPacketQueue.pushBack(ntsa::Packet());
    d_incomingPacketQueue.disable();
    d_incomingThreadGroup.joinAll();

    d_incomingState = e_CLOSED;
}

void Device::closeDriver()
{
    if (d_driver) {
        d_driver->close();
    }
}

Device::Device(const ntsa::DeviceConfig& configuration,
               bslma::Allocator*         basicAllocator)
: d_mutex()
, d_driver()
, d_outgoingPacketQueue(
    configuration.outgoingMaxPackets().has_value() ?
    configuration.outgoingMaxPackets().value() :
    static_cast<bsl::size_t>(ntso::DeviceUtil::k_DEFAULT_OUTGOING_MAX_PACKETS),
    basicAllocator)
, d_outgoingThreadGroup(basicAllocator)
, d_outgoingState(e_DEFAULT)
, d_incomingPacketQueue(
    configuration.outgoingMaxPackets().has_value() ?
    configuration.outgoingMaxPackets().value() :
    static_cast<bsl::size_t>(ntso::DeviceUtil::k_DEFAULT_INCOMING_MAX_PACKETS),
    basicAllocator)
, d_incomingThreadGroup(basicAllocator)
, d_incomingState(e_DEFAULT)
, d_adapter(basicAllocator)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntso::DeviceUtil::sanitizeConfig(&d_config);
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

ntsa::Error Device::enqueue(const ntsa::Packet& packet)
{
    int rc = d_outgoingPacketQueue.pushBack(packet);
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    return ntsa::Error();
}

ntsa::Error Device::enqueue(bslmf::MovableRef<ntsa::Packet> packet)
{
    int rc = d_outgoingPacketQueue.pushBack(
        bslmf::MovableRefUtil::move(packet));
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    return ntsa::Error();
}

ntsa::Error Device::dequeue(ntsa::Packet* result)
{
    result->reset();

    d_incomingPacketQueue.popFront(result);
    if (result->isUndefined()) {
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
    ntsa::Error enqueue(const ntsa::Packet& packet)
                                      BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'packet' for transmission. Return the error.
    ntsa::Error enqueue(bslmf::MovableRef<ntsa::Packet> packet)
                        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the next packet received. Return the
    /// error.
    ntsa::Error dequeue(ntsa::Packet* result) BSLS_KEYWORD_OVERRIDE;

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

ntsa::Error Device::enqueue(const ntsa::Packet& packet)
{
    NTSCFG_WARNING_UNUSED(packet);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Device::enqueue(bslmf::MovableRef<ntsa::Packet> packet)
{
    NTSCFG_WARNING_UNUSED(packet);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Device::dequeue(ntsa::Packet* result)
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

const ntsa::Adapter& Device::adapter() const
{
    return d_adapter;
}

bsl::shared_ptr<ntsi::Device> DeviceUtil::createDevice(
    const ntsa::DeviceConfig& configuration,
    bslma::Allocator*         basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Device> device;
    device.createInplace(allocator, configuration, allocator);

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

    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);

    for (bsl::size_t i = 0; i < adapterList.size(); ++i) {
        const ntsa::Adapter& adapter = adapterList[i];

        if (configuration.adapterName().has_value()) {
            if (adapter.name() != configuration.adapterName().value()) {
                continue;
            }
        }

        if (configuration.ethernetAddress().has_value()) {
            ntsa::EthernetAddress candidateEthernetAddress;
            if (!candidateEthernetAddress.parse(adapter.ethernetAddress())) {
                continue;
            }

            if (candidateEthernetAddress !=
                configuration.ethernetAddress().value())
            {
                continue;
            }
        }

        if (configuration.ipv4Address().has_value()) {
            if (adapter.ipv4Address().isNull()) {
                continue;
            }

            if (adapter.ipv4Address().value() !=
                configuration.ipv4Address().value())
            {
                continue;
            }
        }

        if (configuration.ipv6Address().has_value()) {
            if (adapter.ipv6Address().isNull()) {
                continue;
            }

            if (adapter.ipv6Address().value() !=
                configuration.ipv6Address().value())
            {
                continue;
            }
        }

        *result = adapter;
        return ntsa::Error();
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

bool DeviceUtil::isSupported()
{
#if defined(BSLS_PLATFORM_OS_DARWIN)

    if (geteuid() == 0) {
        return true;
    }

    return false;

#elif defined(BSLS_PLATFORM_OS_LINUX)

    if (geteuid() == 0) {
        return true;
    }

#if defined(_LINUX_CAPABILITY_VERSION_3)
    const unsigned int k_CAP_VERSION = _LINUX_CAPABILITY_VERSION_3;
#else
    const unsigned int k_CAP_VERSION = 0x20080522;
#endif

#if defined(CAP_NET_RAW)
    const unsigned int k_CAP_NET_RAW = CAP_NET_RAW;
#else
    const unsigned int k_CAP_NET_RAW = 13;
#endif

#if defined(CAP_TO_INDEX)
    const unsigned int k_CAP_NET_RAW_INDEX = CAP_TO_INDEX(k_CAP_NET_RAW);
#else
    const unsigned int k_CAP_NET_RAW_INDEX = k_CAP_NET_RAW >> 5;
#endif

#if defined(CAP_TO_MASK)
    const unsigned int k_CAP_NET_RAW_MASK = CAP_TO_MASK(k_CAP_NET_RAW);
#else
    const unsigned int k_CAP_NET_RAW_MASK = (1U << ((k_CAP_NET_RAW) & 31));
#endif

    struct __user_cap_header_struct header;
    struct __user_cap_data_struct   data[2];

    header.version = k_CAP_VERSION;
    header.pid     = 0;

    if (syscall(SYS_capget, &header, data) == -1) {
        return false;
    }

    const bool found =
        (data[k_CAP_NET_RAW_INDEX].effective & k_CAP_NET_RAW_MASK) != 0;

    return found;

#else
    return false;
#endif
}

}  // close package namespace
}  // close enterprise namespace

#endif
