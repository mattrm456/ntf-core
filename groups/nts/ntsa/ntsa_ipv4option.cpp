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

#include <ntsa_ipv4option.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4option_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsa {

bsl::size_t Ipv4Option::paddingSize(const bsl::uint8_t* cursor,
                                   bsl::size_t         optionSize)
{
    const bsl::size_t address = static_cast<bsl::size_t>(
        reinterpret_cast<bsl::uintptr_t>(cursor + optionSize));

    return (4 - (address & 3)) & 3;
}

Ipv4Option::Ipv4Option(bslma::Allocator* basicAllocator)
: d_type(ntsa::Ipv4OptionType::e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Ipv4Option::Ipv4Option(const Ipv4Option& other, bslma::Allocator* basicAllocator)
: d_type(other.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == ntsa::Ipv4OptionType::e_TIMESTAMP) {
        new (d_timestamp.buffer())
            ntsa::Ipv4RouteLedger(other.d_timestamp.object(), d_allocator_p);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_RECORD_ROUTE) {
        new (d_recordRoute.buffer())
            ntsa::Ipv4RouteSequence(other.d_recordRoute.object(), d_allocator_p);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE) {
        new (d_sourceRouteLoose.buffer())
            ntsa::Ipv4RouteSequence(other.d_sourceRouteLoose.object(), d_allocator_p);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT) {
        new (d_sourceRouteTight.buffer())
            ntsa::Ipv4RouteSequence(other.d_sourceRouteTight.object(), d_allocator_p);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_UNASSIGNED) {
        new (d_unassigned.buffer())
            ntsa::Ipv4OptionValue(other.d_unassigned.object(), d_allocator_p);
    }
}

Ipv4Option::~Ipv4Option()
{
    if (d_type == ntsa::Ipv4OptionType::e_TIMESTAMP) {
        typedef ntsa::Ipv4RouteLedger Type;
        d_timestamp.object().~Type();
    }
    else if (d_type == ntsa::Ipv4OptionType::e_RECORD_ROUTE) {
        typedef ntsa::Ipv4RouteSequence Type;
        d_recordRoute.object().~Type();
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE) {
        typedef ntsa::Ipv4RouteSequence Type;
        d_sourceRouteLoose.object().~Type();
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT) {
        typedef ntsa::Ipv4RouteSequence Type;
        d_sourceRouteTight.object().~Type();
    }
    else if (d_type == ntsa::Ipv4OptionType::e_UNASSIGNED) {
        typedef ntsa::Ipv4OptionValue Type;
        d_unassigned.object().~Type();
    }
}

Ipv4Option& Ipv4Option::operator=(const Ipv4Option& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    if (d_type == ntsa::Ipv4OptionType::e_TIMESTAMP) {
        new (d_timestamp.buffer())
            ntsa::Ipv4RouteLedger(other.d_timestamp.object(), d_allocator_p);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_RECORD_ROUTE) {
        new (d_recordRoute.buffer())
            ntsa::Ipv4RouteSequence(other.d_recordRoute.object(), d_allocator_p);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE) {
        new (d_sourceRouteLoose.buffer())
            ntsa::Ipv4RouteSequence(other.d_sourceRouteLoose.object(), d_allocator_p);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT) {
        new (d_sourceRouteTight.buffer())
            ntsa::Ipv4RouteSequence(other.d_sourceRouteTight.object(), d_allocator_p);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_UNASSIGNED) {
        new (d_unassigned.buffer())
            ntsa::Ipv4OptionValue(other.d_unassigned.object(), d_allocator_p);
    }

    d_type = other.d_type;

    return *this;
}

void Ipv4Option::reset()
{
    if (d_type == ntsa::Ipv4OptionType::e_TIMESTAMP) {
        typedef ntsa::Ipv4RouteLedger Type;
        d_timestamp.object().~Type();
    }
    else if (d_type == ntsa::Ipv4OptionType::e_RECORD_ROUTE) {
        typedef ntsa::Ipv4RouteSequence Type;
        d_recordRoute.object().~Type();
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE) {
        typedef ntsa::Ipv4RouteSequence Type;
        d_sourceRouteLoose.object().~Type();
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT) {
        typedef ntsa::Ipv4RouteSequence Type;
        d_sourceRouteTight.object().~Type();
    }
    else if (d_type == ntsa::Ipv4OptionType::e_UNASSIGNED) {
        typedef ntsa::Ipv4OptionValue Type;
        d_unassigned.object().~Type();
    }

    d_type = ntsa::Ipv4OptionType::e_UNDEFINED;
}

