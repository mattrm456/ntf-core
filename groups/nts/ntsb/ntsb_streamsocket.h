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

#ifndef INCLUDED_NTSB_STREAMSOCKET
#define INCLUDED_NTSB_STREAMSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_shutdowntype.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsi_streamsocket.h>
#include <ntsscm_version.h>
#include <bdls_filesystemutil.h>
#include <bslma_managedptr.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsb {

/// @internal @brief
/// Provide a blocking or non-blocking stream socket implemented by the system.
///
/// @par Thread Safey
/// This class is thread safe.
///
/// @ingroup module_ntsb
class StreamSocket : public ntsi::StreamSocket
{
    ntsa::Handle d_handle;

  private:
    StreamSocket(const StreamSocket&) BSLS_KEYWORD_DELETED;
    StreamSocket& operator=(const StreamSocket&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new, uninitialized stream socket.
    StreamSocket();

    /// Create a new stream socket implemented using the specified
    /// 'handle'.
    explicit StreamSocket(ntsa::Handle handle);

    /// Destroy this object.
    ~StreamSocket() BSLS_KEYWORD_OVERRIDE;

    /// Create a new socket of the specified 'transport'. Return the
    /// error.
    ntsa::Error open(ntsa::Transport::Value transport) BSLS_KEYWORD_OVERRIDE;

    /// Acquire ownership of the specified 'handle' to implement this
    /// socket. Return the error.
    ntsa::Error acquire(ntsa::Handle handle) BSLS_KEYWORD_OVERRIDE;

    /// Release ownership of the handle that implements this socket.
    ntsa::Handle release() BSLS_KEYWORD_OVERRIDE;

    /// Bind this socket to the specified source 'endpoint'. If the
    /// specified 'reuseAddress' flag is set, allow this socket to bind to
    /// an address already in use by the operating system. Return the error.
    ntsa::Error bind(const ntsa::Endpoint& endpoint,
                     bool                  reuseAddress) BSLS_KEYWORD_OVERRIDE;

    /// Bind this to any suitable source endpoint appropriate for a socket
    /// of the specified 'transport'. If the specified 'reuseAddress' flag
    /// is set, allow this socket to bind to an address already in use by
    /// the operating system. Return the error.
    ntsa::Error bindAny(ntsa::Transport::Value transport,
                        bool reuseAddress) BSLS_KEYWORD_OVERRIDE;

    /// Connect to the specified remote 'endpoint'. Return the error.
    ntsa::Error connect(const ntsa::Endpoint& endpoint) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' to the socket send buffer according to
    /// the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    ntsa::Error send(ntsa::SendContext*       context,
                     const bdlbb::Blob&       data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' to the socket send buffer according to
    /// the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    ntsa::Error send(ntsa::SendContext*       context,
                     const ntsa::Data&        data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Dequeue from the socket receive buffer into the specified 'data'
    /// according to the specified 'options'. Load into the specified
    /// 'context' the result of the operation. Return the error.
    ntsa::Error receive(ntsa::ReceiveContext*       context,
                        bdlbb::Blob*                data,
                        const ntsa::ReceiveOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Dequeue from the socket receive buffer into the specified 'data'
    /// according to the specified 'options'. Load into the specified
    /// 'context' the result of the operation. Return the error.
    ntsa::Error receive(ntsa::ReceiveContext*       context,
                        ntsa::Data*                 data,
                        const ntsa::ReceiveOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Read data from the socket error queue. Then if the specified
    /// 'notifications' is not null parse fetched data to extract control
    /// messages into the specified 'notifications'. Return the error.
    ntsa::Error receiveNotifications(ntsa::NotificationQueue* notifications)
        BSLS_KEYWORD_OVERRIDE;

    /// Shutdown the stream socket in the specified 'direction'. Return the
    /// error.
    ntsa::Error shutdown(ntsa::ShutdownType::Value direction)
        BSLS_KEYWORD_OVERRIDE;

    /// Unlink the file corresponding to the socket, if the socket is a
    /// local (a.k.a. Unix domain) socket bound to a non-abstract path.
    /// Return the error. Note that this function should only be called
    /// for sockets in the connect role.
    ntsa::Error unlink() BSLS_KEYWORD_OVERRIDE;

    /// Close the socket. Return the error.
    ntsa::Error close() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the source endpoint of this socket.
    /// Return the error.
    ntsa::Error sourceEndpoint(ntsa::Endpoint* result) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the remote endpoint to which this
    /// socket is connected. Return the error.
    ntsa::Error remoteEndpoint(ntsa::Endpoint* result) const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the descriptor handle.
    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;

    // *** Socket Options ***

    /// Set the option for the 'socket' that controls its blocking mode
    /// according to the specified 'blocking' flag. Return the error.
    ntsa::Error setBlocking(bool blocking) BSLS_KEYWORD_OVERRIDE;

    /// Set the specified 'option' for this socket. Return the error.
    ntsa::Error setOption(const ntsa::SocketOption& option)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'option' the socket option of the specified
    /// 'type' set for this socket. Return the error.
    ntsa::Error getOption(ntsa::SocketOption*           option,
                          ntsa::SocketOptionType::Value type)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the last known error encountered
    /// when connecting the socket. Return the error (retrieving the error).
    ntsa::Error getLastError(ntsa::Error* result) BSLS_KEYWORD_OVERRIDE;

    // *** Limits ***

    /// Return the maximum number of buffers that can be the source of a
    /// gathered write. Additional buffers beyond this limit are silently
    /// ignored.
    bsl::size_t maxBuffersPerSend() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of buffers that can be the destination
    /// of a scattered read. Additional buffers beyond this limit are
    /// silently ignored.
    bsl::size_t maxBuffersPerReceive() const BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'client' and 'server' a connected pair of
    /// stream sockets of the specified 'type'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error pair(ntsb::StreamSocket*    client,
                            ntsb::StreamSocket*    server,
                            ntsa::Transport::Value type);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// stream sockets of the specified 'type'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error pair(bslma::ManagedPtr<ntsb::StreamSocket>* client,
                            bslma::ManagedPtr<ntsb::StreamSocket>* server,
                            ntsa::Transport::Value                 type,
                            bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// stream sockets of the specified 'type'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error pair(bsl::shared_ptr<ntsb::StreamSocket>* client,
                            bsl::shared_ptr<ntsb::StreamSocket>* server,
                            ntsa::Transport::Value               type,
                            bslma::Allocator* basicAllocator = 0);
};

}  // close package namespace
}  // close enterprise namespace
#endif
