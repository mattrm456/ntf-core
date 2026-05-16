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

#include <ntsa_icmppayload.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_icmppayload_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

IcmpPayload::IcmpPayload(bslma::Allocator* basicAllocator)
: d_type(ntsa::IcmpType::e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

IcmpPayload::IcmpPayload(bslmf::MovableRef<IcmpPayload> original)
    NTSCFG_NOEXCEPT : d_type(ntsa::IcmpType::e_UNDEFINED)
{
    IcmpPayload& ref = bslmf::MovableRefUtil::access(original);

    d_type        = ref.d_type;
    d_allocator_p = ref.d_allocator_p;

    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REQUEST:
        new (d_echoRequest.buffer())
            ntsa::IcmpEchoRequest(ref.d_echoRequest.object());
        break;
    case ntsa::IcmpType::e_ECHO_RESPONSE:
        new (d_echoResponse.buffer())
            ntsa::IcmpEchoResponse(ref.d_echoResponse.object());
        break;
    case ntsa::IcmpType::e_ROUTER_REQUEST:
        new (d_routerRequest.buffer())
            ntsa::IcmpRouterRequest(ref.d_routerRequest.object());
        break;
    case ntsa::IcmpType::e_ROUTER_RESPONSE:
        new (d_routerResponse.buffer())
            ntsa::IcmpRouterResponse(ref.d_routerResponse.object(),
                                     ref.d_allocator_p);
        break;
    case ntsa::IcmpType::e_REDIRECT:
        new (d_redirect.buffer()) ntsa::IcmpRedirect(ref.d_redirect.object());
        break;
    case ntsa::IcmpType::e_UNREACHABLE:
        new (d_unreachable.buffer())
            ntsa::IcmpUnreachable(ref.d_unreachable.object());
        break;
    case ntsa::IcmpType::e_TIMEOUT:
        new (d_timeout.buffer()) ntsa::IcmpTimeout(ref.d_timeout.object());
        break;
    case ntsa::IcmpType::e_PROBLEM:
        new (d_problem.buffer()) ntsa::IcmpProblem(ref.d_problem.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
    }

    ref.reset();
}

IcmpPayload::IcmpPayload(const IcmpPayload& original,
                         bslma::Allocator*  basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REQUEST:
        new (d_echoRequest.buffer())
            ntsa::IcmpEchoRequest(original.d_echoRequest.object());
        break;
    case ntsa::IcmpType::e_ECHO_RESPONSE:
        new (d_echoResponse.buffer())
            ntsa::IcmpEchoResponse(original.d_echoResponse.object());
        break;
    case ntsa::IcmpType::e_ROUTER_REQUEST:
        new (d_routerRequest.buffer())
            ntsa::IcmpRouterRequest(original.d_routerRequest.object());
        break;
    case ntsa::IcmpType::e_ROUTER_RESPONSE:
        new (d_routerResponse.buffer())
            ntsa::IcmpRouterResponse(original.d_routerResponse.object(),
                                     d_allocator_p);
        break;
    case ntsa::IcmpType::e_REDIRECT:
        new (d_redirect.buffer())
            ntsa::IcmpRedirect(original.d_redirect.object());
        break;
    case ntsa::IcmpType::e_UNREACHABLE:
        new (d_unreachable.buffer())
            ntsa::IcmpUnreachable(original.d_unreachable.object());
        break;
    case ntsa::IcmpType::e_TIMEOUT:
        new (d_timeout.buffer())
            ntsa::IcmpTimeout(original.d_timeout.object());
        break;
    case ntsa::IcmpType::e_PROBLEM:
        new (d_problem.buffer())
            ntsa::IcmpProblem(original.d_problem.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
    }
}

IcmpPayload::~IcmpPayload()
{
    this->reset();
}

IcmpPayload& IcmpPayload::operator=(bslmf::MovableRef<IcmpPayload> other)
    NTSCFG_NOEXCEPT
{
    IcmpPayload& ref = bslmf::MovableRefUtil::access(other);

    if (this == &ref) {
        return *this;
    }

    this->reset();

    d_type = ref.d_type;

    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REQUEST:
        new (d_echoRequest.buffer())
            ntsa::IcmpEchoRequest(ref.d_echoRequest.object());
        break;
    case ntsa::IcmpType::e_ECHO_RESPONSE:
        new (d_echoResponse.buffer())
            ntsa::IcmpEchoResponse(ref.d_echoResponse.object());
        break;
    case ntsa::IcmpType::e_ROUTER_REQUEST:
        new (d_routerRequest.buffer())
            ntsa::IcmpRouterRequest(ref.d_routerRequest.object());
        break;
    case ntsa::IcmpType::e_ROUTER_RESPONSE:
        new (d_routerResponse.buffer())
            ntsa::IcmpRouterResponse(ref.d_routerResponse.object(),
                                     d_allocator_p);
        break;
    case ntsa::IcmpType::e_REDIRECT:
        new (d_redirect.buffer()) ntsa::IcmpRedirect(ref.d_redirect.object());
        break;
    case ntsa::IcmpType::e_UNREACHABLE:
        new (d_unreachable.buffer())
            ntsa::IcmpUnreachable(ref.d_unreachable.object());
        break;
    case ntsa::IcmpType::e_TIMEOUT:
        new (d_timeout.buffer()) ntsa::IcmpTimeout(ref.d_timeout.object());
        break;
    case ntsa::IcmpType::e_PROBLEM:
        new (d_problem.buffer()) ntsa::IcmpProblem(ref.d_problem.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
    }

    ref.reset();

    return *this;
}

IcmpPayload& IcmpPayload::operator=(const IcmpPayload& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    d_type = other.d_type;

    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REQUEST:
        new (d_echoRequest.buffer())
            ntsa::IcmpEchoRequest(other.d_echoRequest.object());
        break;
    case ntsa::IcmpType::e_ECHO_RESPONSE:
        new (d_echoResponse.buffer())
            ntsa::IcmpEchoResponse(other.d_echoResponse.object());
        break;
    case ntsa::IcmpType::e_ROUTER_REQUEST:
        new (d_routerRequest.buffer())
            ntsa::IcmpRouterRequest(other.d_routerRequest.object());
        break;
    case ntsa::IcmpType::e_ROUTER_RESPONSE:
        new (d_routerResponse.buffer())
            ntsa::IcmpRouterResponse(other.d_routerResponse.object(),
                                     d_allocator_p);
        break;
    case ntsa::IcmpType::e_REDIRECT:
        new (d_redirect.buffer())
            ntsa::IcmpRedirect(other.d_redirect.object());
        break;
    case ntsa::IcmpType::e_UNREACHABLE:
        new (d_unreachable.buffer())
            ntsa::IcmpUnreachable(other.d_unreachable.object());
        break;
    case ntsa::IcmpType::e_TIMEOUT:
        new (d_timeout.buffer()) ntsa::IcmpTimeout(other.d_timeout.object());
        break;
    case ntsa::IcmpType::e_PROBLEM:
        new (d_problem.buffer()) ntsa::IcmpProblem(other.d_problem.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
    }

    return *this;
}

void IcmpPayload::reset()
{
    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REQUEST: {
        typedef ntsa::IcmpEchoRequest Type;
        d_echoRequest.object().~Type();
    } break;
    case ntsa::IcmpType::e_ECHO_RESPONSE: {
        typedef ntsa::IcmpEchoResponse Type;
        d_echoResponse.object().~Type();
    } break;
    case ntsa::IcmpType::e_ROUTER_REQUEST: {
        typedef ntsa::IcmpRouterRequest Type;
        d_routerRequest.object().~Type();
    } break;
    case ntsa::IcmpType::e_ROUTER_RESPONSE: {
        typedef ntsa::IcmpRouterResponse Type;
        d_routerResponse.object().~Type();
    } break;
    case ntsa::IcmpType::e_REDIRECT: {
        typedef ntsa::IcmpRedirect Type;
        d_redirect.object().~Type();
    } break;
    case ntsa::IcmpType::e_UNREACHABLE: {
        typedef ntsa::IcmpUnreachable Type;
        d_unreachable.object().~Type();
    } break;
    case ntsa::IcmpType::e_TIMEOUT: {
        typedef ntsa::IcmpTimeout Type;
        d_timeout.object().~Type();
    } break;
    case ntsa::IcmpType::e_PROBLEM: {
        typedef ntsa::IcmpProblem Type;
        d_problem.object().~Type();
    } break;
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
    }

    d_type = ntsa::IcmpType::e_UNDEFINED;
}

