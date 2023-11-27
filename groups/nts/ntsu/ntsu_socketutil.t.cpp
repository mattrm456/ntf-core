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
#include <ntsu_adapterutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>
#include <ntsu_timestamputil.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_simpleblobbufferfactory.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdls_filesystemutil.h>
#include <bslma_testallocator.h>
#include <bslmt_threadgroup.h>
#include <bsls_platform.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_set.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <linux/version.h>

#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <sys/ioctl.h>
#endif

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

namespace test {

/// Provide a in-core storage of bytes.
template <unsigned N>
class Storage
{
    char d_buffer[N];

  public:
    char* data()
    {
        return d_buffer;
    }

    bsl::size_t size() const
    {
        return N;
    }
};

#if defined(BSLS_PLATFORM_OS_LINUX)

bsl::uint32_t timestampingSupport(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    struct ::ethtool_ts_info info;
    bsl::memset(&info, 0, sizeof info);

    info.cmd = ETHTOOL_GET_TS_INFO;

    struct ::ifreq ifr;
    bsl::memset(&ifr, 0, sizeof ifr);

    bsl::strcpy(ifr.ifr_ifrn.ifrn_name, "lo");
    ifr.ifr_ifru.ifru_data = reinterpret_cast<char*>(&info);

    rc = ::ioctl(socket, SIOCETHTOOL, &ifr);
    if (rc != 0) {
        error = ntsa::Error(errno);
        BSLS_LOG_DEBUG("I/O control SIOCETHTOOL failed: %s",
                       error.text().c_str());
        return 0;
    }

    return static_cast<bsl::uint32_t>(info.so_timestamping);
}

bool supportsRxTimestamps(ntsa::Handle socket)
{
    const bsl::uint32_t flags = timestampingSupport(socket);
    return (flags & ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_SOFTWARE) != 0;
}

bool supportsTxTimestamps(ntsa::Handle socket)
{
    const bsl::uint32_t flags = timestampingSupport(socket);
    return (flags & ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_SOFTWARE) != 0;
}

#else

bool supportsRxTimestamps(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);
    return false;
}

bool supportsTxTimestamps(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);
    return false;
}

#endif

void extractZeroCopyNotifications(bsl::list<ntsa::ZeroCopy>* zerocopy,
                                  ntsa::Handle               handle,
                                  bslma::Allocator*          allocator)
{
    ntsa::NotificationQueue notifications(allocator);
    notifications.setHandle(handle);

    ntsa::Error error =
        ntsu::SocketUtil::receiveNotifications(&notifications, handle);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << notifications << NTSCFG_TEST_LOG_END;

    // save zerocopy notifications for later validation
    for (bsl::vector<ntsa::Notification>::const_iterator it =
             notifications.notifications().cbegin();
         it != notifications.notifications().cend();
         ++it)
    {
        NTSCFG_TEST_TRUE(it->isZeroCopy());
        zerocopy->push_back(it->zeroCopy());
    }
}

void extractTimestampNotifications(bsl::list<ntsa::Timestamp>* ts,
                                   ntsa::Handle                handle,
                                   bslma::Allocator*           allocator)
{
    ntsa::NotificationQueue notifications(allocator);
    notifications.setHandle(handle);

    ntsa::Error error =
        ntsu::SocketUtil::receiveNotifications(&notifications, handle);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << notifications << NTSCFG_TEST_LOG_END;

    // save zerocopy notifications for later validation
    for (bsl::vector<ntsa::Notification>::const_iterator it =
             notifications.notifications().cbegin();
         it != notifications.notifications().cend();
         ++it)
    {
        NTSCFG_TEST_TRUE(it->isTimestamp());
        ts->push_back(it->timestamp());
    }
}

void extractNotifications(bsl::list<ntsa::Notification>* nt,
                          ntsa::Handle                   handle,
                          bslma::Allocator*              allocator)
{
    ntsa::NotificationQueue notifications(allocator);
    notifications.setHandle(handle);

    ntsa::Error error =
        ntsu::SocketUtil::receiveNotifications(&notifications, handle);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << notifications << NTSCFG_TEST_LOG_END;

    // save zerocopy notifications for later validation
    for (bsl::vector<ntsa::Notification>::const_iterator it =
             notifications.notifications().cbegin();
         it != notifications.notifications().cend();
         ++it)
    {
        nt->push_back(*it);
    }
}

/// This typedef defines a callback function invoked to test a particular
/// portion of the component using the specified connected 'server' and
/// 'client' having the specified stream socket 'transport', supplying
/// memory using the specified 'allocator'.
typedef bsl::function<void(ntsa::Transport::Value transport,
                           ntsa::Handle           server,
                           ntsa::Handle           client,
                           bslma::Allocator*      allocator)>
    StreamSocketTestCallback;

/// This typedef defines a callback function invoked to test a particular
/// portion of the component using using the specified 'server' bound to the
/// specified 'serverEndpoint' and 'client' bound to the specified
/// 'clientEndpoint', each 'client' and 'server' socket having the
/// specified 'transport', supplying
/// memory using the specified 'allocator'.
typedef bsl::function<void(ntsa::Transport::Value transport,
                           ntsa::Handle           server,
                           const ntsa::Endpoint&  serverEndpoint,
                           ntsa::Handle           client,
                           const ntsa::Endpoint&  clientEndpoint,
                           bslma::Allocator*      allocator)>
    DatagramSocketTestCallback;

void executeStreamSocketTest(const StreamSocketTestCallback& test,
                             bslma::Allocator* basicAllocator = 0)
{
    // Execute the specified stream socket 'test'.

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Error error;

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV4_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV4_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV6_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV6_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(ntsa::Transport::e_LOCAL_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_STREAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        // Create a blocking socket, bind it to any port on the loopback
        // address, then begin listening for connections.

        ntsa::Handle listener;
        error = ntsu::SocketUtil::create(&listener, transport);
        NTSCFG_TEST_ASSERT(!error);

        if (transport == ntsa::Transport::e_TCP_IPV4_STREAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                false,
                listener);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_TCP_IPV6_STREAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                false,
                listener);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_LOCAL_STREAM) {
            ntsa::LocalName localName;
            error = ntsa::LocalName::generateUnique(&localName);
            NTSCFG_TEST_ASSERT(!error);

            error = ntsu::SocketUtil::bind(ntsa::Endpoint(localName),
                                           false,
                                           listener);
            NTSCFG_TEST_ASSERT(!error);
        }
        else {
            NTSCFG_TEST_TRUE(false);
        }

        error = ntsu::SocketUtil::listen(1, listener);
        NTSCFG_TEST_ASSERT(!error);

        // Create a blocking socket for the client, then connect that socket to
        // the listener socket's local endpoint.

        ntsa::Handle client;
        error = ntsu::SocketUtil::create(&client, transport);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint listenerEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::connect(listenerEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        // Create a blocking socket for the server by accepting the connection
        // made to the listener socket.

        ntsa::Handle server;
        error = ntsu::SocketUtil::accept(&server, listener);
        NTSCFG_TEST_ASSERT(!error);

        // Execute the test.

        test(transport, server, client, allocator);

        // Shutdown writing by the client socket.

        error = ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, client);
        NTSCFG_TEST_ASSERT(!error);

        // Dequeue incoming data received by the server socket, and observe
        // that zero bytes are successfully dequeued, indicating the client
        // socket has shut down writing from its side of the connection.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 0);
        }

        // Shutdown writing by the server socket.

        error = ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, server);
        NTSCFG_TEST_ASSERT(!error);

        // Dequeue incoming data received by the client socket, and observe
        // that zero bytes are successfully dequeued, indicating the server
        // socket has shut down writing from its side of the connection.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 0);
        }

        // Close each socket.

        error = ntsu::SocketUtil::close(listener);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(client);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(server);
        NTSCFG_TEST_ASSERT(!error);
    }
}

void executeDatagramSocketTest(const DatagramSocketTestCallback& test,
                               bslma::Allocator* basicAllocator = 0)
{
    // Execute the specified datagram socket 'test'.

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Error error;

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV4_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV6_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_LOCAL_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_DATAGRAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        // Create a blocking socket for the server and bind it to any port on
        // the loopback address.

        ntsa::Handle server;
        error = ntsu::SocketUtil::create(&server, transport);
        NTSCFG_TEST_ASSERT(!error);

        if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                false,
                server);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                false,
                server);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
            ntsa::LocalName localName;
            error = ntsa::LocalName::generateUnique(&localName);
            NTSCFG_TEST_ASSERT(!error);

            error = ntsu::SocketUtil::bind(ntsa::Endpoint(localName),
                                           false,
                                           server);
            NTSCFG_TEST_ASSERT(!error);
        }
        else {
            NTSCFG_TEST_TRUE(false);
        }

        ntsa::Endpoint serverEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint, server);
        NTSCFG_TEST_ASSERT(!error);

        // Create a blocking socket for the client and bind it to any port on
        // the loopback address.

        ntsa::Handle client;
        error = ntsu::SocketUtil::create(&client, transport);
        NTSCFG_TEST_ASSERT(!error);

        if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                false,
                client);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                false,
                client);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
            ntsa::LocalName localName;
            error = ntsa::LocalName::generateUnique(&localName);
            NTSCFG_TEST_ASSERT(!error);

            error = ntsu::SocketUtil::bind(ntsa::Endpoint(localName),
                                           false,
                                           client);
            NTSCFG_TEST_ASSERT(!error);
        }
        else {
            NTSCFG_TEST_TRUE(false);
        }

        ntsa::Endpoint clientEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&clientEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        // Execute the test.

        test(transport,
             server,
             serverEndpoint,
             client,
             clientEndpoint,
             allocator);

        // Close each socket.

        error = ntsu::SocketUtil::close(client);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(server);
        NTSCFG_TEST_ASSERT(!error);
    }
}

void testStreamSocketTransmissionSingleBuffer(ntsa::Transport::Value transport,
                                              ntsa::Handle           server,
                                              ntsa::Handle           client,
                                              bslma::Allocator*      allocator)
{
    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << ": send/recv"
                          << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "Hello, world!";

    char clientBuffer[sizeof DATA - 1];
    char serverBuffer[sizeof DATA - 1];

    bsl::memcpy(clientBuffer, DATA, sizeof DATA - 1);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Data data(ntsa::ConstBuffer(clientBuffer, sizeof clientBuffer));

        error = ntsu::SocketUtil::send(&context, data, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == sizeof clientBuffer);
        NTSCFG_TEST_ASSERT(context.bytesSent() == sizeof clientBuffer);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(
            ntsa::MutableBuffer(serverBuffer, sizeof serverBuffer));

        error = ntsu::SocketUtil::receive(&context, &data, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == sizeof serverBuffer);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == sizeof serverBuffer);

        NTSCFG_TEST_ASSERT(
            bsl::memcmp(serverBuffer, clientBuffer, sizeof clientBuffer) == 0);
    }
}

void testStreamSocketTransmissionSingleBufferWithControlMsg(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    ntsa::Handle           client,
    bslma::Allocator*      allocator)
{
    if (transport != ntsa::Transport::e_LOCAL_STREAM) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << ": send/recv "
                          << "with ancillary data" << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "Hello, world!";

    char clientBuffer[sizeof DATA - 1];
    char serverBuffer[sizeof DATA - 1];

    bsl::memcpy(clientBuffer, DATA, sizeof DATA - 1);

    ntsa::Handle domesticSocket;
    error = ntsu::SocketUtil::create(&domesticSocket, transport);
    NTSCFG_TEST_ASSERT(!error);

    error = ntsu::SocketUtil::bind(
        ntsa::Endpoint(ntsa::LocalName::generateUnique()),
        false,
        domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    ntsa::Endpoint domesticSourceEndpoint;
    error = ntsu::SocketUtil::sourceEndpoint(&domesticSourceEndpoint,
                                             domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setForeignHandle(domesticSocket);

        ntsa::Data data(ntsa::ConstBuffer(clientBuffer, sizeof clientBuffer));

        error = ntsu::SocketUtil::send(&context, data, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == sizeof clientBuffer);
        NTSCFG_TEST_ASSERT(context.bytesSent() == sizeof clientBuffer);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        options.showForeignHandles();

        ntsa::Data data(
            ntsa::MutableBuffer(serverBuffer, sizeof serverBuffer));

        error = ntsu::SocketUtil::receive(&context, &data, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == sizeof serverBuffer);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == sizeof serverBuffer);

        NTSCFG_TEST_ASSERT(
            bsl::memcmp(serverBuffer, clientBuffer, sizeof clientBuffer) == 0);

        NTSCFG_TEST_TRUE(!context.foreignHandle().isNull());

        ntsa::Endpoint foreignSourceEndpoint;
        error =
            ntsu::SocketUtil::sourceEndpoint(&foreignSourceEndpoint,
                                             context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_EQ(foreignSourceEndpoint, domesticSourceEndpoint);

        error = ntsu::SocketUtil::close(domesticSocket);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);
    }
}

