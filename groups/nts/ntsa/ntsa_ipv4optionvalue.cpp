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

#include <ntsa_ipv4optionvalue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4optionvalue_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

Ipv4OptionValue::Ipv4OptionValue(bslma::Allocator* basicAllocator)
: d_kind(0)
, d_payload(basicAllocator)
{
}

Ipv4OptionValue::Ipv4OptionValue(const Ipv4OptionValue& other,
                                 bslma::Allocator*      basicAllocator)
: d_kind(other.d_kind)
, d_payload(other.d_payload, basicAllocator)
{
}

Ipv4OptionValue::~Ipv4OptionValue()
{
}

Ipv4OptionValue& Ipv4OptionValue::operator=(const Ipv4OptionValue& other)
{
    if (this != &other) {
        d_kind    = other.d_kind;
        d_payload = other.d_payload;
    }

    return *this;
}

void Ipv4OptionValue::reset()
{
    d_kind = 0;
    d_payload.clear();
}

void Ipv4OptionValue::setKind(bsl::uint8_t value)
{
    d_kind = value;
}

void Ipv4OptionValue::setPayload(const bsl::vector<bsl::uint8_t>& value)
{
    d_payload = value;
}

bsl::uint8_t Ipv4OptionValue::kind() const
{
    return d_kind;
}

const bsl::vector<bsl::uint8_t>& Ipv4OptionValue::payload() const
{
    return d_payload;
}

bool Ipv4OptionValue::equals(const Ipv4OptionValue& other) const
{
    return d_kind == other.d_kind && d_payload == other.d_payload;
}

bool Ipv4OptionValue::less(const Ipv4OptionValue& other) const
{
    if (d_kind < other.d_kind) {
        return true;
    }

    if (other.d_kind < d_kind) {
        return false;
    }

    return d_payload < other.d_payload;
}

bsl::ostream& Ipv4OptionValue::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("kind", static_cast<int>(d_kind));
    printer.printAttribute("payload", d_payload);

    printer.end();

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&         stream,
                         const Ipv4OptionValue& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Ipv4OptionValue& lhs,
                const Ipv4OptionValue& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Ipv4OptionValue& lhs,
                const Ipv4OptionValue& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Ipv4OptionValue& lhs,
               const Ipv4OptionValue& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
