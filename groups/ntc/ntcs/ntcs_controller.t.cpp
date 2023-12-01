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

#include <ntcs_controller.h>

#include <ntccfg_test.h>
#include <ntsu_socketoptionutil.h>
#include <ntsi_reactor.h>
#include <ntsf_system.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

namespace {

#if defined(BSLS_PLATFORM_OS_UNIX)
const char* k_TEMP_DIR = "TMPDIR";
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
const char* k_TEMP_DIR = "TMP";
#else
#error Not implemented
#endif

void pollAndTest(const bsl::shared_ptr<ntsi::Reactor>& reactor,
                 const ntcs::Controller&               controller,
                 bool                                  readableExpected)
{
    ntsa::Error error;

    bdlb::NullableValue<bsls::TimeInterval> deadline;
    if (!readableExpected) {
        deadline.makeValue(bsls::TimeInterval(0, 0));
    }

    ntsa::EventSet eventSet;
    error = reactor->wait(&eventSet, deadline);
    
    if (!readableExpected) {
        NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));
    }
    else {
        NTCCFG_TEST_OK(error);
    }

    const bool readableFound = eventSet.isReadable(controller.handle());
    NTCCFG_TEST_EQ(readableFound, readableExpected);
}

} // close unnamed namespace

NTCCFG_TEST_CASE(1)
{
    // Concern: Test interruption, pollability and acknowledgement.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntcs::Controller controller;

        bsl::shared_ptr<ntsi::Reactor> reactor = 
            ntsf::System::createReactor(&ta);

        error = reactor->attachSocket(controller.handle());
        NTCCFG_TEST_OK(error);

        error = reactor->showReadable(controller.handle());
        NTCCFG_TEST_OK(error);

        pollAndTest(reactor, controller, false);

        error = controller.acknowledge();
        NTCCFG_TEST_OK(error);
        pollAndTest(reactor, controller, false);

        controller.interrupt(2);
        pollAndTest(reactor, controller, true);
        NTCCFG_TEST_OK(controller.acknowledge());

        pollAndTest(reactor, controller, true);
        NTCCFG_TEST_OK(controller.acknowledge());
        pollAndTest(reactor, controller, false);

        controller.interrupt(1);
        pollAndTest(reactor, controller, true);
        NTCCFG_TEST_OK(controller.acknowledge());
        pollAndTest(reactor, controller, false);

        error = reactor->detachSocket(controller.handle());
        NTCCFG_TEST_OK(error);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Test that control channel can fallback to another
    // implementation on Windows.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        // Test if Unix domain sockets are used by default.

        bool isLocalDefault = false;
        {
            ntcs::Controller controller;

            error = ntsu::SocketOptionUtil::isLocal(&isLocalDefault, 
                                                    controller.handle());
#if defined(BSLS_PLATFORM_OS_WINDOWS)
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_TRUE(isLocalDefault);
#endif
        }

        // Test that the implementation falls back to using TCP sockets when
        // Unix domain sockets may not be used.

        if (isLocalDefault) {
            // Save the old environment variable value.

            bsl::string tempDirOld;
            rc = ntccfg::Platform::getEnvironmentVariable(&tempDirOld, 
                                                          k_TEMP_DIR);
            NTCCFG_TEST_EQ(rc, 0);
            NTCCFG_TEST_FALSE(tempDirOld.empty());

            // Modify the environment variable that defines the path to the
            // user's temporary directory so that it describes a path longer
            // than may be stored in a Unix domain address. In such cases, the
            // implementation must detect that Unix domain sockets cannot be
            // used, and fall back to using TCP.

            {
                bsl::string tempDirNew;
                for (bsl::size_t i = 0; 
                                 i < ntsa::LocalName::k_MAX_PATH_LENGTH; 
                               ++i) 
                {
                    const char c = 'a' + (bsl::rand() % ('z' - 'a'));
                    tempDirNew.append(1, c);
                }

                rc = ntccfg::Platform::setEnvironmentVariable(k_TEMP_DIR, 
                                                              tempDirNew);
                NTCCFG_TEST_EQ(rc, 0);
            }

            ntcs::Controller controller;

            bool isLocal = true;
            error = ntsu::SocketOptionUtil::isLocal(&isLocal, 
                                                    controller.handle());
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_FALSE(isLocal);

            // Attach the socket to a reactor and ensure that it becomes
            // readable after it has been interrupted, and is not readable 
            // after the interruption has been acknowledged.

            bsl::shared_ptr<ntsi::Reactor> reactor = 
                ntsf::System::createReactor(&ta);

            error = reactor->attachSocket(controller.handle());
            NTCCFG_TEST_OK(error);

            error = reactor->showReadable(controller.handle());
            NTCCFG_TEST_OK(error);

            controller.interrupt(1);
            pollAndTest(reactor, controller, true);
            NTCCFG_TEST_OK(controller.acknowledge());
            pollAndTest(reactor, controller, false);

            error = reactor->detachSocket(controller.handle());
            NTCCFG_TEST_OK(error);

            rc = ntccfg::Platform::setEnvironmentVariable(k_TEMP_DIR, 
                                                          tempDirOld);
            NTCCFG_TEST_EQ(rc, 0);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
}
NTCCFG_TEST_DRIVER_END;
