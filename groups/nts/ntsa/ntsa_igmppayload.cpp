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

#include <ntsa_igmppayload.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_igmppayload_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

IgmpPayload::IgmpPayload(bslma::Allocator* basicAllocator)
: d_type(ntsa::IgmpType::e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

IgmpPayload::IgmpPayload(bslmf::MovableRef<IgmpPayload> original)
    NTSCFG_NOEXCEPT : d_type(ntsa::IgmpType::e_UNDEFINED)
{
    IgmpPayload& ref = bslmf::MovableRefUtil::access(original);

    d_type        = ref.d_type;
    d_allocator_p = ref.d_allocator_p;

    switch (d_type) {
    case ntsa::IgmpType::e_REPORT_V2:
        new (d_join.buffer()) ntsa::IgmpJoin(ref.d_join.object());
        break;
    case ntsa::IgmpType::e_LEAVE:
        new (d_leave.buffer()) ntsa::IgmpLeave(ref.d_leave.object());
        break;
    case ntsa::IgmpType::e_QUERY:
        new (d_query.buffer())
            ntsa::IgmpQuery(ref.d_query.object(), d_allocator_p);
        break;
    case ntsa::IgmpType::e_REPORT_V3:
        new (d_report.buffer())
            ntsa::IgmpReport(ref.d_report.object(), d_allocator_p);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IgmpType::e_UNDEFINED);
    }

    ref.reset();
}

IgmpPayload::IgmpPayload(const IgmpPayload& original,
                         bslma::Allocator*  basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_type) {
    case ntsa::IgmpType::e_REPORT_V2:
        new (d_join.buffer()) ntsa::IgmpJoin(original.d_join.object());
        break;
    case ntsa::IgmpType::e_LEAVE:
        new (d_leave.buffer()) ntsa::IgmpLeave(original.d_leave.object());
        break;
    case ntsa::IgmpType::e_QUERY:
        new (d_query.buffer())
            ntsa::IgmpQuery(original.d_query.object(), d_allocator_p);
        break;
    case ntsa::IgmpType::e_REPORT_V3:
        new (d_report.buffer())
            ntsa::IgmpReport(original.d_report.object(), d_allocator_p);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IgmpType::e_UNDEFINED);
    }
}

IgmpPayload::~IgmpPayload()
{
    this->reset();
}

IgmpPayload& IgmpPayload::operator=(bslmf::MovableRef<IgmpPayload> other)
    NTSCFG_NOEXCEPT
{
    IgmpPayload& ref = bslmf::MovableRefUtil::access(other);

    if (this == &ref) {
        return *this;
    }

    this->reset();

    d_type = ref.d_type;

    switch (d_type) {
    case ntsa::IgmpType::e_REPORT_V2:
        new (d_join.buffer()) ntsa::IgmpJoin(ref.d_join.object());
        break;
    case ntsa::IgmpType::e_LEAVE:
        new (d_leave.buffer()) ntsa::IgmpLeave(ref.d_leave.object());
        break;
    case ntsa::IgmpType::e_QUERY:
        new (d_query.buffer())
            ntsa::IgmpQuery(ref.d_query.object(), d_allocator_p);
        break;
    case ntsa::IgmpType::e_REPORT_V3:
        new (d_report.buffer())
            ntsa::IgmpReport(ref.d_report.object(), d_allocator_p);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IgmpType::e_UNDEFINED);
    }

    ref.reset();

    return *this;
}

IgmpPayload& IgmpPayload::operator=(const IgmpPayload& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    d_type = other.d_type;

    switch (d_type) {
    case ntsa::IgmpType::e_REPORT_V2:
        new (d_join.buffer()) ntsa::IgmpJoin(other.d_join.object());
        break;
    case ntsa::IgmpType::e_LEAVE:
        new (d_leave.buffer()) ntsa::IgmpLeave(other.d_leave.object());
        break;
    case ntsa::IgmpType::e_QUERY:
        new (d_query.buffer())
            ntsa::IgmpQuery(other.d_query.object(), d_allocator_p);
        break;
    case ntsa::IgmpType::e_REPORT_V3:
        new (d_report.buffer())
            ntsa::IgmpReport(other.d_report.object(), d_allocator_p);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IgmpType::e_UNDEFINED);
    }

    return *this;
}