ntsa::IcmpEchoRequest& IcmpPayload::makeEchoRequest()
{
    if (d_type == ntsa::IcmpType::e_ECHO_REQUEST) {
        d_echoRequest.object().reset();
    }
    else {
        this->reset();
        new (d_echoRequest.buffer()) ntsa::IcmpEchoRequest();
        d_type = ntsa::IcmpType::e_ECHO_REQUEST;
    }

    return d_echoRequest.object();
}

ntsa::IcmpEchoRequest& IcmpPayload::makeEchoRequest(
    const ntsa::IcmpEchoRequest& value)
{
    if (d_type == ntsa::IcmpType::e_ECHO_REQUEST) {
        d_echoRequest.object() = value;
    }
    else {
        this->reset();
        new (d_echoRequest.buffer()) ntsa::IcmpEchoRequest(value);
        d_type = ntsa::IcmpType::e_ECHO_REQUEST;
    }

    return d_echoRequest.object();
}

ntsa::IcmpEchoResponse& IcmpPayload::makeEchoResponse()
{
    if (d_type == ntsa::IcmpType::e_ECHO_RESPONSE) {
        d_echoResponse.object().reset();
    }
    else {
        this->reset();
        new (d_echoResponse.buffer()) ntsa::IcmpEchoResponse();
        d_type = ntsa::IcmpType::e_ECHO_RESPONSE;
    }

    return d_echoResponse.object();
}

