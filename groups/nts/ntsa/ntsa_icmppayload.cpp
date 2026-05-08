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

IcmpPayload::IcmpPayload()
: d_type(ntsa::IcmpType::e_UNDEFINED)
{
}

IcmpPayload::IcmpPayload(bslmf::MovableRef<IcmpPayload> original)
    NTSCFG_NOEXCEPT
: d_type(ntsa::IcmpType::e_UNDEFINED)
{
    IcmpPayload& ref = bslmf::MovableRefUtil::access(original);

    d_type = ref.d_type;

    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REPLY:
        new (d_echoReply.buffer())
            ntsa::IcmpPong(ref.d_echoReply.object());
        break;
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        new (d_destinationUnreachable.buffer())
            ntsa::IcmpUnreachable(ref.d_destinationUnreachable.object());
        break;
    case ntsa::IcmpType::e_REDIRECT:
        new (d_redirect.buffer())
            ntsa::IcmpRedirect(ref.d_redirect.object());
        break;
    case ntsa::IcmpType::e_ECHO:
        new (d_echo.buffer())
            ntsa::IcmpPing(ref.d_echo.object());
        break;
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        new (d_timeExceeded.buffer())
            ntsa::IcmpTimeout(ref.d_timeExceeded.object());
        break;
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        new (d_parameterProblem.buffer())
            ntsa::IcmpProblem(ref.d_parameterProblem.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
    }

    ref.reset();
}

IcmpPayload::IcmpPayload(const IcmpPayload& original)
: d_type(original.d_type)
{
    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REPLY:
        new (d_echoReply.buffer())
            ntsa::IcmpPong(original.d_echoReply.object());
        break;
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        new (d_destinationUnreachable.buffer())
            ntsa::IcmpUnreachable(original.d_destinationUnreachable.object());
        break;
    case ntsa::IcmpType::e_REDIRECT:
        new (d_redirect.buffer())
            ntsa::IcmpRedirect(original.d_redirect.object());
        break;
    case ntsa::IcmpType::e_ECHO:
        new (d_echo.buffer())
            ntsa::IcmpPing(original.d_echo.object());
        break;
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        new (d_timeExceeded.buffer())
            ntsa::IcmpTimeout(original.d_timeExceeded.object());
        break;
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        new (d_parameterProblem.buffer())
            ntsa::IcmpProblem(original.d_parameterProblem.object());
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
    case ntsa::IcmpType::e_ECHO_REPLY:
        new (d_echoReply.buffer())
            ntsa::IcmpPong(ref.d_echoReply.object());
        break;
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        new (d_destinationUnreachable.buffer())
            ntsa::IcmpUnreachable(ref.d_destinationUnreachable.object());
        break;
    case ntsa::IcmpType::e_REDIRECT:
        new (d_redirect.buffer())
            ntsa::IcmpRedirect(ref.d_redirect.object());
        break;
    case ntsa::IcmpType::e_ECHO:
        new (d_echo.buffer())
            ntsa::IcmpPing(ref.d_echo.object());
        break;
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        new (d_timeExceeded.buffer())
            ntsa::IcmpTimeout(ref.d_timeExceeded.object());
        break;
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        new (d_parameterProblem.buffer())
            ntsa::IcmpProblem(ref.d_parameterProblem.object());
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
    case ntsa::IcmpType::e_ECHO_REPLY:
        new (d_echoReply.buffer())
            ntsa::IcmpPong(other.d_echoReply.object());
        break;
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        new (d_destinationUnreachable.buffer())
            ntsa::IcmpUnreachable(other.d_destinationUnreachable.object());
        break;
    case ntsa::IcmpType::e_REDIRECT:
        new (d_redirect.buffer())
            ntsa::IcmpRedirect(other.d_redirect.object());
        break;
    case ntsa::IcmpType::e_ECHO:
        new (d_echo.buffer())
            ntsa::IcmpPing(other.d_echo.object());
        break;
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        new (d_timeExceeded.buffer())
            ntsa::IcmpTimeout(other.d_timeExceeded.object());
        break;
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        new (d_parameterProblem.buffer())
            ntsa::IcmpProblem(other.d_parameterProblem.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
    }

    return *this;
}