void testStreamSocketTransmissionMultipleBuffers(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    ntsa::Handle           client,
    bslma::Allocator*      allocator)
{
    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << ": writev/readv"
                          << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "123456789";

    char clientBuffer0[sizeof DATA - 1];
    char clientBuffer1[sizeof DATA - 1];
    char clientBuffer2[sizeof DATA - 1];

    char serverBuffer0[sizeof DATA - 1];
    char serverBuffer1[sizeof DATA - 1];
    char serverBuffer2[sizeof DATA - 1];

    bsl::memcpy(clientBuffer0, DATA + 0, 3);
    bsl::memcpy(clientBuffer1, DATA + 3, 3);
    bsl::memcpy(clientBuffer2, DATA + 6, 3);

    ntsa::ConstBuffer clientBufferArray[3];
    clientBufferArray[0].setBuffer(clientBuffer0, 3);
    clientBufferArray[1].setBuffer(clientBuffer1, 3);
    clientBufferArray[2].setBuffer(clientBuffer2, 3);

    ntsa::MutableBuffer serverBufferArray[3];
    serverBufferArray[0].setBuffer(serverBuffer0, 3);
    serverBufferArray[1].setBuffer(serverBuffer1, 3);
    serverBufferArray[2].setBuffer(serverBuffer2, 3);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Data data(ntsa::ConstBufferPtrArray(clientBufferArray, 3));

        error = ntsu::SocketUtil::send(&context, data, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(ntsa::MutableBufferPtrArray(serverBufferArray, 3));

        error = ntsu::SocketUtil::receive(&context, &data, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

        NTSCFG_TEST_ASSERT(bsl::memcmp(serverBufferArray[0].data(),
                                       clientBufferArray[0].data(),
                                       3) == 0);
        NTSCFG_TEST_ASSERT(bsl::memcmp(serverBufferArray[1].data(),
                                       clientBufferArray[1].data(),
                                       3) == 0);
        NTSCFG_TEST_ASSERT(bsl::memcmp(serverBufferArray[2].data(),
                                       clientBufferArray[2].data(),
                                       3) == 0);
    }
}

void testStreamSocketTransmissionBlob(ntsa::Transport::Value transport,
                                      ntsa::Handle           server,
                                      ntsa::Handle           client,
                                      bslma::Allocator*      allocator)
{
    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << ": writev/readv (blob)"
                          << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "123456789";

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(3, allocator);

    bdlbb::Blob clientBlob(&blobBufferFactory, allocator);
    bdlbb::BlobUtil::append(&clientBlob, DATA, sizeof DATA - 1);

    bdlbb::Blob serverBlob(&blobBufferFactory, allocator);
    serverBlob.setLength(sizeof DATA - 1);
    serverBlob.setLength(0);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = ntsu::SocketUtil::send(&context, clientBlob, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error =
            ntsu::SocketUtil::receive(&context, &serverBlob, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

        NTSCFG_TEST_ASSERT(serverBlob.length() == 9);
        NTSCFG_TEST_ASSERT(bdlbb::BlobUtil::compare(serverBlob, clientBlob) ==
                           0);
    }
}

void testStreamSocketTransmissionBlobWithControlMsg(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    ntsa::Handle           client,
    bslma::Allocator*      allocator)
{
    if (transport != ntsa::Transport::e_LOCAL_STREAM) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport
                          << ": writev/readv (blob) "
                          << "with ancillary data" << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "123456789";

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(3, allocator);

    bdlbb::Blob clientBlob(&blobBufferFactory, allocator);
    bdlbb::BlobUtil::append(&clientBlob, DATA, sizeof DATA - 1);

    bdlbb::Blob serverBlob(&blobBufferFactory, allocator);
    serverBlob.setLength(sizeof DATA - 1);
    serverBlob.setLength(0);

    ntsa::Handle domesticSocket;
    error = ntsu::SocketUtil::create(&domesticSocket, transport);
    NTSCFG_TEST_ASSERT(!error);

    error = ntsu::SocketUtil::bind(
        ntsa::Endpoint(ntsa::LocalName::generateUnique()),
        false,
        domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    ntsa::Endpoint domesticSourceEndpoint;
    error = ntsu::SocketUtil::sourceEndpoint(&domesticSourceEndpoint,
                                             domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setForeignHandle(domesticSocket);

        error = ntsu::SocketUtil::send(&context, clientBlob, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        options.showForeignHandles();

        error =
            ntsu::SocketUtil::receive(&context, &serverBlob, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

        NTSCFG_TEST_ASSERT(serverBlob.length() == 9);
        NTSCFG_TEST_ASSERT(bdlbb::BlobUtil::compare(serverBlob, clientBlob) ==
                           0);

        NTSCFG_TEST_TRUE(!context.foreignHandle().isNull());

        ntsa::Endpoint foreignSourceEndpoint;
        error =
            ntsu::SocketUtil::sourceEndpoint(&foreignSourceEndpoint,
                                             context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_EQ(foreignSourceEndpoint, domesticSourceEndpoint);

        error = ntsu::SocketUtil::close(domesticSocket);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);
    }
}

void testStreamSocketTransmissionWithControlMsgDropped(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    ntsa::Handle           client,
    bslma::Allocator*      allocator)
{
    if (transport != ntsa::Transport::e_LOCAL_STREAM) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport
                          << ": writev/readv (blob) "
                          << "with ancillary data" << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "123456789";

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(3, allocator);

    ntsa::Handle domesticSocket;
    error = ntsu::SocketUtil::create(&domesticSocket, transport);
    NTSCFG_TEST_ASSERT(!error);

    error = ntsu::SocketUtil::bind(
        ntsa::Endpoint(ntsa::LocalName::generateUnique()),
        false,
        domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    ntsa::Endpoint domesticSourceEndpoint;
    error = ntsu::SocketUtil::sourceEndpoint(&domesticSourceEndpoint,
                                             domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    for (bsl::size_t iteration = 0; iteration < 2; ++iteration) {
        NTSCFG_TEST_LOG_DEBUG << "Testing iteration " << iteration
                              << NTSCFG_TEST_LOG_END;

        bdlbb::Blob clientBlob(&blobBufferFactory, allocator);
        bdlbb::BlobUtil::append(&clientBlob, DATA, sizeof DATA - 1);

        bdlbb::Blob serverBlob(&blobBufferFactory, allocator);
        serverBlob.setLength(sizeof DATA - 1);
        serverBlob.setLength(0);

        // Enqueue outgoing data to transmit by the client socket.

        {
            ntsa::SendContext context;
            ntsa::SendOptions options;

            if (iteration == 0) {
                options.setForeignHandle(domesticSocket);
            }

            error =
                ntsu::SocketUtil::send(&context, clientBlob, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
        }

        // Dequeue incoming data received by the server socket.

        {
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            if (iteration == 1) {
                options.showForeignHandles();
            }

            error = ntsu::SocketUtil::receive(&context,
                                              &serverBlob,
                                              options,
                                              server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

            NTSCFG_TEST_ASSERT(serverBlob.length() == 9);
            NTSCFG_TEST_ASSERT(
                bdlbb::BlobUtil::compare(serverBlob, clientBlob) == 0);

            if (iteration == 0) {
                NTSCFG_TEST_TRUE(context.foreignHandle().isNull());
            }
            else if (iteration == 1) {
                NTSCFG_TEST_TRUE(context.foreignHandle().isNull());

                // Operating system implementation note: a file descriptor
                // passed using socket control messages that is not "picked up"
                // immediately when it is available is not subsequently
                // retrievable with a subsequent call to 'recvmsg'.

                error = ntsu::SocketUtil::close(domesticSocket);
                NTSCFG_TEST_ASSERT(!error);
            }
        }
    }
}

void testStreamSocketTransmissionFile(ntsa::Transport::Value transport,
                                      ntsa::Handle           server,
                                      ntsa::Handle           client,
                                      bslma::Allocator*      allocator)
{
    // This test succeeds when run locally but fails when run on build machines
    // during continuous integration, probably because the temporary files
    // cannot be created and/or written. Disable this test while this issue is
    // investigated.
    return;

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS) ||   \
    defined(BSLS_PLATFORM_OS_WINDOWS)

#if defined(BSLS_PLATFORM_OS_SOLARIS)

    // Solaris 'sendfile' returns EAFNOSUPPORT for Unix domain sockets.
    if (transport == ntsa::Transport::e_LOCAL_STREAM) {
        return;
    }
#endif

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << ": sendfile"
                          << NTSCFG_TEST_LOG_END;

    ntsa::Error error;
    int         rc;

    char DATA[] = "123456789";

    char serverBuffer0[sizeof DATA - 1];
    char serverBuffer1[sizeof DATA - 1];
    char serverBuffer2[sizeof DATA - 1];

    bsl::string filePathPrefix;
    {
        bsl::string filePathDirectory;

#if defined(BSLS_PLATFORM_OS_UNIX)

        const char* variable = bsl::getenv("TMPDIR");
        if (variable) {
            filePathDirectory = variable;
        }
        else {
            filePathDirectory = "/tmp";
        }

        filePathPrefix = filePathDirectory + "/" + "ntsu_socketutil.t.";

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

        char filePathDirectoryBuffer[MAX_PATH + 1];
        if (0 == GetTempPath(sizeof filePathDirectoryBuffer,
                             filePathDirectoryBuffer))
        {
            filePathDirectory = "C:\\Windows\\Temp";
        }
        else {
            filePathDirectory = filePathDirectoryBuffer;
        }

        filePathPrefix = filePathDirectory + "\\" + "ntsu_socketutil.t.";

#else
#error Not implemented
#endif
    }

    NTSCFG_TEST_LOG_DEBUG << "Creating temporary file prefix '"
                          << filePathPrefix << "'" << NTSCFG_TEST_LOG_END;

    bsl::string                          filePath;
    bdls::FilesystemUtil::FileDescriptor fileDescriptor =
        bdls::FilesystemUtil::createTemporaryFile(&filePath,
                                                  filePathPrefix.c_str());
    NTSCFG_TEST_ASSERT(fileDescriptor != bdls::FilesystemUtil::k_INVALID_FD);

    NTSCFG_TEST_LOG_DEBUG << "Created temporary file '" << filePath << "'"
                          << NTSCFG_TEST_LOG_END;

    rc = bdls::FilesystemUtil::write(fileDescriptor, DATA, sizeof DATA - 1);
    NTSCFG_TEST_ASSERT(rc == sizeof DATA - 1);

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(3, allocator);

    bdlbb::Blob clientBlob(&blobBufferFactory, allocator);
    bdlbb::BlobUtil::append(&clientBlob, DATA, sizeof DATA - 1);

    bdlbb::Blob serverBlob(&blobBufferFactory, allocator);
    serverBlob.setLength(sizeof DATA - 1);
    serverBlob.setLength(0);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Data data(ntsa::File(fileDescriptor, 0, 9));

        error = ntsu::SocketUtil::send(&context, data, options, client);
        if (error) {
            BSLS_LOG_ERROR("Transport %s error: %s",
                           ntsa::Transport::toString(transport),
                           error.text().c_str());
        }
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error =
            ntsu::SocketUtil::receive(&context, &serverBlob, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

        NTSCFG_TEST_ASSERT(serverBlob.length() == 9);
        NTSCFG_TEST_ASSERT(bdlbb::BlobUtil::compare(serverBlob, clientBlob) ==
                           0);
    }

    rc = bdls::FilesystemUtil::remove(filePath);
    NTSCFG_TEST_ASSERT(rc == 0);

    rc = bdls::FilesystemUtil::close(fileDescriptor);
    NTSCFG_TEST_ASSERT(rc == 0);

#else

    ntsa::SendContext context;
    ntsa::SendOptions options;

    ntsa::Data data(ntsa::File(bdls::FilesystemUtil::k_INVALID_FD, 0, 0));

    ntsa::Error error =
        ntsu::SocketUtil::send(&context, data, options, client);
    NTSCFG_TEST_ASSERT(error.code() == ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

void testDatagramSocketTransmissionSingleBuffer(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    const ntsa::Endpoint&  serverEndpoint,
    ntsa::Handle           client,
    const ntsa::Endpoint&  clientEndpoint,
    bslma::Allocator*      allocator)
{
    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << ": sendto/recvfrom"
                          << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "Hello, world!";

    char clientBuffer[sizeof DATA - 1];
    char serverBuffer[sizeof DATA - 1];

    bsl::memcpy(clientBuffer, DATA, sizeof DATA - 1);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverEndpoint);

        ntsa::Data data(ntsa::ConstBuffer(clientBuffer, sizeof clientBuffer));

        error = ntsu::SocketUtil::send(&context, data, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == sizeof clientBuffer);
        NTSCFG_TEST_ASSERT(context.bytesSent() == sizeof clientBuffer);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(
            ntsa::MutableBuffer(serverBuffer, sizeof serverBuffer));

        error = ntsu::SocketUtil::receive(&context, &data, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == sizeof serverBuffer);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == sizeof serverBuffer);

        NTSCFG_TEST_ASSERT(!context.endpoint().isNull());
        NTSCFG_TEST_ASSERT(context.endpoint().value() == clientEndpoint);

        NTSCFG_TEST_ASSERT(
            bsl::memcmp(serverBuffer, clientBuffer, sizeof clientBuffer) == 0);
    }
}

void testDatagramSocketTransmissionSingleBufferWithControlMsg(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    const ntsa::Endpoint&  serverEndpoint,
    ntsa::Handle           client,
    const ntsa::Endpoint&  clientEndpoint,
    bslma::Allocator*      allocator)
{
    if (transport != ntsa::Transport::e_LOCAL_DATAGRAM) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << ": sendto/recvfrom "
                          << "with ancillary data" << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "Hello, world!";

    char clientBuffer[sizeof DATA - 1];
    char serverBuffer[sizeof DATA - 1];

    bsl::memcpy(clientBuffer, DATA, sizeof DATA - 1);

    ntsa::Handle domesticSocket;
    error = ntsu::SocketUtil::create(&domesticSocket, transport);
    NTSCFG_TEST_ASSERT(!error);

    error = ntsu::SocketUtil::bind(
        ntsa::Endpoint(ntsa::LocalName::generateUnique()),
        false,
        domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    ntsa::Endpoint domesticSourceEndpoint;
    error = ntsu::SocketUtil::sourceEndpoint(&domesticSourceEndpoint,
                                             domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverEndpoint);
        options.setForeignHandle(domesticSocket);

        ntsa::Data data(ntsa::ConstBuffer(clientBuffer, sizeof clientBuffer));

        error = ntsu::SocketUtil::send(&context, data, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == sizeof clientBuffer);
        NTSCFG_TEST_ASSERT(context.bytesSent() == sizeof clientBuffer);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        options.showForeignHandles();

        ntsa::Data data(
            ntsa::MutableBuffer(serverBuffer, sizeof serverBuffer));

        error = ntsu::SocketUtil::receive(&context, &data, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == sizeof serverBuffer);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == sizeof serverBuffer);

        NTSCFG_TEST_ASSERT(!context.endpoint().isNull());
        NTSCFG_TEST_ASSERT(context.endpoint().value() == clientEndpoint);

        NTSCFG_TEST_ASSERT(
            bsl::memcmp(serverBuffer, clientBuffer, sizeof clientBuffer) == 0);

        NTSCFG_TEST_TRUE(!context.foreignHandle().isNull());

        ntsa::Endpoint foreignSourceEndpoint;
        error =
            ntsu::SocketUtil::sourceEndpoint(&foreignSourceEndpoint,
                                             context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_EQ(foreignSourceEndpoint, domesticSourceEndpoint);

        error = ntsu::SocketUtil::close(domesticSocket);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);
    }
}

void testDatagramSocketTransmissionMultipleBuffers(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    const ntsa::Endpoint&  serverEndpoint,
    ntsa::Handle           client,
    const ntsa::Endpoint&  clientEndpoint,
    bslma::Allocator*      allocator)
{
    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport
                          << ": sendmsg/recvmsg (array)"
                          << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "123456789";

    char clientBuffer0[sizeof DATA - 1];
    char clientBuffer1[sizeof DATA - 1];
    char clientBuffer2[sizeof DATA - 1];

    char serverBuffer0[sizeof DATA - 1];
    char serverBuffer1[sizeof DATA - 1];
    char serverBuffer2[sizeof DATA - 1];

    bsl::memcpy(clientBuffer0, DATA + 0, 3);
    bsl::memcpy(clientBuffer1, DATA + 3, 3);
    bsl::memcpy(clientBuffer2, DATA + 6, 3);

    ntsa::ConstBuffer clientBufferArray[3];
    clientBufferArray[0].setBuffer(clientBuffer0, 3);
    clientBufferArray[1].setBuffer(clientBuffer1, 3);
    clientBufferArray[2].setBuffer(clientBuffer2, 3);

    ntsa::MutableBuffer serverBufferArray[3];
    serverBufferArray[0].setBuffer(serverBuffer0, 3);
    serverBufferArray[1].setBuffer(serverBuffer1, 3);
    serverBufferArray[2].setBuffer(serverBuffer2, 3);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverEndpoint);

        ntsa::Data data(ntsa::ConstBufferPtrArray(clientBufferArray, 3));

        error = ntsu::SocketUtil::send(&context, data, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(ntsa::MutableBufferPtrArray(serverBufferArray, 3));

        error = ntsu::SocketUtil::receive(&context, &data, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

        NTSCFG_TEST_ASSERT(!context.endpoint().isNull());
        NTSCFG_TEST_ASSERT(context.endpoint().value() == clientEndpoint);

        NTSCFG_TEST_ASSERT(bsl::memcmp(serverBufferArray[0].data(),
                                       clientBufferArray[0].data(),
                                       3) == 0);
        NTSCFG_TEST_ASSERT(bsl::memcmp(serverBufferArray[1].data(),
                                       clientBufferArray[1].data(),
                                       3) == 0);
        NTSCFG_TEST_ASSERT(bsl::memcmp(serverBufferArray[2].data(),
                                       clientBufferArray[2].data(),
                                       3) == 0);
    }
}

void testDatagramSocketTransmissionBlob(ntsa::Transport::Value transport,
                                        ntsa::Handle           server,
                                        const ntsa::Endpoint&  serverEndpoint,
                                        ntsa::Handle           client,
                                        const ntsa::Endpoint&  clientEndpoint,
                                        bslma::Allocator*      allocator)
{
    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport
                          << ": sendmsg/recvmsg (blob)" << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "123456789";

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(3, allocator);

    bdlbb::Blob clientBlob(&blobBufferFactory, allocator);
    bdlbb::BlobUtil::append(&clientBlob, DATA, sizeof DATA - 1);

    bdlbb::Blob serverBlob(&blobBufferFactory, allocator);
    serverBlob.setLength(sizeof DATA - 1);
    serverBlob.setLength(0);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverEndpoint);

        error = ntsu::SocketUtil::send(&context, clientBlob, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error =
            ntsu::SocketUtil::receive(&context, &serverBlob, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

        NTSCFG_TEST_ASSERT(!context.endpoint().isNull());
        NTSCFG_TEST_ASSERT(context.endpoint().value() == clientEndpoint);

        NTSCFG_TEST_ASSERT(serverBlob.length() == 9);
        NTSCFG_TEST_ASSERT(bdlbb::BlobUtil::compare(serverBlob, clientBlob) ==
                           0);
    }
}

void testDatagramSocketTransmissionBlobWithControlMsg(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    const ntsa::Endpoint&  serverEndpoint,
    ntsa::Handle           client,
    const ntsa::Endpoint&  clientEndpoint,
    bslma::Allocator*      allocator)
{
    if (transport != ntsa::Transport::e_LOCAL_DATAGRAM) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport
                          << ": sendmsg/recvmsg (blob) "
                          << "with ancillary data" << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "123456789";

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(3, allocator);

    bdlbb::Blob clientBlob(&blobBufferFactory, allocator);
    bdlbb::BlobUtil::append(&clientBlob, DATA, sizeof DATA - 1);

    bdlbb::Blob serverBlob(&blobBufferFactory, allocator);
    serverBlob.setLength(sizeof DATA - 1);
    serverBlob.setLength(0);

    ntsa::Handle domesticSocket;
    error = ntsu::SocketUtil::create(&domesticSocket, transport);
    NTSCFG_TEST_ASSERT(!error);

    error = ntsu::SocketUtil::bind(
        ntsa::Endpoint(ntsa::LocalName::generateUnique()),
        false,
        domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    ntsa::Endpoint domesticSourceEndpoint;
    error = ntsu::SocketUtil::sourceEndpoint(&domesticSourceEndpoint,
                                             domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverEndpoint);
        options.setForeignHandle(domesticSocket);

        error = ntsu::SocketUtil::send(&context, clientBlob, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        options.showForeignHandles();

        error =
            ntsu::SocketUtil::receive(&context, &serverBlob, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

        NTSCFG_TEST_ASSERT(!context.endpoint().isNull());
        NTSCFG_TEST_ASSERT(context.endpoint().value() == clientEndpoint);

        NTSCFG_TEST_ASSERT(serverBlob.length() == 9);
        NTSCFG_TEST_ASSERT(bdlbb::BlobUtil::compare(serverBlob, clientBlob) ==
                           0);

        NTSCFG_TEST_TRUE(!context.foreignHandle().isNull());

        ntsa::Endpoint foreignSourceEndpoint;
        error =
            ntsu::SocketUtil::sourceEndpoint(&foreignSourceEndpoint,
                                             context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_EQ(foreignSourceEndpoint, domesticSourceEndpoint);

        error = ntsu::SocketUtil::close(domesticSocket);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);
    }
}

void testDatagramSocketTransmissionWithControlMsgDropped(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    const ntsa::Endpoint&  serverEndpoint,
    ntsa::Handle           client,
    const ntsa::Endpoint&  clientEndpoint,
    bslma::Allocator*      allocator)
{
    if (transport != ntsa::Transport::e_LOCAL_DATAGRAM) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport
                          << ": sendmsg/recvmsg (blob) "
                          << "with ancillary data" << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "123456789";

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(3, allocator);

    ntsa::Handle domesticSocket;
    error = ntsu::SocketUtil::create(&domesticSocket, transport);
    NTSCFG_TEST_ASSERT(!error);

    error = ntsu::SocketUtil::bind(
        ntsa::Endpoint(ntsa::LocalName::generateUnique()),
        false,
        domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    ntsa::Endpoint domesticSourceEndpoint;
    error = ntsu::SocketUtil::sourceEndpoint(&domesticSourceEndpoint,
                                             domesticSocket);
    NTSCFG_TEST_ASSERT(!error);

    for (bsl::size_t iteration = 0; iteration < 2; ++iteration) {
        NTSCFG_TEST_LOG_DEBUG << "Testing iteration " << iteration
                              << NTSCFG_TEST_LOG_END;

        bdlbb::Blob clientBlob(&blobBufferFactory, allocator);
        bdlbb::BlobUtil::append(&clientBlob, DATA, sizeof DATA - 1);

        bdlbb::Blob serverBlob(&blobBufferFactory, allocator);
        serverBlob.setLength(sizeof DATA - 1);
        serverBlob.setLength(0);

        // Enqueue outgoing data to transmit by the client socket.

        {
            ntsa::SendContext context;
            ntsa::SendOptions options;

            options.setEndpoint(serverEndpoint);

            if (iteration == 0) {
                options.setForeignHandle(domesticSocket);
            }

            error =
                ntsu::SocketUtil::send(&context, clientBlob, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
        }

        // Dequeue incoming data received by the server socket.

        {
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            if (iteration == 1) {
                options.showForeignHandles();
            }

            error = ntsu::SocketUtil::receive(&context,
                                              &serverBlob,
                                              options,
                                              server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

            NTSCFG_TEST_ASSERT(!context.endpoint().isNull());
            NTSCFG_TEST_ASSERT(context.endpoint().value() == clientEndpoint);

            NTSCFG_TEST_ASSERT(serverBlob.length() == 9);
            NTSCFG_TEST_ASSERT(
                bdlbb::BlobUtil::compare(serverBlob, clientBlob) == 0);

            if (iteration == 0) {
                NTSCFG_TEST_TRUE(context.foreignHandle().isNull());
            }
            else if (iteration == 1) {
                NTSCFG_TEST_TRUE(context.foreignHandle().isNull());

                // Operating system implementation note: a file descriptor
                // passed using socket control messages that is not "picked up"
                // immediately when it is available is not subsequently
                // retrievable with a subsequent call to 'recvmsg'.

                error = ntsu::SocketUtil::close(domesticSocket);
                NTSCFG_TEST_ASSERT(!error);
            }
        }
    }
}

void testDatagramSocketTransmissionSingleMessage(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    const ntsa::Endpoint&  serverEndpoint,
    ntsa::Handle           client,
    const ntsa::Endpoint&  clientEndpoint,
    bslma::Allocator*      allocator)
{
// TODO: Uncomment after restoring a ntsa::Message-based API.
#if 0
    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport
                          << ": sendmsg/recvmsg (message)"
                          << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    char DATA[] = "123456789";

    char clientBuffer0[sizeof DATA - 1];
    char clientBuffer1[sizeof DATA - 1];
    char clientBuffer2[sizeof DATA - 1];

    char serverBuffer0[sizeof DATA - 1];
    char serverBuffer1[sizeof DATA - 1];
    char serverBuffer2[sizeof DATA - 1];

    bsl::memcpy(clientBuffer0, DATA + 0, 3);
    bsl::memcpy(clientBuffer1, DATA + 3, 3);
    bsl::memcpy(clientBuffer2, DATA + 6, 3);

    ntsa::ConstMessage clientMessage;
    clientMessage.setEndpoint(serverEndpoint);
    clientMessage.appendBuffer(clientBuffer0, 3);
    clientMessage.appendBuffer(clientBuffer1, 3);
    clientMessage.appendBuffer(clientBuffer2, 3);

    NTSCFG_TEST_ASSERT(clientMessage.size() == 9);
    NTSCFG_TEST_ASSERT(clientMessage.capacity() == 9);

    ntsa::MutableMessage serverMessage;
    serverMessage.appendBuffer(serverBuffer0, 3);
    serverMessage.appendBuffer(serverBuffer1, 3);
    serverMessage.appendBuffer(serverBuffer2, 3);

    NTSCFG_TEST_ASSERT(serverMessage.size() == 0);
    NTSCFG_TEST_ASSERT(serverMessage.capacity() == 9);

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = ntsu::SocketUtil::send(
            &context, clientMessage, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesSent() == 9);
    }

    // Dequeue incoming data received by the server socket.

    {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = ntsu::SocketUtil::receive(
            &context, &serverMessage, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == 9);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == 9);

        NTSCFG_TEST_ASSERT(serverMessage.endpoint() == clientEndpoint);
        NTSCFG_TEST_ASSERT(serverMessage.size() == 9);

        NTSCFG_TEST_ASSERT(bsl::memcmp(serverMessage.buffer(0).data(),
                                       clientMessage.buffer(0).data(),
                                       3) == 0);
        NTSCFG_TEST_ASSERT(bsl::memcmp(serverMessage.buffer(1).data(),
                                       clientMessage.buffer(1).data(),
                                       3) == 0);
        NTSCFG_TEST_ASSERT(bsl::memcmp(serverMessage.buffer(2).data(),
                                       clientMessage.buffer(2).data(),
                                       3) == 0);
    }
#endif
}

void testDatagramSocketTransmissionMultipleMessages(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    const ntsa::Endpoint&  serverEndpoint,
    ntsa::Handle           client,
    const ntsa::Endpoint&  clientEndpoint,
    bslma::Allocator*      allocator)
{
    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << ": sendmmsg/recvmmsg"
                          << NTSCFG_TEST_LOG_END;

    enum { NUM_MESSAGES = 3 };

    ntsa::Error error;

    char DATA[] = "123456789";

    typedef test::Storage<sizeof DATA - 1> StorageBuffer;

    StorageBuffer clientBuffer0[NUM_MESSAGES];
    StorageBuffer clientBuffer1[NUM_MESSAGES];
    StorageBuffer clientBuffer2[NUM_MESSAGES];

    StorageBuffer serverBuffer0[NUM_MESSAGES];
    StorageBuffer serverBuffer1[NUM_MESSAGES];
    StorageBuffer serverBuffer2[NUM_MESSAGES];

    for (bsl::size_t messageIndex = 0; messageIndex < NUM_MESSAGES;
         ++messageIndex)
    {
        bsl::memcpy(clientBuffer0[messageIndex].data(), DATA + 0, 3);
        bsl::memcpy(clientBuffer1[messageIndex].data(), DATA + 3, 3);
        bsl::memcpy(clientBuffer2[messageIndex].data(), DATA + 6, 3);
    }

    ntsa::ConstMessage clientMessage[NUM_MESSAGES];
    for (bsl::size_t messageIndex = 0; messageIndex < NUM_MESSAGES;
         ++messageIndex)
    {
        clientMessage[messageIndex].setEndpoint(serverEndpoint);
        clientMessage[messageIndex].appendBuffer(
            clientBuffer0[messageIndex].data(),
            3);
        clientMessage[messageIndex].appendBuffer(
            clientBuffer1[messageIndex].data(),
            3);
        clientMessage[messageIndex].appendBuffer(
            clientBuffer2[messageIndex].data(),
            3);

        NTSCFG_TEST_ASSERT(clientMessage[messageIndex].size() == 9);
        NTSCFG_TEST_ASSERT(clientMessage[messageIndex].capacity() == 9);
    }

    ntsa::MutableMessage serverMessage[NUM_MESSAGES];

    for (bsl::size_t messageIndex = 0; messageIndex < NUM_MESSAGES;
         ++messageIndex)
    {
        serverMessage[messageIndex].appendBuffer(
            serverBuffer0[messageIndex].data(),
            3);
        serverMessage[messageIndex].appendBuffer(
            serverBuffer1[messageIndex].data(),
            3);
        serverMessage[messageIndex].appendBuffer(
            serverBuffer2[messageIndex].data(),
            3);

        NTSCFG_TEST_ASSERT(serverMessage[messageIndex].size() == 0);
        NTSCFG_TEST_ASSERT(serverMessage[messageIndex].capacity() == 9);
    }

    // Enqueue outgoing data to transmit by the client socket.

    {
        bsl::size_t numBytesSendable;
        bsl::size_t numBytesSent;
        bsl::size_t numMessagesSendable;
        bsl::size_t numMessagesSent;

        error = ntsu::SocketUtil::sendToMultiple(&numBytesSendable,
                                                 &numBytesSent,
                                                 &numMessagesSendable,
                                                 &numMessagesSent,
                                                 clientMessage,
                                                 NUM_MESSAGES,
                                                 client);

        if (error && error == ntsa::Error::e_NOT_IMPLEMENTED) {
            NTSCFG_TEST_LOG_DEBUG
                << "Failed to send multiple messages: " << error
                << NTSCFG_TEST_LOG_END;
            return;
        }

        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(numBytesSendable == NUM_MESSAGES * 9);
        NTSCFG_TEST_ASSERT(numBytesSent == NUM_MESSAGES * 9);

        NTSCFG_TEST_ASSERT(numMessagesSendable == NUM_MESSAGES);
        NTSCFG_TEST_ASSERT(numMessagesSent == NUM_MESSAGES);
    }

    bslmt::ThreadUtil::sleep(bsls::TimeInterval(1.0));

    // Dequeue incoming data received by the server socket.

    {
        bsl::size_t numBytesReceivable;
        bsl::size_t numBytesReceived;
        bsl::size_t numMessagesReceivable;
        bsl::size_t numMessagesReceived;

        error = ntsu::SocketUtil::receiveFromMultiple(&numBytesReceivable,
                                                      &numBytesReceived,
                                                      &numMessagesReceivable,
                                                      &numMessagesReceived,
                                                      serverMessage,
                                                      NUM_MESSAGES,
                                                      server);

        if (error && error == ntsa::Error::e_NOT_IMPLEMENTED) {
            NTSCFG_TEST_LOG_DEBUG
                << "Failed to receive multiple messages: " << error
                << NTSCFG_TEST_LOG_END;
            return;
        }

        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(numBytesReceivable == NUM_MESSAGES * 9);
        NTSCFG_TEST_ASSERT(numBytesReceived == NUM_MESSAGES * 9);

        NTSCFG_TEST_ASSERT(numMessagesReceivable == NUM_MESSAGES);
        NTSCFG_TEST_ASSERT(numMessagesReceived == NUM_MESSAGES);

        for (bsl::size_t messageIndex = 0; messageIndex < NUM_MESSAGES;
             ++messageIndex)
        {
            NTSCFG_TEST_ASSERT(serverMessage[messageIndex].endpoint() ==
                               clientEndpoint);
            NTSCFG_TEST_ASSERT(serverMessage[messageIndex].size() == 9);

            NTSCFG_TEST_ASSERT(
                bsl::memcmp(serverMessage[messageIndex].buffer(0).data(),
                            clientMessage[messageIndex].buffer(0).data(),
                            3) == 0);
            NTSCFG_TEST_ASSERT(
                bsl::memcmp(serverMessage[messageIndex].buffer(1).data(),
                            clientMessage[messageIndex].buffer(1).data(),
                            3) == 0);
            NTSCFG_TEST_ASSERT(
                bsl::memcmp(serverMessage[messageIndex].buffer(2).data(),
                            clientMessage[messageIndex].buffer(2).data(),
                            3) == 0);
        }
    }
}

void testStreamSocketMsgZeroCopy(ntsa::Transport::Value transport,
                                 ntsa::Handle           server,
                                 ntsa::Handle           client,
                                 bslma::Allocator*      allocator)
{
    if (transport == ntsa::Transport::e_LOCAL_STREAM) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << NTSCFG_TEST_LOG_END;

    // Note: for this test case msgSize is not really important as loopback
    // device is used - it means that even if MSG_ZEROCOPY option is used then
    // anyway data will be copied

    const int msgSize           = 200;
    const int numMessagesToSend = 200;

    ntsa::Error error;

    error = ntsu::SocketOptionUtil::setAllowMsgZeroCopy(client, true);
    NTSCFG_TEST_OK(error);

    bsl::vector<char> message(msgSize, allocator);
    for (int i = 0; i < msgSize; ++i) {
        message[i] = bsl::rand() % 100;
    }
    const ntsa::Data data(ntsa::ConstBuffer(message.data(), message.size()));

    bsl::list<ntsa::ZeroCopy>         feedback(allocator);
    bsl::unordered_set<bsl::uint32_t> sendIDs(allocator);

    for (int i = 0; i < numMessagesToSend; ++i) {
        ntsa::SendContext context;
        ntsa::SendOptions options;
        options.setZeroCopy(true);

        error = ntsu::SocketUtil::send(&context, data, options, client);
        if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK) ||
            error == ntsa::Error(ntsa::Error::e_LIMIT))
        {
            --i;
            continue;
        }
        NTSCFG_TEST_OK(error);
        sendIDs.insert(i);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == msgSize);
        NTSCFG_TEST_ASSERT(context.bytesSent() == msgSize);

        test::extractZeroCopyNotifications(&feedback, client, allocator);
    }

    // receive data
    {
        bsl::vector<char> rBuffer(msgSize, allocator);
        for (int totalSend = msgSize * numMessagesToSend; totalSend > 0;) {
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(
                ntsa::MutableBuffer(rBuffer.data(), rBuffer.size()));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            if (!error) {
                totalSend -= context.bytesReceived();
            }
        }
    }

    // retrieve data from the socket error queue until all send system
    // calls are acknowledged by the OS
    while (!sendIDs.empty()) {
        test::extractZeroCopyNotifications(&feedback, client, allocator);

        while (!feedback.empty()) {
            const ntsa::ZeroCopy& zc = feedback.front();
            NTSCFG_TEST_EQ(zc.code(), 1);  // we know that OS copied data
            if (zc.from() == zc.to()) {
                NTSCFG_TEST_EQ(sendIDs.erase(zc.from()), 1);
            }
            else {
                for (bsl::uint32_t i = zc.from(); i != (zc.to() + 1); ++i) {
                    NTSCFG_TEST_EQ(sendIDs.erase(i), 1);
                }
            }
            feedback.pop_front();
        }
    }
}

void testDatagramSocketTxTimestamps(ntsa::Transport::Value transport,
                                    ntsa::Handle           server,
                                    const ntsa::Endpoint&  serverEndpoint,
                                    ntsa::Handle           client,
                                    const ntsa::Endpoint&  clientEndpoint,
                                    bslma::Allocator*      allocator)
{
    if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
        return;
    }

    if (!ntscfg::Platform::supportsTimestamps()) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    error = ntsu::SocketOptionUtil::setTimestampOutgoingData(client, true);

    NTSCFG_TEST_OK(error);

    const int msgSize           = 200;
    const int numMessagesToSend = 100;

    bsl::vector<char> message(msgSize, allocator);
    for (int i = 0; i < msgSize; ++i) {
        message[i] = bsl::rand() % 100;
    }
    const ntsa::Data data(ntsa::ConstBuffer(message.data(), message.size()));

    bsl::list<ntsa::Timestamp> feedback(allocator);

    // for each TS id there is a map of each expected TS type and a reference
    // time
    bsl::unordered_map<
        bsl::uint32_t,
        bsl::unordered_map<ntsa::TimestampType::Value, bsls::TimeInterval> >
        timestampsToValidate(allocator);

    // Enqueue outgoing data to transmit by the client socket.

    for (int i = 0; i < numMessagesToSend; ++i) {
        ntsa::SendContext context;
        ntsa::SendOptions options;
        options.setEndpoint(serverEndpoint);

        const bsls::TimeInterval sysTimeBeforeSending =
            bdlt::CurrentTime::now();
        error = ntsu::SocketUtil::send(&context, data, options, client);
        if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK) ||
            error == ntsa::Error(ntsa::Error::e_LIMIT))
        {
            --i;
            continue;
        }
        NTSCFG_TEST_OK(error);

        timestampsToValidate[i][ntsa::TimestampType::e_SENT] =
            sysTimeBeforeSending;
        timestampsToValidate[i][ntsa::TimestampType::e_SCHEDULED] =
            sysTimeBeforeSending;

        NTSCFG_TEST_ASSERT(context.bytesSendable() == msgSize);
        NTSCFG_TEST_ASSERT(context.bytesSent() == msgSize);

        test::extractTimestampNotifications(&feedback, client, allocator);
    }

    // receive data
    {
        bsl::vector<char> rBuffer(msgSize, allocator);
        for (int totalSend = msgSize * numMessagesToSend; totalSend > 0;) {
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(
                ntsa::MutableBuffer(rBuffer.data(), rBuffer.size()));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            if (!error) {
                totalSend -= context.bytesReceived();
            }
        }
    }

    // retrieve data from the socket error queue until all send system
    // calls related timestamps received
    while (!timestampsToValidate.empty()) {
        test::extractTimestampNotifications(&feedback, client, allocator);

        while (!feedback.empty()) {
            const ntsa::Timestamp& ts = feedback.front();
            NTSCFG_TEST_EQ(timestampsToValidate.count(ts.id()), 1);
            NTSCFG_TEST_EQ(timestampsToValidate[ts.id()].count(ts.type()), 1);
            NTSCFG_TEST_LT(timestampsToValidate[ts.id()][ts.type()],
                           ts.time());
            timestampsToValidate[ts.id()].erase(ts.type());
            if (timestampsToValidate[ts.id()].empty()) {
                timestampsToValidate.erase(ts.id());
            }
            feedback.pop_front();
        }
    }
}

void testStreamSocketTxTimestamps(ntsa::Transport::Value transport,
                                  ntsa::Handle           server,
                                  ntsa::Handle           client,
                                  bslma::Allocator*      allocator)
{
    if (transport == ntsa::Transport::e_LOCAL_STREAM) {
        return;
    }

    if (!ntscfg::Platform::supportsTimestamps()) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    error = ntsu::SocketOptionUtil::setTimestampOutgoingData(client, true);

    NTSCFG_TEST_OK(error);

    const int msgSize           = 200;
    const int numMessagesToSend = 5;

    bsl::vector<char> message(msgSize, allocator);
    for (int i = 0; i < msgSize; ++i) {
        message[i] = bsl::rand() % 100;
    }
    const ntsa::Data data(ntsa::ConstBuffer(message.data(), message.size()));

    bsl::list<ntsa::Timestamp> feedback(allocator);

    // for each TS id there is a map of each expected TS type and a reference
    // time
    bsl::unordered_map<
        bsl::uint32_t,
        bsl::unordered_map<ntsa::TimestampType::Value, bsls::TimeInterval> >
        timestampsToValidate(allocator);

    bsl::uint32_t byteCounter = 0;
    for (int i = 0; i < numMessagesToSend; ++i) {
        ntsa::SendContext context;
        ntsa::SendOptions options;
        options.setZeroCopy(true);

        const bsls::TimeInterval sysTimeBeforeSending =
            bdlt::CurrentTime::now();
        error = ntsu::SocketUtil::send(&context, data, options, client);
        if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK) ||
            error == ntsa::Error(ntsa::Error::e_LIMIT))
        {
            --i;
            continue;
        }
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == msgSize);
        NTSCFG_TEST_ASSERT(context.bytesSent() == msgSize);

        byteCounter += msgSize;

        timestampsToValidate[byteCounter - 1][ntsa::TimestampType::e_SENT] =
            sysTimeBeforeSending;
        timestampsToValidate[byteCounter - 1]
                            [ntsa::TimestampType::e_SCHEDULED] =
                                sysTimeBeforeSending;
        timestampsToValidate[byteCounter - 1]
                            [ntsa::TimestampType::e_ACKNOWLEDGED] =
                                sysTimeBeforeSending;

        test::extractTimestampNotifications(&feedback, client, allocator);
    }

    // receive data
    {
        bsl::vector<char> rBuffer(msgSize, allocator);
        for (int totalSend = msgSize * numMessagesToSend; totalSend > 0;) {
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(
                ntsa::MutableBuffer(rBuffer.data(), rBuffer.size()));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            if (!error) {
                totalSend -= context.bytesReceived();
            }
        }
    }

    // retrieve data from the socket error queue until all send system
    // calls related timestamps received
    while (!timestampsToValidate.empty()) {
        test::extractTimestampNotifications(&feedback, client, allocator);

        while (!feedback.empty()) {
            const ntsa::Timestamp& ts = feedback.front();
            NTSCFG_TEST_EQ(timestampsToValidate.count(ts.id()), 1);
            NTSCFG_TEST_EQ(timestampsToValidate[ts.id()].count(ts.type()), 1);
            NTSCFG_TEST_LT(timestampsToValidate[ts.id()][ts.type()],
                           ts.time());
            timestampsToValidate[ts.id()].erase(ts.type());
            if (timestampsToValidate[ts.id()].empty()) {
                timestampsToValidate.erase(ts.id());
            }
            feedback.pop_front();
        }
    }
}