ntsa::IcmpEchoResponse& IcmpPayload::makeEchoResponse(
    const ntsa::IcmpEchoResponse& value)
{
    if (d_type == ntsa::IcmpType::e_ECHO_RESPONSE) {
        d_echoResponse.object() = value;
    }
    else {
        this->reset();
        new (d_echoResponse.buffer()) ntsa::IcmpEchoResponse(value);
        d_type = ntsa::IcmpType::e_ECHO_RESPONSE;
    }

    return d_echoResponse.object();
}

ntsa::IcmpRouterRequest& IcmpPayload::makeRouterRequest()
{
    if (d_type == ntsa::IcmpType::e_ROUTER_REQUEST) {
        d_routerRequest.object().reset();
    }
    else {
        this->reset();
        new (d_routerRequest.buffer()) ntsa::IcmpRouterRequest();
        d_type = ntsa::IcmpType::e_ROUTER_REQUEST;
    }

    return d_routerRequest.object();
}

ntsa::IcmpRouterRequest& IcmpPayload::makeRouterRequest(
    const ntsa::IcmpRouterRequest& value)
{
    if (d_type == ntsa::IcmpType::e_ROUTER_REQUEST) {
        d_routerRequest.object() = value;
    }
    else {
        this->reset();
        new (d_routerRequest.buffer()) ntsa::IcmpRouterRequest(value);
        d_type = ntsa::IcmpType::e_ROUTER_REQUEST;
    }

    return d_routerRequest.object();
}

ntsa::IcmpRouterResponse& IcmpPayload::makeRouterResponse()
{
    if (d_type == ntsa::IcmpType::e_ROUTER_RESPONSE) {
        d_routerResponse.object().reset();
    }
    else {
        this->reset();
        new (d_routerResponse.buffer())
            ntsa::IcmpRouterResponse(d_allocator_p);
        d_type = ntsa::IcmpType::e_ROUTER_RESPONSE;
    }

    return d_routerResponse.object();
}

