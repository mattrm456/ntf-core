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

#ifndef INCLUDED_NTSU_DEVICEUTIL
#define INCLUDED_NTSU_DEVICEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_adapter.h>
#include <ntsa_deviceconfig.h>
#include <ntsa_devicetype.h>
#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntsa_packet.h>
#include <ntsa_packetfactory.h>
#include <ntsa_packetqueue.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsls_timeinterval.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <ball_log.h>

namespace BloombergLP {
namespace ntsu {

/// Provide a guard to automatically close a device unless otherwise released.
class DeviceGuard
{
    ntsa::Handle d_device;

  private:
    DeviceGuard(const DeviceGuard&) BSLS_KEYWORD_DELETED;
    DeviceGuard& operator=(const DeviceGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Guard the specified 'device'.
    explicit DeviceGuard(ntsa::Handle device);

    /// Close the guarded device, if any, and destroy this object.
    ~DeviceGuard();

    /// Stop guarding the underyling device, and return the guarded device.
    ntsa::Handle release();
};

/// @internal @brief
/// Provide utilities for link-level devices.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsu
class DeviceUtil
{
    /// Provide a private, platform-specific implementation of utilities for
    /// link-level devices.
    class Impl;

    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.DEVICEUTIL");

  public:
    /// Open the device described by the specified 'adapter' according to the
    /// specified 'configuration'. Load into the specified 'result' the handle
    /// to the device. Load into the specified 'type' the link-level data
    /// format used by the device. Load into the specified 'txBufferSize' the
    /// size of each blob buffer produced by the blob buffer factory supplied
    /// to 'enqueuePacket'. Load into the specified 'rxBufferSize' the size of
    /// each blob buffer produced by the blob buffer factory supplied to
    /// 'dequeuePacket'. Return the error.
    static ntsa::Error open(ntsa::Handle*             result,
                            ntsa::DeviceType::Value*  type,
                            bsl::size_t*              txBufferSize,
                            bsl::size_t*              rxBufferSize,
                            const ntsa::Adapter&      adapter,
                            const ntsa::DeviceConfig& configuration);

    /// Apply the specified packet 'filter' to the specified 'device'. Return
    /// the error.
    static ntsa::Error applyFilter(ntsa::Handle              device,
                                   ntsa::DeviceType::Value   deviceType,
                                   const ntsa::Adapter&      adapter,
                                   const ntsa::PacketFilter& filter);

    /// Set the blocking mode of the specified 'device' to the specified
    /// 'value'. Return the error.
    static ntsa::Error setBlocking(ntsa::Handle device, bool value);

    /// Get the blocking mode of the specified 'device' and load it into the
    /// specified 'result'. Return the error.
    static ntsa::Error getBlocking(ntsa::Handle device, bool* result);

    /// Wait indefinitely until the specified 'device' is readable or the
    /// wait is interrupted by a signal. Return the error.
    static ntsa::Error waitUntilReadable(ntsa::Handle device);

    /// Wait until the specified 'device' is readable or the wait is
    /// interrupted by a signal or the specified absolute 'timeout' elapses.
    /// Return the error.
    static ntsa::Error waitUntilReadable(ntsa::Handle              device,
                                         const bsls::TimeInterval& timeout);

    /// Wait indefinitely until the specified 'device' is writable or the
    /// wait is interrupted by a signal. Return the error.
    static ntsa::Error waitUntilWritable(ntsa::Handle device);

    /// Wait until the specified 'device' is writable or the wait is
    /// interrupted by a signal or the specified absolute 'timeout' elapses.
    /// Return the error.
    static ntsa::Error waitUntilWritable(ntsa::Handle              device,
                                         const bsls::TimeInterval& timeout);

    /// Wait indefinitely until the specified 'device' has an error or the wait
    /// is interrupted by a signal. Return the error.
    static ntsa::Error waitUntilError(ntsa::Handle device);

    /// Wait until the specified 'device' has an error or the wait is
    /// interrupted by a signal or the specified absolute 'timeout' elapses.
    /// Return the error.
    static ntsa::Error waitUntilError(ntsa::Handle              device,
                                      const bsls::TimeInterval& timeout);

    /// Block until at least one packet can be enqueued for transmission by the
    /// specified 'device' then enqueue the specified 'packet' for
    /// transmission. Return the error.
    static ntsa::Error enqueuePacket(
        ntsa::Handle                                device,
        ntsa::DeviceType::Value                     deviceType,
        const bsl::shared_ptr<ntsa::Packet>&        packet,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory);

    /// Block until at least one packet has been received by the specified
    /// 'device' then enqueue to the specified 'packetQueue' each packet
    /// received. Return the error.
    static ntsa::Error dequeuePacket(
        ntsa::Handle                                device,
        ntsa::DeviceType::Value                     deviceType,
        const bsl::shared_ptr<ntsa::PacketQueue>&   packetQueue,
        const bsl::shared_ptr<ntsa::PacketFactory>& packetFactory);

    /// Close the specified 'device'. Return the error.
    static ntsa::Error close(ntsa::Handle device);

    /// Shut down transmission and reception for the specified 'device'. Return
    /// the error.
    static ntsa::Error shutdown(ntsa::Handle device);

    /// Return true if the device is supported, otherwise return false.
    static bool isSupported();
};

}  // end namespace ntsu
}  // end namespace BloombergLP
#endif