void testDatagramSocketTxTimestampsAndZeroCopy(
    ntsa::Transport::Value transport,
    ntsa::Handle           server,
    const ntsa::Endpoint&  serverEndpoint,
    ntsa::Handle           client,
    const ntsa::Endpoint&  clientEndpoint,
    bslma::Allocator*      allocator)
{
    if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
        return;
    }

    if (!ntscfg::Platform::supportsTimestamps()) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    error = ntsu::SocketOptionUtil::setTimestampOutgoingData(client, true);
    NTSCFG_TEST_OK(error);

    error = ntsu::SocketOptionUtil::setAllowMsgZeroCopy(client, true);
    NTSCFG_TEST_OK(error);

    const int msgSize           = 200;
    const int numMessagesToSend = 100;

    bsl::vector<char> message(msgSize, allocator);
    for (int i = 0; i < msgSize; ++i) {
        message[i] = bsl::rand() % 100;
    }
    const ntsa::Data data(ntsa::ConstBuffer(message.data(), message.size()));

    bsl::list<ntsa::Notification> feedback(allocator);

    // for each TS id there is a map of each expected TS type and a reference
    // time
    bsl::unordered_map<
        bsl::uint32_t,
        bsl::unordered_map<ntsa::TimestampType::Value, bsls::TimeInterval> >
                                      timestampsToValidate(allocator);
    bsl::unordered_set<bsl::uint32_t> zeroCopyToValidate(allocator);

    // Enqueue outgoing data to transmit by the client socket.

    for (int i = 0; i < numMessagesToSend; ++i) {
        ntsa::SendContext context;
        ntsa::SendOptions options;
        options.setEndpoint(serverEndpoint);
        options.setZeroCopy(true);

        const bsls::TimeInterval sysTimeBeforeSending =
            bdlt::CurrentTime::now();
        error = ntsu::SocketUtil::send(&context, data, options, client);
        if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK) ||
            error == ntsa::Error(ntsa::Error::e_LIMIT))
        {
            --i;
            continue;
        }
        NTSCFG_TEST_OK(error);

        timestampsToValidate[i][ntsa::TimestampType::e_SENT] =
            sysTimeBeforeSending;
        timestampsToValidate[i][ntsa::TimestampType::e_SCHEDULED] =
            sysTimeBeforeSending;
        zeroCopyToValidate.insert(i);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == msgSize);
        NTSCFG_TEST_ASSERT(context.bytesSent() == msgSize);

        test::extractNotifications(&feedback, client, allocator);
    }

    // receive data
    {
        bsl::vector<char> rBuffer(msgSize, allocator);
        for (int totalSend = msgSize * numMessagesToSend; totalSend > 0;) {
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(
                ntsa::MutableBuffer(rBuffer.data(), rBuffer.size()));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            if (!error) {
                totalSend -= context.bytesReceived();
            }
        }
    }

    // retrieve data from the socket error queue until all send system
    // calls related timestamps received
    while (!timestampsToValidate.empty() || !zeroCopyToValidate.empty()) {
        test::extractNotifications(&feedback, client, allocator);

        while (!feedback.empty()) {
            const ntsa::Notification& nt = feedback.front();
            if (nt.isTimestamp()) {
                const ntsa::Timestamp& ts = nt.timestamp();
                NTSCFG_TEST_EQ(timestampsToValidate.count(ts.id()), 1);
                NTSCFG_TEST_EQ(timestampsToValidate[ts.id()].count(ts.type()),
                               1);
                NTSCFG_TEST_LT(timestampsToValidate[ts.id()][ts.type()],
                               ts.time());
                timestampsToValidate[ts.id()].erase(ts.type());
                if (timestampsToValidate[ts.id()].empty()) {
                    timestampsToValidate.erase(ts.id());
                }
            }
            else if (nt.isZeroCopy()) {
                const ntsa::ZeroCopy& zc = nt.zeroCopy();
                NTSCFG_TEST_EQ(zc.code(), 1);
                if (zc.from() == zc.to()) {
                    NTSCFG_TEST_EQ(zeroCopyToValidate.erase(zc.from()), 1);
                }
                else {
                    for (bsl::uint32_t i = zc.from(); i != (zc.to() + 1); ++i)
                    {
                        NTSCFG_TEST_EQ(zeroCopyToValidate.erase(i), 1);
                    }
                }
            }
            else {
                NTSCFG_TEST_ASSERT(false);
            }
            feedback.pop_front();
        }
    }
}