void IgmpPayload::reset()
{
    switch (d_type) {
    case ntsa::IgmpType::e_REPORT_V2: {
        typedef ntsa::IgmpJoin Type;
        d_join.object().~Type();
    } break;
    case ntsa::IgmpType::e_LEAVE: {
        typedef ntsa::IgmpLeave Type;
        d_leave.object().~Type();
    } break;
    case ntsa::IgmpType::e_QUERY: {
        typedef ntsa::IgmpQuery Type;
        d_query.object().~Type();
    } break;
    case ntsa::IgmpType::e_REPORT_V3: {
        typedef ntsa::IgmpReport Type;
        d_report.object().~Type();
    } break;
    default:
        BSLS_ASSERT(d_type == ntsa::IgmpType::e_UNDEFINED);
    }

    d_type = ntsa::IgmpType::e_UNDEFINED;
}

ntsa::IgmpJoin& IgmpPayload::makeJoin()
{
    if (d_type == ntsa::IgmpType::e_REPORT_V2) {
        d_join.object().reset();
    }
    else {
        this->reset();
        new (d_join.buffer()) ntsa::IgmpJoin();
        d_type = ntsa::IgmpType::e_REPORT_V2;
    }

    return d_join.object();
}

ntsa::IgmpJoin& IgmpPayload::makeJoin(const ntsa::IgmpJoin& value)
{
    if (d_type == ntsa::IgmpType::e_REPORT_V2) {
        d_join.object() = value;
    }
    else {
        this->reset();
        new (d_join.buffer()) ntsa::IgmpJoin(value);
        d_type = ntsa::IgmpType::e_REPORT_V2;
    }

    return d_join.object();
}

ntsa::IgmpLeave& IgmpPayload::makeLeave()
{
    if (d_type == ntsa::IgmpType::e_LEAVE) {
        d_leave.object().reset();
    }
    else {
        this->reset();
        new (d_leave.buffer()) ntsa::IgmpLeave();
        d_type = ntsa::IgmpType::e_LEAVE;
    }

    return d_leave.object();
}

ntsa::IgmpLeave& IgmpPayload::makeLeave(const ntsa::IgmpLeave& value)
{
    if (d_type == ntsa::IgmpType::e_LEAVE) {
        d_leave.object() = value;
    }
    else {
        this->reset();
        new (d_leave.buffer()) ntsa::IgmpLeave(value);
        d_type = ntsa::IgmpType::e_LEAVE;
    }

    return d_leave.object();
}

ntsa::IgmpQuery& IgmpPayload::makeQuery()
{
    if (d_type == ntsa::IgmpType::e_QUERY) {
        d_query.object().reset();
    }
    else {
        this->reset();
        new (d_query.buffer()) ntsa::IgmpQuery(d_allocator_p);
        d_type = ntsa::IgmpType::e_QUERY;
    }

    return d_query.object();
}

ntsa::IgmpQuery& IgmpPayload::makeQuery(const ntsa::IgmpQuery& value)
{
    if (d_type == ntsa::IgmpType::e_QUERY) {
        d_query.object() = value;
    }
    else {
        this->reset();
        new (d_query.buffer()) ntsa::IgmpQuery(value, d_allocator_p);
        d_type = ntsa::IgmpType::e_QUERY;
    }

    return d_query.object();
}

ntsa::IgmpReport& IgmpPayload::makeReport()
{
    if (d_type == ntsa::IgmpType::e_REPORT_V3) {
        d_report.object().reset();
    }
    else {
        this->reset();
        new (d_report.buffer()) ntsa::IgmpReport(d_allocator_p);
        d_type = ntsa::IgmpType::e_REPORT_V3;
    }

    return d_report.object();
}

ntsa::IgmpReport& IgmpPayload::makeReport(const ntsa::IgmpReport& value)
{
    if (d_type == ntsa::IgmpType::e_REPORT_V3) {
        d_report.object() = value;
    }
    else {
        this->reset();
        new (d_report.buffer()) ntsa::IgmpReport(value, d_allocator_p);
        d_type = ntsa::IgmpType::e_REPORT_V3;
    }

    return d_report.object();
}