void Ipv4Option::makePadding()
{
    if (d_type != ntsa::Ipv4OptionType::e_PADDING) {
        reset();
        d_type = ntsa::Ipv4OptionType::e_PADDING;
    }
}

void Ipv4Option::makeAlert()
{
    if (d_type != ntsa::Ipv4OptionType::e_ALERT) {
        reset();
        d_type = ntsa::Ipv4OptionType::e_ALERT;
    }
}

ntsa::Ipv4RouteLedger& Ipv4Option::makeTimestamp()
{
    if (d_type == ntsa::Ipv4OptionType::e_TIMESTAMP) {
        d_timestamp.object().reset();
    }
    else {
        this->reset();
        new (d_timestamp.buffer())
            ntsa::Ipv4RouteLedger(d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_TIMESTAMP;
    }

    return d_timestamp.object();
}

ntsa::Ipv4RouteLedger& Ipv4Option::makeTimestamp(
    const ntsa::Ipv4RouteLedger& value)
{
    if (d_type == ntsa::Ipv4OptionType::e_TIMESTAMP) {
        d_timestamp.object() = value;
    }
    else {
        this->reset();
        new (d_timestamp.buffer())
            ntsa::Ipv4RouteLedger(value, d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_TIMESTAMP;
    }

    return d_timestamp.object();
}


ntsa::Ipv4RouteSequence& Ipv4Option::makeRecordRoute()
{
    if (d_type == ntsa::Ipv4OptionType::e_RECORD_ROUTE) {
        d_recordRoute.object().reset();
    }
    else {
        this->reset();
        new (d_recordRoute.buffer())
            ntsa::Ipv4RouteSequence(d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_RECORD_ROUTE;
    }

    return d_recordRoute.object();
}

ntsa::Ipv4RouteSequence& Ipv4Option::makeRecordRoute(
    const ntsa::Ipv4RouteSequence& value)
{
    if (d_type == ntsa::Ipv4OptionType::e_RECORD_ROUTE) {
        d_recordRoute.object() = value;
    }
    else {
        this->reset();
        new (d_recordRoute.buffer())
            ntsa::Ipv4RouteSequence(value, d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_RECORD_ROUTE;
    }

    return d_recordRoute.object();
}

ntsa::Ipv4RouteSequence& Ipv4Option::makeSourceRouteLoose()
{
    if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE) {
        d_sourceRouteLoose.object().reset();
    }
    else {
        this->reset();
        new (d_sourceRouteLoose.buffer())
            ntsa::Ipv4RouteSequence(d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE;
    }

    return d_sourceRouteLoose.object();
}

ntsa::Ipv4RouteSequence& Ipv4Option::makeSourceRouteLoose(
    const ntsa::Ipv4RouteSequence& value)
{
    if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE) {
        d_sourceRouteLoose.object() = value;
    }
    else {
        this->reset();
        new (d_sourceRouteLoose.buffer())
            ntsa::Ipv4RouteSequence(value, d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE;
    }

    return d_sourceRouteLoose.object();
}

ntsa::Ipv4RouteSequence& Ipv4Option::makeSourceRouteTight()
{
    if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT) {
        d_sourceRouteTight.object().reset();
    }
    else {
        this->reset();
        new (d_sourceRouteTight.buffer())
            ntsa::Ipv4RouteSequence(d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT;
    }

    return d_sourceRouteTight.object();
}

ntsa::Ipv4RouteSequence& Ipv4Option::makeSourceRouteTight(
    const ntsa::Ipv4RouteSequence& value)
{
    if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT) {
        d_sourceRouteTight.object() = value;
    }
    else {
        this->reset();
        new (d_sourceRouteTight.buffer())
            ntsa::Ipv4RouteSequence(value, d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT;
    }

    return d_sourceRouteTight.object();
}

ntsa::Ipv4OptionValue& Ipv4Option::makeUnassigned()
{
    if (d_type == ntsa::Ipv4OptionType::e_UNASSIGNED) {
        d_unassigned.object().reset();
    }
    else {
        this->reset();
        new (d_unassigned.buffer()) ntsa::Ipv4OptionValue(d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_UNASSIGNED;
    }

    return d_unassigned.object();
}

ntsa::Ipv4OptionValue& Ipv4Option::makeUnassigned(
    const ntsa::Ipv4OptionValue& value)
{
    if (d_type == ntsa::Ipv4OptionType::e_UNASSIGNED) {
        d_unassigned.object() = value;
    }
    else {
        this->reset();
        new (d_unassigned.buffer()) ntsa::Ipv4OptionValue(value, d_allocator_p);
        d_type = ntsa::Ipv4OptionType::e_UNASSIGNED;
    }

    return d_unassigned.object();
}

ntsa::Ipv4RouteLedger& Ipv4Option::timestamp()
{
    BSLS_ASSERT(isTimestamp());
    return d_timestamp.object();
}

ntsa::Ipv4RouteSequence& Ipv4Option::recordRoute()
{
    BSLS_ASSERT(isRecordRoute());
    return d_recordRoute.object();
}

ntsa::Ipv4RouteSequence& Ipv4Option::sourceRouteLoose()
{
    BSLS_ASSERT(isSourceRouteLoose());
    return d_sourceRouteLoose.object();
}

ntsa::Ipv4RouteSequence& Ipv4Option::sourceRouteTight()
{
    BSLS_ASSERT(isSourceRouteTight());
    return d_sourceRouteTight.object();
}

ntsa::Ipv4OptionValue& Ipv4Option::unassigned()
{
    BSLS_ASSERT(isUnassigned());
    return d_unassigned.object();
}

ntsa::Error Ipv4Option::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    this->reset();

    bsl::uint8_t type;
    error = decoder->decodeUint8(&type);
    if (error) {
        return error;
    }

    if (type == ntsa::Ipv4OptionType::e_UNDEFINED) {
        this->reset();
    }
    else if (type == ntsa::Ipv4OptionType::e_PADDING) {
        this->makePadding();
    }
    else {
        bsl::uint8_t payloadSize;
        error = decoder->decodeUint8(&payloadSize);
        if (error) {
            return error;
        }

        if (payloadSize < 2) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        payloadSize -= 2;

        if (type == ntsa::Ipv4OptionType::e_ALERT) {
            if (payloadSize != sizeof(bsl::uint16_t)) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::uint16_t value;
            error = decoder->decodeUint16(&value);
            if (error) {
                return error;
            }

            if (value != 0) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            this->makeAlert();
        }
        else if (type == ntsa::Ipv4OptionType::e_TIMESTAMP) {
            ntsa::Ipv4RouteLedger& routeLedger = this->makeTimestamp();

            error = routeLedger.decode(decoder, payloadSize);
            if (error) {
                return error;
            }
        }
        else if (type == ntsa::Ipv4OptionType::e_RECORD_ROUTE) {
            ntsa::Ipv4RouteSequence& routeSequence = this->makeRecordRoute();

            error = routeSequence.decode(decoder, payloadSize);
            if (error) {
                return error;
            }
        }
        else if (type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE) {
            ntsa::Ipv4RouteSequence& routeSequence =
                this->makeSourceRouteLoose();

            error = routeSequence.decode(decoder, payloadSize);
            if (error) {
                return error;
            }
        }
        else if (type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT) {
            ntsa::Ipv4RouteSequence& routeSequence =
                this->makeSourceRouteTight();

            error = routeSequence.decode(decoder, payloadSize);
            if (error) {
                return error;
            }
        }
        else {
            BSLS_LOG_WARN("Unknown IPv4 option %d size = %zu",
                          static_cast<int>(type),
                          static_cast<bsl::size_t>(payloadSize));

            ntsa::Ipv4OptionValue& unassigned = this->makeUnassigned();

            unassigned.setKind(type);

            bsl::vector<bsl::uint8_t> payloadData(payloadSize);
            BSLS_ASSERT(payloadData.size() == payloadSize);

            error = decoder->decodeRaw(&payloadData.front(), payloadSize);
            if (error) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Ipv4Option::encode(ntsa::PacketEncoder* encoder, bool final) const
{
    ntsa::Error error;

    if (d_type == ntsa::Ipv4OptionType::e_ALERT) {
        const bsl::size_t payloadSize = sizeof(bdlb::BigEndianUint16);

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::Ipv4OptionType::e_PADDING));
            if (error) {
                return error;
            }
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(d_type));
        if (error) {
            return error;
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(optionSize));
        if (error) {
            return error;
        }

        error = encoder->encodeUint16(static_cast<bsl::uint16_t>(0));
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::Ipv4OptionType::e_TIMESTAMP) {
        const ntsa::Ipv4RouteLedger& routeLedger = this->timestamp();

        const bsl::size_t payloadSize = routeLedger.payloadSize();

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::Ipv4OptionType::e_PADDING));
            if (error) {
                return error;
            }
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(d_type));
        if (error) {
            return error;
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(optionSize));
        if (error) {
            return error;
        }

        error = routeLedger.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::Ipv4OptionType::e_RECORD_ROUTE) {
        const ntsa::Ipv4RouteSequence& routeSequence = this->recordRoute();

        const bsl::size_t payloadSize = routeSequence.payloadSize();

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::Ipv4OptionType::e_PADDING));
            if (error) {
                return error;
            }
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(d_type));
        if (error) {
            return error;
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(optionSize));
        if (error) {
            return error;
        }

        error = routeSequence.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE) {
        const ntsa::Ipv4RouteSequence& routeSequence =
            this->sourceRouteLoose();

        const bsl::size_t payloadSize = routeSequence.payloadSize();

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::Ipv4OptionType::e_PADDING));
            if (error) {
                return error;
            }
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(d_type));
        if (error) {
            return error;
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(optionSize));
        if (error) {
            return error;
        }

        error = routeSequence.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT) {
        const ntsa::Ipv4RouteSequence& routeSequence =
            this->sourceRouteTight();

        const bsl::size_t payloadSize = routeSequence.payloadSize();

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::Ipv4OptionType::e_PADDING));
            if (error) {
                return error;
            }
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(d_type));
        if (error) {
            return error;
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(optionSize));
        if (error) {
            return error;
        }

        error = routeSequence.encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::Ipv4OptionType::e_UNASSIGNED) {
        const bsl::size_t payloadSize = d_unassigned.object().payload().size();

        if (payloadSize == 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::Ipv4OptionType::e_PADDING));
            if (error) {
                return error;
            }
        }

        error = encoder->encodeUint8(
            static_cast<bsl::uint8_t>(d_unassigned.object().kind()));
        if (error) {
            return error;
        }

        error = encoder->encodeUint8(static_cast<bsl::uint8_t>(optionSize));
        if (error) {
            return error;
        }

        error = encoder->encodeRaw(
            &d_unassigned.object().payload().front(), payloadSize);
        if (error) {
            return error;
        }
    }
    else if (d_type != ntsa::Ipv4OptionType::e_PADDING &&
             d_type != ntsa::Ipv4OptionType::e_UNDEFINED)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