void testStreamSocketTxTimestampsAndZeroCopy(ntsa::Transport::Value transport,
                                             ntsa::Handle           server,
                                             ntsa::Handle           client,
                                             bslma::Allocator*      allocator)
{
    if (transport == ntsa::Transport::e_LOCAL_STREAM) {
        return;
    }

    if (!ntscfg::Platform::supportsTimestamps()) {
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Testing " << transport << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    error = ntsu::SocketOptionUtil::setTimestampOutgoingData(client, true);
    NTSCFG_TEST_OK(error);

    error = ntsu::SocketOptionUtil::setAllowMsgZeroCopy(client, true);
    NTSCFG_TEST_OK(error);

    const int msgSize           = 200;
    const int numMessagesToSend = 5;

    bsl::vector<char> message(msgSize, allocator);
    for (int i = 0; i < msgSize; ++i) {
        message[i] = bsl::rand() % 100;
    }
    const ntsa::Data data(ntsa::ConstBuffer(message.data(), message.size()));

    bsl::list<ntsa::Notification> feedback(allocator);

    // for each TS id there is a map of each expected TS type and a reference
    // time
    bsl::unordered_map<
        bsl::uint32_t,
        bsl::unordered_map<ntsa::TimestampType::Value, bsls::TimeInterval> >
                                      timestampsToValidate(allocator);
    bsl::unordered_set<bsl::uint32_t> zeroCopyToValidate(allocator);

    // Enqueue outgoing data to transmit by the client socket.

    bsl::uint32_t byteCounter = 0;
    for (int i = 0; i < numMessagesToSend; ++i) {
        ntsa::SendContext context;
        ntsa::SendOptions options;
        options.setZeroCopy(true);

        const bsls::TimeInterval sysTimeBeforeSending =
            bdlt::CurrentTime::currentTimeDefault();
        if (sysTimeBeforeSending.totalSeconds() > 0) {
            error = ntsu::SocketUtil::send(&context, data, options, client);
            if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK) ||
                error == ntsa::Error(ntsa::Error::e_LIMIT))
            {
                --i;
                continue;
            }
        }
        NTSCFG_TEST_OK(error);

        byteCounter += msgSize;

        timestampsToValidate[byteCounter - 1][ntsa::TimestampType::e_SENT] =
            sysTimeBeforeSending;
        timestampsToValidate[byteCounter - 1]
                            [ntsa::TimestampType::e_SCHEDULED] =
                                sysTimeBeforeSending;
        timestampsToValidate[byteCounter - 1]
                            [ntsa::TimestampType::e_ACKNOWLEDGED] =
                                sysTimeBeforeSending;
        zeroCopyToValidate.insert(i);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == msgSize);
        NTSCFG_TEST_ASSERT(context.bytesSent() == msgSize);

        test::extractNotifications(&feedback, client, allocator);
    }

    // receive data
    {
        bsl::vector<char> rBuffer(msgSize, allocator);
        for (int totalSend = msgSize * numMessagesToSend; totalSend > 0;) {
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(
                ntsa::MutableBuffer(rBuffer.data(), rBuffer.size()));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            if (!error) {
                totalSend -= context.bytesReceived();
            }
        }
    }

    // retrieve data from the socket error queue until all send system
    // calls related timestamps received
    while (!timestampsToValidate.empty() || !zeroCopyToValidate.empty()) {
        test::extractNotifications(&feedback, client, allocator);

        while (!feedback.empty()) {
            const ntsa::Notification& nt = feedback.front();
            if (nt.isTimestamp()) {
                const ntsa::Timestamp& ts = nt.timestamp();
                NTSCFG_TEST_EQ(timestampsToValidate.count(ts.id()), 1);
                NTSCFG_TEST_EQ(timestampsToValidate[ts.id()].count(ts.type()),
                               1);
                NTSCFG_TEST_LT(timestampsToValidate[ts.id()][ts.type()],
                               ts.time());
                timestampsToValidate[ts.id()].erase(ts.type());
                if (timestampsToValidate[ts.id()].empty()) {
                    timestampsToValidate.erase(ts.id());
                }
            }
            else if (nt.isZeroCopy()) {
                const ntsa::ZeroCopy& zc = nt.zeroCopy();
                NTSCFG_TEST_EQ(zc.code(), 1);
                if (zc.from() == zc.to()) {
                    NTSCFG_TEST_EQ(zeroCopyToValidate.erase(zc.from()), 1);
                }
                else {
                    for (bsl::uint32_t i = zc.from(); i != (zc.to() + 1); ++i)
                    {
                        NTSCFG_TEST_EQ(zeroCopyToValidate.erase(i), 1);
                    }
                }
            }
            else {
                NTSCFG_TEST_ASSERT(false);
            }
            feedback.pop_front();
        }
    }
}

/// Comparator with is used to help sorting Timestamps according to their time
/// value.
struct TimestampTimeComparator {
    /// Return true if the specified 'a' occurred earlier than the specified
    /// 'b'. Otherwise return false.
    bool operator()(const ntsa::Timestamp& a, const ntsa::Timestamp& b)
    {
        return a.time() < b.time();
    }
};

}  // close namespace 'test'

NTSCFG_TEST_CASE(1)
{
    // Concern: Stream socket breathing test, which also serves as the
    // usage example.
    //
    // Plan:

    ntsa::Error error;

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV4_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV4_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV6_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV6_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(ntsa::Transport::e_LOCAL_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_STREAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        // Create a blocking socket, bind it to any port on the loopback
        // address, then begin listening for connections.

        ntsa::Handle listener;
        error = ntsu::SocketUtil::create(&listener, transport);
        NTSCFG_TEST_ASSERT(!error);

        if (transport == ntsa::Transport::e_TCP_IPV4_STREAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                false,
                listener);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_TCP_IPV6_STREAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                false,
                listener);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_LOCAL_STREAM) {
            ntsa::LocalName localName;
            error = ntsa::LocalName::generateUnique(&localName);
            NTSCFG_TEST_ASSERT(!error);

            error = ntsu::SocketUtil::bind(ntsa::Endpoint(localName),
                                           false,
                                           listener);
            NTSCFG_TEST_ASSERT(!error);
        }
        else {
            NTSCFG_TEST_TRUE(false);
        }

        error = ntsu::SocketUtil::listen(1, listener);
        NTSCFG_TEST_ASSERT(!error);

        // Create a blocking socket for the client, then connect that socket to
        // the listener socket's local endpoint.

        ntsa::Handle client;
        error = ntsu::SocketUtil::create(&client, transport);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint listenerEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::connect(listenerEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        // Create a blocking socket for the server by accepting the connection
        // made to the listener socket.

        ntsa::Handle server;
        error = ntsu::SocketUtil::accept(&server, listener);
        NTSCFG_TEST_ASSERT(!error);

        // Get the client source and remote endpoints.

        ntsa::Endpoint clientSourceEndpoint;
        error =
            ntsu::SocketUtil::sourceEndpoint(&clientSourceEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint clientRemoteEndpoint;
        error =
            ntsu::SocketUtil::remoteEndpoint(&clientRemoteEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        // Get the server source and remote endpoints.

        ntsa::Endpoint serverSourceEndpoint;
        error =
            ntsu::SocketUtil::sourceEndpoint(&serverSourceEndpoint, server);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint serverRemoteEndpoint;
        error =
            ntsu::SocketUtil::remoteEndpoint(&serverRemoteEndpoint, server);
        NTSCFG_TEST_ASSERT(!error);

        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Listener at " << listenerEndpoint << bsl::endl;

            bsl::cout << "Client at " << clientSourceEndpoint << " to "
                      << clientRemoteEndpoint << bsl::endl;

            bsl::cout << "Server at " << serverSourceEndpoint << " to "
                      << serverRemoteEndpoint << bsl::endl;
        }

        // Enqueue outgoing data to transmit by the client socket.

        {
            char              buffer = 'C';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the server socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'C');
        }

        // Enqueue outgoing data to transmit by the server socket.

        {
            char              buffer = 'S';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the client socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'S');
        }

        // Shutdown writing by the client socket.

        error = ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, client);
        NTSCFG_TEST_ASSERT(!error);

        // Dequeue incoming data received by the server socket, and observe
        // that zero bytes are successfully dequeued, indicating the client
        // socket has shut down writing from its side of the connection.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 0);
        }

        // Shutdown writing by the server socket.

        error = ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, server);
        NTSCFG_TEST_ASSERT(!error);

        // Dequeue incoming data received by the client socket, and observe
        // that zero bytes are successfully dequeued, indicating the server
        // socket has shut down writing from its side of the connection.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 0);
        }

        // Close each socket.

        error = ntsu::SocketUtil::close(listener);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(client);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(server);
        NTSCFG_TEST_ASSERT(!error);
    }
}

NTSCFG_TEST_CASE(2)
{
    // Concern: Datagram socket breathing test, which also serves as the
    // usage example.
    //
    // Plan:

    ntsa::Error error;

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV4_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV6_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_LOCAL_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_DATAGRAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        // Create a blocking socket for the server and bind it to any port on
        // the loopback address.

        ntsa::Handle server;
        error = ntsu::SocketUtil::create(&server, transport);
        NTSCFG_TEST_ASSERT(!error);

        if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                false,
                server);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                false,
                server);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
            ntsa::LocalName localName;
            error = ntsa::LocalName::generateUnique(&localName);
            NTSCFG_TEST_ASSERT(!error);

            error = ntsu::SocketUtil::bind(ntsa::Endpoint(localName),
                                           false,
                                           server);
            NTSCFG_TEST_ASSERT(!error);
        }
        else {
            NTSCFG_TEST_TRUE(false);
        }

        ntsa::Endpoint serverEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint, server);
        NTSCFG_TEST_ASSERT(!error);

        // Create a blocking socket for the client and bind it to any port on
        // the loopback address.

        ntsa::Handle client;
        error = ntsu::SocketUtil::create(&client, transport);
        NTSCFG_TEST_ASSERT(!error);

        if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                false,
                client);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
            error = ntsu::SocketUtil::bind(
                ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                false,
                client);
            NTSCFG_TEST_ASSERT(!error);
        }
        else if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
            ntsa::LocalName localName;
            error = ntsa::LocalName::generateUnique(&localName);
            NTSCFG_TEST_ASSERT(!error);

            error = ntsu::SocketUtil::bind(ntsa::Endpoint(localName),
                                           false,
                                           client);
            NTSCFG_TEST_ASSERT(!error);
        }
        else {
            NTSCFG_TEST_TRUE(false);
        }

        ntsa::Endpoint clientEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&clientEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        // Enqueue outgoing data to transmit by the client socket.

        {
            char              buffer = 'C';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            options.setEndpoint(serverEndpoint);

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the server socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(!context.endpoint().isNull());
            NTSCFG_TEST_ASSERT(context.endpoint().value() == clientEndpoint);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'C');
        }

        // Enqueue outgoing data to transmit by the server socket.

        {
            char              buffer = 'S';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            options.setEndpoint(clientEndpoint);

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the client socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(!context.endpoint().isNull());
            NTSCFG_TEST_ASSERT(context.endpoint().value() == serverEndpoint);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'S');
        }

        // Close each socket.

        error = ntsu::SocketUtil::close(client);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(server);
        NTSCFG_TEST_ASSERT(!error);
    }
}

