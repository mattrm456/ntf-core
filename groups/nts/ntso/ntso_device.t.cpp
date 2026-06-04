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

using namespace BloombergLP;

#if NTSO_DEVICE_ENABLED
namespace BloombergLP {
namespace ntso {

// Provide tests for 'ntso::Device'.
class DeviceTest
{
  public:
    // Concern: TODO.
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntso::DeviceTest::verify)
{
    if (!ntso::DeviceUtil::isSupported()) {
        return;
    }

    ntsa::Error error;

    ntsa::DeviceConfig deviceConfig(NTSCFG_TEST_ALLOCATOR);
    deviceConfig.setAdapterName("en0");
    // deviceConfig.setIpv4Address(ntsa::Ipv4Address::loopback());

    bsl::shared_ptr<ntsi::Device> device = ntso::DeviceUtil::createDevice(
        deviceConfig, NTSCFG_TEST_ALLOCATOR);

    error = device->open();
    NTSCFG_TEST_OK(error);

    bslmt::ThreadUtil::sleep(bsls::TimeInterval(2));

    NTSCFG_TEST_LOG_DEBUG << "Closing device" << NTSCFG_TEST_LOG_END;

    error = device->close();
    NTSCFG_TEST_OK(error);
}

}  // close namespace ntso
}  // close namespace BloombergLP
#endif