const ntsa::Ipv4RouteLedger& Ipv4Option::timestamp() const
{
    BSLS_ASSERT(isTimestamp());
    return d_timestamp.object();
}

const ntsa::Ipv4RouteSequence& Ipv4Option::recordRoute() const
{
    BSLS_ASSERT(isRecordRoute());
    return d_recordRoute.object();
}

const ntsa::Ipv4RouteSequence& Ipv4Option::sourceRouteLoose() const
{
    BSLS_ASSERT(isSourceRouteLoose());
    return d_sourceRouteLoose.object();
}

const ntsa::Ipv4RouteSequence& Ipv4Option::sourceRouteTight() const
{
    BSLS_ASSERT(isSourceRouteTight());
    return d_sourceRouteTight.object();
}

const ntsa::Ipv4OptionValue& Ipv4Option::unassigned() const
{
    BSLS_ASSERT(isUnassigned());
    return d_unassigned.object();
}

ntsa::Ipv4OptionType::Value Ipv4Option::type() const
{
    return d_type;
}

const char* Ipv4Option::name() const
{
    switch (d_type) {
    case ntsa::Ipv4OptionType::e_UNDEFINED:
        return "end";
    case ntsa::Ipv4OptionType::e_PADDING:
        return "padding";
    case ntsa::Ipv4OptionType::e_ALERT:
        return "alert";
    case ntsa::Ipv4OptionType::e_TIMESTAMP:
        return "timestamp";
    case ntsa::Ipv4OptionType::e_RECORD_ROUTE:
        return "recordRoute";
    case ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE:
        return "sourceRouteLoose";
    case ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT:
        return "sourceRouteTight";
    case ntsa::Ipv4OptionType::e_UNASSIGNED:
        return "unassigned";
    default:
        return "???";
    }
}

