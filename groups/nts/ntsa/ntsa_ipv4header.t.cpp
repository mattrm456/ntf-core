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
BSLS_IDENT_RCSID(ntsa_ipv4header_t_cpp, "$Id$ $CSID$")

#include <ntsa_ipv4header.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Ipv4Header'.
class Ipv4HeaderTest
{
  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test usage example.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntsa::Ipv4HeaderTest::verifyTypeTraits)
{
    const bool isBitwiseInitializable =
        NTSCFG_TYPE_CHECK_BITWISE_INITIALIZABLE(ntsa::Ipv4Header);

    NTSCFG_TEST_TRUE(isBitwiseInitializable);

    const bool isBitwiseMovable =
        NTSCFG_TYPE_CHECK_BITWISE_MOVABLE(ntsa::Ipv4Header);

    NTSCFG_TEST_TRUE(isBitwiseMovable);

    const bool isBitwiseCopyable =
        NTSCFG_TYPE_CHECK_BITWISE_COPYABLE(ntsa::Ipv4Header);

    NTSCFG_TEST_TRUE(isBitwiseCopyable);
}

NTSCFG_TEST_FUNCTION(ntsa::Ipv4HeaderTest::verifyUsage)
{
    
}

}  // close namespace ntsa
}  // close namespace BloombergLP
