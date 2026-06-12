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
BSLS_IDENT_RCSID(ntsa_ipv4extension_t_cpp, "$Id$ $CSID$")

#include <ntsa_ipv4extension.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Ipv4Extension'.
class Ipv4ExtensionTest
{
  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test usage example.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntsa::Ipv4ExtensionTest::verifyTypeTraits)
{
    const bool isAllocateAware =
        NTSCFG_TYPE_CHECK_ALLOCATOR_AWARE(ntsa::Ipv4Extension);

    NTSCFG_TEST_TRUE(isAllocateAware);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4ExtensionTest::verifyUsage)
{
    ntsa::Ipv4Extension extension;

    {
        ntsa::Ipv4Option option(NTSCFG_TEST_ALLOCATOR);
        option.makeAlert();

        extension.add(option);
    }

    {
        ntsa::Ipv4Option option(NTSCFG_TEST_ALLOCATOR);

        ntsa::Ipv4RouteLedger& routeLedger = option.makeTimestamp();

        routeLedger.setCount(2);
        routeLedger.setIndex(1);
        routeLedger.setFlags(ntsa::Ipv4RouteLedger::k_TIMESTAMP_AND_ADDRESS);

        routeLedger.entry(0).setAddress(ntsa::Ipv4Address::loopback());
        routeLedger.entry(0).setTimestamp(123);

        extension.add(option);
    }

    {
        ntsa::Ipv4Option option(NTSCFG_TEST_ALLOCATOR);

        ntsa::Ipv4RouteSequence& routeSequence = option.makeRecordRoute();

        routeSequence.setCount(3);
        routeSequence.setIndex(1);

        routeSequence.entry(0) = ntsa::Ipv4Address("192.168.1.111");
        routeSequence.entry(1) = ntsa::Ipv4Address("192.168.1.112");
        routeSequence.entry(2) = ntsa::Ipv4Address("192.168.1.113");

        extension.add(option);
    }

    {
        ntsa::Ipv4Option option(NTSCFG_TEST_ALLOCATOR);

        ntsa::Ipv4RouteSequence& routeSequence = option.makeSourceRouteTight();

        routeSequence.setCount(3);
        routeSequence.setIndex(1);

        routeSequence.entry(0) = ntsa::Ipv4Address("192.168.2.121");
        routeSequence.entry(1) = ntsa::Ipv4Address("192.168.2.122");
        routeSequence.entry(2) = ntsa::Ipv4Address("192.168.2.123");

        extension.add(option);
    }

    {
        ntsa::Ipv4Option option(NTSCFG_TEST_ALLOCATOR);

        ntsa::Ipv4RouteSequence& routeSequence = option.makeSourceRouteLoose();

        routeSequence.setCount(3);
        routeSequence.setIndex(1);

        routeSequence.entry(0) = ntsa::Ipv4Address("192.168.3.131");
        routeSequence.entry(1) = ntsa::Ipv4Address("192.168.3.132");
        routeSequence.entry(2) = ntsa::Ipv4Address("192.168.3.133");

        extension.add(option);
    }

    NTSCFG_TEST_LOG_TRACE << "Options = " << extension << NTSCFG_TEST_LOG_END;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
