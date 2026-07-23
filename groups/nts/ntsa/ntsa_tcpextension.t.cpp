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
BSLS_IDENT_RCSID(ntsa_tcpextension_t_cpp, "$Id$ $CSID$")

#include <ntsa_tcpextension.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::TcpExtension'.
class TcpExtensionTest
{
  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test usage example.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntsa::TcpExtensionTest::verifyTypeTraits)
{
    const bool isAllocatorAware =
        NTSCFG_TYPE_CHECK_ALLOCATOR_AWARE(ntsa::TcpExtension);

    NTSCFG_TEST_TRUE(isAllocatorAware);
}

NTSCFG_TEST_FUNCTION(ntsa::TcpExtensionTest::verifyUsage)
{
    ntsa::TcpExtension extension;

    {
        ntsa::TcpOption option;
        option.makeMaxSegmentSize(65495);

        extension.add(option);
    }

    {
        ntsa::TcpOption option;
        option.makeSelectiveAckPermitted();

        extension.add(option);
    }

    {
        ntsa::TcpTimePointInterval timestamp;
        timestamp.setTx(ntsa::TcpSequenceNumber(3711540824));
        timestamp.setRx(ntsa::TcpSequenceNumber(0));

        ntsa::TcpOption option;
        option.makeTimestamp(timestamp);

        extension.add(option);
    }

    {
        ntsa::TcpOption option;
        option.makeWindowScale(11);

        extension.add(option);
    }

    NTSCFG_TEST_LOG_TRACE << "Options = " << extension << NTSCFG_TEST_LOG_END;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
