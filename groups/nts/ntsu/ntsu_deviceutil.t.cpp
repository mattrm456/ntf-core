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
    // Execute the reader loop.
    static void reader(ntsa::Handle              device,
                       const bsls::TimeInterval& duration);

    // Execute the writer loop.
    static void writer(ntsa::Handle              device,
                       const bsls::TimeInterval& duration);

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

void DeviceUtilTest::reader(ntsa::Handle              device,
                            const bsls::TimeInterval& duration)
{
    ntsa::Error error;

    error = ntsu::DeviceUtil::shutdown(device);
    NTSCFG_TEST_OK(error);
}

void DeviceUtilTest::writer(ntsa::Handle              device,
                            const bsls::TimeInterval& duration)
{
    ntsa::Error error;

    error = ntsu::DeviceUtil::shutdown(device);
    NTSCFG_TEST_OK(error);
}

void DeviceUtilTest::verifyAdapter(const ntsa::Adapter& adapter)
{
    BALL_LOG_INFO << "Verifying adapter " << adapter << BALL_LOG_END;

    ntsa::Error error;
    int         rc;

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
                                 duration),
            outgoingThreadAttributes);
        NTSCFG_TEST_EQ(rc, 0);
    }

    outgoingThreadGroup.joinAll();
    incomingThreadGroup.joinAll();

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

    bdlb::NullableValue<ntsa::Adapter> adapter;
    {
        bsl::vector<ntsa::Adapter> adapterList;
        ntsu::AdapterUtil::discoverAdapterList(&adapterList);

        for (bsl::size_t i = 0; i < adapterList.size(); ++i) {
            const ntsa::Adapter& candidateAdapter = adapterList[i];
            if (candidateAdapter.ipv4Address().has_value()) {
                if (candidateAdapter.ipv4Address().value().isLoopback()) {
                    adapter = candidateAdapter;
                    break;
                }
            }
        }
    }

    if (adapter.has_value()) {
        DeviceUtilTest::verifyAdapter(adapter.value());
    }
}

NTSCFG_TEST_FUNCTION(ntsu::DeviceUtilTest::verifyDefault)
{
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    if (!ntsu::DeviceUtil::isSupported()) {
        return;
    }

    bdlb::NullableValue<ntsa::Adapter> adapter;
    {
        bsl::vector<ntsa::Adapter> adapterList;
        ntsu::AdapterUtil::discoverAdapterList(&adapterList);

        for (bsl::size_t i = 0; i < adapterList.size(); ++i) {
            const ntsa::Adapter& candidateAdapter = adapterList[i];
            if (candidateAdapter.ipv4Address().has_value()) {
                if (!candidateAdapter.ipv4Address().value().isLoopback()) {
                    adapter = candidateAdapter;
                    break;
                }
            }
        }
    }

    if (adapter.has_value()) {
        DeviceUtilTest::verifyAdapter(adapter.value());
    }
}

}  // close namespace ntsu
}  // close namespace BloombergLP
