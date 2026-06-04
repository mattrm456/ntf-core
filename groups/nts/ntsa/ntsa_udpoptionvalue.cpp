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

#include <ntsa_udpoptionvalue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_udpoptionvalue_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

UdpOptionValue::UdpOptionValue(bslma::Allocator* basicAllocator)
: d_kind(0)
, d_payload(basicAllocator)
{
}

UdpOptionValue::UdpOptionValue(const UdpOptionValue& other,
                               bslma::Allocator*     basicAllocator)
: d_kind(other.d_kind)
, d_payload(other.d_payload, basicAllocator)
{
}

UdpOptionValue::~UdpOptionValue()
{
}

UdpOptionValue& UdpOptionValue::operator=(const UdpOptionValue& other)
{
    if (this != &other) {
        d_kind    = other.d_kind;
        d_payload = other.d_payload;
    }

    return *this;
}

void UdpOptionValue::reset()
{
    d_kind = 0;
    d_payload.clear();
}

void UdpOptionValue::setKind(bsl::uint8_t value)
{
    d_kind = value;
}

void UdpOptionValue::setPayload(const bsl::vector<bsl::uint8_t>& value)
{
    d_payload = value;
}

bsl::uint8_t UdpOptionValue::kind() const
{
    return d_kind;
}

const bsl::vector<bsl::uint8_t>& UdpOptionValue::payload() const
{
    return d_payload;
}

bool UdpOptionValue::equals(const UdpOptionValue& other) const
{
    return d_kind == other.d_kind && d_payload == other.d_payload;
}

bool UdpOptionValue::less(const UdpOptionValue& other) const
{
    if (d_kind < other.d_kind) {
        return true;
    }

    if (other.d_kind < d_kind) {
        return false;
    }

    return d_payload < other.d_payload;
}

bsl::ostream& UdpOptionValue::print(bsl::ostream& stream,
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
                         const UdpOptionValue& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const UdpOptionValue& lhs,
                const UdpOptionValue& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const UdpOptionValue& lhs,
                const UdpOptionValue& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const UdpOptionValue& lhs,
               const UdpOptionValue& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