bool Ipv4Option::isUndefined() const
{
    return d_type == ntsa::Ipv4OptionType::e_UNDEFINED;
}

bool Ipv4Option::isPadding() const
{
    return d_type == ntsa::Ipv4OptionType::e_PADDING;
}

bool Ipv4Option::isAlert() const
{
    return d_type == ntsa::Ipv4OptionType::e_ALERT;
}

bool Ipv4Option::isTimestamp() const
{
    return d_type == ntsa::Ipv4OptionType::e_TIMESTAMP;
}

bool Ipv4Option::isRecordRoute() const
{
    return d_type == ntsa::Ipv4OptionType::e_RECORD_ROUTE;
}

bool Ipv4Option::isSourceRouteLoose() const
{
    return d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE;
}

bool Ipv4Option::isSourceRouteTight() const
{
    return d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT;
}

bool Ipv4Option::isUnassigned() const
{
    return d_type == ntsa::Ipv4OptionType::e_UNASSIGNED;
}

bool Ipv4Option::equals(const Ipv4Option& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::Ipv4OptionType::e_PADDING:
        return true;
    case ntsa::Ipv4OptionType::e_ALERT:
        return true;
    case ntsa::Ipv4OptionType::e_TIMESTAMP:
        return d_timestamp.object() == other.d_timestamp.object();
    case ntsa::Ipv4OptionType::e_RECORD_ROUTE:
        return d_recordRoute.object() == other.d_recordRoute.object();
    case ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE:
        return d_sourceRouteLoose.object() == other.d_sourceRouteLoose.object();
    case ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT:
        return d_sourceRouteTight.object() == other.d_sourceRouteTight.object();
    case ntsa::Ipv4OptionType::e_UNASSIGNED:
        return d_unassigned.object() == other.d_unassigned.object();
    default:
        BSLS_ASSERT(d_type == ntsa::Ipv4OptionType::e_UNDEFINED);
        return true;
    }
}