NTSCFG_TEST_CASE(3)
{
    // Concern: Stream socket transmission: single buffer.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(
            &test::testStreamSocketTransmissionSingleBuffer);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(4)
{
    // Concern: Stream socket transmission: multiple buffers.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(
            &test::testStreamSocketTransmissionMultipleBuffers);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(5)
{
    // Concern: Stream socket transmission: blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(&test::testStreamSocketTransmissionBlob);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(6)
{
    // Concern: Stream socket transmission: file.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(&test::testStreamSocketTransmissionFile);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(7)
{
    // Concern: Stream socket transmission: single buffer.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(
            &test::testDatagramSocketTransmissionSingleBuffer);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(8)
{
    // Concern: Stream socket transmission: multiple buffers.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(
            &test::testDatagramSocketTransmissionMultipleBuffers);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(9)
{
    // Concern: Stream socket transmission: blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(
            &test::testDatagramSocketTransmissionBlob);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(10)
{
    // Concern: Stream socket transmission: single message.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(
            &test::testDatagramSocketTransmissionSingleMessage);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(11)
{
    // Concern: Stream socket transmission: multiple messages.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(
            &test::testDatagramSocketTransmissionMultipleMessages);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(12)
{
    // Concern: Listener socket maximum backlog
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::Transport::Value> socketTypes;

        if (ntsu::AdapterUtil::supportsTransport(
                ntsa::Transport::e_TCP_IPV4_STREAM))
        {
            socketTypes.push_back(ntsa::Transport::e_TCP_IPV4_STREAM);
        }

        if (ntsu::AdapterUtil::supportsTransport(
                ntsa::Transport::e_TCP_IPV6_STREAM))
        {
            socketTypes.push_back(ntsa::Transport::e_TCP_IPV6_STREAM);
        }

        if (ntsu::AdapterUtil::supportsTransport(
                ntsa::Transport::e_LOCAL_STREAM))
        {
            socketTypes.push_back(ntsa::Transport::e_LOCAL_STREAM);
        }

        const bsl::size_t systemSoMaxConn = ntsu::SocketUtil::maxBacklog();

        if (NTSCFG_TEST_VERBOSITY > 0) {
            bsl::cout << "SOMAXCONN = " << systemSoMaxConn << bsl::endl;
        }

        for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
            ntsa::Transport::Value transport = socketTypes[i];

            bsl::size_t BACKLOG[7];
            BACKLOG[0] = 0;
            BACKLOG[1] = 1;
            BACKLOG[2] = systemSoMaxConn - 1;
            BACKLOG[3] = systemSoMaxConn;
            BACKLOG[4] = systemSoMaxConn + 1;
            BACKLOG[5] = 4096;
            BACKLOG[6] = 8192;

            for (bsl::size_t backlogIndex = 0; backlogIndex < 7;
                 ++backlogIndex)
            {
                bsl::size_t backlog = BACKLOG[backlogIndex];

                // Create a blocking socket, bind it to any port on the loopback
                // address, then begin listening for connections.

                ntsa::Handle listener;
                error = ntsu::SocketUtil::create(&listener, transport);
                NTSCFG_TEST_ASSERT(!error);

                if (transport == ntsa::Transport::e_TCP_IPV4_STREAM) {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                        false,
                        listener);
                    NTSCFG_TEST_ASSERT(!error);
                }
                else if (transport == ntsa::Transport::e_TCP_IPV6_STREAM) {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                        false,
                        listener);
                    NTSCFG_TEST_ASSERT(!error);
                }
                else if (transport == ntsa::Transport::e_LOCAL_STREAM) {
                    ntsa::LocalName localName;
                    error = ntsa::LocalName::generateUnique(&localName);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(ntsa::Endpoint(localName),
                                                   false,
                                                   listener);
                    NTSCFG_TEST_ASSERT(!error);
                }
                else {
                    NTSCFG_TEST_TRUE(false);
                }

                ntsa::Endpoint listenerEndpoint;
                error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint,
                                                         listener);
                NTSCFG_TEST_ASSERT(!error);

                if (NTSCFG_TEST_VERBOSITY > 0) {
                    bsl::cout << "Testing backlog = " << backlog << bsl::endl;
                }

                error = ntsu::SocketUtil::listen(backlog, listener);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(listener);
                NTSCFG_TEST_ASSERT(!error);
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(13)
{
    // Concern: Duplicating sockets
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        // Create a blocking socket, bind it to any port on the loopback
        // address, then begin listening for connections.

        ntsa::Handle listener;
        error = ntsu::SocketUtil::create(&listener,
                                         ntsa::Transport::e_TCP_IPV4_STREAM);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
            false,
            listener);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::listen(1, listener);
        NTSCFG_TEST_ASSERT(!error);

        // Create a blocking socket for the client, then connect that socket to
        // the listener socket's local endpoint.

        ntsa::Handle client;
        error = ntsu::SocketUtil::create(&client,
                                         ntsa::Transport::e_TCP_IPV4_STREAM);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint listenerEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::connect(listenerEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        // Create a blocking socket for the server by accepting the connection
        // made to the listener socket.

        ntsa::Handle server;
        error = ntsu::SocketUtil::accept(&server, listener);
        NTSCFG_TEST_ASSERT(!error);

        // Duplicate the client socket handle the close it. The subseequent
        // test ensures that operations on the socket still succeed, because
        // the original handle is not yet closed.

        {
            ntsa::Handle clientDuplicate = ntsa::k_INVALID_HANDLE;
            error = ntsu::SocketUtil::duplicate(&clientDuplicate, client);
            NTSCFG_TEST_NE(clientDuplicate, ntsa::k_INVALID_HANDLE);

            error = ntsu::SocketUtil::close(clientDuplicate);
            NTSCFG_TEST_ASSERT(!error);
        }

        // Duplicate the server socket handle the close it. The subseequent
        // test ensures that operations on the socket still succeed, because
        // the original handle is not yet closed.

        {
            ntsa::Handle serverDuplicate = ntsa::k_INVALID_HANDLE;
            error = ntsu::SocketUtil::duplicate(&serverDuplicate, server);
            NTSCFG_TEST_NE(serverDuplicate, ntsa::k_INVALID_HANDLE);

            error = ntsu::SocketUtil::close(serverDuplicate);
            NTSCFG_TEST_ASSERT(!error);
        }

        // Enqueue outgoing data to transmit by the client socket.

        {
            char              buffer = 'C';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the server socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'C');
        }

        // Enqueue outgoing data to transmit by the server socket.

        {
            char              buffer = 'S';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the client socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'S');
        }

        // Shutdown writing by the client socket.

        error = ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, client);
        NTSCFG_TEST_ASSERT(!error);

        // Dequeue incoming data received by the server socket, and observe
        // that zero bytes are successfully dequeued, indicating the client
        // socket has shut down writing from its side of the connection.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 0);
        }

        // Shutdown writing by the server socket.

        error = ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, server);
        NTSCFG_TEST_ASSERT(!error);

        // Dequeue incoming data received by the client socket, and observe
        // that zero bytes are successfully dequeued, indicating the server
        // socket has shut down writing from its side of the connection.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 0);
        }

        // Close each socket.

        error = ntsu::SocketUtil::close(listener);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(client);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(server);
        NTSCFG_TEST_ASSERT(!error);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(14)
{
    // Concern: Binding datagram sockets.
    //
    // Notes:
    // Immediately after calling the POSIX function 'socket' to create
    // a datagram socket, but even before calling any of the POSIX functions
    // 'bind', 'connect', or 'sendto', datagram sockets have a defined "name".
    // That is, the POSIX function 'getsockname' never fails for a valid,
    // unclosed socket file descriptor. This "name" is sometimes referred to as
    // the "local address", or "source endpoint". Initially, the datagram
    // socket reports a name of "any address, any port" for IP sockets (i.e.,
    // "0.0.0.0:0" for IPv4 socket and "[:0]:0" for IPv6 sockets), or "unnamed"
    // for Unix domain sockets. An "unnamed" Unix domain socket is a name with
    // a zero-length. On Linux, which supports a notion of an "abstract" Unix
    // domain namespace (i.e., a namespace where the names are not mirrored by
    // file system artifacts that must be manually 'unlink'-ed by the user),
    // the initial Unix domain socket is "unnamed" in the abstract namespace.
    //
    // A datagram socket's name may change up to two times during the lifetime
    // of the socket. A datagram socket's name changes as a result of callling
    // the POSIX function 'bind', but may change as a result of calling the
    // POSIX functions 'connect' or 'sendto' or 'sendmsg'. It is not necessary
    // to manually call 'bind' before calling either 'connect', 'sendto', or
    // 'sendmsg'.
    //
    // The datagram socket may be change as a result of either manually calling
    // 'bind', or implicitly by calling 'connect' or 'sendto'. Once 'bind',
    // 'connect', or 'sendto' is called on a datagram socket, however, the
    // socket may not be manually re-'binded'-ed to another name, even if that
    // name exactly matches its current name, *or* even if that name "matches"
    // but in a sense "restricts" the address (e.g. an IPv4 datagram socket may
    // not be bound to "0.0.0.0:12345", then re-'bind'-ded to
    // "127.0.0.1:12345".)
    //
    // A non-manually 'bind'-ed, unconnected datagram socket may 'connect' to
    // a peer even though it is not manually bound, and the 'connect' results
    // in the datagram socket being bound to a specific port and an address
    // assigned to a network interface for IPv4 and IPv6 sockets, and "unnamed"
    // for Unix domain sockets.
    //
    // A non-manually 'bind'-ed, unconnected datagram socket may 'sendto' a
    // peer even though it is not manually bound, and the 'sendto' results in
    // the datagram socket being bound to a specific port but still to "any
    // address" for IPv4 and IPv6 sockets, and "unnamed" for Unix domain
    // sockets. Note this behavior is different than in the case of
    // 'connect'-ing an non-manually 'bind'-ed socket.
    //
    // To emphasize, it is legitimate for the name of a datagram socket to
    // change twice during its usage. The following table describes such a
    // sequence of function calls, and the resulting name after each function
    // call.
    //
    // Function                                   Resulting Name
    // socket(AF_INET, SOCK_STREAM, IPPROTO_UDP)  0.0.0.0:0
    // bind(0.0.0.0:0)                            0.0.0.0:<SP>
    // connect(<DA>:<DP>)                         <SA>:<SP>
    //
    // Where <SP> is the source port on the local machine chosen by the local
    // machine's operating system, <DA> is the IPv4 address of the destination
    // machine, <DP> is the port number on the destination machine, and <SA>
    // is the IPv4 address assigned to one of the network interfaces on the
    // local machine that can route to <DA>.
    //
    // However, note that the name of a datagram socket may also change twice
    // during its usage even when not manually being 'bind'-ed, but simply
    // being 'connect'-ed after some packets are tramsitted by calling
    // 'sendto'. The following table describes this sequence of function calls,
    // and the resulting name after each function call.
    //
    // Function                                   Resulting Name
    // socket(AF_INET, SOCK_STREAM, IPPROTO_UDP)  0.0.0.0:0
    // sendto(<DA>:<DP>)                          0.0.0.0:<SP>
    // connect(<DA>:<DP>)                         <SA>:<SP>

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        const bool k_REUSE_ADDRESS = false;

        // Test IPv4.

        {
            // Concern 1: The source endpoint of a socket initially after
            // creation is "0.0.0.0:0". [On Windows 'getsockname' returns an
            // error.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);

#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 2: The socket may be bound after creation to
            // "0.0.0.0:0", and the socket's resulting source endpoint is the
            // host "0.0.0.0" with a non-zero port.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 3: After the socket is bound to "0.0.0.0:0", the
            // socket's resulting source endpoint is the host "0.0.0.0" with
            // a non-zero port. The socket may *not* be subsequently bound to
            // an IPv4 address that matches one of IPv4 addresses assigned to
            // one of the network interfaces even if the port matches the
            // existing port to which the socket is bound.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                {
                    ntsa::Endpoint sourceEndpoint1;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint1,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint1.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().port() != 0);

                    ntsa::Endpoint sourceEndpoint2;
                    {
                        ntsa::Adapter adapter;
                        bool found = ntsu::AdapterUtil::discoverAdapter(
                            &adapter,
                            ntsa::IpAddressType::e_V4,
                            false);
                        NTSCFG_TEST_TRUE(found);
                        NTSCFG_TEST_FALSE(adapter.ipv4Address().isNull());

                        sourceEndpoint2 =
                            ntsa::Endpoint(adapter.ipv4Address().value(),
                                           sourceEndpoint1.ip().port());
                    }

                    error = ntsu::SocketUtil::bind(sourceEndpoint2,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 4: The socket may be bound after creation to an IPv4
            // address that matches one of IPv4 addresses assigned to one of
            // the network interfaces and port zero, and the resulting socket's
            // source endpoint is that IPv4 address with a port chosen by the
            // operating system.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint explicitSourceEndpoint;
                    {
                        ntsa::Adapter adapter;
                        bool found = ntsu::AdapterUtil::discoverAdapter(
                            &adapter,
                            ntsa::IpAddressType::e_V4,
                            false);
                        NTSCFG_TEST_TRUE(found);
                        NTSCFG_TEST_FALSE(adapter.ipv4Address().isNull());

                        explicitSourceEndpoint =
                            ntsa::Endpoint(adapter.ipv4Address().value(), 0);
                    }

                    error = ntsu::SocketUtil::bind(explicitSourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_EQ(sourceEndpoint.ip().host(),
                                   explicitSourceEndpoint.ip().host());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5: A datagram socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'connect', and may not
            // explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_EQ(remoteEndpoint, serverEndpoint);
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5a: A datagram socket may be explicitly bound to "any"
            // address and "any" port, which results in the source endpoint
            // being the "any" address and a non-zero port. The socket may then
            // be implicitly rebound by the operating system as a result of
            // calling 'connect', so that the resulting source endpoint is now
            // an explicit IP address but still having the same port (i.e., the
            // address changes from "any" to a specific IP address assigned to
            // a network interface, but the port does not change), but the
            // datagram socket may not explicitly rebound afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_EQ(remoteEndpoint, serverEndpoint);
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 6: A datagram socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'sendto', and may not
            // explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    ntsa::SendContext sendContext;
                    ntsa::SendOptions sendOptions;

                    sendOptions.setEndpoint(serverEndpoint);

                    char storage = 'X';

                    error = ntsu::SocketUtil::send(
                        &sendContext,
                        ntsa::Data(ntsa::ConstBuffer(&storage, 1)),
                        sendOptions,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 7: A datagram socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'sendto', *will* be
            // implicitly rebound as a result of calling 'connect', and may not
            // explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    ntsa::SendContext sendContext;
                    ntsa::SendOptions sendOptions;

                    sendOptions.setEndpoint(serverEndpoint);

                    char storage = 'X';

                    error = ntsu::SocketUtil::send(
                        &sendContext,
                        ntsa::Data(ntsa::ConstBuffer(&storage, 1)),
                        sendOptions,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_EQ(remoteEndpoint, serverEndpoint);
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 8: A datagram socket may connect and reconnect to the
            // same remote endpoint successfully, and may, while already
            // connected, connect and reconnect to a different remote endpoint
            // successfully.

            {
                ntsa::Handle serverOne;
                error = ntsu::SocketUtil::create(
                    &serverOne,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverOneEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        serverOne);
                    NTSCFG_TEST_ASSERT(!error);

                    error =
                        ntsu::SocketUtil::sourceEndpoint(&serverOneEndpoint,
                                                         serverOne);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverOneEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverOneEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(
                        serverOneEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(serverOneEndpoint.ip().port() == 0);
                }

                ntsa::Handle serverTwo;
                error = ntsu::SocketUtil::create(
                    &serverTwo,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverTwoEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        serverTwo);
                    NTSCFG_TEST_ASSERT(!error);

                    error =
                        ntsu::SocketUtil::sourceEndpoint(&serverTwoEndpoint,
                                                         serverTwo);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverTwoEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverTwoEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(
                        serverTwoEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(serverTwoEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                for (bsl::size_t i = 0; i < 2; ++i) {
                    // Connect to server one then reconnect to server one.

                    for (bsl::size_t j = 0; j < 2; ++j) {
                        error = ntsu::SocketUtil::connect(serverOneEndpoint,
                                                          socket);
                        NTSCFG_TEST_ASSERT(!error);

                        ntsa::Endpoint sourceEndpoint;
                        error =
                            ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                        NTSCFG_TEST_ASSERT(!error);

                        NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                        NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                        NTSCFG_TEST_FALSE(
                            sourceEndpoint.ip().host().v4().isAny());
                        NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                        ntsa::Endpoint remoteEndpoint;
                        error =
                            ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                        NTSCFG_TEST_ASSERT(!error);

                        NTSCFG_TEST_EQ(remoteEndpoint, serverOneEndpoint);
                    }

                    // Connect to server two then reconnect to server two.

                    for (bsl::size_t j = 0; j < 2; ++j) {
                        error = ntsu::SocketUtil::connect(serverTwoEndpoint,
                                                          socket);
                        NTSCFG_TEST_ASSERT(!error);

                        ntsa::Endpoint sourceEndpoint;
                        error =
                            ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                        NTSCFG_TEST_ASSERT(!error);

                        NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                        NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                        NTSCFG_TEST_FALSE(
                            sourceEndpoint.ip().host().v4().isAny());
                        NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                        ntsa::Endpoint remoteEndpoint;
                        error =
                            ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                        NTSCFG_TEST_ASSERT(!error);

                        NTSCFG_TEST_EQ(remoteEndpoint, serverTwoEndpoint);
                    }

                    // Note: attempting to "disconnect" from the remote
                    // endpoint, by connecting to something like AF_UNSPEC,
                    // fails on Linux and Darwin. We assume "disconnecting"
                    // a connected datagram socket is either entirely not
                    // supported or at least very unportable.
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(serverTwo);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(serverOne);
                NTSCFG_TEST_ASSERT(!error);
            }
        }

        // Test IPv6.

        if (ntsu::AdapterUtil::supportsIpv6()) {
            // Concern 1: The source endpoint of a socket initially after
            // creation is "[:0]:0".

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 2: The socket may be bound after creation to
            // "[:0]:0", and the socket's resulting source endpoint is the
            // host ":0" with a non-zero port.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 3: After the socket is bound to "[:0]:0", the
            // socket's resulting source endpoint is the host ":0" with
            // a non-zero port. The socket may *not* be subsequently bound to
            // an IPv6 address that matches one of IPv6 addresses assigned to
            // one of the network interfaces even if the port matches the
            // existing port to which the socket is bound.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                {
                    ntsa::Endpoint sourceEndpoint1;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint1,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint1.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().port() != 0);

                    ntsa::Endpoint sourceEndpoint2;
                    {
                        ntsa::Adapter adapter;
                        bool found = ntsu::AdapterUtil::discoverAdapter(
                            &adapter,
                            ntsa::IpAddressType::e_V6,
                            false);
                        NTSCFG_TEST_TRUE(found);
                        NTSCFG_TEST_FALSE(adapter.ipv6Address().isNull());

                        sourceEndpoint2 =
                            ntsa::Endpoint(adapter.ipv6Address().value(),
                                           sourceEndpoint1.ip().port());
                    }

                    error = ntsu::SocketUtil::bind(sourceEndpoint2,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 4: The socket may be bound after creation to an IPv6
            // address that matches one of IPv6 addresses assigned to one of
            // the network interfaces and port zero, and the resulting socket's
            // source endpoint is that IPv6 address with a port chosen by the
            // operating system.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint explicitSourceEndpoint;
                    {
                        ntsa::Adapter adapter;
                        bool found = ntsu::AdapterUtil::discoverAdapter(
                            &adapter,
                            ntsa::IpAddressType::e_V6,
                            false);
                        NTSCFG_TEST_TRUE(found);
                        NTSCFG_TEST_FALSE(adapter.ipv6Address().isNull());

                        explicitSourceEndpoint =
                            ntsa::Endpoint(adapter.ipv6Address().value(), 0);
                    }

                    error = ntsu::SocketUtil::bind(explicitSourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_EQ(sourceEndpoint.ip().host(),
                                   explicitSourceEndpoint.ip().host());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5: A datagram socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'connect', and may not
            // explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(remoteEndpoint.isIp());
                    NTSCFG_TEST_TRUE(remoteEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(
                        remoteEndpoint.ip().host().v6().equalsScopeless(
                            serverEndpoint.ip().host().v6()));
                    NTSCFG_TEST_TRUE(remoteEndpoint.ip().port() ==
                                     serverEndpoint.ip().port());
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5a: A datagram socket may be explicitly bound to "any"
            // address and "any" port, which results in the source endpoint
            // being the "any" address and a non-zero port. The socket may then
            // be implicitly rebound by the operating system as a result of
            // calling 'connect', so that the resulting source endpoint is now
            // an explicit IP address but still having the same port (i.e., the
            // address changes from "any" to a specific IP address assigned to
            // a network interface, but the port does not change), but the
            // datagram socket may not explicitly rebound afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(remoteEndpoint.isIp());
                    NTSCFG_TEST_TRUE(remoteEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(
                        remoteEndpoint.ip().host().v6().equalsScopeless(
                            serverEndpoint.ip().host().v6()));
                    NTSCFG_TEST_TRUE(remoteEndpoint.ip().port() ==
                                     serverEndpoint.ip().port());
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 6: A datagram socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'sendto', and may not
            // explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    ntsa::SendContext sendContext;
                    ntsa::SendOptions sendOptions;

                    sendOptions.setEndpoint(serverEndpoint);

                    char storage = 'X';

                    error = ntsu::SocketUtil::send(
                        &sendContext,
                        ntsa::Data(ntsa::ConstBuffer(&storage, 1)),
                        sendOptions,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 7: A datagram socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'sendto', *will* be
            // implicitly rebound as a result of calling 'connect', and may not
            // explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    ntsa::SendContext sendContext;
                    ntsa::SendOptions sendOptions;

                    sendOptions.setEndpoint(serverEndpoint);

                    char storage = 'X';

                    error = ntsu::SocketUtil::send(
                        &sendContext,
                        ntsa::Data(ntsa::ConstBuffer(&storage, 1)),
                        sendOptions,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(
                        remoteEndpoint.ip().host().v6().equalsScopeless(
                            serverEndpoint.ip().host().v6()));
                    NTSCFG_TEST_TRUE(remoteEndpoint.ip().port() ==
                                     serverEndpoint.ip().port());
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 8: A datagram socket may connect and reconnect to the
            // same remote endpoint successfully, and may, while already
            // connected, connect and reconnect to a different remote endpoint
            // successfully.

            {
                ntsa::Handle serverOne;
                error = ntsu::SocketUtil::create(
                    &serverOne,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverOneEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        serverOne);
                    NTSCFG_TEST_ASSERT(!error);

                    error =
                        ntsu::SocketUtil::sourceEndpoint(&serverOneEndpoint,
                                                         serverOne);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverOneEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverOneEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(
                        serverOneEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(serverOneEndpoint.ip().port() == 0);
                }

                ntsa::Handle serverTwo;
                error = ntsu::SocketUtil::create(
                    &serverTwo,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverTwoEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        serverTwo);
                    NTSCFG_TEST_ASSERT(!error);

                    error =
                        ntsu::SocketUtil::sourceEndpoint(&serverTwoEndpoint,
                                                         serverTwo);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverTwoEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverTwoEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(
                        serverTwoEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(serverTwoEndpoint.ip().port() == 0);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                for (bsl::size_t i = 0; i < 2; ++i) {
                    // Connect to server one then reconnect to server one.

                    for (bsl::size_t j = 0; j < 2; ++j) {
                        error = ntsu::SocketUtil::connect(serverOneEndpoint,
                                                          socket);
                        NTSCFG_TEST_ASSERT(!error);

                        ntsa::Endpoint sourceEndpoint;
                        error =
                            ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                        NTSCFG_TEST_ASSERT(!error);

                        NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                        NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                        NTSCFG_TEST_FALSE(
                            sourceEndpoint.ip().host().v6().isAny());
                        NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                        ntsa::Endpoint remoteEndpoint;
                        error =
                            ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                        NTSCFG_TEST_ASSERT(!error);

                        NTSCFG_TEST_TRUE(
                            remoteEndpoint.ip().host().v6().equalsScopeless(
                                serverOneEndpoint.ip().host().v6()));
                        NTSCFG_TEST_TRUE(remoteEndpoint.ip().port() ==
                                         serverOneEndpoint.ip().port());
                    }

                    // Connect to server two then reconnect to server two.

                    for (bsl::size_t j = 0; j < 2; ++j) {
                        error = ntsu::SocketUtil::connect(serverTwoEndpoint,
                                                          socket);
                        NTSCFG_TEST_ASSERT(!error);

                        ntsa::Endpoint sourceEndpoint;
                        error =
                            ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                        NTSCFG_TEST_ASSERT(!error);

                        NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                        NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                        NTSCFG_TEST_FALSE(
                            sourceEndpoint.ip().host().v6().isAny());
                        NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                        ntsa::Endpoint remoteEndpoint;
                        error =
                            ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                        NTSCFG_TEST_ASSERT(!error);

                        NTSCFG_TEST_TRUE(
                            remoteEndpoint.ip().host().v6().equalsScopeless(
                                serverTwoEndpoint.ip().host().v6()));
                        NTSCFG_TEST_TRUE(remoteEndpoint.ip().port() ==
                                         serverTwoEndpoint.ip().port());
                    }

                    // Note: attempting to "disconnect" from the remote
                    // endpoint, by connecting to something like AF_UNSPEC,
                    // fails on Linux and Darwin. We assume "disconnecting"
                    // a connected datagram socket is either entirely not
                    // supported or at least very unportable.
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(serverTwo);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(serverOne);
                NTSCFG_TEST_ASSERT(!error);
            }
        }

        // Test Unix domain.

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL &&                             \
    !defined(BSLS_PLATFORM_OS_WINDOWS)
        {
            // Concern 1: The source endpoint of a socket initially after
            // creation is the unnamed local name.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_LOCAL_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    if (error) {
                        BSLS_LOG_ERROR("Failed to get source endpoint: %s",
                                       error.text().c_str());
                    }
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isUnnamed());
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 2: The socket may be bound after creation to an unnamed
            // name, and the socket's resulting source endpoint is *not* the
            // unnamed name, but an explicit name chosen by the operating
            // system (on Linux, in the abstract namespace.) [This
            // functionality is only true on Linux].

#if defined(BSLS_PLATFORM_OS_LINUX)
            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_LOCAL_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint unnamedSourceEndpoint =
                        ntsa::Endpoint(ntsa::LocalName());
                    NTSCFG_TEST_ASSERT(unnamedSourceEndpoint.isLocal());
                    NTSCFG_TEST_ASSERT(
                        unnamedSourceEndpoint.local().isUnnamed());

                    error = ntsu::SocketUtil::bind(unnamedSourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_NE(sourceEndpoint, unnamedSourceEndpoint);
                    NTSCFG_TEST_GT(sourceEndpoint.local().value().size(), 0);
#if defined(BSLS_PLATFORM_OS_LINUX)
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isAbstract());
#endif
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }
#endif

            // Concern 3: The socket may be bound after creation to an explicit
            // name, and the socket's resulting source endpoint is the explicit
            // name.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_LOCAL_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::LocalName localName;
                    error = ntsa::LocalName::generateUnique(&localName);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint explicitSourceEndpoint =
                        ntsa::Endpoint(localName);

                    error = ntsu::SocketUtil::bind(explicitSourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_EQ(sourceEndpoint, explicitSourceEndpoint);
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 4: A datagram socket is implicitly bound to an unnamed
            // source endpoint as a result of calling 'connect', and *may*,
            // on Linux, explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_LOCAL_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    ntsa::LocalName localName;
                    error = ntsa::LocalName::generateUnique(&localName);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint explicitServerEndpoint =
                        ntsa::Endpoint(localName);

                    error = ntsu::SocketUtil::bind(explicitServerEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isLocal());
                    NTSCFG_TEST_EQ(serverEndpoint, explicitServerEndpoint);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_LOCAL_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isUnnamed());
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isUnnamed());

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_EQ(remoteEndpoint, serverEndpoint);
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

#if defined(BSLS_PLATFORM_OS_LINUX)
                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);
#endif
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::unlink(server);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5: A datagram socket is implicitly bound to an
            // *implicit* source endpoint as a result of calling 'sendto'
            // (i.e., the operating system does not generate a name), and
            // *may*, on Linux, explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_LOCAL_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    ntsa::LocalName localName;
                    error = ntsa::LocalName::generateUnique(&localName);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint explicitServerEndpoint =
                        ntsa::Endpoint(localName);

                    error = ntsu::SocketUtil::bind(explicitServerEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isLocal());
                    NTSCFG_TEST_EQ(serverEndpoint, explicitServerEndpoint);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_LOCAL_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isUnnamed());
                }

                {
                    ntsa::SendContext sendContext;
                    ntsa::SendOptions sendOptions;

                    sendOptions.setEndpoint(serverEndpoint);

                    char storage = 'X';

                    error = ntsu::SocketUtil::send(
                        &sendContext,
                        ntsa::Data(ntsa::ConstBuffer(&storage, 1)),
                        sendOptions,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isUnnamed());
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

#if defined(BSLS_PLATFORM_OS_LINUX)
                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);
#endif
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::unlink(server);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }
        }
#endif
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(15)
{
    // Concern: Binding stream sockets.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        const bool k_REUSE_ADDRESS = false;

        // Test IPv4.

        {
            // Concern 1: The source endpoint of a socket initially after
            // creation is "0.0.0.0:0".

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV4_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 2: The socket may be bound after creation to
            // "0.0.0.0:0", and the socket's resulting source endpoint is the
            // host "0.0.0.0" with a non-zero port.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV4_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 3: After the socket is bound to "0.0.0.0:0", the
            // socket's resulting source endpoint is the host "0.0.0.0" with
            // a non-zero port. The socket may *not* be subsequently bound to
            // an IPv4 address that matches one of IPv4 addresses assigned to
            // one of the network interfaces even if the port matches the
            // existing port to which the socket is bound.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV4_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                {
                    ntsa::Endpoint sourceEndpoint1;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint1,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint1.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().port() != 0);

                    ntsa::Endpoint sourceEndpoint2;
                    {
                        ntsa::Adapter adapter;
                        bool found = ntsu::AdapterUtil::discoverAdapter(
                            &adapter,
                            ntsa::IpAddressType::e_V4,
                            false);
                        NTSCFG_TEST_TRUE(found);
                        NTSCFG_TEST_FALSE(adapter.ipv4Address().isNull());

                        sourceEndpoint2 =
                            ntsa::Endpoint(adapter.ipv4Address().value(),
                                           sourceEndpoint1.ip().port());
                    }

                    error = ntsu::SocketUtil::bind(sourceEndpoint2,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 4: The socket may be bound after creation to an IPv4
            // address that matches one of IPv4 addresses assigned to one of
            // the network interfaces and port zero, and the resulting socket's
            // source endpoint is that IPv4 address with a port chosen by the
            // operating system.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV4_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint explicitSourceEndpoint;
                    {
                        ntsa::Adapter adapter;
                        bool found = ntsu::AdapterUtil::discoverAdapter(
                            &adapter,
                            ntsa::IpAddressType::e_V4,
                            false);
                        NTSCFG_TEST_TRUE(found);
                        NTSCFG_TEST_FALSE(adapter.ipv4Address().isNull());

                        explicitSourceEndpoint =
                            ntsa::Endpoint(adapter.ipv4Address().value(), 0);
                    }

                    error = ntsu::SocketUtil::bind(explicitSourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_EQ(sourceEndpoint.ip().host(),
                                   explicitSourceEndpoint.ip().host());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5: A stream socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'connect', and may not
            // explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_TCP_IPV4_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);

                    error = ntsu::SocketUtil::listen(100, server);
                    NTSCFG_TEST_ASSERT(!error);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_EQ(remoteEndpoint, serverEndpoint);
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5a: A stream socket may be explicitly bound to "any"
            // address and "any" port, which results in the source endpoint
            // being the "any" address and a non-zero port. The socket may then
            // be implicitly rebound by the operating system as a result of
            // calling 'connect', so that the resulting source endpoint is now
            // an explicit IP address but still having the same port (i.e., the
            // address changes from "any" to a specific IP address assigned to
            // a network interface, but the port does not change), but the
            // stream socket may not explicitly rebound afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_TCP_IPV4_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);

                    error = ntsu::SocketUtil::listen(100, server);
                    NTSCFG_TEST_ASSERT(!error);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV4_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv4Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_EQ(remoteEndpoint, serverEndpoint);
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 6: A stream socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'listen', and may not
            // explicitly rebind afterwards. [On Windows this is an error.]

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV4_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::listen(100, socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV4());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v4().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }
        }

        // Test IPv6.

        if (ntsu::AdapterUtil::supportsIpv6()) {
            // Concern 1: The source endpoint of a socket initially after
            // creation is "[:0]:0".

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV6_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 2: The socket may be bound after creation to
            // "[:0]:0", and the socket's resulting source endpoint is the
            // host ":0" with a non-zero port.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV6_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 3: After the socket is bound to "[:0]:0", the
            // socket's resulting source endpoint is the host ":0" with
            // a non-zero port. The socket may *not* be subsequently bound to
            // an IPv6 address that matches one of IPv6 addresses assigned to
            // one of the network interfaces even if the port matches the
            // existing port to which the socket is bound.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV6_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                {
                    ntsa::Endpoint sourceEndpoint1;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint1,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint1.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint1.ip().port() != 0);

                    ntsa::Endpoint sourceEndpoint2;
                    {
                        ntsa::Adapter adapter;
                        bool found = ntsu::AdapterUtil::discoverAdapter(
                            &adapter,
                            ntsa::IpAddressType::e_V6,
                            false);
                        NTSCFG_TEST_TRUE(found);
                        NTSCFG_TEST_FALSE(adapter.ipv6Address().isNull());

                        sourceEndpoint2 =
                            ntsa::Endpoint(adapter.ipv6Address().value(),
                                           sourceEndpoint1.ip().port());
                    }

                    error = ntsu::SocketUtil::bind(sourceEndpoint2,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 4: The socket may be bound after creation to an IPv6
            // address that matches one of IPv6 addresses assigned to one of
            // the network interfaces and port zero, and the resulting socket's
            // source endpoint is that IPv6 address with a port chosen by the
            // operating system.

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV6_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint explicitSourceEndpoint;
                    {
                        ntsa::Adapter adapter;
                        bool found = ntsu::AdapterUtil::discoverAdapter(
                            &adapter,
                            ntsa::IpAddressType::e_V6,
                            false);
                        NTSCFG_TEST_TRUE(found);
                        NTSCFG_TEST_FALSE(adapter.ipv6Address().isNull());

                        explicitSourceEndpoint =
                            ntsa::Endpoint(adapter.ipv6Address().value(), 0);
                    }

                    error = ntsu::SocketUtil::bind(explicitSourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_EQ(sourceEndpoint.ip().host(),
                                   explicitSourceEndpoint.ip().host());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5: A stream socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'connect', and may not
            // explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_TCP_IPV6_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);

                    error = ntsu::SocketUtil::listen(100, server);
                    NTSCFG_TEST_ASSERT(!error);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(remoteEndpoint.isIp());
                    NTSCFG_TEST_TRUE(remoteEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(
                        remoteEndpoint.ip().host().v6().equalsScopeless(
                            serverEndpoint.ip().host().v6()));
                    NTSCFG_TEST_TRUE(remoteEndpoint.ip().port() ==
                                     serverEndpoint.ip().port());
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5a: A stream socket may be explicitly bound to "any"
            // address and "any" port, which results in the source endpoint
            // being the "any" address and a non-zero port. The socket may then
            // be implicitly rebound by the operating system as a result of
            // calling 'connect', so that the resulting source endpoint is now
            // an explicit IP address but still having the same port (i.e., the
            // address changes from "any" to a specific IP address assigned to
            // a network interface, but the port does not change), but the
            // stream socket may not explicitly rebound afterwards.

            {
                ntsa::Handle server;
                error = ntsu::SocketUtil::create(
                    &server,
                    ntsa::Transport::e_TCP_IPV6_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::loopback(), 0),
                        k_REUSE_ADDRESS,
                        server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(serverEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(serverEndpoint.ip().port() == 0);

                    error = ntsu::SocketUtil::listen(100, server);
                    NTSCFG_TEST_ASSERT(!error);
                }

                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV6_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::bind(
                        ntsa::Endpoint(ntsa::Ipv6Address::any(), 0),
                        k_REUSE_ADDRESS,
                        socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() != 0);
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(remoteEndpoint.isIp());
                    NTSCFG_TEST_TRUE(remoteEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(serverEndpoint.isIp());
                    NTSCFG_TEST_TRUE(serverEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(
                        remoteEndpoint.ip().host().v6().equalsScopeless(
                            serverEndpoint.ip().host().v6()));
                    NTSCFG_TEST_TRUE(remoteEndpoint.ip().port() ==
                                     serverEndpoint.ip().port());
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 6: A stream socket is implicitly bound to an explicit
            // source endpoint as a result of calling 'listen', and may not
            // explicitly rebind afterwards. [On Windows this is an error.]

            {
                ntsa::Handle socket;
                error = ntsu::SocketUtil::create(
                    &socket,
                    ntsa::Transport::e_TCP_IPV6_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    error = ntsu::SocketUtil::listen(100, socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isIp());
                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().isV6());

                    NTSCFG_TEST_TRUE(sourceEndpoint.ip().host().v6().isAny());
                    NTSCFG_TEST_FALSE(sourceEndpoint.ip().port() == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(error);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    NTSCFG_TEST_ASSERT(error);
#endif
                }

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }
        }

        // Test Unix domain.

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
        {
            // Concern 1: The source endpoint of a socket initially after
            // creation is the unnamed local name, except on Windows, which
            // specifically describes its non-standard behavior where a
            // Unix domain socket is not implicitly assigned the unnamed
            // address.

            {
                ntsa::Handle socket;
                error =
                    ntsu::SocketUtil::create(&socket,
                                             ntsa::Transport::e_LOCAL_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);
                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isUnnamed());
#else
                    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
                    NTSCFG_TEST_TRUE(sourceEndpoint.isUndefined());
#endif
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 2: The socket may be bound after creation to an unnamed
            // name, and the socket's resulting source endpoint is *not* the
            // unnamed name, but an explicit name chosen by the operating
            // system (on Linux, in the abstract namespace.) [This
            // functionality is only available on Linux.]

#if defined(BSLS_PLATFORM_OS_LINUX)
            {
                ntsa::Handle socket;
                error =
                    ntsu::SocketUtil::create(&socket,
                                             ntsa::Transport::e_LOCAL_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint unnamedSourceEndpoint =
                        ntsa::Endpoint(ntsa::LocalName());
                    NTSCFG_TEST_ASSERT(unnamedSourceEndpoint.isLocal());
                    NTSCFG_TEST_ASSERT(
                        unnamedSourceEndpoint.local().isUnnamed());

                    error = ntsu::SocketUtil::bind(unnamedSourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_NE(sourceEndpoint, unnamedSourceEndpoint);
                    NTSCFG_TEST_GT(sourceEndpoint.local().value().size(), 0);
#if defined(BSLS_PLATFORM_OS_LINUX)
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isAbstract());
#endif
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }
#endif

            // Concern 3: The socket may be bound after creation to an explicit
            // name, and the socket's resulting source endpoint is the explicit
            // name.

            {
                ntsa::Handle socket;
                error =
                    ntsu::SocketUtil::create(&socket,
                                             ntsa::Transport::e_LOCAL_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::LocalName localName;
                    error = ntsa::LocalName::generateUnique(&localName);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint explicitSourceEndpoint =
                        ntsa::Endpoint(localName);

                    error = ntsu::SocketUtil::bind(explicitSourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_EQ(sourceEndpoint, explicitSourceEndpoint);
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 4: A stream socket is implicitly bound to an unnamed
            // source endpoint as a result of calling 'connect', and *may*, on
            // Linux, explicitly rebind afterwards.

            {
                ntsa::Handle server;
                error =
                    ntsu::SocketUtil::create(&server,
                                             ntsa::Transport::e_LOCAL_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                ntsa::Endpoint serverEndpoint;

                {
                    ntsa::LocalName localName;
                    error = ntsa::LocalName::generateUnique(&localName);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint explicitServerEndpoint =
                        ntsa::Endpoint(localName);

                    error = ntsu::SocketUtil::bind(explicitServerEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   server);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint,
                                                             server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(serverEndpoint.isLocal());
                    NTSCFG_TEST_EQ(serverEndpoint, explicitServerEndpoint);

                    error = ntsu::SocketUtil::listen(100, server);
                    NTSCFG_TEST_ASSERT(!error);
                }

                ntsa::Handle socket;
                error =
                    ntsu::SocketUtil::create(&socket,
                                             ntsa::Transport::e_LOCAL_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);

#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);
                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isUnnamed());
#else
                    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
                    NTSCFG_TEST_TRUE(sourceEndpoint.isUndefined());
#endif
                }

                {
                    error = ntsu::SocketUtil::connect(serverEndpoint, socket);
                    NTSCFG_TEST_ASSERT(!error);

                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isUnnamed());

                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_EQ(remoteEndpoint, serverEndpoint);
                }

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);
                    NTSCFG_TEST_ASSERT(!error);

#if defined(BSLS_PLATFORM_OS_LINUX)
                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);

                    error = ntsu::SocketUtil::bind(sourceEndpoint,
                                                   k_REUSE_ADDRESS,
                                                   socket);
                    NTSCFG_TEST_ASSERT(!error);
#endif
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::unlink(server);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(server);
                NTSCFG_TEST_ASSERT(!error);
            }

            // Concern 5: A stream socket is implicitly bound to an unnamed
            // source endpoint as a result of calling 'listen', and *may*
            // explicitly rebind afterwards. [This functionality is only
            // available on Linux, on all other platforms a stream socket
            // must be explicitly bound before calling 'listen'.)

            {
                ntsa::Handle socket;
                error =
                    ntsu::SocketUtil::create(&socket,
                                             ntsa::Transport::e_LOCAL_STREAM);
                NTSCFG_TEST_ASSERT(!error);

                {
                    ntsa::Endpoint sourceEndpoint;
                    error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint,
                                                             socket);

#if defined(BSLS_PLATFORM_OS_UNIX)
                    NTSCFG_TEST_ASSERT(!error);
                    NTSCFG_TEST_TRUE(sourceEndpoint.isLocal());
                    NTSCFG_TEST_TRUE(sourceEndpoint.local().isUnnamed());
#else
                    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
                    NTSCFG_TEST_TRUE(sourceEndpoint.isUndefined());
#endif
                }

                {
                    error = ntsu::SocketUtil::listen(100, socket);
                    NTSCFG_TEST_ASSERT(error);
                }

                error = ntsu::SocketUtil::unlink(socket);
                NTSCFG_TEST_ASSERT(!error);

                error = ntsu::SocketUtil::close(socket);
                NTSCFG_TEST_ASSERT(!error);
            }
        }
#endif
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(16)
{
    // Concern: Listing system sockets.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntsa::Handle listener;
        error = ntsu::SocketUtil::create(&listener,
                                         ntsa::Transport::e_TCP_IPV4_STREAM);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
            true,
            listener);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint listenerEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::listen(1, listener);
        NTSCFG_TEST_ASSERT(!error);

        BSLS_LOG_INFO("Listening at %s", listenerEndpoint.text().c_str());

        {
            ntsa::SocketInfoFilter filter;
            filter.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
            filter.setAll(false);

            bsl::stringstream ss;
            ntsu::SocketUtil::reportInfo(ss, filter);

            BSLS_LOG_INFO("Dump status:\n%s", ss.str().c_str());
        }

        error = ntsu::SocketUtil::close(listener);
        NTSCFG_TEST_ASSERT(!error);

        {
            ntsa::SocketInfoFilter filter;
            filter.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
            filter.setAll(false);

            bsl::stringstream ss;
            ntsu::SocketUtil::reportInfo(ss, filter);

            BSLS_LOG_INFO("Dump status:\n%s", ss.str().c_str());
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(17)
{
    // Concern: Create stream socket pairs.
    //
    // Plan:

    ntsa::Error error;

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV4_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV4_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV6_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV6_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(ntsa::Transport::e_LOCAL_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_STREAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        // Create a blocking socket pair.

        ntsa::Handle client;
        ntsa::Handle server;
        error = ntsu::SocketUtil::pair(&client, &server, transport);
        NTSCFG_TEST_ASSERT(!error);

        // Get the client source and remote endpoints.

        ntsa::Endpoint clientSourceEndpoint;
        error =
            ntsu::SocketUtil::sourceEndpoint(&clientSourceEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint clientRemoteEndpoint;
        error =
            ntsu::SocketUtil::remoteEndpoint(&clientRemoteEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        // Get the server source and remote endpoints.

        ntsa::Endpoint serverSourceEndpoint;
        error =
            ntsu::SocketUtil::sourceEndpoint(&serverSourceEndpoint, server);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint serverRemoteEndpoint;
        error =
            ntsu::SocketUtil::remoteEndpoint(&serverRemoteEndpoint, server);
        NTSCFG_TEST_ASSERT(!error);

        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Client at " << clientSourceEndpoint << " to "
                      << clientRemoteEndpoint << bsl::endl;

            bsl::cout << "Server at " << serverSourceEndpoint << " to "
                      << serverRemoteEndpoint << bsl::endl;
        }

        // Enqueue outgoing data to transmit by the client socket.

        {
            char              buffer = 'C';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the server socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'C');
        }

        // Enqueue outgoing data to transmit by the server socket.

        {
            char              buffer = 'S';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the client socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'S');
        }

        // Validate RX timestamping functionality.

        {
            error =
                ntsu::SocketOptionUtil::setTimestampIncomingData(client, true);
#if defined(BSLS_PLATFORM_OS_LINUX)
            NTSCFG_TEST_OK(error);
            // sleep for 100 ms to let the kernel apply changes
            bslmt::ThreadUtil::microSleep(100000, 0);
#else
            NTSCFG_TEST_ERROR(error, ntsa::Error::e_NOT_IMPLEMENTED);
#endif

            // Enqueue outgoing data to transmit by the client socket.

            bsls::TimeInterval sysTimeBeforeSending;
            {
                char              buffer = 'S';
                ntsa::SendContext context;
                ntsa::SendOptions options;

                ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

                sysTimeBeforeSending = bdlt::CurrentTime::now();

                error =
                    ntsu::SocketUtil::send(&context, data, options, server);
                NTSCFG_TEST_ASSERT(!error);

                NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
                NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
            }

            // Dequeue incoming data received by the server socket.

            {
                char                 buffer;
                ntsa::ReceiveContext context;
                ntsa::ReceiveOptions options;
                options.showTimestamp();

                ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

                error = ntsu::SocketUtil::receive(&context,
                                                  &data,
                                                  options,
                                                  client);
                NTSCFG_TEST_ASSERT(!error);

                NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
                NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
                NTSCFG_TEST_ASSERT(buffer == 'S');
#if defined(BSLS_PLATFORM__OS_LINUX)
                int major, minor, patch, build;
                NTSCFG_TEST_ASSERT(ntsscm::Version::systemVersion(&major,
                                                                  &minor,
                                                                  &patch,
                                                                  &build) ==
                                   0);

                // Linux kernels versions <= 3.10.0 have restricted
                // timestamping support.

                if (KERNEL_VERSION(major, minor, patch) <=
                    KERNEL_VERSION(3, 10, 0))
                {
                    NTSCFG_TEST_TRUE(context.softwareTimestamp().isNull());
                    NTSCFG_TEST_TRUE(context.hardwareTimestamp().isNull());
                }
                else {
                    // On a modern kernel timestamps will be present for
                    // AF_INET stream sockets only.

                    if (transport == ntsa::Transport::e_LOCAL_STREAM) {
                        NTSCFG_TEST_TRUE(context.softwareTimestamp().isNull());
                        NTSCFG_TEST_TRUE(context.hardwareTimestamp().isNull());
                    }
                    else {
                        BSLS_LOG_DEBUG("Detected RX timestamp");
                        NTSCFG_TEST_TRUE(
                            context.softwareTimestamp().has_value());
                        NTSCFG_TEST_LE(sysTimeBeforeSending,
                                       context.softwareTimestamp().value());

                        // We cannot make any assertion about hardware
                        // timestamp availability as it strictly OS+NIC
                        // dependent.
                    }
                }
#else
                // Ensure that no timestamp was generated for other platforms
                // except Linux.

                NTSCFG_TEST_TRUE(context.softwareTimestamp().isNull());
                NTSCFG_TEST_TRUE(context.hardwareTimestamp().isNull());
#endif
            }

            // Now switch off the option and check that requested timestamp is
            // not available.

            {
                error =
                    ntsu::SocketOptionUtil::setTimestampIncomingData(client,
                                                                     false);
#if defined(BSLS_PLATFORM_OS_LINUX)
                NTSCFG_TEST_OK(error);
#else
                NTSCFG_TEST_ERROR(error, ntsa::Error::e_NOT_IMPLEMENTED);
#endif

                // Enqueue outgoing data to transmit by the client socket.

                {
                    char              buffer = 'S';
                    ntsa::SendContext context;
                    ntsa::SendOptions options;

                    ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

                    sysTimeBeforeSending = bdlt::CurrentTime::now();

                    error = ntsu::SocketUtil::send(&context,
                                                   data,
                                                   options,
                                                   server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
                    NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
                }

                // Dequeue incoming data received by the server socket.

                {
                    char                 buffer;
                    ntsa::ReceiveContext context;
                    ntsa::ReceiveOptions options;
                    options.showTimestamp();

                    ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

                    error = ntsu::SocketUtil::receive(&context,
                                                      &data,
                                                      options,
                                                      client);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
                    NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
                    NTSCFG_TEST_ASSERT(buffer == 'S');
                    NTSCFG_TEST_TRUE(context.softwareTimestamp().isNull());
                    NTSCFG_TEST_TRUE(context.hardwareTimestamp().isNull());
                }
            }
        }

        // Shutdown writing by the client socket.

        error = ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, client);
        NTSCFG_TEST_ASSERT(!error);

        // Dequeue incoming data received by the server socket, and observe
        // that zero bytes are successfully dequeued, indicating the client
        // socket has shut down writing from its side of the connection.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 0);
        }

        // Shutdown writing by the server socket.

        error = ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, server);
        NTSCFG_TEST_ASSERT(!error);

        // Dequeue incoming data received by the client socket, and observe
        // that zero bytes are successfully dequeued, indicating the server
        // socket has shut down writing from its side of the connection.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 0);
        }

        // Close each socket.

        error = ntsu::SocketUtil::close(client);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(server);
        NTSCFG_TEST_ASSERT(!error);
    }
}

NTSCFG_TEST_CASE(18)
{
    // Concern: Create datagram socket pairs.
    //
    // Plan:

    ntsa::Error error;

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV4_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV6_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_LOCAL_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_DATAGRAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        // Create a socket pair.

        ntsa::Handle client;
        ntsa::Handle server;
        error = ntsu::SocketUtil::pair(&client, &server, transport);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint clientEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&clientEndpoint, client);
        NTSCFG_TEST_ASSERT(!error);

        ntsa::Endpoint serverEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint, server);
        NTSCFG_TEST_ASSERT(!error);

        // Enqueue outgoing data to transmit by the client socket.

        {
            char              buffer = 'C';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the server socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;
            options.showEndpoint();

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(!context.endpoint().isNull());

            if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
                NTSCFG_TEST_ASSERT(context.endpoint().value() ==
                                   clientEndpoint);
            }
            else if (transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
                NTSCFG_TEST_TRUE(
                    context.endpoint()
                        .value()
                        .ip()
                        .host()
                        .v6()
                        .equalsScopeless(clientEndpoint.ip().host().v6()));
                NTSCFG_TEST_TRUE(context.endpoint().value().ip().port() ==
                                 clientEndpoint.ip().port());
            }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
            else if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
                // Local datagram sockets created with POSIX 'socketpair'
                // are connected but are unnamed on some platforms. Those
                // platforms report the sender address as "undefined".
                NTSCFG_TEST_TRUE(
                    (clientEndpoint.isImplicit() &&
                     context.endpoint().value().isImplicit()) ||
                    (context.endpoint().value() == clientEndpoint));
            }
#endif
            else {
                NTSCFG_TEST_TRUE(false);
            }

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'C');
        }

        // Enqueue outgoing data to transmit by the server socket.

        {
            char              buffer = 'C';
            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = ntsu::SocketUtil::send(&context, data, options, server);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
        }

        // Dequeue incoming data received by the client socket.

        {
            char                 buffer;
            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;
            options.showEndpoint();

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error =
                ntsu::SocketUtil::receive(&context, &data, options, client);
            NTSCFG_TEST_ASSERT(!error);

            NTSCFG_TEST_ASSERT(!context.endpoint().isNull());

            if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
                NTSCFG_TEST_ASSERT(context.endpoint().value() ==
                                   serverEndpoint);
            }
            else if (transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
                NTSCFG_TEST_TRUE(
                    context.endpoint()
                        .value()
                        .ip()
                        .host()
                        .v6()
                        .equalsScopeless(serverEndpoint.ip().host().v6()));
                NTSCFG_TEST_TRUE(context.endpoint().value().ip().port() ==
                                 serverEndpoint.ip().port());
            }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
            else if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
                // Local datagram sockets created with POSIX 'socketpair'
                // are connected but are unnamed on some platforms. Those
                // platforms report the sender address as "undefined".
                NTSCFG_TEST_TRUE(
                    (serverEndpoint.isImplicit() &&
                     context.endpoint().value().isImplicit()) ||
                    (context.endpoint().value() == serverEndpoint));
            }
#endif
            else {
                NTSCFG_TEST_TRUE(false);
            }

            NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
            NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
            NTSCFG_TEST_ASSERT(buffer == 'C');
        }

        // Test RX timestamping functionality.

        {
            error =
                ntsu::SocketOptionUtil::setTimestampIncomingData(server, true);
#if defined(BSLS_PLATFORM_OS_LINUX)
            NTSCFG_TEST_OK(error);
            // sleep for 100 ms to let the kernel apply changes
            bslmt::ThreadUtil::microSleep(100000, 0);
#else
            NTSCFG_TEST_ERROR(error, ntsa::Error::e_NOT_IMPLEMENTED);
#endif

            // Enqueue outgoing data to transmit by the client socket.

            bsls::TimeInterval sysTimeBeforeSending;
            {
                char              buffer = 'C';
                ntsa::SendContext context;
                ntsa::SendOptions options;

                ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

                sysTimeBeforeSending = bdlt::CurrentTime::now();

                error =
                    ntsu::SocketUtil::send(&context, data, options, client);
                NTSCFG_TEST_ASSERT(!error);

                NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
                NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
            }

            // Dequeue incoming data received by the server socket.

            {
                char                 buffer;
                ntsa::ReceiveContext context;
                ntsa::ReceiveOptions options;
                options.showTimestamp();

                ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

                error = ntsu::SocketUtil::receive(&context,
                                                  &data,
                                                  options,
                                                  server);
                NTSCFG_TEST_ASSERT(!error);

                NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
                NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
                NTSCFG_TEST_ASSERT(buffer == 'C');

#if defined(BSLS_PLATFORM_OS_LINUX)
                int major, minor, patch, build;
                NTSCFG_TEST_ASSERT(ntsscm::Version::systemVersion(&major,
                                                                  &minor,
                                                                  &patch,
                                                                  &build) ==
                                   0);

                // Linux kernels versions <= 2.6.32 have restricted
                // timestamping support.

                if (KERNEL_VERSION(major, minor, patch) >
                    KERNEL_VERSION(2, 6, 32))
                {
                    NTSCFG_TEST_FALSE(context.softwareTimestamp().isNull());
                    NTSCFG_TEST_LE(sysTimeBeforeSending,
                                   context.softwareTimestamp().value());
                    if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
                        NTSCFG_TEST_TRUE(context.hardwareTimestamp().isNull());
                    }
                }
                else {
                    if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
                        NTSCFG_TEST_TRUE(context.softwareTimestamp().isNull());
                        NTSCFG_TEST_TRUE(context.softwareTimestamp().isNull());
                    }
                    else {
                        BSLS_LOG_DEBUG("Detected RX timestamp");
                        NTSCFG_TEST_FALSE(
                            context.softwareTimestamp().isNull());
                        NTSCFG_TEST_LE(sysTimeBeforeSending,
                                       context.softwareTimestamp().value());
                    }
                }
#else
                NTSCFG_TEST_TRUE(context.softwareTimestamp().isNull());
                NTSCFG_TEST_TRUE(context.hardwareTimestamp().isNull());
#endif
            }

            // Now switch off the option and check that it is impossible to get
            // a timestamp.

            {
                error =
                    ntsu::SocketOptionUtil::setTimestampIncomingData(server,
                                                                     false);
#if defined(BSLS_PLATFORM_OS_LINUX)
                NTSCFG_TEST_OK(error);
#else
                NTSCFG_TEST_ERROR(error, ntsa::Error::e_NOT_IMPLEMENTED);
#endif

                // Enqueue outgoing data to transmit by the client socket.

                {
                    char              buffer = 'C';
                    ntsa::SendContext context;
                    ntsa::SendOptions options;

                    ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

                    sysTimeBeforeSending = bdlt::CurrentTime::now();

                    error = ntsu::SocketUtil::send(&context,
                                                   data,
                                                   options,
                                                   client);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
                    NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
                }

                // Dequeue incoming data received by the server socket.

                {
                    char                 buffer;
                    ntsa::ReceiveContext context;
                    ntsa::ReceiveOptions options;
                    options.showTimestamp();

                    ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

                    error = ntsu::SocketUtil::receive(&context,
                                                      &data,
                                                      options,
                                                      server);
                    NTSCFG_TEST_ASSERT(!error);

                    NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
                    NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
                    NTSCFG_TEST_ASSERT(buffer == 'C');

                    NTSCFG_TEST_TRUE(context.softwareTimestamp().isNull());
                    NTSCFG_TEST_TRUE(context.hardwareTimestamp().isNull());
                }
            }
        }

        // Close each socket.

        error = ntsu::SocketUtil::close(client);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::close(server);
        NTSCFG_TEST_ASSERT(!error);
    }
}

