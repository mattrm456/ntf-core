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
BSLS_IDENT_RCSID(ntsa_udpextension_t_cpp, "$Id$ $CSID$")

#include <ntsa_udpextension.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::UdpExtension'.
//
// maxSegmentSize = 65495 selectiveAckPermitted = true timestamp = [ tx = 3711540824 rx = 0 ] padding = true windowScale = 11
class UdpExtensionTest
{
  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test usage example.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntsa::UdpExtensionTest::verifyTypeTraits)
{
    const bool isAllocatorAware =
        NTSCFG_TYPE_CHECK_ALLOCATOR_AWARE(ntsa::UdpExtension);

    NTSCFG_TEST_TRUE(isAllocatorAware);
}

NTSCFG_TEST_FUNCTION(ntsa::UdpExtensionTest::verifyUsage)
{
    ntsa::UdpExtension extension;

    {
        ntsa::UdpOption option;
        option.makeMaxDatagramSize(65495);

        extension.add(option);
    }

    {
        ntsa::UdpFragmentation fragmentation;
        fragmentation.setIdentifier(12345);
        fragmentation.setStart(32);
        fragmentation.setOffset(4096);

        ntsa::UdpOption option;
        option.makeFragmentation(fragmentation);

        extension.add(option);
    }

    {
        ntsa::UdpReassembly reassembly;
        reassembly.setMaxSize(16384);
        reassembly.setMaxFragments(8);

        ntsa::UdpOption option;
        option.makeReassembly(reassembly);

        extension.add(option);
    }

    {
        ntsa::UdpTimePointInterval timestamp;
        timestamp.setTx(ntsa::UdpTimePoint(3711540824));
        timestamp.setRx(ntsa::UdpTimePoint(0));

        ntsa::UdpOption option;
        option.makeTimestamp(timestamp);

        extension.add(option);
    }

    NTSCFG_TEST_LOG_TRACE << "Options = " << extension << NTSCFG_TEST_LOG_END;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