ntsa::IcmpRouterResponse& IcmpPayload::makeRouterResponse(
    const ntsa::IcmpRouterResponse& value)
{
    if (d_type == ntsa::IcmpType::e_ROUTER_RESPONSE) {
        d_routerResponse.object() = value;
    }
    else {
        this->reset();
        new (d_routerResponse.buffer())
            ntsa::IcmpRouterResponse(value, d_allocator_p);
        d_type = ntsa::IcmpType::e_ROUTER_RESPONSE;
    }

    return d_routerResponse.object();
}

ntsa::IcmpRedirect& IcmpPayload::makeRedirect()
{
    if (d_type == ntsa::IcmpType::e_REDIRECT) {
        d_redirect.object().reset();
    }
    else {
        this->reset();
        new (d_redirect.buffer()) ntsa::IcmpRedirect();
        d_type = ntsa::IcmpType::e_REDIRECT;
    }

    return d_redirect.object();
}

ntsa::IcmpRedirect& IcmpPayload::makeRedirect(const ntsa::IcmpRedirect& value)
{
    if (d_type == ntsa::IcmpType::e_REDIRECT) {
        d_redirect.object() = value;
    }
    else {
        this->reset();
        new (d_redirect.buffer()) ntsa::IcmpRedirect(value);
        d_type = ntsa::IcmpType::e_REDIRECT;
    }

    return d_redirect.object();
}

ntsa::IcmpUnreachable& IcmpPayload::makeUnreachable()
{
    if (d_type != ntsa::IcmpType::e_UNREACHABLE) {
        this->reset();
        new (d_unreachable.buffer()) ntsa::IcmpUnreachable();
        d_type = ntsa::IcmpType::e_UNREACHABLE;
    }

    return d_unreachable.object();
}

ntsa::IcmpUnreachable& IcmpPayload::makeUnreachable(
    const ntsa::IcmpUnreachable& value)
{
    if (d_type == ntsa::IcmpType::e_UNREACHABLE) {
        d_unreachable.object() = value;
    }
    else {
        this->reset();
        new (d_unreachable.buffer()) ntsa::IcmpUnreachable(value);
        d_type = ntsa::IcmpType::e_UNREACHABLE;
    }

    return d_unreachable.object();
}

ntsa::IcmpTimeout& IcmpPayload::makeTimeout()
{
    if (d_type != ntsa::IcmpType::e_TIMEOUT) {
        this->reset();
        new (d_timeout.buffer()) ntsa::IcmpTimeout();
        d_type = ntsa::IcmpType::e_TIMEOUT;
    }

    return d_timeout.object();
}

ntsa::IcmpTimeout& IcmpPayload::makeTimeout(const ntsa::IcmpTimeout& value)
{
    if (d_type == ntsa::IcmpType::e_TIMEOUT) {
        d_timeout.object() = value;
    }
    else {
        this->reset();
        new (d_timeout.buffer()) ntsa::IcmpTimeout(value);
        d_type = ntsa::IcmpType::e_TIMEOUT;
    }

    return d_timeout.object();
}

ntsa::IcmpProblem& IcmpPayload::makeProblem()
{
    if (d_type == ntsa::IcmpType::e_PROBLEM) {
        d_problem.object().reset();
    }
    else {
        this->reset();
        new (d_problem.buffer()) ntsa::IcmpProblem();
        d_type = ntsa::IcmpType::e_PROBLEM;
    }

    return d_problem.object();
}

ntsa::IcmpProblem& IcmpPayload::makeProblem(const ntsa::IcmpProblem& value)
{
    if (d_type == ntsa::IcmpType::e_PROBLEM) {
        d_problem.object() = value;
    }
    else {
        this->reset();
        new (d_problem.buffer()) ntsa::IcmpProblem(value);
        d_type = ntsa::IcmpType::e_PROBLEM;
    }

    return d_problem.object();
}

ntsa::IcmpEchoRequest& IcmpPayload::echoRequest()
{
    BSLS_ASSERT(isEchoRequest());
    return d_echoRequest.object();
}

ntsa::IcmpEchoResponse& IcmpPayload::echoResponse()
{
    BSLS_ASSERT(isEchoResponse());
    return d_echoResponse.object();
}