void IcmpPayload::reset()
{
    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REPLY: {
        typedef ntsa::IcmpPong Type;
        d_echoReply.object().~Type();
    } break;
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE: {
        typedef ntsa::IcmpUnreachable Type;
        d_destinationUnreachable.object().~Type();
    } break;
    case ntsa::IcmpType::e_REDIRECT: {
       typedef ntsa::IcmpRedirect Type;
        d_redirect.object().~Type();
    } break;
    case ntsa::IcmpType::e_ECHO: {
        typedef ntsa::IcmpPing Type;
        d_echo.object().~Type();
     } break;
    case ntsa::IcmpType::e_TIME_EXCEEDED: {
        typedef ntsa::IcmpTimeout Type;
        d_timeExceeded.object().~Type();
    } break;
    case ntsa::IcmpType::e_PARAMETER_PROBLEM: {
        typedef ntsa::IcmpProblem Type;
        d_parameterProblem.object().~Type();
    } break;
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
    }

    d_type = ntsa::IcmpType::e_UNDEFINED;
}

ntsa::IcmpPong& IcmpPayload::makeEchoReply()
{
    if (d_type == ntsa::IcmpType::e_ECHO_REPLY) {
        d_echoReply.object().reset();
    }
    else {
        this->reset();
        new (d_echoReply.buffer()) ntsa::IcmpPong();
        d_type = ntsa::IcmpType::e_ECHO_REPLY;
    }

    return d_echoReply.object();
}

ntsa::IcmpPong& IcmpPayload::makeEchoReply(const ntsa::IcmpPong& value)
{
    if (d_type == ntsa::IcmpType::e_ECHO_REPLY) {
        d_echoReply.object() = value;
    }
    else {
        this->reset();
        new (d_echoReply.buffer()) ntsa::IcmpPong(value);
        d_type = ntsa::IcmpType::e_ECHO_REPLY;
    }

    return d_echoReply.object();
}

