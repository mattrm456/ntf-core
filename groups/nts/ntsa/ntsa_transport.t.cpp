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
BSLS_IDENT_RCSID(ntsa_transport_t_cpp, "$Id$ $CSID$")

#include <ntsa_transport.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::TransportSecurity'.
class TransportSecurityTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::TransportSecurityTest::verifyFromInt)
{
    // clang-format off
    struct TestData {
        int                            d_result;
        int                            d_input;
        ntsa::TransportSecurity::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  0, ntsa::TransportSecurity::e_UNDEFINED },
        {  0,  1, ntsa::TransportSecurity::e_TLS       },
        {  0,  2, ntsa::TransportSecurity::e_SSH       },
        { -1, -1, ntsa::TransportSecurity::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportSecurity::Value value;
        int rc = ntsa::TransportSecurity::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportSecurityTest::verifyFromString)
{
    // clang-format off
    struct TestData {
        int                            d_result;
        const char*                    d_input;
        ntsa::TransportSecurity::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  "UNDEFINED", ntsa::TransportSecurity::e_UNDEFINED },
        {  0,  "TLS",       ntsa::TransportSecurity::e_TLS       },
        {  0,  "SSH",       ntsa::TransportSecurity::e_SSH       },
        { -1,  "!",         ntsa::TransportSecurity::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportSecurity::Value value;
        int rc = ntsa::TransportSecurity::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportSecurityTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        {  "UNDEFINED", 0 },
        {  "TLS",       1 },
        {  "SSH",       2 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::TransportSecurity::Value input =
            static_cast<ntsa::TransportSecurity::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

// Provide tests for 'ntsa::TransportProtocol'.
class TransportProtocolTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::TransportProtocolTest::verifyFromInt)
{
    // clang-format off
    struct TestData {
        int                            d_result;
        int                            d_input;
        ntsa::TransportProtocol::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  0, ntsa::TransportProtocol::e_UNDEFINED },
        {  0,  1, ntsa::TransportProtocol::e_TCP       },
        {  0,  2, ntsa::TransportProtocol::e_UDP       },
        {  0,  3, ntsa::TransportProtocol::e_LOCAL     },
        { -1, -1, ntsa::TransportProtocol::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportProtocol::Value value;
        int rc = ntsa::TransportProtocol::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportProtocolTest::verifyFromString)
{
    // clang-format off
    struct TestData {
        int                            d_result;
        const char*                    d_input;
        ntsa::TransportProtocol::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  "UNDEFINED", ntsa::TransportProtocol::e_UNDEFINED },
        {  0,  "TCP",       ntsa::TransportProtocol::e_TCP       },
        {  0,  "UDP",       ntsa::TransportProtocol::e_UDP       },
        {  0,  "LOCAL",     ntsa::TransportProtocol::e_LOCAL     },
        { -1,  "!",         ntsa::TransportProtocol::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportProtocol::Value value;
        int rc = ntsa::TransportProtocol::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportProtocolTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        {  "UNDEFINED", 0 },
        {  "TCP",       1 },
        {  "UDP",       2 },
        {  "LOCAL",     3 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::TransportProtocol::Value input =
            static_cast<ntsa::TransportProtocol::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

// Provide tests for 'ntsa::TransportDomain'.
class TransportDomainTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::TransportDomainTest::verifyFromInt)
{
    // clang-format off
    struct TestData {
        int                          d_result;
        int                          d_input;
        ntsa::TransportDomain::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  0, ntsa::TransportDomain::e_UNDEFINED },
        {  0,  1, ntsa::TransportDomain::e_IPV4      },
        {  0,  2, ntsa::TransportDomain::e_IPV6      },
        {  0,  3, ntsa::TransportDomain::e_LOCAL     },
        { -1, -1, ntsa::TransportDomain::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportDomain::Value value;
        int rc = ntsa::TransportDomain::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportDomainTest::verifyFromString)
{
    // clang-format off
    struct TestData {
        int                          d_result;
        const char*                  d_input;
        ntsa::TransportDomain::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  "UNDEFINED", ntsa::TransportDomain::e_UNDEFINED },
        {  0,  "IPV4",      ntsa::TransportDomain::e_IPV4      },
        {  0,  "IPV6",      ntsa::TransportDomain::e_IPV6      },
        {  0,  "LOCAL",     ntsa::TransportDomain::e_LOCAL     },
        { -1,  "!",         ntsa::TransportDomain::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportDomain::Value value;
        int rc = ntsa::TransportDomain::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportDomainTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        {  "UNDEFINED", 0 },
        {  "IPV4",      1 },
        {  "IPV6",      2 },
        {  "LOCAL",     3 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::TransportDomain::Value input =
            static_cast<ntsa::TransportDomain::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

// Provide tests for 'ntsa::TransportMode'.
class TransportModeTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::TransportModeTest::verifyFromInt)
{
    // clang-format off
    struct TestData {
        int                        d_result;
        int                        d_input;
        ntsa::TransportMode::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  0, ntsa::TransportMode::e_UNDEFINED },
        {  0,  1, ntsa::TransportMode::e_STREAM    },
        {  0,  2, ntsa::TransportMode::e_DATAGRAM  },
        { -1, -1, ntsa::TransportMode::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportMode::Value value;
        int rc = ntsa::TransportMode::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportModeTest::verifyFromString)
{
    // clang-format off
    struct TestData {
        int                        d_result;
        const char*                d_input;
        ntsa::TransportMode::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  "UNDEFINED", ntsa::TransportMode::e_UNDEFINED },
        {  0,  "STREAM",    ntsa::TransportMode::e_STREAM    },
        {  0,  "DATAGRAM",  ntsa::TransportMode::e_DATAGRAM  },
        { -1,  "!",         ntsa::TransportMode::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportMode::Value value;
        int rc = ntsa::TransportMode::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportModeTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        {  "UNDEFINED", 0 },
        {  "STREAM",    1 },
        {  "DATAGRAM",  2 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::TransportMode::Value input =
            static_cast<ntsa::TransportMode::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

// Provide tests for 'ntsa::TransportRole'.
class TransportRoleTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::TransportRoleTest::verifyFromInt)
{
    // clang-format off
    struct TestData {
        int                        d_result;
        int                        d_input;
        ntsa::TransportRole::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  0, ntsa::TransportRole::e_UNDEFINED },
        {  0,  1, ntsa::TransportRole::e_CLIENT    },
        {  0,  2, ntsa::TransportRole::e_SERVER    },
        { -1, -1, ntsa::TransportRole::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportRole::Value value;
        int rc = ntsa::TransportRole::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportRoleTest::verifyFromString)
{
    // clang-format off
    struct TestData {
        int                        d_result;
        const char*                d_input;
        ntsa::TransportRole::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  "UNDEFINED", ntsa::TransportRole::e_UNDEFINED },
        {  0,  "CLIENT",    ntsa::TransportRole::e_CLIENT    },
        {  0,  "SERVER",    ntsa::TransportRole::e_SERVER    },
        { -1,  "!",         ntsa::TransportRole::e_UNDEFINED }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::TransportRole::Value value;
        int rc = ntsa::TransportRole::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportRoleTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        {  "UNDEFINED", 0 },
        {  "CLIENT",    1 },
        {  "SERVER",    2 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::TransportRole::Value input =
            static_cast<ntsa::TransportRole::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

// Provide tests for 'ntsa::Transport'.
class TransportTest
{
  public:
    // Verify conversion from an integer.
    static void verifyFromInt();

    // Verify conversion from a string.
    static void verifyFromString();

    // Verify conversion to a string and printing.
    static void verifyPrint();
};

NTSCFG_TEST_FUNCTION(ntsa::TransportTest::verifyFromInt)
{
    // clang-format off
    struct TestData {
        int                    d_result;
        int                    d_input;
        ntsa::Transport::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  0, ntsa::Transport::e_UNDEFINED         },
        {  0,  1, ntsa::Transport::e_TCP_IPV4_STREAM   },
        {  0,  2, ntsa::Transport::e_TCP_IPV6_STREAM   },
        {  0,  3, ntsa::Transport::e_UDP_IPV4_DATAGRAM },
        {  0,  4, ntsa::Transport::e_UDP_IPV6_DATAGRAM },
        {  0,  5, ntsa::Transport::e_LOCAL_STREAM      },
        {  0,  6, ntsa::Transport::e_LOCAL_DATAGRAM    },
        { -1, -1, ntsa::Transport::e_UNDEFINED         }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::Transport::Value value;
        int rc = ntsa::Transport::fromInt(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportTest::verifyFromString)
{
    // clang-format off
    struct TestData {
        int                    d_result;
        const char*            d_input;
        ntsa::Transport::Value d_output;
    };

    const TestData k_TEST_DATA[] = {
        {  0,  "UNDEFINED",         ntsa::Transport::e_UNDEFINED         },
        {  0,  "TCP_IPV4_STREAM",   ntsa::Transport::e_TCP_IPV4_STREAM   },
        {  0,  "TCP_IPV6_STREAM",   ntsa::Transport::e_TCP_IPV6_STREAM   },
        {  0,  "UDP_IPV4_DATAGRAM", ntsa::Transport::e_UDP_IPV4_DATAGRAM },
        {  0,  "UDP_IPV6_DATAGRAM", ntsa::Transport::e_UDP_IPV6_DATAGRAM },
        {  0,  "LOCAL_STREAM",      ntsa::Transport::e_LOCAL_STREAM      },
        {  0,  "LOCAL_DATAGRAM",    ntsa::Transport::e_LOCAL_DATAGRAM    },
        { -1,  "!",                 ntsa::Transport::e_UNDEFINED         }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        ntsa::Transport::Value value;
        int rc = ntsa::Transport::fromString(&value, testData.d_input);

        NTSCFG_TEST_EQ(rc, testData.d_result);
        if (rc == 0) {
            NTSCFG_TEST_EQ(value, testData.d_output);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::TransportTest::verifyPrint)
{
    // clang-format off
    struct TestData {
        const char* d_result;
        int         d_input;
    };

    const TestData k_TEST_DATA[] = {
        {  "UNDEFINED",           0 },
        {  "TCP_IPV4_STREAM",     1 },
        {  "TCP_IPV6_STREAM",     2 },
        {  "UDP_IPV4_DATAGRAM",   3 },
        {  "UDP_IPV6_DATAGRAM",   4 },
        {  "LOCAL_STREAM",        5 },
        {  "LOCAL_DATAGRAM",      6 }
    };
    // clang-format on

    const bsl::size_t k_TEST_DATA_COUNT = sizeof(k_TEST_DATA) /
                                          sizeof(k_TEST_DATA[0]);

    for (bsl::size_t i = 0; i < k_TEST_DATA_COUNT; ++i) {
        const TestData& testData = k_TEST_DATA[i];

        const ntsa::Transport::Value input =
            static_cast<ntsa::Transport::Value>(testData.d_input);

        bsl::ostringstream oss;
        bsl::ostream& result = oss << input;
        oss.flush();

        NTSCFG_TEST_EQ(result.rdbuf(), oss.rdbuf());
        NTSCFG_TEST_EQ(oss.str(), bsl::string(testData.d_result));
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