ntsa::IcmpRouterRequest& IcmpPayload::routerRequest()
{
    BSLS_ASSERT(isRouterRequest());
    return d_routerRequest.object();
}

ntsa::IcmpRouterResponse& IcmpPayload::routerResponse()
{
    BSLS_ASSERT(isRouterResponse());
    return d_routerResponse.object();
}

ntsa::IcmpRedirect& IcmpPayload::redirect()
{
    BSLS_ASSERT(isRedirect());
    return d_redirect.object();
}

ntsa::IcmpUnreachable& IcmpPayload::unreachable()
{
    BSLS_ASSERT(isUnreachable());
    return d_unreachable.object();
}

ntsa::IcmpTimeout& IcmpPayload::timeout()
{
    BSLS_ASSERT(isTimeout());
    return d_timeout.object();
}

ntsa::IcmpProblem& IcmpPayload::problem()
{
    BSLS_ASSERT(isProblem());
    return d_problem.object();
}

const ntsa::IcmpEchoRequest& IcmpPayload::echoRequest() const
{
    BSLS_ASSERT(isEchoRequest());
    return d_echoRequest.object();
}

const ntsa::IcmpEchoResponse& IcmpPayload::echoResponse() const
{
    BSLS_ASSERT(isEchoResponse());
    return d_echoResponse.object();
}

const ntsa::IcmpRouterRequest& IcmpPayload::routerRequest() const
{
    BSLS_ASSERT(isRouterRequest());
    return d_routerRequest.object();
}

const ntsa::IcmpRouterResponse& IcmpPayload::routerResponse() const
{
    BSLS_ASSERT(isRouterResponse());
    return d_routerResponse.object();
}

const ntsa::IcmpRedirect& IcmpPayload::redirect() const
{
    BSLS_ASSERT(isRedirect());
    return d_redirect.object();
}

const ntsa::IcmpUnreachable& IcmpPayload::unreachable() const
{
    BSLS_ASSERT(isUnreachable());
    return d_unreachable.object();
}

const ntsa::IcmpTimeout& IcmpPayload::timeout() const
{
    BSLS_ASSERT(isTimeout());
    return d_timeout.object();
}

const ntsa::IcmpProblem& IcmpPayload::problem() const
{
    BSLS_ASSERT(isProblem());
    return d_problem.object();
}

ntsa::IcmpType::Value IcmpPayload::type() const
{
    return d_type;
}

const char* IcmpPayload::name() const
{
    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REQUEST:
        return "echoRequest";
    case ntsa::IcmpType::e_ECHO_RESPONSE:
        return "echoResponse";
    case ntsa::IcmpType::e_ROUTER_REQUEST:
        return "routerRequest";
    case ntsa::IcmpType::e_ROUTER_RESPONSE:
        return "routerResponse";
    case ntsa::IcmpType::e_REDIRECT:
        return "redirect";
    case ntsa::IcmpType::e_UNREACHABLE:
        return "unreachable";
    case ntsa::IcmpType::e_TIMEOUT:
        return "timeout";
    case ntsa::IcmpType::e_PROBLEM:
        return "problem";
    default:
        return "???";
    }
}

bool IcmpPayload::isUndefined() const
{
    return d_type == ntsa::IcmpType::e_UNDEFINED;
}

bool IcmpPayload::isEchoRequest() const
{
    return d_type == ntsa::IcmpType::e_ECHO_REQUEST;
}

bool IcmpPayload::isEchoResponse() const
{
    return d_type == ntsa::IcmpType::e_ECHO_RESPONSE;
}

bool IcmpPayload::isRouterRequest() const
{
    return d_type == ntsa::IcmpType::e_ROUTER_REQUEST;
}

bool IcmpPayload::isRouterResponse() const
{
    return d_type == ntsa::IcmpType::e_ROUTER_RESPONSE;
}

bool IcmpPayload::isRedirect() const
{
    return d_type == ntsa::IcmpType::e_REDIRECT;
}