ntsa::IcmpUnreachable& IcmpPayload::makeDestinationUnreachable()
{
    if (d_type != ntsa::IcmpType::e_DESTINATION_UNREACHABLE) {
        this->reset();
        new (d_destinationUnreachable.buffer()) ntsa::IcmpUnreachable();
        d_type = ntsa::IcmpType::e_DESTINATION_UNREACHABLE;
    }

    return d_destinationUnreachable.object();
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

ntsa::IcmpPing& IcmpPayload::makeEcho()
{
    if (d_type == ntsa::IcmpType::e_ECHO) {
        d_echo.object().reset();
    }
    else {
        this->reset();
        new (d_echo.buffer()) ntsa::IcmpPing();
        d_type = ntsa::IcmpType::e_ECHO;
    }

    return d_echo.object();
}

ntsa::IcmpPing& IcmpPayload::makeEcho(const ntsa::IcmpPing& value)
{
    if (d_type == ntsa::IcmpType::e_ECHO) {
        d_echo.object() = value;
    }
    else {
        this->reset();
        new (d_echo.buffer()) ntsa::IcmpPing(value);
        d_type = ntsa::IcmpType::e_ECHO;
    }

    return d_echo.object();
}

ntsa::IcmpTimeout& IcmpPayload::makeTimeExceeded()
{
    if (d_type != ntsa::IcmpType::e_TIME_EXCEEDED) {
        this->reset();
        new (d_timeExceeded.buffer()) ntsa::IcmpTimeout();
        d_type = ntsa::IcmpType::e_TIME_EXCEEDED;
    }

    return d_timeExceeded.object();
}

ntsa::IcmpProblem& IcmpPayload::makeParameterProblem()
{
    if (d_type == ntsa::IcmpType::e_PARAMETER_PROBLEM) {
        d_parameterProblem.object().reset();
    }
    else {
        this->reset();
        new (d_parameterProblem.buffer()) ntsa::IcmpProblem();
        d_type = ntsa::IcmpType::e_PARAMETER_PROBLEM;
    }

    return d_parameterProblem.object();
}

ntsa::IcmpProblem& IcmpPayload::makeParameterProblem(
    const ntsa::IcmpProblem& value)
{
    if (d_type == ntsa::IcmpType::e_PARAMETER_PROBLEM) {
        d_parameterProblem.object() = value;
    }
    else {
        this->reset();
        new (d_parameterProblem.buffer()) ntsa::IcmpProblem(value);
        d_type = ntsa::IcmpType::e_PARAMETER_PROBLEM;
    }

    return d_parameterProblem.object();
}

ntsa::IcmpPong& IcmpPayload::echoReply()
{
    BSLS_ASSERT(isEchoReply());
    return d_echoReply.object();
}

ntsa::IcmpUnreachable& IcmpPayload::destinationUnreachable()
{
    BSLS_ASSERT(isDestinationUnreachable());
    return d_destinationUnreachable.object();
}

ntsa::IcmpRedirect& IcmpPayload::redirect()
{
    BSLS_ASSERT(isRedirect());
    return d_redirect.object();
}

ntsa::IcmpPing& IcmpPayload::echo()
{
    BSLS_ASSERT(isEcho());
    return d_echo.object();
}

ntsa::IcmpTimeout& IcmpPayload::timeExceeded()
{
    BSLS_ASSERT(isTimeExceeded());
    return d_timeExceeded.object();
}

ntsa::IcmpProblem& IcmpPayload::parameterProblem()
{
    BSLS_ASSERT(isParameterProblem());
    return d_parameterProblem.object();
}

ntsa::Error IcmpPayload::decode(const bdlbb::BlobBuffer& buffer,
                                bsl::size_t              offset,
                                bsl::size_t              packetSize,
                                ntsa::IcmpType::Value    type)
{
    this->reset();

    switch (type) {
    case ntsa::IcmpType::e_ECHO_REPLY:
        return this->makeEchoReply().decode(buffer, offset, packetSize);
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        return this->makeDestinationUnreachable().decode(
            buffer, offset, packetSize);
    case ntsa::IcmpType::e_REDIRECT:
        return this->makeRedirect().decode(buffer, offset, packetSize);
    case ntsa::IcmpType::e_ECHO:
        return this->makeEcho().decode(buffer, offset, packetSize);
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        return this->makeTimeExceeded().decode(buffer, offset, packetSize);
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        return this->makeParameterProblem().decode(buffer, offset, packetSize);
    default:
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error IcmpPayload::encode(bdlbb::BlobBuffer* buffer,
                                bsl::size_t        offset) const
{
    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REPLY:
        return d_echoReply.object().encode(buffer, offset);
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        return d_destinationUnreachable.object().encode(buffer, offset);
    case ntsa::IcmpType::e_REDIRECT:
        return d_redirect.object().encode(buffer, offset);
    case ntsa::IcmpType::e_ECHO:
        return d_echo.object().encode(buffer, offset);
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        return d_timeExceeded.object().encode(buffer, offset);
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        return d_parameterProblem.object().encode(buffer, offset);
    default:
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

const ntsa::IcmpPong& IcmpPayload::echoReply() const
{
    BSLS_ASSERT(isEchoReply());
    return d_echoReply.object();
}

const ntsa::IcmpUnreachable& IcmpPayload::destinationUnreachable() const
{
    BSLS_ASSERT(isDestinationUnreachable());
    return d_destinationUnreachable.object();
}

const ntsa::IcmpRedirect& IcmpPayload::redirect() const
{
    BSLS_ASSERT(isRedirect());
    return d_redirect.object();
}

const ntsa::IcmpPing& IcmpPayload::echo() const
{
    BSLS_ASSERT(isEcho());
    return d_echo.object();
}

const ntsa::IcmpTimeout& IcmpPayload::timeExceeded() const
{
    BSLS_ASSERT(isTimeExceeded());
    return d_timeExceeded.object();
}

const ntsa::IcmpProblem& IcmpPayload::parameterProblem() const
{
    BSLS_ASSERT(isParameterProblem());
    return d_parameterProblem.object();
}

ntsa::IcmpType::Value IcmpPayload::type() const
{
    return d_type;
}

const char* IcmpPayload::name() const
{
    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REPLY:
        return "echoReply";
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        return "destinationUnreachable";
    case ntsa::IcmpType::e_REDIRECT:
        return "redirect";
    case ntsa::IcmpType::e_ECHO:
        return "echo";
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        return "timeExceeded";
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        return "parameterProblem";
    default:
        return "???";
    }
}

bool IcmpPayload::isUndefined() const
{
    return d_type == ntsa::IcmpType::e_UNDEFINED;
}

bool IcmpPayload::isEchoReply() const
{
    return d_type == ntsa::IcmpType::e_ECHO_REPLY;
}

bool IcmpPayload::isDestinationUnreachable() const
{
    return d_type == ntsa::IcmpType::e_DESTINATION_UNREACHABLE;
}

bool IcmpPayload::isRedirect() const
{
    return d_type == ntsa::IcmpType::e_REDIRECT;
}

bool IcmpPayload::isEcho() const
{
    return d_type == ntsa::IcmpType::e_ECHO;
}

bool IcmpPayload::isTimeExceeded() const
{
    return d_type == ntsa::IcmpType::e_TIME_EXCEEDED;
}

bool IcmpPayload::isParameterProblem() const
{
    return d_type == ntsa::IcmpType::e_PARAMETER_PROBLEM;
}

bool IcmpPayload::equals(const IcmpPayload& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REPLY:
        return d_echoReply.object().equals(other.d_echoReply.object());
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        return d_destinationUnreachable.object().equals(
            other.d_destinationUnreachable.object());
    case ntsa::IcmpType::e_REDIRECT:
        return d_redirect.object().equals(other.d_redirect.object());
    case ntsa::IcmpType::e_ECHO:
        return d_echo.object().equals(other.d_echo.object());
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        return d_timeExceeded.object().equals(other.d_timeExceeded.object());
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        return d_parameterProblem.object().equals(
            other.d_parameterProblem.object());
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
    case ntsa::IcmpType::e_ECHO_REPLY:
        return d_echoReply.object().less(other.d_echoReply.object());
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        return d_destinationUnreachable.object().less(
            other.d_destinationUnreachable.object());
    case ntsa::IcmpType::e_REDIRECT:
        return d_redirect.object().less(other.d_redirect.object());
    case ntsa::IcmpType::e_ECHO:
        return d_echo.object().less(other.d_echo.object());
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        return d_timeExceeded.object().less(other.d_timeExceeded.object());
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        return d_parameterProblem.object().less(
            other.d_parameterProblem.object());
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
    if (d_type == ntsa::IcmpType::e_ECHO_REPLY) {
        printer->printAttribute("echoReply", d_echoReply.object());
    }
    else if (d_type == ntsa::IcmpType::e_DESTINATION_UNREACHABLE) {
        printer->printAttribute("destinationUnreachable",
                                d_destinationUnreachable.object());
    }
    else if (d_type == ntsa::IcmpType::e_REDIRECT) {
        printer->printAttribute("redirect", d_redirect.object());
    }
    else if (d_type == ntsa::IcmpType::e_ECHO) {
        printer->printAttribute("echo", d_echo.object());
    }
    else if (d_type == ntsa::IcmpType::e_TIME_EXCEEDED) {
        printer->printAttribute("timeExceeded", d_timeExceeded.object());
    }
    else if (d_type == ntsa::IcmpType::e_PARAMETER_PROBLEM) {
        printer->printAttribute("parameterProblem",
                                d_parameterProblem.object());
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
