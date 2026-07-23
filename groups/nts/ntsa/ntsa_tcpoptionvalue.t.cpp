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
BSLS_IDENT_RCSID(ntsa_tcpoptionvalue_t_cpp, "$Id$ $CSID$")

#include <ntsa_tcpoptionvalue.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::TcpOptionValue'.
class TcpOptionValueTest
{
  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test value semantics: default constructor.
    static void verifyDefaultConstructor();

    // Test value semantics: move constructor.
    static void verifyMoveConstructor();

    // Test value semantics: copy constructor.
    static void verifyCopyConstructor();

    // Test value semantics: copy assignment.
    static void verifyCopyAssignment();

    // Test value semantics: move assignment.
    static void verifyMoveAssignment();

    // Test value semantics: reset.
    static void verifyReset();

    // Test value semantics: setters/getters.
    static void verifyManipulators();

    // Test value semantics: equality.
    static void verifyEquals();

    // Test value semantics: less-than comparison.
    static void verifyLess();

    // Test value semantics: hashing.
    static void verifyHash();
};

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyTypeTraits)
{
    const bool isAllocatorAware =
        NTSCFG_TYPE_CHECK_ALLOCATOR_AWARE(ntsa::TcpOptionValue);

    NTSCFG_TEST_TRUE(isAllocatorAware);
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyDefaultConstructor)
{
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyMoveConstructor)
{
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyCopyConstructor)
{
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyCopyAssignment)
{
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyMoveAssignment)
{
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyReset)
{
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyManipulators)
{
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyEquals)
{
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyLess)
{
}

NTSCFG_TEST_FUNCTION(ntsa::TcpOptionValueTest::verifyHash)
{
}

}  // close namespace ntsa
}  // close namespace BloombergLP