NTSCFG_TEST_CASE(19)
{
    // Concern: Datagram socket transmission with control data: single buffer.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(
            &test::testDatagramSocketTransmissionSingleBufferWithControlMsg);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(20)
{
    // Concern: Datagram socket transmission with control data: blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(
            &test::testDatagramSocketTransmissionBlobWithControlMsg);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(21)
{
    // Concern: Datagram socket transmission with control data: dropped.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(
            &test::testDatagramSocketTransmissionWithControlMsgDropped);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(22)
{
    // Concern: Stream socket transmission with control data: single buffer.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(
            &test::testStreamSocketTransmissionSingleBufferWithControlMsg);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(23)
{
    // Concern: Stream socket transmission with control data: blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(
            &test::testStreamSocketTransmissionBlobWithControlMsg);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(24)
{
    // Concern: Stream socket transmission with control data: dropped
    // Plan:

    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(
            &test::testStreamSocketTransmissionWithControlMsgDropped);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(25)
{
    // Concern: validate that an incoming software timestamp and a file handle
    // can be simultaneously retrieved from one control message

    if (!ntscfg::Platform::supportsTimestamps()) {
        NTSCFG_TEST_LOG_DEBUG
            << "Platform does not support timestamps, ignore the TC"
            << NTSCFG_TEST_LOG_END;
        return;
    }
    const ntsa::Transport::Value transport = ntsa::Transport::e_LOCAL_DATAGRAM;
    if (!ntsu::AdapterUtil::supportsTransport(transport)) {
        NTSCFG_TEST_LOG_DEBUG << transport
                              << " is not supported, ignore the TC"
                              << NTSCFG_TEST_LOG_END;
        return;
    }

    NTSCFG_TEST_LOG_DEBUG << "Starting the test" << NTSCFG_TEST_LOG_END;

    ntsa::Error error;

    // Create a socket pair.

    ntsa::Handle client;
    ntsa::Handle server;
    {
        error = ntsu::SocketUtil::pair(&client, &server, transport);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketOptionUtil::setTimestampIncomingData(server, true);
        NTSCFG_TEST_OK(error);
    }

    // create file handle to be transferred
    ntsa::Handle   domesticSocket;
    ntsa::Endpoint domesticSourceEndpoint;
    {
        error = ntsu::SocketUtil::create(&domesticSocket, transport);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::LocalName::generateUnique()),
            false,
            domesticSocket);
        NTSCFG_TEST_ASSERT(!error);

        error = ntsu::SocketUtil::sourceEndpoint(&domesticSourceEndpoint,
                                                 domesticSocket);
    }

    const bsls::TimeInterval sysTimeBeforeSending = bdlt::CurrentTime::now();
    // enqueue data transmitted by the client
    {
        char              buffer = 'C';
        ntsa::SendContext context;
        ntsa::SendOptions options;
        options.setForeignHandle(domesticSocket);

        ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

        error = ntsu::SocketUtil::send(&context, data, options, client);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesSendable() == 1);
        NTSCFG_TEST_ASSERT(context.bytesSent() == 1);
    }

    // Dequeue incoming data received by the server socket.

    {
        char                 buffer;
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;
        options.showTimestamp();
        options.showForeignHandles();

        ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

        error = ntsu::SocketUtil::receive(&context, &data, options, server);
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_ASSERT(context.bytesReceivable() == 1);
        NTSCFG_TEST_ASSERT(context.bytesReceived() == 1);
        NTSCFG_TEST_ASSERT(buffer == 'C');

        NTSCFG_TEST_FALSE(context.softwareTimestamp().isNull());
        NTSCFG_TEST_GT(context.softwareTimestamp().value(),
                       sysTimeBeforeSending);

        NTSCFG_TEST_TRUE(!context.foreignHandle().isNull());

        ntsa::Endpoint foreignSourceEndpoint;
        error =
            ntsu::SocketUtil::sourceEndpoint(&foreignSourceEndpoint,
                                             context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);

        NTSCFG_TEST_EQ(foreignSourceEndpoint, domesticSourceEndpoint);

        error = ntsu::SocketUtil::close(context.foreignHandle().value());
        NTSCFG_TEST_ASSERT(!error);
    }

    // Close each socket.

    error = ntsu::SocketUtil::close(client);
    NTSCFG_TEST_ASSERT(!error);

    error = ntsu::SocketUtil::close(server);
    NTSCFG_TEST_ASSERT(!error);

    error = ntsu::SocketUtil::close(domesticSocket);
    NTSCFG_TEST_ASSERT(!error);
}

