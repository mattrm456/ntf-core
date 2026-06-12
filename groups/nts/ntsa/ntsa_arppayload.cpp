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

#include <ntsa_arppayload.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_arppayload_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

ArpPayload::ArpPayload(bslma::Allocator* basicAllocator)
: d_type(ntsa::ArpType::e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ArpPayload::ArpPayload(bslmf::MovableRef<ArpPayload> original)
    NTSCFG_NOEXCEPT : d_type(ntsa::ArpType::e_UNDEFINED)
{
    ArpPayload& ref = bslmf::MovableRefUtil::access(original);

    d_type        = ref.d_type;
    d_allocator_p = ref.d_allocator_p;

    switch (d_type) {
    case ntsa::ArpType::e_REQUEST:
        new (d_request.buffer())
            ntsa::ArpRequest(ref.d_request.object());
        break;
    case ntsa::ArpType::e_RESPONSE:
        new (d_response.buffer())
            ntsa::ArpResponse(ref.d_response.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::ArpType::e_UNDEFINED);
    }

    ref.reset();
}

ArpPayload::ArpPayload(const ArpPayload& original,
                         bslma::Allocator*  basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_type) {
    case ntsa::ArpType::e_REQUEST:
        new (d_request.buffer())
            ntsa::ArpRequest(original.d_request.object());
        break;
    case ntsa::ArpType::e_RESPONSE:
        new (d_response.buffer())
            ntsa::ArpResponse(original.d_response.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::ArpType::e_UNDEFINED);
    }
}

ArpPayload::~ArpPayload()
{
    this->reset();
}

ArpPayload& ArpPayload::operator=(bslmf::MovableRef<ArpPayload> other)
    NTSCFG_NOEXCEPT
{
    ArpPayload& ref = bslmf::MovableRefUtil::access(other);

    if (this == &ref) {
        return *this;
    }

    this->reset();

    d_type = ref.d_type;

    switch (d_type) {
    case ntsa::ArpType::e_REQUEST:
        new (d_request.buffer())
            ntsa::ArpRequest(ref.d_request.object());
        break;
    case ntsa::ArpType::e_RESPONSE:
        new (d_response.buffer())
            ntsa::ArpResponse(ref.d_response.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::ArpType::e_UNDEFINED);
    }

    ref.reset();

    return *this;
}

ArpPayload& ArpPayload::operator=(const ArpPayload& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    d_type = other.d_type;

    switch (d_type) {
    case ntsa::ArpType::e_REQUEST:
        new (d_request.buffer())
            ntsa::ArpRequest(other.d_request.object());
        break;
    case ntsa::ArpType::e_RESPONSE:
        new (d_response.buffer())
            ntsa::ArpResponse(other.d_response.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::ArpType::e_UNDEFINED);
    }

    return *this;
}

void ArpPayload::reset()
{
    switch (d_type) {
    case ntsa::ArpType::e_REQUEST: {
        typedef ntsa::ArpRequest Type;
        d_request.object().~Type();
    } break;
    case ntsa::ArpType::e_RESPONSE: {
        typedef ntsa::ArpResponse Type;
        d_response.object().~Type();
    } break;
    default:
        BSLS_ASSERT(d_type == ntsa::ArpType::e_UNDEFINED);
    }

    d_type = ntsa::ArpType::e_UNDEFINED;
}

ntsa::ArpRequest& ArpPayload::makeRequest()
{
    if (d_type == ntsa::ArpType::e_REQUEST) {
        d_request.object().reset();
    }
    else {
        this->reset();
        new (d_request.buffer()) ntsa::ArpRequest();
        d_type = ntsa::ArpType::e_REQUEST;
    }

    return d_request.object();
}

ntsa::ArpRequest& ArpPayload::makeRequest(
    const ntsa::ArpRequest& value)
{
    if (d_type == ntsa::ArpType::e_REQUEST) {
        d_request.object() = value;
    }
    else {
        this->reset();
        new (d_request.buffer()) ntsa::ArpRequest(value);
        d_type = ntsa::ArpType::e_REQUEST;
    }

    return d_request.object();
}

ntsa::ArpResponse& ArpPayload::makeResponse()
{
    if (d_type == ntsa::ArpType::e_RESPONSE) {
        d_response.object().reset();
    }
    else {
        this->reset();
        new (d_response.buffer()) ntsa::ArpResponse();
        d_type = ntsa::ArpType::e_RESPONSE;
    }

    return d_response.object();
}

ntsa::ArpResponse& ArpPayload::makeResponse(
    const ntsa::ArpResponse& value)
{
    if (d_type == ntsa::ArpType::e_RESPONSE) {
        d_response.object() = value;
    }
    else {
        this->reset();
        new (d_response.buffer()) ntsa::ArpResponse(value);
        d_type = ntsa::ArpType::e_RESPONSE;
    }

    return d_response.object();
}

ntsa::ArpRequest& ArpPayload::request()
{
    BSLS_ASSERT(isRequest());
    return d_request.object();
}

ntsa::ArpResponse& ArpPayload::response()
{
    BSLS_ASSERT(isResponse());
    return d_response.object();
}

const ntsa::ArpRequest& ArpPayload::request() const
{
    BSLS_ASSERT(isRequest());
    return d_request.object();
}

const ntsa::ArpResponse& ArpPayload::response() const
{
    BSLS_ASSERT(isResponse());
    return d_response.object();
}

ntsa::ArpType::Value ArpPayload::type() const
{
    return d_type;
}

const char* ArpPayload::name() const
{
    switch (d_type) {
    case ntsa::ArpType::e_REQUEST:
        return "request";
    case ntsa::ArpType::e_RESPONSE:
        return "response";
    default:
        return "???";
    }
}

bool ArpPayload::isUndefined() const
{
    return d_type == ntsa::ArpType::e_UNDEFINED;
}

bool ArpPayload::isRequest() const
{
    return d_type == ntsa::ArpType::e_REQUEST;
}

bool ArpPayload::isResponse() const
{
    return d_type == ntsa::ArpType::e_RESPONSE;
}

bool ArpPayload::equals(const ArpPayload& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::ArpType::e_REQUEST:
        return d_request.object().equals(other.d_request.object());
    case ntsa::ArpType::e_RESPONSE:
        return d_response.object().equals(other.d_response.object());
    default:
        BSLS_ASSERT(d_type == ntsa::ArpType::e_UNDEFINED);
        return true;
    }
}

bool ArpPayload::less(const ArpPayload& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::ArpType::e_REQUEST:
        return d_request.object().less(other.d_request.object());
    case ntsa::ArpType::e_RESPONSE:
        return d_response.object().less(other.d_response.object());
    default:
        BSLS_ASSERT(d_type == ntsa::ArpType::e_UNDEFINED);
        return false;
    }
}

bsl::ostream& ArpPayload::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void ArpPayload::print(bslim::Printer* printer) const
{
    if (d_type == ntsa::ArpType::e_REQUEST) {
        printer->printAttribute("request", d_request.object());
    }
    else if (d_type == ntsa::ArpType::e_RESPONSE) {
        printer->printAttribute("response", d_response.object());
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const ArpPayload& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const ArpPayload& lhs, const ArpPayload& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const ArpPayload& lhs, const ArpPayload& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const ArpPayload& lhs, const ArpPayload& rhs)
{
    return lhs.less(rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
