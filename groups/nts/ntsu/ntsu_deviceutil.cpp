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

#include <ntsu_deviceutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_deviceutil_cpp, "$Id$ $CSID$")

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
#include <ntsu_packetutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

#include <bdlb_string.h>
#include <bdlbb_blob.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlcc_fixedqueue.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlt_currenttime.h>

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

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_string.h>
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
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_DARWIN)
#include <arpa/inet.h>
#include <net/bpf.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>
#include <netinet/if_ether.h>
#include <sys/sysctl.h>
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <linux/errqueue.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
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
namespace ntsu {

#if defined(BSLS_PLATFORM_OS_DARWIN)

#define NTSU_DEVICEUTIL_LOG_OPEN(device,                                      \
                                 adapter,                                     \
                                 path,                                        \
                                 bufferSize,                                  \
                                 readTimeout,                                 \
                                 dataLinkType,                                \
                                 dataLinkTypeSupport)                         \
    do {                                                                      \
        BALL_LOG_TRACE_BLOCK                                                  \
        {                                                                     \
            BALL_LOG_OUTPUT_STREAM                                            \
                << "BPF device descriptor " << (device)                       \
                << " open [ interface = " << (adapter).name()                 \
                << " path = " << (path) << " bufferSize = " << (bufferSize)   \
                << " readTimeout = " << (readTimeout) << " dataLinkType = ";  \
                                                                              \
            DeviceUtil::Impl::printDataLinkType(BALL_LOG_OUTPUT_STREAM,       \
                                                (dataLinkType));              \
                                                                              \
            BALL_LOG_OUTPUT_STREAM << " dataLinkTypeSupport = ";              \
                                                                              \
            DeviceUtil::Impl::printDataLinkTypeSupport(                       \
                BALL_LOG_OUTPUT_STREAM,                                       \
                (dataLinkTypeSupport));                                       \
                                                                              \
            BALL_LOG_OUTPUT_STREAM << " ]";                                   \
        };                                                                    \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_OPEN_DISABLED()                                   \
    do {                                                                      \
        BALL_LOG_ERROR << "BPF device driver failed to open: "                \
                       << "not enabled for either reading or writing"         \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_OPEN_FAILED(error)                                \
    do {                                                                      \
        BALL_LOG_ERROR << "BPF device failed to open: " << (error)            \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_CLOSE_STARTING(device)                            \
    do {                                                                      \
        BALL_LOG_TRACE << "BPF device descriptor " << (device)                \
                       << " close starting" << BALL_LOG_END;                  \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_CLOSE_COMPLETE(device)                            \
    do {                                                                      \
        BALL_LOG_TRACE << "BPF device descriptor " << (device)                \
                       << " close complete" << BALL_LOG_END;                  \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_CLOSE_FAILED(device, error)                       \
    do {                                                                      \
        BALL_LOG_ERROR << "BPF device descriptor " << (device)                \
                       << " close failed: " << (error) << BALL_LOG_END;       \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_ERROR(device, operation, error)                   \
    do {                                                                      \
        BALL_LOG_ERROR << "BPF device descriptor " << (device)                \
                       << " failed to " << (operation) << ": " << (error)     \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_PACKET_DECODER_ERROR(buffer, packet, error)       \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to decode packet " << (packet) << ": "    \
           << (error) << "\n"                                                 \
           << bdlb::PrintStringHexDumper((buffer).data(), (buffer).size());   \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_PACKET_ENCODER_ERROR(packet, error)               \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to encode packet " << (packet) << ": "    \
           << (error);                                                        \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_PACKET_WRITER_ERROR(packet, error)                \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to write packet " << (packet) << ": "     \
           << (error);                                                        \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_PACKET_WRITER_ERROR_EOF(packet)                   \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to write packet " << (packet) << ": EOF"; \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_PACKET_WRITER_UNEXPECTED_BYTES_SENT(packet,       \
                                                                buffer,       \
                                                                bytesSent)    \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Device driver failed to write packet " << (packet)             \
           << ": unexpected number of bytes sent: expected "                  \
           << (buffer).size() << " but found " << (bytesSent);                \
                                                                              \
        BSLS_LOG_ERROR("%s", ss.str().c_str());                               \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_PACKET_INCOMING_DROP(packet)                      \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Incoming packet dropped = " << (packet);                       \
                                                                              \
        BSLS_LOG_WARN("%s", ss.str().c_str());                                \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_PACKET_INCOMING(packet)                           \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Incoming packet = " << (packet);                               \
                                                                              \
        BSLS_LOG_DEBUG("%s", ss.str().c_str());                               \
    } while (false)

#define NTSU_DEVICEUTIL_LOG_PACKET_OUTGOING(packet, buffer)                   \
    do {                                                                      \
        bsl::stringstream ss;                                                 \
        ss << "Outgoing packet " << (packet) << ":\n"                         \
           << bdlb::PrintStringHexDumper((buffer).data(), (buffer).size());   \
                                                                              \
        BSLS_LOG_DEBUG("%s", ss.str().c_str());                               \
    } while (false)

/// Provide a private, platform-specific implementation of utilities for
/// link-level devices.
class DeviceUtil::Impl
{
  public:
    /// Enumerates the constants used by the implementation.
    enum Constant {
        /// The default TX buffer size.
        k_DEFAULT_TX_BUFFER_SIZE = 2048,

        /// The default RX buffer size.
        k_DEFAULT_RX_BUFFER_SIZE = 65536
    };

    /// Set the immediate mode of the specified 'device' to the specified
    /// 'value'. Return the error.
    static ntsa::Error setImmediate(ntsa::Handle device, bool value);

    /// Set the promiscuous mode of the specified 'device' to the specified
    /// 'value'. Return the error.
    static ntsa::Error setPromiscuous(ntsa::Handle device, bool value);

    /// Set the blocking mode of the specified 'device' to the specified
    /// 'value'. Return the error.
    static ntsa::Error setBlocking(ntsa::Handle device, bool value);

    /// Get the blocking mode of the specified 'device' and load it into the
    /// specified 'result'. Return the error.
    static ntsa::Error getBlocking(ntsa::Handle device, bool* result);

    /// Set the buffer size of the specified 'device' to the specified 'value'.
    /// Return the error.
    static ntsa::Error setBufferSize(ntsa::Handle device, bsl::size_t value);

    /// Get the buffer size of the specified 'device' and load it into the
    /// specified 'result'. Return the error.
    static ntsa::Error getBufferSize(ntsa::Handle device, bsl::size_t* result);

    /// Set the transmission visibility mode of the specified 'device' to the
    /// specified 'value'. Return the error.
    static ntsa::Error setSeeSent(ntsa::Handle device, bool value);

    /// Get the transmission visibility mode of the specified 'device' and load
    /// it into the specified 'result'. Return the error.
    static ntsa::Error getSeeSent(ntsa::Handle device, bool* result);

    /// Set the link-layer complete header mode of the specified 'device' to
    /// the specified 'value'. Return the error.
    static ntsa::Error setHeaderComplete(ntsa::Handle device, bool value);

    /// Get the link-layer complete header mode of the specified 'device' and
    /// load it into the specified 'result'. Return the error.
    static ntsa::Error getHeaderComplete(ntsa::Handle device, bool* result);

    /// Set the read timeout of the specified 'device' to the specified
    /// 'value'. Return the error.
    static ntsa::Error setReadTimeout(ntsa::Handle              device,
                                      const bsls::TimeInterval& value);

    /// Get the read timeout of the specified 'device' and load it into the
    /// specified 'result'. Return the error.
    static ntsa::Error getReadTimeout(ntsa::Handle        device,
                                      bsls::TimeInterval* result);

    /// Set the data link type of the specified 'device' to the specified
    /// 'value'. Return the error.
    static ntsa::Error setDataLinkType(ntsa::Handle  device,
                                       bsl::uint32_t value);

    /// Get the data link type of the specified 'device' and load it into the
    /// specified 'result'. Return the error.
    static ntsa::Error getDataLinkType(ntsa::Handle   device,
                                       bsl::uint32_t* result);

    /// Get the data link type support of the specified 'device' and load it
    /// into the specified 'result'. Return the error.
    static ntsa::Error getDataLinkTypeSupport(
        ntsa::Handle                device,
        bsl::vector<bsl::uint32_t>* result);

    /// Get the device type of the specified 'device' and load it into the
    /// specified 'result'. Return the error.
    static ntsa::Error getDeviceType(
        ntsa::Handle                device,
        ntsa::DeviceType::Value*    result);

    /// Set the network interface of the specified 'device' to the specified
    /// 'value'. Return the error.
    static ntsa::Error setAdapter(ntsa::Handle         device,
                                  const ntsa::Adapter& value);

    /// Get the network interface of the specified 'device' and load it into
    /// the specified 'result'. Return the error.
    static ntsa::Error getAdapter(ntsa::Handle device, ntsa::Adapter* result);

    /// Apply the specified packet 'filter' to the specified 'device'. Return
    /// the error.
    static ntsa::Error applyFilter(ntsa::Handle              device,
                                   ntsa::DeviceType::Value   deviceType,
                                   const ntsa::Adapter&      adapter,
                                   const ntsa::PacketFilter& filter);

    /// Print a formatted, human-readable description of the specified
    /// 'dataLinkType' to the specified 'stream'. Return a reference to the
    /// modifiable 'stream'.
    static bsl::ostream& printDataLinkType(bsl::ostream& stream,
                                           bsl::uint32_t dataLinkType);

    /// Print a formatted, human-readable description of the specified
    /// 'dataLinkTypeSupport' to the specified 'stream'. Return a reference to
    /// the modifiable 'stream'.
    static bsl::ostream& printDataLinkTypeSupport(
        bsl::ostream&                     stream,
        const bsl::vector<bsl::uint32_t>& dataLinkTypeSupport);

    /// Load into the specified 'result' the device type converted from the
    /// specified 'dataLinkType'. Return the error.
    static ntsa::Error convertFromDataLinkType(
        ntsa::DeviceType::Value* result,
        bsl::uint32_t            dataLinkType);
};

ntsa::Error DeviceUtil::Impl::setImmediate(ntsa::Handle device, bool value)
{
    ntsa::Error error;
    int         rc;

    unsigned int immediateMode = value ? 1 : 0;

    rc = ioctl(device, BIOCIMMEDIATE, &immediateMode);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "set immediate mode", error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::setPromiscuous(ntsa::Handle device, bool value)
{
    ntsa::Error error;
    int         rc;

    unsigned int promiscuousMode = value ? 1 : 0;

    rc = ioctl(device, BIOCPROMISC, &promiscuousMode);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "set promiscuous mode", error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::setBlocking(ntsa::Handle device, bool value)
{
    ntsa::Error error;
    int         rc;

    int flags = fcntl(device, F_GETFL, 0);
    if (flags < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "get blocking mode", error);
        return error;
    }

    if (value) {
        flags &= ~O_NONBLOCK;
    }
    else {
        flags |= O_NONBLOCK;
    }

    rc = fcntl(device, F_SETFL, flags);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "set blocking mode", error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::getBlocking(ntsa::Handle device, bool* result)
{
    ntsa::Error error;

    *result = false;

    int flags = fcntl(device, F_GETFL, 0);
    if (flags < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "get blocking mode", error);
        return error;
    }

    *result = ((flags & O_NONBLOCK) == 0);

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::setBufferSize(ntsa::Handle device,
                                            bsl::size_t  value)
{
    ntsa::Error error;
    int         rc;

    unsigned int deviceBufferSize = static_cast<unsigned int>(value);

    rc = ioctl(device, BIOCSBLEN, &deviceBufferSize);
    if (rc < 0) {
        error = ntsa::Error::last();
        NTSU_DEVICEUTIL_LOG_ERROR(device, "set buffer size", error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::getBufferSize(ntsa::Handle device,
                                            bsl::size_t* result)
{
    ntsa::Error error;
    int         rc;

    *result = 0;

    unsigned int deviceBufferSize = 0;

    rc = ioctl(device, BIOCGBLEN, &deviceBufferSize);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "get buffer size", error);
        return error;
    }

    *result = static_cast<bsl::size_t>(deviceBufferSize);

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::setSeeSent(ntsa::Handle device, bool value)
{
#if defined(BSLS_PLATFORM_OS_DARWIN)

    ntsa::Error error;
    int         rc;

    unsigned int seeSent = value ? 1 : 0;

    rc = ioctl(device, BIOCSSEESENT, &seeSent);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device,
                                  "set self-transmission visibility",
                                  error);
        return error;
    }

    return ntsa::Error();

#elif defined(BSLS_PLATFORM_OS_FREEBSD)

    ntsa::Error error;
    int         rc;

    unsigned int direction = value ? BPF_D_INOUT : BPF_D_IN;

    rc = ioctl(device, BIOCGDIRECTION, &direction);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device,
                                  "set self-transmission visibility",
                                  error);
        return error;
    }

    return ntsa::Error();

#else
#error Not implemented
#endif
}

ntsa::Error DeviceUtil::Impl::getSeeSent(ntsa::Handle device, bool* result)
{
#if defined(BSLS_PLATFORM_OS_DARWIN)

    ntsa::Error error;
    int         rc;

    *result = false;

    unsigned int seeSent = 0;

    rc = ioctl(device, BIOCGSEESENT, &seeSent);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device,
                                  "get self-transmisison visibility",
                                  error);
        return error;
    }

    *result = static_cast<bool>(seeSent);

    return ntsa::Error();

#elif defined(BSLS_PLATFORM_OS_FREEBSD)

    ntsa::Error error;
    int         rc;

    *result = false;

    unsigned int direction = 0;

    rc = ioctl(device, BIOCGDIRECTION, &direction);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device,
                                  "get self-transmission visibility",
                                  error);
        return error;
    }

    if (direction == BPF_D_INOUT) {
        *result = true;
    }
    else if (direction == BPF_D_IN) {
        *result = false;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();

#else
#error Not implemented
#endif
}

ntsa::Error DeviceUtil::Impl::setHeaderComplete(ntsa::Handle device,
                                                bool         value)
{
    ntsa::Error error;
    int         rc;

    unsigned int headerComplete = value ? 1 : 0;

    rc = ioctl(device, BIOCSHDRCMPLT, &headerComplete);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "set header complete", error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::getHeaderComplete(ntsa::Handle device,
                                                bool*        result)
{
    ntsa::Error error;
    int         rc;

    *result = false;

    unsigned int headerComplete = 0;

    rc = ioctl(device, BIOCGHDRCMPLT, &headerComplete);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "get header complete", error);
        return error;
    }

    *result = static_cast<bool>(headerComplete);

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::setReadTimeout(ntsa::Handle              device,
                                             const bsls::TimeInterval& value)
{
    ntsa::Error error;
    int         rc;

    struct timeval tv;
    NTSCFG_MEMORY_ZERO(&tv, sizeof tv);

    tv.tv_sec  = 1;
    tv.tv_usec = 0;

    rc = ioctl(device, BIOCSRTIMEOUT, &tv);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "set read timeout", error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::getReadTimeout(ntsa::Handle        device,
                                             bsls::TimeInterval* result)
{
    ntsa::Error error;
    int         rc;

    *result = bsls::TimeInterval();

    struct timeval tv;
    NTSCFG_MEMORY_ZERO(&tv, sizeof tv);

    rc = ioctl(device, BIOCGRTIMEOUT, &tv);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "get read timeout", error);
        return error;
    }

    result->addSeconds(static_cast<bsls::Types::Int64>(tv.tv_sec));
    result->addMicroseconds(static_cast<bsls::Types::Int64>(tv.tv_usec));

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::setDataLinkType(ntsa::Handle  device,
                                              bsl::uint32_t value)
{
    ntsa::Error error;
    int         rc;

    unsigned int dataLinkType = static_cast<unsigned int>(value);

    rc = ioctl(device, BIOCSDLT, &dataLinkType);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "set data link type", error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::getDataLinkType(ntsa::Handle   device,
                                              bsl::uint32_t* result)
{
    ntsa::Error error;
    int         rc;

    *result = false;

    unsigned int dataLinkType = 0;

    rc = ioctl(device, BIOCGDLT, &dataLinkType);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "get data link type", error);
        return error;
    }

    *result = static_cast<bsl::uint32_t>(dataLinkType);

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::getDataLinkTypeSupport(
    ntsa::Handle                device,
    bsl::vector<bsl::uint32_t>* result)
{
    BSLMF_ASSERT(sizeof(bsl::uint32_t) == sizeof(u_int));

    ntsa::Error error;
    int         rc;

    result->clear();

    struct bpf_dltlist dlt_list;
    NTSCFG_MEMORY_ZERO(&dlt_list, sizeof dlt_list);

    rc = ioctl(device, BIOCGDLTLIST, &dlt_list);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device,
                                  "get data link type support requirements",
                                  error);
        return error;
    }

    if (dlt_list.bfl_len == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    result->resize(static_cast<bsl::size_t>(dlt_list.bfl_len));
    dlt_list.bfl_u.bflu_list = &result->front();

    rc = ioctl(device, BIOCGDLTLIST, &dlt_list);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "get data link type support", error);
        return error;
    }

    bsl::sort(result->begin(), result->end());

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::getDeviceType(
        ntsa::Handle                device,
        ntsa::DeviceType::Value*    result)
{
    ntsa::Error error;

    bsl::uint32_t dataLinkType;
    error = DeviceUtil::Impl::getDataLinkType(device, &dataLinkType);
    if (error) {
        return error;
    }

    error = DeviceUtil::Impl::convertFromDataLinkType(result, dataLinkType);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::setAdapter(ntsa::Handle         device,
                                         const ntsa::Adapter& value)
{
    ntsa::Error error;
    int         rc;

    struct ifreq ifr;
    NTSCFG_MEMORY_ZERO(&ifr, sizeof ifr);

    bsl::strncpy(ifr.ifr_name, value.name().c_str(), sizeof ifr.ifr_name - 1);

    rc = ioctl(device, BIOCSETIF, &ifr);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "set network interface", error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::getAdapter(ntsa::Handle   device,
                                         ntsa::Adapter* result)
{
    ntsa::Error error;
    int         rc;

    result->reset();

    struct ifreq ifr;
    NTSCFG_MEMORY_ZERO(&ifr, sizeof ifr);

    rc = ioctl(device, BIOCGETIF, &ifr);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "get network interface", error);
        return error;
    }

    bsl::size_t nameLength = bsl::strlen(ifr.ifr_name);

    bsl::string name(ifr.ifr_name, nameLength);

    bsl::vector<ntsa::Adapter> adapterVector;
    ntsu::AdapterUtil::discoverAdapterList(&adapterVector);

    for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
        if (adapterVector[i].name() == name) {
            *result = adapterVector[i];
            break;
        }
    }

    if (result->name().empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::Impl::applyFilter(ntsa::Handle              device,
                                          ntsa::DeviceType::Value   deviceType,
                                          const ntsa::Adapter&      adapter,
                                          const ntsa::PacketFilter& filter)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(filter);

    ntsa::Error error;
    int         rc;

    if (deviceType == ntsa::DeviceType::e_LOCAL ||
        deviceType == ntsa::DeviceType::e_LOOPBACK)
    {
        return ntsa::Error();  // TODO
    }

    if (deviceType != ntsa::DeviceType::e_ETHERNET) {
        BALL_LOG_ERROR << "Device descriptor " << device << " failed to apply packet filter: the device type " << deviceType << " is not supported" << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    ntsa::EthernetAddress ethernetAddress;
    if (!ethernetAddress.parse(adapter.ethernetAddress())) {
        BALL_LOG_ERROR << "Device descriptor " << device << " failed to apply packet filter: failed to parse ethernet address '" << adapter.ethernetAddress() << "'" << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // MRM
#if 1
    unsigned char target_mac[6];
    NTSCFG_MEMORY_COPY(target_mac, &ethernetAddress, sizeof target_mac);

    // NNNN

    // 5. Define BPF Bytecode Instructions
    // This cBPF code evaluates 'ether dst' or 'dst host' at the link layer
    struct bpf_insn bpf_code[] = {
        // Load the first 4 bytes of the destination MAC address into
        // accumulator

        /* 0 */
        BPF_STMT(BPF_LD + BPF_W + BPF_ABS, 0),

        // Compare against the first 4 bytes of our target MAC

        /* 1 */
        BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K,
                 ((bpf_u_int32)(target_mac[0]) << 24) |
                     ((bpf_u_int32)(target_mac[1]) << 16) |
                     ((bpf_u_int32)(target_mac[2]) << 8) |
                     (bpf_u_int32)(target_mac[3]),
                 0,
                 3),

        // Load the next 2 bytes of the destination MAC address

        /* 2 */
        BPF_STMT(BPF_LD + BPF_H + BPF_ABS, 4),

        // Compare against the last 2 bytes of our target MAC

        /* 3 */
        BPF_JUMP(
            BPF_JMP + BPF_JEQ + BPF_K,
            ((bpf_u_int32)(target_mac[4]) << 8) | (bpf_u_int32)(target_mac[5]),
            0,
            1),

        // If it matches, accept the packet (return max cap length)

        /* 4 */
        BPF_STMT(BPF_RET + BPF_K, (u_int)-1),

        // If it does not match, drop the packet (return 0 bytes)

        /* 5 */
        BPF_STMT(BPF_RET + BPF_K, 0)};

    struct bpf_program bpf_prog = {
        .bf_len   = sizeof(bpf_code) / sizeof(struct bpf_insn),
        .bf_insns = bpf_code};
#endif

    ntsu::PacketFilter::Program program;
    error = ntsu::PacketUtil::compile(&program , deviceType, adapter, filter);
    if (error) {
        BALL_LOG_ERROR << "Failed to compiler packet filter program: "
                       << error << BALL_LOG_END;
        return error;
    }

    // MRM
    #if 1
    BSLMF_ASSERT(sizeof(struct bpf_insn) ==
                 sizeof(ntsu::PacketFilter::Instruction));

    BALL_LOG_INFO << "Canonical BPF:\n"
                  << bdlb::PrintStringHexDumper(
                        reinterpret_cast<const char*>(bpf_prog.bf_insns),
                        bpf_prog.bf_len * sizeof(struct bpf_insn))
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Compiled/Linked BPF:\n"
                  << bdlb::PrintStringHexDumper(
                        reinterpret_cast<const char*>(&program.front()),
                        program.size() * sizeof(struct bpf_insn))
                  << BALL_LOG_END;

    if (program.size() != bpf_prog.bf_len) {
        BALL_LOG_ERROR << "Filter programs instruction counts are not "
                            "equal: expected "
                        << bpf_prog.bf_len << " but found "
                        << program.size() << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    for (bsl::size_t i = 0; i < program.size(); ++i) {
        const struct bpf_insn& e = bpf_prog.bf_insns[i];
        const ntsu::PacketFilter::Instruction& f = program[i];

        BSLMF_ASSERT(sizeof e == sizeof f);

        if (NTSCFG_MEMORY_COMPARE(&f, &e, sizeof f) != 0) {
            BALL_LOG_INFO << "E[" << i << "]:\n"
                        << bdlb::PrintStringHexDumper(
                                reinterpret_cast<const char*>(&e),
                                sizeof(struct bpf_insn))
                        << BALL_LOG_END;

            BALL_LOG_INFO << "F[" << i << "]:\n"
                        << bdlb::PrintStringHexDumper(
                                reinterpret_cast<const char*>(&f),
                                sizeof f)
                        << BALL_LOG_END;
        }
    }

    if (NTSCFG_MEMORY_COMPARE(
            &program.front(),
            bpf_prog.bf_insns,
            bpf_prog.bf_len * sizeof(struct bpf_insn)) != 0)
    {
        BALL_LOG_ERROR << "Filter programs are not equal" << BALL_LOG_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    #endif

    struct bpf_program bpf;
    NTSCFG_MEMORY_ZERO(&bpf, sizeof bpf);

    bpf.bf_insns = reinterpret_cast<struct bpf_insn*>(&program.front());
    bpf.bf_len   = static_cast<u_int>(program.size());

    rc = ioctl(device, BIOCSETF, &bpf);
    if (rc < 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_ERROR(device, "apply packet filter", error);
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& DeviceUtil::Impl::printDataLinkType(bsl::ostream& stream,
                                                  bsl::uint32_t dataLinkType)
{
    if (dataLinkType == DLT_NULL) {
        stream << "DLT_NULL";
    }
    else if (dataLinkType == DLT_LOOP) {
        stream << "DLT_LOOP";
    }
    else if (dataLinkType == DLT_RAW) {
        stream << "DLT_RAW";
    }
    else if (dataLinkType == DLT_EN10MB) {
        stream << "DLT_EN10MB";
    }
    else if (dataLinkType == DLT_IEEE802_11_RADIO) {
        stream << "DLT_IEEE802_11_RADIO";
    }
    else {
        stream << dataLinkType;
    }

    return stream;
}

bsl::ostream& DeviceUtil::Impl::printDataLinkTypeSupport(
    bsl::ostream&                     stream,
    const bsl::vector<bsl::uint32_t>& dataLinkTypeSupport)
{
    stream << "[";
    for (bsl::size_t i = 0; i < dataLinkTypeSupport.size(); ++i) {
        const bsl::uint32_t dataLinkType = dataLinkTypeSupport[i];
        stream << " ";
        DeviceUtil::Impl::printDataLinkType(stream, dataLinkType);
    }
    stream << " ]";

    return stream;
}

ntsa::Error DeviceUtil::Impl::convertFromDataLinkType(
    ntsa::DeviceType::Value* result,
    bsl::uint32_t            dataLinkType)
{
    if (dataLinkType == DLT_NULL) {
        *result = ntsa::DeviceType::e_LOCAL;
    }
    else if (dataLinkType == DLT_LOOP) {
        *result = ntsa::DeviceType::e_LOOPBACK;
    }
    else if (dataLinkType == DLT_RAW) {
        *result = ntsa::DeviceType::e_IP;
    }
    else if (dataLinkType == DLT_EN10MB) {
        *result = ntsa::DeviceType::e_ETHERNET;
    }
    else if (dataLinkType == DLT_IEEE802_11_RADIO) {
        *result = ntsa::DeviceType::e_WIRELESS;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::open(ntsa::Handle*             result,
                             ntsa::DeviceType::Value*  type,
                             bsl::size_t*              txBufferSize,
                             bsl::size_t*              rxBufferSize,
                             const ntsa::Adapter&      adapter,
                             const ntsa::DeviceConfig& configuration)
{
    ntsa::Error error;

    *result       = ntsa::k_INVALID_HANDLE;
    *type         = ntsa::DeviceType::e_UNDEFINED;
    *txBufferSize = 0;
    *rxBufferSize = 0;

    ntsa::Handle device = ntsa::k_INVALID_HANDLE;

    bool loopback = false;
    if (adapter.ipv4Address().has_value() &&
        adapter.ipv4Address().value().isLoopback())
    {
        loopback = true;
    }

    const bool outgoing = configuration.outgoingEnabled().value_or(false);
    const bool incoming = configuration.incomingEnabled().value_or(false);

    char path[PATH_MAX];
    NTSCFG_MEMORY_ZERO(path, sizeof path);

    int flags = 0;
    if (outgoing && incoming) {
        flags = O_RDWR;
    }
    else if (outgoing) {
        flags = O_WRONLY;
    }
    else if (incoming) {
        flags = O_RDONLY;
    }
    else {
        NTSU_DEVICEUTIL_LOG_OPEN_DISABLED();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    for (int i = 0; i < 99; i++) {
        bsl::snprintf(path, sizeof path - 1, "/dev/bpf%d", i);
        int fd = ::open(path, flags);

        if (fd < 0) {
            const int lastError = errno;
            if (lastError == EBUSY) {
                continue;
            }
            else {
                error = ntsa::Error(lastError);
                NTSU_DEVICEUTIL_LOG_OPEN_FAILED(error);
                return error;
            }
        }

        device = fd;
        break;
    }

    if (device == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    ntsu::DeviceGuard guard(device);

    // Configure the internal buffer size.

    error = DeviceUtil::Impl::setBufferSize(
        device,
        static_cast<bsl::size_t>(DeviceUtil::Impl::k_DEFAULT_RX_BUFFER_SIZE));
    if (error) {
        return error;
    }

    // Configure the read timeout.

    error = DeviceUtil::Impl::setReadTimeout(device, bsls::TimeInterval(1, 0));
    if (error) {
        return error;
    }

    // Configure blocking mode.

    error = DeviceUtil::Impl::setBlocking(device, true);
    if (error) {
        return error;
    }

    // Configure immediate mode.

    error = DeviceUtil::Impl::setImmediate(device, true);
    if (error) {
        return error;
    }

    // Configure the visibility of transmitted packets.

    error = DeviceUtil::Impl::setSeeSent(device, true);
    if (error) {
        return error;
    }

    // Configure complete Ethernet headers.

    error = DeviceUtil::Impl::setHeaderComplete(device, true);
    if (error) {
        return error;
    }

    // Configure the packet filter.

    ntsa::PacketFilter packetFilter;
    if (configuration.incomingPacketFilter().has_value()) {
        packetFilter = configuration.incomingPacketFilter().value();
    }

    if (loopback) {
        error =
            DeviceUtil::Impl::applyFilter(device, ntsa::DeviceType::e_LOCAL, adapter, packetFilter);
        if (error) {
            return error;
        }
    }
    else {
        error =
            DeviceUtil::Impl::applyFilter(device, ntsa::DeviceType::e_ETHERNET, adapter, packetFilter);
        if (error) {
            return error;
        }
    }

    // Configure the network interface.

    error = DeviceUtil::Impl::setAdapter(device, adapter);
    if (error) {
        return error;
    }

    // Get the internal buffer size.

    bsl::size_t bufferSize = 0;

    error = DeviceUtil::Impl::getBufferSize(device, &bufferSize);
    if (error) {
        return error;
    }

    // Get the read timeout.

    bsls::TimeInterval readTimeout;

    error = DeviceUtil::Impl::getReadTimeout(device, &readTimeout);
    if (error) {
        return error;
    }

    // Get the supported data link types.

    bsl::vector<bsl::uint32_t> dataLinkTypeSupport;

    error =
        DeviceUtil::Impl::getDataLinkTypeSupport(device, &dataLinkTypeSupport);
    if (error) {
        return error;
    }

    // Get the current data link type.

    bsl::uint32_t dataLinkType = DLT_NULL;

    error = DeviceUtil::Impl::getDataLinkType(device, &dataLinkType);
    if (error) {
        return error;
    }

    if (loopback) {
        if (dataLinkType != DLT_NULL && dataLinkType != DLT_LOOP) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    error = DeviceUtil::Impl::convertFromDataLinkType(type, dataLinkType);
    if (error) {
        return error;
    }

    NTSU_DEVICEUTIL_LOG_OPEN(device,
                             adapter,
                             path,
                             bufferSize,
                             readTimeout,
                             dataLinkType,
                             dataLinkTypeSupport);

    guard.release();

    *result = device;

    *txBufferSize =
        static_cast<bsl::size_t>(DeviceUtil::Impl::k_DEFAULT_TX_BUFFER_SIZE);

    *rxBufferSize = static_cast<bsl::size_t>(bufferSize);

    return ntsa::Error();
}

ntsa::Error DeviceUtil::applyFilter(ntsa::Handle              device,
                                    ntsa::DeviceType::Value   deviceType,
                                    const ntsa::Adapter&      adapter,
                                    const ntsa::PacketFilter& filter)
{
    ntsa::Error error;

    error = DeviceUtil::Impl::applyFilter(device, deviceType, adapter, filter);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::setBlocking(ntsa::Handle device, bool value)
{
    return DeviceUtil::Impl::setBlocking(device, value);
}

ntsa::Error DeviceUtil::getBlocking(ntsa::Handle device, bool* result)
{
    return DeviceUtil::Impl::getBlocking(device, result);
}

ntsa::Error DeviceUtil::waitUntilReadable(ntsa::Handle device)
{
    struct ::pollfd pfd;

    pfd.fd      = device;
    pfd.events  = POLLIN | POLLHUP | POLLERR | POLLNVAL;
    pfd.revents = 0;

    int rc = ::poll(&pfd, 1, -1);
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if (((pfd.revents & POLLIN) != 0) || ((pfd.revents & POLLHUP) != 0)) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error DeviceUtil::waitUntilReadable(ntsa::Handle              device,
                                          const bsls::TimeInterval& timeout)
{
    struct ::pollfd pfd;

    pfd.fd      = device;
    pfd.events  = POLLIN | POLLHUP | POLLERR | POLLNVAL;
    pfd.revents = 0;

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsls::TimeInterval delta;
    if (timeout > now) {
        delta = timeout - now;
    }

    bsl::int64_t milliseconds =
        static_cast<bsl::int64_t>(delta.totalMilliseconds());

    if (milliseconds > bsl::numeric_limits<int>::max()) {
        milliseconds = bsl::numeric_limits<int>::max();
    }

    int rc = ::poll(&pfd, 1, static_cast<int>(milliseconds));
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if (((pfd.revents & POLLIN) != 0) || ((pfd.revents & POLLHUP) != 0)) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error DeviceUtil::waitUntilWritable(ntsa::Handle device)
{
    struct ::pollfd pfd;

    pfd.fd      = device;
    pfd.events  = POLLOUT | POLLERR | POLLNVAL;
    pfd.revents = 0;

    int rc = ::poll(&pfd, 1, -1);
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if ((pfd.revents & POLLOUT) != 0) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error DeviceUtil::waitUntilWritable(ntsa::Handle              device,
                                          const bsls::TimeInterval& timeout)
{
    struct ::pollfd pfd;

    pfd.fd      = device;
    pfd.events  = POLLOUT | POLLERR | POLLNVAL;
    pfd.revents = 0;

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsls::TimeInterval delta;
    if (timeout > now) {
        delta = timeout - now;
    }

    bsl::int64_t milliseconds =
        static_cast<bsl::int64_t>(delta.totalMilliseconds());

    if (milliseconds > bsl::numeric_limits<int>::max()) {
        milliseconds = bsl::numeric_limits<int>::max();
    }

    int rc = ::poll(&pfd, 1, static_cast<int>(milliseconds));
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if ((pfd.revents & POLLOUT) != 0) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error DeviceUtil::waitUntilError(ntsa::Handle device)
{
    struct ::pollfd pfd;

    pfd.fd      = device;
    pfd.events  = POLLERR | POLLNVAL;
    pfd.revents = 0;

    int rc = ::poll(&pfd, 1, -1);
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if ((pfd.revents & POLLERR) != 0) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error DeviceUtil::waitUntilError(ntsa::Handle              device,
                                       const bsls::TimeInterval& timeout)
{
    struct ::pollfd pfd;

    pfd.fd      = device;
    pfd.events  = POLLERR | POLLNVAL;
    pfd.revents = 0;

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsls::TimeInterval delta;
    if (timeout > now) {
        delta = timeout - now;
    }

    bsl::int64_t milliseconds =
        static_cast<bsl::int64_t>(delta.totalMilliseconds());

    if (milliseconds > bsl::numeric_limits<int>::max()) {
        milliseconds = bsl::numeric_limits<int>::max();
    }

    int rc = ::poll(&pfd, 1, static_cast<int>(milliseconds));
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if ((pfd.revents & POLLERR) != 0) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error DeviceUtil::enqueuePacket(
    ntsa::Handle                                device,
    ntsa::DeviceType::Value                     deviceType,
    const bsl::shared_ptr<ntsa::Packet>&        packet,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory)
{
    ntsa::Error error;

    if (!packet) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!packet->isEthernet()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const ntsa::EthernetPacket& ethernet = packet->ethernet();

    if (deviceType == ntsa::DeviceType::e_LOCAL ||
        deviceType == ntsa::DeviceType::e_LOOPBACK)
    {
        bdlbb::BlobBuffer buffer;
        packetFactory->createOutgoingBlobBuffer(&buffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        encoderOptions.setLoopback(true);
        encoderOptions.setIgnoreChecksum(true);

        ntsa::PacketEncoder encoder(&buffer);

        if (ethernet.payload().isUndefined()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
        else if (ethernet.payload().isIpv4()) {
            if (ethernet.header().protocol() != ntsa::EthernetProtocol::e_IPV4)
            {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            const ntsa::Ipv4Packet& ipv4 = ethernet.payload().ipv4();

            bsl::uint32_t packetType;
            if (deviceType == ntsa::DeviceType::e_LOCAL) {
                packetType = PF_INET;
            }
            else {
                packetType = BSLS_BYTEORDER_HOST_TO_BE(PF_INET);
            }

            if (buffer.size() < sizeof packetType) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            NTSCFG_MEMORY_COPY(buffer.data(), &packetType, sizeof packetType);

            error = encoder.advance(sizeof packetType);
            if (error) {
                return error;
            }

            error = ipv4.encode(&encoderContext, &encoder, encoderOptions);
            if (error) {
                NTSU_DEVICEUTIL_LOG_PACKET_ENCODER_ERROR(packet, error);
                return error;
            }
        }
        else if (ethernet.payload().isIpv6()) {
            if (ethernet.header().protocol() != ntsa::EthernetProtocol::e_IPV6)
            {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            const ntsa::Ipv6Packet& ipv6 = ethernet.payload().ipv6();

            bsl::uint32_t packetType;
            if (deviceType == ntsa::DeviceType::e_LOCAL) {
                packetType = PF_INET6;
            }
            else {
                packetType = BSLS_BYTEORDER_HOST_TO_BE(PF_INET6);
            }

            if (buffer.size() < sizeof packetType) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            NTSCFG_MEMORY_COPY(buffer.data(), &packetType, sizeof packetType);

            error = encoder.advance(sizeof packetType);
            if (error) {
                return error;
            }

            error = ipv6.encode(&encoderContext, &encoder, encoderOptions);
            if (error) {
                NTSU_DEVICEUTIL_LOG_PACKET_ENCODER_ERROR(packet, error);
                return error;
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = encoder.flush();
        if (error) {
            return error;
        }

        buffer.setSize(encoder.position());

        NTSU_DEVICEUTIL_LOG_PACKET_OUTGOING(packet, buffer);

        do {
            ssize_t bytesSent =
                ::write(device,
                        buffer.data(),
                        static_cast<bsl::size_t>(buffer.size()));
            if (bytesSent < 0) {
                error = ntsa::Error::last();
                if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                    continue;
                }
                if (error == ntsa::Error(ntsa::Error::e_INTERRUPTED)) {
                    continue;
                }
                NTSU_DEVICEUTIL_LOG_PACKET_WRITER_ERROR(packet, error);
                return error;
            }
            else if (bytesSent == 0) {
                NTSU_DEVICEUTIL_LOG_PACKET_WRITER_ERROR_EOF(packet);
                return ntsa::Error(ntsa::Error::e_EOF);
            }
            else if (bytesSent < static_cast<ssize_t>(buffer.size())) {
                NTSU_DEVICEUTIL_LOG_PACKET_WRITER_UNEXPECTED_BYTES_SENT(
                    packet,
                    buffer,
                    bytesSent);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
            else if (bytesSent > static_cast<ssize_t>(buffer.size())) {
                NTSU_DEVICEUTIL_LOG_PACKET_WRITER_UNEXPECTED_BYTES_SENT(
                    packet,
                    buffer,
                    bytesSent);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            BSLS_ASSERT(bytesSent == static_cast<ssize_t>(buffer.size()));
        } while (false);
    }
    else {
        bdlbb::BlobBuffer buffer;
        packetFactory->createOutgoingBlobBuffer(&buffer);

        ntsa::PacketEncoderContext encoderContext;
        ntsa::PacketEncoderOptions encoderOptions;

        ntsa::PacketEncoder encoder(&buffer);

        error = ethernet.encode(&encoderContext, &encoder, encoderOptions);
        if (error) {
            NTSU_DEVICEUTIL_LOG_PACKET_ENCODER_ERROR(packet, error);
            return error;
        }

        error = encoder.flush();
        if (error) {
            return error;
        }

        buffer.setSize(encoder.position());

        NTSU_DEVICEUTIL_LOG_PACKET_OUTGOING(packet, buffer);

        do {
            ssize_t bytesSent =
                ::write(device,
                        buffer.data(),
                        static_cast<bsl::size_t>(buffer.size()));
            if (bytesSent < 0) {
                error = ntsa::Error::last();
                if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                    continue;
                }
                if (error == ntsa::Error(ntsa::Error::e_INTERRUPTED)) {
                    continue;
                }
                NTSU_DEVICEUTIL_LOG_PACKET_WRITER_ERROR(packet, error);
                return error;
            }
            else if (bytesSent == 0) {
                NTSU_DEVICEUTIL_LOG_PACKET_WRITER_ERROR_EOF(packet);
                return ntsa::Error(ntsa::Error::e_EOF);
            }
            else if (bytesSent < static_cast<ssize_t>(buffer.size())) {
                NTSU_DEVICEUTIL_LOG_PACKET_WRITER_UNEXPECTED_BYTES_SENT(
                    packet,
                    buffer,
                    bytesSent);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
            else if (bytesSent > static_cast<ssize_t>(buffer.size())) {
                NTSU_DEVICEUTIL_LOG_PACKET_WRITER_UNEXPECTED_BYTES_SENT(
                    packet,
                    buffer,
                    bytesSent);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            BSLS_ASSERT(bytesSent == static_cast<ssize_t>(buffer.size()));
        } while (false);
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::dequeuePacket(
    ntsa::Handle                                device,
    ntsa::DeviceType::Value                     deviceType,
    const bsl::shared_ptr<ntsa::PacketQueue>&   packetQueue,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory)
{
    ntsa::Error error;

    bdlbb::BlobBuffer buffer;
    packetFactory->createIncomingBlobBuffer(&buffer);

    bsl::memset(buffer.data(), 0, static_cast<bsl::size_t>(buffer.size()));

    // MRM
#if 0
    BALL_LOG_DEBUG << "BPF device driver read starting" << BALL_LOG_END;
#endif

    ssize_t bytesRead =
        ::read(device, buffer.data(), static_cast<bsl::size_t>(buffer.size()));

    // MRM
#if 0
    BALL_LOG_DEBUG << "BPF device driver read complete: rc = " << bytesRead
                   << BALL_LOG_END;
#endif

    if (bytesRead < 0) {
        error = ntsa::Error::last();
        BSLS_LOG_ERROR("BPF device driver failed to read packet: %s",
                       error.text().c_str());
        return error;
    }
    else if (bytesRead == 0) {
        BSLS_LOG_ERROR("BPF device driver failed to read packet: EOF");
        return ntsa::Error(ntsa::Error::e_EOF);
    }
    else if (bytesRead > static_cast<ssize_t>(buffer.size())) {
        BSLS_LOG_ERROR("BPF device driver failed to read packet: too long");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    BSLS_ASSERT(bytesRead > 0);
    BSLS_ASSERT(bytesRead <= static_cast<ssize_t>(buffer.size()));

    buffer.setSize(static_cast<int>(bytesRead));

    char* metaFrame    = buffer.data();
    char* metaFrameEnd = metaFrame + buffer.size();

    while (metaFrame < metaFrameEnd) {
        struct bpf_hdr* bpf = reinterpret_cast<struct bpf_hdr*>(metaFrame);

        // MRM
#if 0
        BSLS_LOG_TRACE("BPF device read packet meta-data "
                       "[ caplen = %zu datalen = %zu hdrlen = %zu ]",
                       static_cast<bsl::size_t>(bpf->bh_caplen),
                       static_cast<bsl::size_t>(bpf->bh_datalen),
                       static_cast<bsl::size_t>(bpf->bh_hdrlen));
#endif

        if (bpf->bh_caplen != bpf->bh_datalen) {
            BSLS_LOG_ERROR("BPF device driver failed to read packet: "
                           "the captured length %zu "
                           "does not match the data length %zu",
                           static_cast<bsl::size_t>(bpf->bh_caplen),
                           static_cast<bsl::size_t>(bpf->bh_datalen));

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        char*       packetData = metaFrame + bpf->bh_hdrlen;
        bsl::size_t packetSize = static_cast<bsl::size_t>(bpf->bh_datalen);

        if (deviceType == ntsa::DeviceType::e_LOCAL ||
            deviceType == ntsa::DeviceType::e_LOOPBACK)
        {
            bsl::uint32_t packetType;
            NTSCFG_MEMORY_COPY(&packetType, packetData, sizeof packetType);

            if (deviceType == ntsa::DeviceType::e_LOOPBACK) {
                packetType = BSLS_BYTEORDER_BE_TO_HOST(packetType);
            }

            packetData += sizeof(bsl::uint32_t);
            packetSize -= sizeof(bsl::uint32_t);

            bdlbb::BlobBuffer packetBuffer(
                bsl::shared_ptr<char>(buffer.buffer(), packetData),
                static_cast<int>(packetSize));

            ntsa::PacketDecoderContext decoderContext;
            ntsa::PacketDecoderOptions decoderOptions;

            decoderContext.setSourceEthernetAddress(ntsa::EthernetAddress());
            decoderContext.setDestinationEthernetAddress(
                ntsa::EthernetAddress());

            decoderOptions.setLoopback(true);
            decoderOptions.setIgnoreChecksum(true);

            if (packetType == PF_UNSPEC) {
                bsl::shared_ptr<ntsa::Packet> packet;
                packetFactory->createIncomingPacket(&packet);

                packet->makeEthernet();

                error = packet->decode(&decoderContext,
                                       packetBuffer,
                                       decoderOptions);
                if (error) {
                    if (error == ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED)) {
                        NTSU_DEVICEUTIL_LOG_PACKET_INCOMING_DROP(packet);
                    }
                    else {
                        NTSU_DEVICEUTIL_LOG_PACKET_DECODER_ERROR(packetBuffer,
                                                                 packet,
                                                                 error);
                    }
                }
                else {
                    NTSU_DEVICEUTIL_LOG_PACKET_INCOMING(packet);

                    error = packetQueue->enqueue(NTSCFG_MOVE(packet));
                    if (error) {
                        return error;
                    }
                }
            }
            else if (packetType == PF_INET) {
                bsl::shared_ptr<ntsa::Packet> packet;
                packetFactory->createIncomingPacket(&packet);

                packet->makeEthernet();
                packet->ethernet().header().setProtocol(
                    ntsa::EthernetProtocol::e_IPV4);

                packet->ethernet().payload().makeIpv4();

                ntsa::PacketDecoder decoder(&packetBuffer);

                error =
                    packet->ethernet().payload().ipv4().decode(&decoderContext,
                                                               &decoder,
                                                               decoderOptions);
                if (error) {
                    if (error == ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED)) {
                        NTSU_DEVICEUTIL_LOG_PACKET_INCOMING_DROP(packet);
                    }
                    else {
                        NTSU_DEVICEUTIL_LOG_PACKET_DECODER_ERROR(packetBuffer,
                                                                 packet,
                                                                 error);
                        return error;
                    }
                }
                else {
                    NTSU_DEVICEUTIL_LOG_PACKET_INCOMING(packet);

                    error = packetQueue->enqueue(NTSCFG_MOVE(packet));
                    if (error) {
                        return error;
                    }
                }
            }
            else if (packetType == PF_INET6) {
                BSLS_LOG_WARN(
                    "BPF device driver dropping loopback IPv6 packet");
            }
            else {
                BSLS_LOG_ERROR("BPF device driver failed to decode packet: "
                               "unsupported loopback device packet type: %zu",
                               static_cast<bsl::size_t>(packetType));
            }
        }
        else {
            bdlbb::BlobBuffer packetBuffer(
                bsl::shared_ptr<char>(buffer.buffer(), packetData),
                static_cast<int>(packetSize));

            ntsa::PacketDecoderContext decoderContext;
            ntsa::PacketDecoderOptions decoderOptions;

            bsl::shared_ptr<ntsa::Packet> packet;
            packetFactory->createIncomingPacket(&packet);

            packet->makeEthernet();

            error =
                packet->decode(&decoderContext, packetBuffer, decoderOptions);
            if (error) {
                if (error == ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED)) {
                    NTSU_DEVICEUTIL_LOG_PACKET_INCOMING_DROP(packet);
                }
                else {
                    NTSU_DEVICEUTIL_LOG_PACKET_DECODER_ERROR(packetBuffer,
                                                             packet,
                                                             error);
                }
            }
            else {
                NTSU_DEVICEUTIL_LOG_PACKET_INCOMING(packet);

                error = packetQueue->enqueue(NTSCFG_MOVE(packet));
                if (error) {
                    return error;
                }
            }
        }

        metaFrame += BPF_WORDALIGN(bpf->bh_hdrlen + bpf->bh_caplen);
    }

    return ntsa::Error();
}

ntsa::Error DeviceUtil::shutdown(ntsa::Handle device)
{
    return DeviceUtil::setBlocking(device, false);
}

ntsa::Error DeviceUtil::close(ntsa::Handle device)
{
    ntsa::Error error;
    int         rc;

    if (device == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    NTSU_DEVICEUTIL_LOG_CLOSE_STARTING(device);

    rc = ::close(device);
    if (rc != 0) {
        const int lastError = errno;
        error               = ntsa::Error(lastError);
        NTSU_DEVICEUTIL_LOG_CLOSE_FAILED(device, error);
        return error;
    }

    NTSU_DEVICEUTIL_LOG_CLOSE_COMPLETE(device);

    return ntsa::Error();
}

bool DeviceUtil::isSupported()
{
    if (geteuid() == 0) {
        return true;
    }

    return false;
}

#elif defined(BSLS_PLATFORM_OS_LINUX)

ntsa::Error DeviceUtil::open(ntsa::Handle*             result,
                             ntsa::DeviceType::Value*  type,
                             bsl::size_t*              txBufferSize,
                             bsl::size_t*              rxBufferSize,
                             const ntsa::Adapter&      adapter,
                             const ntsa::DeviceConfig& configuration)
{
    *result       = ntsa::k_INVALID_HANDLE;
    *type         = ntsa::DeviceType::e_UNDEFINED;
    *txBufferSize = 0;
    *rxBufferSize = 0;

    NTSCFG_WARNING_UNUSED(adapter);
    NTSCFG_WARNING_UNUSED(configuration);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::applyFilter(ntsa::Handle              device,
                                    ntsa::DeviceType::Value   deviceType,
                                    const ntsa::Adapter&      adapter,
                                    const ntsa::PacketFilter& filter)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(deviceType);
    NTSCFG_WARNING_UNUSED(adapter);
    NTSCFG_WARNING_UNUSED(filter);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::setBlocking(ntsa::Handle device, bool value)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(value);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::getBlocking(ntsa::Handle device, bool* result)
{
    *result = false;

    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilReadable(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilReadable(ntsa::Handle              device,
                                          const bsls::TimeInterval& timeout)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(timeout);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilWritable(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilWritable(ntsa::Handle              device,
                                          const bsls::TimeInterval& timeout)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(timeout);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilError(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilError(ntsa::Handle              device,
                                       const bsls::TimeInterval& timeout)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(timeout);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::enqueuePacket(
    ntsa::Handle                                device,
    ntsa::DeviceType::Value                     deviceType,
    const bsl::shared_ptr<ntsa::Packet>&        packet,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(deviceType);
    NTSCFG_WARNING_UNUSED(packet);
    NTSCFG_WARNING_UNUSED(packetFactory);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::dequeuePacket(
    ntsa::Handle                                device,
    ntsa::DeviceType::Value                     deviceType,
    const bsl::shared_ptr<ntsa::PacketQueue>&   packetQueue,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(deviceType);
    NTSCFG_WARNING_UNUSED(packetQueue);
    NTSCFG_WARNING_UNUSED(packetFactory);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::shutdown(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::close(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bool DeviceUtil::isSupported()
{
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
}

#else

ntsa::Error DeviceUtil::open(ntsa::Handle*             result,
                             ntsa::DeviceType::Value*  type,
                             bsl::size_t*              txBufferSize,
                             bsl::size_t*              rxBufferSize,
                             const ntsa::Adapter&      adapter,
                             const ntsa::DeviceConfig& configuration)
{
    *result       = ntsa::k_INVALID_HANDLE;
    *type         = ntsa::DeviceType::e_UNDEFINED;
    *txBufferSize = 0;
    *rxBufferSize = 0;

    NTSCFG_WARNING_UNUSED(adapter);
    NTSCFG_WARNING_UNUSED(configuration);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::applyFilter(ntsa::Handle              device,
                                    ntsa::DeviceType::Value   deviceType,
                                    const ntsa::Adapter&      adapter,
                                    const ntsa::PacketFilter& filter)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(deviceType);
    NTSCFG_WARNING_UNUSED(adapter);
    NTSCFG_WARNING_UNUSED(filter);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::setBlocking(ntsa::Handle device, bool value)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(value);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::getBlocking(ntsa::Handle device, bool* result)
{
    *result = false;

    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilReadable(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilReadable(ntsa::Handle              device,
                                          const bsls::TimeInterval& timeout)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(timeout);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilWritable(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilWritable(ntsa::Handle              device,
                                          const bsls::TimeInterval& timeout)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(timeout);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilError(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::waitUntilError(ntsa::Handle              device,
                                       const bsls::TimeInterval& timeout)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(timeout);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::enqueuePacket(
    ntsa::Handle                                device,
    ntsa::DeviceType::Value                     deviceType,
    const bsl::shared_ptr<ntsa::Packet>&        packet,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(deviceType);
    NTSCFG_WARNING_UNUSED(packet);
    NTSCFG_WARNING_UNUSED(packetFactory);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::dequeuePacket(
    ntsa::Handle                                device,
    ntsa::DeviceType::Value                     deviceType,
    const bsl::shared_ptr<ntsa::PacketQueue>&   packetQueue,
    const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory)
{
    NTSCFG_WARNING_UNUSED(device);
    NTSCFG_WARNING_UNUSED(deviceType);
    NTSCFG_WARNING_UNUSED(packetQueue);
    NTSCFG_WARNING_UNUSED(packetFactory);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::shutdown(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DeviceUtil::close(ntsa::Handle device)
{
    NTSCFG_WARNING_UNUSED(device);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bool DeviceUtil::isSupported()
{
    return false;
}

#endif

DeviceGuard::DeviceGuard(ntsa::Handle device)
: d_device(device)
{
}

DeviceGuard::~DeviceGuard()
{
    if (d_device != ntsa::k_INVALID_HANDLE) {
        ntsu::DeviceUtil::close(d_device);
    }
}

ntsa::Handle DeviceGuard::release()
{
    ntsa::Handle result = d_device;
    d_device            = ntsa::k_INVALID_HANDLE;
    return result;
}

}  // close package namespace
}  // close enterprise namespace