NTSCFG_TEST_CASE(26)
{
    // Concern: ntsu::SocketUtil::isSocket correctly indicates true if a
    // file descriptor is an alias for a socket, and false, after the file
    // descriptor is closed.

    ntsa::Error error;

    {
        bool result = ntsu::SocketUtil::isSocket(ntsa::k_INVALID_HANDLE);
        NTSCFG_TEST_FALSE(result);
    }

    {
        bool result = ntsu::SocketUtil::isSocket(12345);
        NTSCFG_TEST_FALSE(result);
    }

    {
        ntsa::Handle socket = ntsa::k_INVALID_HANDLE;
        error               = ntsu::SocketUtil::create(&socket,
                                         ntsa::Transport::e_TCP_IPV4_STREAM);
        NTSCFG_TEST_OK(error);

        bool result1 = ntsu::SocketUtil::isSocket(socket);
        NTSCFG_TEST_TRUE(result1);

        error = ntsu::SocketUtil::close(socket);
        NTSCFG_TEST_OK(error);

        bool result2 = ntsu::SocketUtil::isSocket(socket);
        NTSCFG_TEST_FALSE(result2);
    }
}

NTSCFG_TEST_CASE(27)
{
    // Concern: Test that Linux MSG_ZEROCOPY mechanism is applied for DATAGRAM
    // sockets

    // Note that on that level we cannot really validate whether data is
    // actually copied into the send buffer or not. We can only validate that
    // if data is sent with MSG_ZEROCOPY flag then related notifications will
    // appear on a socket error queue.
    //
    // By default, the test sends data to a loopback address using loopback
    // device. Though, to see how the system behaves when another device is
    // used it is possible to use some random (but reachable) IPv4/6 addresses.
    // See related code section below.

#if defined(BSLS_PLATFORM_OS_LINUX)
    // Linux kernels versions < 5.0.0 do not support MSG_ZEROCOPY for DGRAM
    // sockets
    {
        int major, minor, patch, build;
        NTSCFG_TEST_ASSERT(
            ntsscm::Version::systemVersion(&major, &minor, &patch, &build) ==
            0);

        if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(5, 0, 0)) {
            return;
        }
    }

    bsl::vector<ntsa::Transport::Value> socketTypes;
    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV4_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    }
    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV6_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    }

    for (bsl::vector<ntsa::Transport::Value>::const_iterator transport =
             socketTypes.cbegin();
         transport != socketTypes.cend();
         ++transport)
    {
        NTSCFG_TEST_LOG_DEBUG << "Testing " << *transport
                              << NTSCFG_TEST_LOG_END;

        ntscfg::TestAllocator ta;
        {
            // Observation: if system MTU is 1500 bytes then maximum payload
            // size of UDP IPV4 packet for which MSG_ZEROCOPY functionality can
            // really work is 1472 bytes (because UDP header is 8 bytes and
            // IPV4 header is 20 bytes).

            const int msgSize           = 1472;
            const int numMessagesToSend = 200;

            ntsa::Error  error;
            ntsa::Handle handle = ntsa::k_INVALID_HANDLE;

            error = ntsu::SocketUtil::create(&handle, *transport);
            NTSCFG_TEST_ASSERT(!error);

            error = ntsu::SocketOptionUtil::setAllowMsgZeroCopy(handle, true);
            NTSCFG_TEST_OK(error);

            bsl::vector<char> message(msgSize, &ta);
            for (int i = 0; i < msgSize; ++i) {
                message[i] = bsl::rand() % 100;
            }
            const ntsa::Data data(
                ntsa::ConstBuffer(message.data(), message.size()));

            ntsa::Endpoint endpoint;
            if (*transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
                NTSCFG_TEST_TRUE(endpoint.parse("127.0.0.1:5555"));
                // NTSCFG_TEST_TRUE(endpoint.parse("108.22.44.23:5555"));
            }
            else if (*transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
                NTSCFG_TEST_TRUE(endpoint.parse("[::1]:5555"));
                // NTSCFG_TEST_TRUE(endpoint.parse("[fe80::215:5dff:fe8d:6bd1]:5555"));
            }

            bsl::list<ntsa::ZeroCopy>         feedback(&ta);
            bsl::unordered_set<bsl::uint32_t> sendIDs(&ta);

            for (int i = 0; i < numMessagesToSend; ++i) {
                ntsa::SendContext context;
                ntsa::SendOptions options;
                options.setEndpoint(endpoint);
                options.setZeroCopy(true);

                error =
                    ntsu::SocketUtil::send(&context, data, options, handle);
                if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK) ||
                    error == ntsa::Error(ntsa::Error::e_LIMIT))
                {
                    --i;
                    continue;
                }
                NTSCFG_TEST_OK(error);
                sendIDs.insert(i);

                NTSCFG_TEST_ASSERT(context.bytesSendable() == msgSize);
                NTSCFG_TEST_ASSERT(context.bytesSent() == msgSize);

                test::extractZeroCopyNotifications(&feedback, handle, &ta);
            }

            // retrieve data from the socket error queue until all send system
            // calls are acknowledged by the OS
            while (!sendIDs.empty()) {
                test::extractZeroCopyNotifications(&feedback, handle, &ta);

                while (!feedback.empty()) {
                    const ntsa::ZeroCopy& zc = feedback.front();
                    if (zc.from() == zc.to()) {
                        NTSCFG_TEST_EQ(sendIDs.erase(zc.from()), 1);
                    }
                    else {
                        for (bsl::uint32_t i = zc.from(); i != (zc.to() + 1);
                             ++i)
                        {
                            NTSCFG_TEST_EQ(sendIDs.erase(i), 1);
                        }
                    }
                    feedback.pop_front();
                }
            }
        }
        NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
    }