ntsa::IgmpJoin& IgmpPayload::join()
{
    BSLS_ASSERT(isJoin());
    return d_join.object();
}

ntsa::IgmpLeave& IgmpPayload::leave()
{
    BSLS_ASSERT(isLeave());
    return d_leave.object();
}

ntsa::IgmpQuery& IgmpPayload::query()
{
    BSLS_ASSERT(isQuery());
    return d_query.object();
}

ntsa::IgmpReport& IgmpPayload::report()
{
    BSLS_ASSERT(isReport());
    return d_report.object();
}

const ntsa::IgmpJoin& IgmpPayload::join() const
{
    BSLS_ASSERT(isJoin());
    return d_join.object();
}

const ntsa::IgmpLeave& IgmpPayload::leave() const
{
    BSLS_ASSERT(isLeave());
    return d_leave.object();
}

const ntsa::IgmpQuery& IgmpPayload::query() const
{
    BSLS_ASSERT(isQuery());
    return d_query.object();
}

const ntsa::IgmpReport& IgmpPayload::report() const
{
    BSLS_ASSERT(isReport());
    return d_report.object();
}

ntsa::IgmpType::Value IgmpPayload::type() const
{
    return d_type;
}

const char* IgmpPayload::name() const
{
    switch (d_type) {
    case ntsa::IgmpType::e_REPORT_V2:
        return "join";
    case ntsa::IgmpType::e_LEAVE:
        return "leave";
    case ntsa::IgmpType::e_QUERY:
        return "query";
    case ntsa::IgmpType::e_REPORT_V3:
        return "report";
    default:
        return "???";
    }
}

bool IgmpPayload::isUndefined() const
{
    return d_type == ntsa::IgmpType::e_UNDEFINED;
}

bool IgmpPayload::isJoin() const
{
    return d_type == ntsa::IgmpType::e_REPORT_V2;
}

bool IgmpPayload::isLeave() const
{
    return d_type == ntsa::IgmpType::e_LEAVE;
}

bool IgmpPayload::isQuery() const
{
    return d_type == ntsa::IgmpType::e_QUERY;
}

bool IgmpPayload::isReport() const
{
    return d_type == ntsa::IgmpType::e_REPORT_V3;
}

bool IgmpPayload::equals(const IgmpPayload& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::IgmpType::e_REPORT_V2:
        return d_join.object().equals(other.d_join.object());
    case ntsa::IgmpType::e_LEAVE:
        return d_leave.object().equals(other.d_leave.object());
    case ntsa::IgmpType::e_QUERY:
        return d_query.object().equals(other.d_query.object());
    case ntsa::IgmpType::e_REPORT_V3:
        return d_report.object().equals(other.d_report.object());
    default:
        BSLS_ASSERT(d_type == ntsa::IgmpType::e_UNDEFINED);
        return true;
    }
}

bool IgmpPayload::less(const IgmpPayload& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::IgmpType::e_REPORT_V2:
        return d_join.object().less(other.d_join.object());
    case ntsa::IgmpType::e_LEAVE:
        return d_leave.object().less(other.d_leave.object());
    case ntsa::IgmpType::e_QUERY:
        return d_query.object().less(other.d_query.object());
    case ntsa::IgmpType::e_REPORT_V3:
        return d_report.object().less(other.d_report.object());
    default:
        BSLS_ASSERT(d_type == ntsa::IgmpType::e_UNDEFINED);
        return false;
    }
}

bsl::ostream& IgmpPayload::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void IgmpPayload::print(bslim::Printer* printer) const
{
    if (d_type == ntsa::IgmpType::e_REPORT_V2) {
        printer->printAttribute("join", d_join.object());
    }
    else if (d_type == ntsa::IgmpType::e_LEAVE) {
        printer->printAttribute("leave", d_leave.object());
    }
    else if (d_type == ntsa::IgmpType::e_QUERY) {
        printer->printAttribute("query", d_query.object());
    }
    else if (d_type == ntsa::IgmpType::e_REPORT_V3) {
        printer->printAttribute("report", d_report.object());
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const IgmpPayload& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const IgmpPayload& lhs, const IgmpPayload& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const IgmpPayload& lhs, const IgmpPayload& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const IgmpPayload& lhs, const IgmpPayload& rhs)
{
    return lhs.less(rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