bool IcmpPayload::isUnreachable() const
{
    return d_type == ntsa::IcmpType::e_UNREACHABLE;
}

bool IcmpPayload::isTimeout() const
{
    return d_type == ntsa::IcmpType::e_TIMEOUT;
}

bool IcmpPayload::isProblem() const
{
    return d_type == ntsa::IcmpType::e_PROBLEM;
}

bool IcmpPayload::equals(const IcmpPayload& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REQUEST:
        return d_echoRequest.object().equals(other.d_echoRequest.object());
    case ntsa::IcmpType::e_ECHO_RESPONSE:
        return d_echoResponse.object().equals(other.d_echoResponse.object());
    case ntsa::IcmpType::e_ROUTER_REQUEST:
        return d_routerRequest.object().equals(other.d_routerRequest.object());
    case ntsa::IcmpType::e_ROUTER_RESPONSE:
        return d_routerResponse.object().equals(
            other.d_routerResponse.object());
    case ntsa::IcmpType::e_REDIRECT:
        return d_redirect.object().equals(other.d_redirect.object());
    case ntsa::IcmpType::e_UNREACHABLE:
        return d_unreachable.object().equals(other.d_unreachable.object());
    case ntsa::IcmpType::e_TIMEOUT:
        return d_timeout.object().equals(other.d_timeout.object());
    case ntsa::IcmpType::e_PROBLEM:
        return d_problem.object().equals(other.d_problem.object());
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
        return true;
    }
}

bool IcmpPayload::less(const IcmpPayload& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REQUEST:
        return d_echoRequest.object().less(other.d_echoRequest.object());
    case ntsa::IcmpType::e_ECHO_RESPONSE:
        return d_echoResponse.object().less(other.d_echoResponse.object());
    case ntsa::IcmpType::e_ROUTER_REQUEST:
        return d_routerRequest.object().less(other.d_routerRequest.object());
    case ntsa::IcmpType::e_ROUTER_RESPONSE:
        return d_routerResponse.object().less(other.d_routerResponse.object());
    case ntsa::IcmpType::e_REDIRECT:
        return d_redirect.object().less(other.d_redirect.object());
    case ntsa::IcmpType::e_UNREACHABLE:
        return d_unreachable.object().less(other.d_unreachable.object());
    case ntsa::IcmpType::e_TIMEOUT:
        return d_timeout.object().less(other.d_timeout.object());
    case ntsa::IcmpType::e_PROBLEM:
        return d_problem.object().less(other.d_problem.object());
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
        return false;
    }
}

bsl::ostream& IcmpPayload::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IcmpPayload::print(bslim::Printer* printer) const
{
    if (d_type == ntsa::IcmpType::e_ECHO_REQUEST) {
        printer->printAttribute("echoRequest", d_echoRequest.object());
    }
    else if (d_type == ntsa::IcmpType::e_ECHO_RESPONSE) {
        printer->printAttribute("echoResponse", d_echoResponse.object());
    }
    else if (d_type == ntsa::IcmpType::e_ROUTER_REQUEST) {
        printer->printAttribute("routerRequest", d_routerRequest.object());
    }
    else if (d_type == ntsa::IcmpType::e_ROUTER_RESPONSE) {
        printer->printAttribute("routerResponse", d_routerResponse.object());
    }
    else if (d_type == ntsa::IcmpType::e_REDIRECT) {
        printer->printAttribute("redirect", d_redirect.object());
    }
    else if (d_type == ntsa::IcmpType::e_UNREACHABLE) {
        printer->printAttribute("unreachable", d_unreachable.object());
    }
    else if (d_type == ntsa::IcmpType::e_TIMEOUT) {
        printer->printAttribute("timeout", d_timeout.object());
    }
    else if (d_type == ntsa::IcmpType::e_PROBLEM) {
        printer->printAttribute("problem", d_problem.object());
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const IcmpPayload& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const IcmpPayload& lhs, const IcmpPayload& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const IcmpPayload& lhs, const IcmpPayload& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const IcmpPayload& lhs, const IcmpPayload& rhs)
{
    return lhs.less(rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
