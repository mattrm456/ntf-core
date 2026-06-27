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
BSLS_IDENT_RCSID(ntsu_packetutil_t_cpp, "$Id$ $CSID$")

#include <ntsu_packetutil.h>

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_DARWIN)
#include <net/bpf.h>
#endif

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsu {

// Provide tests for 'ntsu::PacketUtil'.
class PacketUtilTest
{
  public:
    // TODO
    static void verifyConstants();
};

#if defined(BSLS_PLATFORM_OS_DARWIN)

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyConstants)
{
    NTSCFG_TEST_EQ(NTSU_BPF_LD, BPF_LD);
    NTSCFG_TEST_EQ(NTSU_BPF_LDX, BPF_LDX);
    NTSCFG_TEST_EQ(NTSU_BPF_ST, BPF_ST);
    NTSCFG_TEST_EQ(NTSU_BPF_STX, BPF_STX);
    NTSCFG_TEST_EQ(NTSU_BPF_ALU, BPF_ALU);
    NTSCFG_TEST_EQ(NTSU_BPF_JMP, BPF_JMP);
    NTSCFG_TEST_EQ(NTSU_BPF_RET, BPF_RET);
    NTSCFG_TEST_EQ(NTSU_BPF_MISC, NTSU_BPF_MISC);

    NTSCFG_TEST_EQ(NTSU_BPF_W, BPF_W);
    NTSCFG_TEST_EQ(NTSU_BPF_H, BPF_H);
    NTSCFG_TEST_EQ(NTSU_BPF_B, BPF_B);

    NTSCFG_TEST_EQ(NTSU_BPF_IMM, BPF_IMM);
    NTSCFG_TEST_EQ(NTSU_BPF_ABS, BPF_ABS);
    NTSCFG_TEST_EQ(NTSU_BPF_IND, BPF_IND);
    NTSCFG_TEST_EQ(NTSU_BPF_MEM, BPF_MEM);
    NTSCFG_TEST_EQ(NTSU_BPF_LEN, BPF_LEN);
    NTSCFG_TEST_EQ(NTSU_BPF_MSH, BPF_MSH);

    NTSCFG_TEST_EQ(NTSU_BPF_ADD, BPF_ADD);
    NTSCFG_TEST_EQ(NTSU_BPF_SUB, BPF_SUB);
    NTSCFG_TEST_EQ(NTSU_BPF_MUL, BPF_MUL);
    NTSCFG_TEST_EQ(NTSU_BPF_DIV, BPF_DIV);
    NTSCFG_TEST_EQ(NTSU_BPF_OR, BPF_OR);
    NTSCFG_TEST_EQ(NTSU_BPF_AND, BPF_AND);
    NTSCFG_TEST_EQ(NTSU_BPF_LSH, BPF_LSH);
    NTSCFG_TEST_EQ(NTSU_BPF_RSH, BPF_RSH);
    NTSCFG_TEST_EQ(NTSU_BPF_NEG, BPF_NEG);
    NTSCFG_TEST_EQ(NTSU_BPF_JA, BPF_JA);
    NTSCFG_TEST_EQ(NTSU_BPF_JEQ, BPF_JEQ);
    NTSCFG_TEST_EQ(NTSU_BPF_JGT, BPF_JGT);
    NTSCFG_TEST_EQ(NTSU_BPF_JGE, BPF_JGE);
    NTSCFG_TEST_EQ(NTSU_BPF_JSET, BPF_JSET);

    NTSCFG_TEST_EQ(NTSU_BPF_K, BPF_K);
    NTSCFG_TEST_EQ(NTSU_BPF_X, BPF_X);

    NTSCFG_TEST_EQ(NTSU_BPF_A, BPF_A);

    NTSCFG_TEST_EQ(NTSU_BPF_TAX, BPF_TAX);
    NTSCFG_TEST_EQ(NTSU_BPF_TXA, BPF_TXA);
}

#else

NTSCFG_TEST_FUNCTION(ntsu::PacketUtilTest::verifyConstants)
{

}

#endif

}  // close namespace ntsu
}  // close namespace BloombergLP