#endif
}

NTSCFG_TEST_CASE(28)
{
    // Concern: Test that Linux MSG_ZEROCOPY mechanism is applied for STREAM
    // sockets

    // Note that on that level we cannot really validate whether data is
    // actually copied into the send buffer or not. We can only validate that
    // if data is sent with MSG_ZEROCOPY flag then related notifications will
    // appear on a socket error queue.

#if defined(BSLS_PLATFORM_OS_LINUX)
    // Linux kernels versions < 4.14.0 do not support MSG_ZEROCOPY for STREAM
    // sockets
    {
        int major, minor, patch, build;
        NTSCFG_TEST_ASSERT(
            ntsscm::Version::systemVersion(&major, &minor, &patch, &build) ==
            0);

        if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(4, 14, 0)) {
            return;
        }
    }
    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(&test::testStreamSocketMsgZeroCopy);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTSCFG_TEST_CASE(29)
{
    // Concern: Test TX timestamping functionality for DATAGRAM sockets
#if defined(BSLS_PLATFORM_OS_LINUX)
    // Linux kernels versions < 5.0.0 do not support MSG_ZEROCOPY for DGRAM
    // sockets
    {
        int major, minor, patch, build;
        NTSCFG_TEST_ASSERT(
            ntsscm::Version::systemVersion(&major, &minor, &patch, &build) ==
            0);

        if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(5, 0, 0)) {
            return;
        }
    }
    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(&test::testDatagramSocketTxTimestamps);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTSCFG_TEST_CASE(30)
{
    // Concern: Test TX timestamping functionality for STREAM sockets
#if defined(BSLS_PLATFORM_OS_LINUX)
    // Linux kernels versions < 4.14.0 do not support MSG_ZEROCOPY for STREAM
    // sockets
    {
        int major, minor, patch, build;
        NTSCFG_TEST_ASSERT(
            ntsscm::Version::systemVersion(&major, &minor, &patch, &build) ==
            0);

        if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(4, 14, 0)) {
            return;
        }
    }
    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(&test::testStreamSocketTxTimestamps);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTSCFG_TEST_CASE(31)
{
    // Concern: Test TX timestamping an MSG_ZEROCOPY functionality for
    // DATAGRAM sockets
#if defined(BSLS_PLATFORM_OS_LINUX)

    // Linux kernels versions < 5.0.0 do not support MSG_ZEROCOPY for DGRAM
    // sockets
    {
        int major, minor, patch, build;
        NTSCFG_TEST_ASSERT(
            ntsscm::Version::systemVersion(&major, &minor, &patch, &build) ==
            0);

        if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(5, 0, 0)) {
            return;
        }
    }

    ntscfg::TestAllocator ta;
    {
        test::executeDatagramSocketTest(
            &test::testDatagramSocketTxTimestampsAndZeroCopy);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTSCFG_TEST_CASE(32)
{
    // Concern: Test TX timestamping an MSG_ZEROCOPY functionality for
    // STREAM sockets
#if defined(BSLS_PLATFORM_OS_LINUX)
    // Linux kernels versions < 4.14.0 do not support MSG_ZEROCOPY for STREAM
    // sockets
    {
        int major, minor, patch, build;
        NTSCFG_TEST_ASSERT(
            ntsscm::Version::systemVersion(&major, &minor, &patch, &build) ==
            0);

        if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(4, 14, 0)) {
            return;
        }
    }
    ntscfg::TestAllocator ta;
    {
        test::executeStreamSocketTest(
            &test::testStreamSocketTxTimestampsAndZeroCopy);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
    NTSCFG_TEST_REGISTER(5);
    NTSCFG_TEST_REGISTER(6);
    NTSCFG_TEST_REGISTER(7);
    NTSCFG_TEST_REGISTER(8);
    NTSCFG_TEST_REGISTER(9);
    NTSCFG_TEST_REGISTER(10);
    NTSCFG_TEST_REGISTER(11);
    NTSCFG_TEST_REGISTER(12);
    NTSCFG_TEST_REGISTER(13);
    NTSCFG_TEST_REGISTER(14);
    NTSCFG_TEST_REGISTER(15);
    NTSCFG_TEST_REGISTER(16);
    NTSCFG_TEST_REGISTER(17);
    NTSCFG_TEST_REGISTER(18);
    NTSCFG_TEST_REGISTER(19);
    NTSCFG_TEST_REGISTER(20);
    NTSCFG_TEST_REGISTER(21);
    NTSCFG_TEST_REGISTER(22);
    NTSCFG_TEST_REGISTER(23);
    NTSCFG_TEST_REGISTER(24);
    NTSCFG_TEST_REGISTER(25);
    NTSCFG_TEST_REGISTER(26);
    NTSCFG_TEST_REGISTER(27);
    NTSCFG_TEST_REGISTER(28);
    NTSCFG_TEST_REGISTER(29);
    NTSCFG_TEST_REGISTER(30);
    NTSCFG_TEST_REGISTER(31);
    NTSCFG_TEST_REGISTER(32);
}
NTSCFG_TEST_DRIVER_END;