bool Ipv4Option::less(const Ipv4Option& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::Ipv4OptionType::e_PADDING:
        return false;
    case ntsa::Ipv4OptionType::e_ALERT:
        return false;
    case ntsa::Ipv4OptionType::e_TIMESTAMP:
        return d_timestamp.object() < other.d_timestamp.object();
    case ntsa::Ipv4OptionType::e_RECORD_ROUTE:
        return d_recordRoute.object() < other.d_recordRoute.object();
    case ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE:
        return d_sourceRouteLoose.object() < other.d_sourceRouteLoose.object();
    case ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT:
        return d_sourceRouteTight.object() < other.d_sourceRouteTight.object();
    case ntsa::Ipv4OptionType::e_UNASSIGNED:
        return d_unassigned.object() < other.d_unassigned.object();
    default:
        BSLS_ASSERT(d_type == ntsa::Ipv4OptionType::e_UNDEFINED);
        return false;
    }
}

bsl::ostream& Ipv4Option::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    switch (d_type) {
    case ntsa::Ipv4OptionType::e_PADDING:
        printer.printAttribute("padding", true);
        break;
    case ntsa::Ipv4OptionType::e_ALERT:
        printer.printAttribute("alert", true);
        break;
    case ntsa::Ipv4OptionType::e_TIMESTAMP:
        printer.printAttribute("timestamp", d_timestamp.object());
        break;
    case ntsa::Ipv4OptionType::e_RECORD_ROUTE:
        printer.printAttribute("recordRoute", d_recordRoute.object());
        break;
    case ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE:
        printer.printAttribute("sourceRouteLoose", d_sourceRouteLoose.object());
        break;
    case ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT:
        printer.printAttribute("sourceRouteTight", d_sourceRouteTight.object());
        break;
    case ntsa::Ipv4OptionType::e_UNASSIGNED:
        printer.printAttribute("unassigned", d_unassigned.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::Ipv4OptionType::e_UNDEFINED);
        stream << "UNDEFINED";
        break;
    }

    printer.end();

    return stream;
}

void Ipv4Option::print(bslim::Printer* printer) const
{
    if (d_type == ntsa::Ipv4OptionType::e_PADDING) {
        printer->printAttribute("padding", true);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_ALERT) {
        printer->printAttribute("alert", true);
    }
    else if (d_type == ntsa::Ipv4OptionType::e_TIMESTAMP) {
        printer->printAttribute("timestamp", d_timestamp.object());
    }
    else if (d_type == ntsa::Ipv4OptionType::e_RECORD_ROUTE) {
        printer->printAttribute("recordRoute", d_recordRoute.object());
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_LOOSE) {
        printer->printAttribute("sourceRouteLoose", d_sourceRouteLoose.object());
    }
    else if (d_type == ntsa::Ipv4OptionType::e_SOURCE_ROUTE_TIGHT) {
        printer->printAttribute("sourceRouteTight", d_sourceRouteTight.object());
    }
    else if (d_type == ntsa::Ipv4OptionType::e_UNASSIGNED) {
        printer->printAttribute("unassigned", d_unassigned.object());
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Option& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Ipv4Option& lhs, const Ipv4Option& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Ipv4Option& lhs, const Ipv4Option& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Ipv4Option& lhs, const Ipv4Option& rhs)
{
    return lhs.less(rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
