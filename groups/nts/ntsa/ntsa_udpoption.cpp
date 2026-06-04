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

#include <ntsa_udpoption.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_udpoption_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsa {

bsl::size_t UdpOption::paddingSize(const bsl::uint8_t* cursor,
                                   bsl::size_t         optionSize)
{
    const bsl::size_t address = static_cast<bsl::size_t>(
        reinterpret_cast<bsl::uintptr_t>(cursor + optionSize));

    return (4 - (address & 3)) & 3;
}

UdpOption::UdpOption(bslma::Allocator* basicAllocator)
: d_type(ntsa::UdpOptionType::e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

UdpOption::UdpOption(const UdpOption& other, bslma::Allocator* basicAllocator)
: d_type(other.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_type) {
    case ntsa::UdpOptionType::e_PADDING:
        break;
    case ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM:
        new (d_additionalChecksum.buffer())
            bsl::uint32_t(other.d_additionalChecksum.object());
        break;
    case ntsa::UdpOptionType::e_FRAGMENTATION:
        new (d_fragmentation.buffer())
            ntsa::UdpFragmentation(other.d_fragmentation.object());
        break;
    case ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE:
        new (d_maxDatagramSize.buffer())
            bsl::uint32_t(other.d_maxDatagramSize.object());
        break;
    case ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE:
        new (d_reassembly.buffer())
            ntsa::UdpReassembly(other.d_reassembly.object());
        break;
    case ntsa::UdpOptionType::e_ECHO_REQUEST:
        new (d_echoRequest.buffer())
            bsl::uint32_t(other.d_echoRequest.object());
        break;
    case ntsa::UdpOptionType::e_ECHO_RESPONSE:
        new (d_echoResponse.buffer())
            bsl::uint32_t(other.d_echoResponse.object());
        break;
    case ntsa::UdpOptionType::e_TIMESTAMP:
        new (d_timestamp.buffer())
            ntsa::UdpTimePointInterval(other.d_timestamp.object());
        break;
    case ntsa::UdpOptionType::e_UNASSIGNED:
        new (d_unassigned.buffer()) ntsa::UdpOptionValue(
            other.d_unassigned.object(), d_allocator_p);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::UdpOptionType::e_UNDEFINED);
    }
}

UdpOption::~UdpOption()
{
    if (isFragmentation()) {
        typedef ntsa::UdpFragmentation Type;
        d_fragmentation.object().~Type();
    }
    else if (isReassembly()) {
        typedef ntsa::UdpReassembly Type;
        d_reassembly.object().~Type();
    }
    else if (isUnassigned()) {
        typedef ntsa::UdpOptionValue Type;
        d_unassigned.object().~Type();
    }
}

UdpOption& UdpOption::operator=(const UdpOption& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    switch (other.d_type) {
    case ntsa::UdpOptionType::e_PADDING:
        break;
    case ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM:
        new (d_additionalChecksum.buffer())
            bsl::uint32_t(other.d_additionalChecksum.object());
        break;
    case ntsa::UdpOptionType::e_FRAGMENTATION:
        new (d_fragmentation.buffer())
            ntsa::UdpFragmentation(other.d_fragmentation.object());
        break;
    case ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE:
        new (d_maxDatagramSize.buffer())
            bsl::uint32_t(other.d_maxDatagramSize.object());
        break;
    case ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE:
        new (d_reassembly.buffer())
            ntsa::UdpReassembly(other.d_reassembly.object());
        break;
    case ntsa::UdpOptionType::e_ECHO_REQUEST:
        new (d_echoRequest.buffer())
            bsl::uint32_t(other.d_echoRequest.object());
        break;
    case ntsa::UdpOptionType::e_ECHO_RESPONSE:
        new (d_echoResponse.buffer())
            bsl::uint32_t(other.d_echoResponse.object());
        break;
    case ntsa::UdpOptionType::e_TIMESTAMP:
        new (d_timestamp.buffer())
            ntsa::UdpTimePointInterval(other.d_timestamp.object());
        break;
    case ntsa::UdpOptionType::e_UNASSIGNED:
        new (d_unassigned.buffer()) ntsa::UdpOptionValue(
            other.d_unassigned.object(), d_allocator_p);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::UdpOptionType::e_UNDEFINED);
    }

    d_type = other.d_type;

    return *this;
}

void UdpOption::reset()
{
    if (isFragmentation()) {
        typedef ntsa::UdpFragmentation Type;
        d_fragmentation.object().~Type();
    }
    else if (isReassembly()) {
        typedef ntsa::UdpReassembly Type;
        d_reassembly.object().~Type();
    }
    else if (isUnassigned()) {
        typedef ntsa::UdpOptionValue Type;
        d_unassigned.object().~Type();
    }

    d_type = ntsa::UdpOptionType::e_UNDEFINED;
}

void UdpOption::makePadding()
{
    if (d_type != ntsa::UdpOptionType::e_PADDING) {
        reset();
        d_type = ntsa::UdpOptionType::e_PADDING;
    }
}

bsl::uint32_t& UdpOption::makeAdditionalChecksum()
{
    if (d_type == ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM) {
        d_additionalChecksum.object() = 0;
    }
    else {
        this->reset();
        new (d_additionalChecksum.buffer()) bsl::uint32_t(0);
        d_type = ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM;
    }

    return d_additionalChecksum.object();
}

bsl::uint32_t& UdpOption::makeAdditionalChecksum(bsl::uint32_t value)
{
    if (d_type == ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM) {
        d_additionalChecksum.object() = value;
    }
    else {
        this->reset();
        new (d_additionalChecksum.buffer()) bsl::size_t(value);
        d_type = ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM;
    }

    return d_additionalChecksum.object();
}

ntsa::UdpFragmentation& UdpOption::makeFragmentation()
{
    if (d_type == ntsa::UdpOptionType::e_FRAGMENTATION) {
        d_fragmentation.object().reset();
    }
    else {
        this->reset();
        new (d_fragmentation.buffer()) ntsa::UdpFragmentation();
        d_type = ntsa::UdpOptionType::e_FRAGMENTATION;
    }

    return d_fragmentation.object();
}

ntsa::UdpFragmentation& UdpOption::makeFragmentation(
    const ntsa::UdpFragmentation& value)
{
    if (d_type == ntsa::UdpOptionType::e_FRAGMENTATION) {
        d_fragmentation.object() = value;
    }
    else {
        this->reset();
        new (d_fragmentation.buffer()) ntsa::UdpFragmentation(value);
        d_type = ntsa::UdpOptionType::e_FRAGMENTATION;
    }

    return d_fragmentation.object();
}

bsl::uint32_t& UdpOption::makeMaxDatagramSize()
{
    if (d_type == ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE) {
        d_maxDatagramSize.object() = 0;
    }
    else {
        this->reset();
        new (d_maxDatagramSize.buffer()) bsl::uint32_t(0);
        d_type = ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE;
    }

    return d_maxDatagramSize.object();
}

bsl::uint32_t& UdpOption::makeMaxDatagramSize(bsl::uint32_t value)
{
    if (d_type == ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE) {
        d_maxDatagramSize.object() = value;
    }
    else {
        this->reset();
        new (d_maxDatagramSize.buffer()) bsl::size_t(value);
        d_type = ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE;
    }

    return d_maxDatagramSize.object();
}

ntsa::UdpReassembly& UdpOption::makeReassembly()
{
    if (d_type == ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE) {
        d_reassembly.object().reset();
    }
    else {
        this->reset();
        new (d_reassembly.buffer()) ntsa::UdpReassembly();
        d_type = ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE;
    }

    return d_reassembly.object();
}

ntsa::UdpReassembly& UdpOption::makeReassembly(
    const ntsa::UdpReassembly& value)
{
    if (d_type == ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE) {
        d_reassembly.object() = value;
    }
    else {
        this->reset();
        new (d_reassembly.buffer()) ntsa::UdpReassembly(value);
        d_type = ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE;
    }

    return d_reassembly.object();
}

bsl::uint32_t& UdpOption::makeEchoRequest()
{
    if (d_type == ntsa::UdpOptionType::e_ECHO_REQUEST) {
        d_echoRequest.object() = 0;
    }
    else {
        this->reset();
        new (d_echoRequest.buffer()) bsl::uint32_t(0);
        d_type = ntsa::UdpOptionType::e_ECHO_REQUEST;
    }

    return d_echoRequest.object();
}

bsl::uint32_t& UdpOption::makeEchoRequest(bsl::uint32_t value)
{
    if (d_type == ntsa::UdpOptionType::e_ECHO_REQUEST) {
        d_echoRequest.object() = value;
    }
    else {
        this->reset();
        new (d_echoRequest.buffer()) bsl::size_t(value);
        d_type = ntsa::UdpOptionType::e_ECHO_REQUEST;
    }

    return d_echoRequest.object();
}

bsl::uint32_t& UdpOption::makeEchoResponse()
{
    if (d_type == ntsa::UdpOptionType::e_ECHO_RESPONSE) {
        d_echoResponse.object() = 0;
    }
    else {
        this->reset();
        new (d_echoResponse.buffer()) bsl::uint32_t(0);
        d_type = ntsa::UdpOptionType::e_ECHO_RESPONSE;
    }

    return d_echoResponse.object();
}

bsl::uint32_t& UdpOption::makeEchoResponse(bsl::uint32_t value)
{
    if (d_type == ntsa::UdpOptionType::e_ECHO_RESPONSE) {
        d_echoResponse.object() = value;
    }
    else {
        this->reset();
        new (d_echoResponse.buffer()) bsl::size_t(value);
        d_type = ntsa::UdpOptionType::e_ECHO_RESPONSE;
    }

    return d_echoResponse.object();
}

ntsa::UdpTimePointInterval& UdpOption::makeTimestamp()
{
    if (d_type == ntsa::UdpOptionType::e_TIMESTAMP) {
        d_timestamp.object().reset();
    }
    else {
        this->reset();
        new (d_timestamp.buffer()) ntsa::UdpTimePointInterval();
        d_type = ntsa::UdpOptionType::e_TIMESTAMP;
    }

    return d_timestamp.object();
}

ntsa::UdpTimePointInterval& UdpOption::makeTimestamp(
    const ntsa::UdpTimePointInterval& value)
{
    if (d_type == ntsa::UdpOptionType::e_TIMESTAMP) {
        d_timestamp.object() = value;
    }
    else {
        this->reset();
        new (d_timestamp.buffer()) ntsa::UdpTimePointInterval(value);
        d_type = ntsa::UdpOptionType::e_TIMESTAMP;
    }

    return d_timestamp.object();
}

ntsa::UdpOptionValue& UdpOption::makeUnassigned()
{
    if (d_type == ntsa::UdpOptionType::e_UNASSIGNED) {
        d_unassigned.object().reset();
    }
    else {
        this->reset();
        new (d_unassigned.buffer()) ntsa::UdpOptionValue(d_allocator_p);
        d_type = ntsa::UdpOptionType::e_UNASSIGNED;
    }

    return d_unassigned.object();
}

ntsa::UdpOptionValue& UdpOption::makeUnassigned(
    const ntsa::UdpOptionValue& value)
{
    if (d_type == ntsa::UdpOptionType::e_UNASSIGNED) {
        d_unassigned.object() = value;
    }
    else {
        this->reset();
        new (d_unassigned.buffer()) ntsa::UdpOptionValue(value, d_allocator_p);
        d_type = ntsa::UdpOptionType::e_UNASSIGNED;
    }

    return d_unassigned.object();
}

bsl::uint32_t& UdpOption::additionalChecksum()
{
    BSLS_ASSERT(isAdditionalChecksum());
    return d_additionalChecksum.object();
}

ntsa::UdpFragmentation& UdpOption::fragmentation()
{
    BSLS_ASSERT(isFragmentation());
    return d_fragmentation.object();
}

bsl::uint32_t& UdpOption::maxDatagramSize()
{
    BSLS_ASSERT(isMaxDatagramSize());
    return d_maxDatagramSize.object();
}

ntsa::UdpReassembly& UdpOption::reassembly()
{
    BSLS_ASSERT(isReassembly());
    return d_reassembly.object();
}

bsl::uint32_t& UdpOption::echoRequest()
{
    BSLS_ASSERT(isEchoRequest());
    return d_echoRequest.object();
}

bsl::uint32_t& UdpOption::echoResponse()
{
    BSLS_ASSERT(isEchoResponse());
    return d_echoResponse.object();
}

ntsa::UdpTimePointInterval& UdpOption::timestamp()
{
    BSLS_ASSERT(isTimestamp());
    return d_timestamp.object();
}

ntsa::UdpOptionValue& UdpOption::unassigned()
{
    BSLS_ASSERT(isUnassigned());
    return d_unassigned.object();
}

ntsa::Error UdpOption::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    this->reset();

    bsl::uint8_t type;
    error = decoder->decodeUint8(&type);
    if (error) {
        return error;
    }

    if (type == ntsa::UdpOptionType::e_UNDEFINED) {
        this->reset();
    }
    else if (type == ntsa::UdpOptionType::e_PADDING) {
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

        if (type == ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM) {
            if (payloadSize != sizeof(bsl::uint32_t)) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::uint32_t checksum;
            error = decoder->decodeUint32(&checksum);
            if (error) {
                return error;
            }

            this->makeAdditionalChecksum(checksum);
        }
        else if (type == ntsa::UdpOptionType::e_FRAGMENTATION) {
            if (payloadSize != 8 && payloadSize != 10) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ntsa::UdpFragmentation& fragmentation =
                this->makeFragmentation();

            bsl::uint16_t start;
            error = decoder->decodeUint16(&start);
            if (error) {
                return error;
            }

            fragmentation.setStart(start);

            bsl::uint32_t identifier;
            error = decoder->decodeUint32(&identifier);
            if (error) {
                return error;
            }

            fragmentation.setIdentifier(identifier);

            bsl::uint16_t offset;
            error = decoder->decodeUint16(&offset);
            if (error) {
                return error;
            }

            fragmentation.setOffset(offset);

            if (payloadSize == 10) {
                bsl::uint16_t rdos;
                error = decoder->decodeUint16(&rdos);
                if (error) {
                    return error;
                }

                fragmentation.setOffset(rdos);
            }

            this->makeFragmentation(fragmentation);
        }
        else if (type == ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE) {
            if (payloadSize != 4) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::uint32_t maxDatagramSize;
            error = decoder->decodeUint32(&maxDatagramSize);
            if (error) {
                return error;
            }

            this->makeMaxDatagramSize(maxDatagramSize);
        }
        else if (type == ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE) {
            if (payloadSize != 3) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ntsa::UdpReassembly& reassembly = this->makeReassembly();

            bsl::uint16_t maxSize;
            error = decoder->decodeUint16(&maxSize);
            if (error) {
                return error;
            }

            reassembly.setMaxSize(maxSize);

            bsl::uint8_t maxFragments;
            error = decoder->decodeUint8(&maxFragments);
            if (error) {
                return error;
            }

            reassembly.setMaxFragments(maxFragments);

            this->makeReassembly(reassembly);
        }
        else if (type == ntsa::UdpOptionType::e_ECHO_REQUEST) {
            if (payloadSize != 4) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::uint32_t token;
            error = decoder->decodeUint32(&token);
            if (error) {
                return error;
            }

            this->makeEchoRequest(token);
        }
        else if (type == ntsa::UdpOptionType::e_ECHO_RESPONSE) {
            if (payloadSize != 4) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::uint32_t token;
            error = decoder->decodeUint32(&token);
            if (error) {
                return error;
            }

            this->makeEchoResponse(token);
        }
        else if (type == ntsa::UdpOptionType::e_TIMESTAMP) {
            if (payloadSize !=
                sizeof(bdlb::BigEndianUint32) + sizeof(bdlb::BigEndianUint32))
            {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::uint32_t tx;
            error = decoder->decodeUint32(&tx);
            if (error) {
                return error;
            }

            bsl::uint32_t rx;
            error = decoder->decodeUint32(&rx);
            if (error) {
                return error;
            }

            ntsa::UdpTimePointInterval& timestamp = this->makeTimestamp();

            timestamp.setTx(ntsa::UdpTimePoint(tx));
            timestamp.setRx(ntsa::UdpTimePoint(rx));
        }
        else {
            BSLS_LOG_WARN("Unknown UDP option %d size = %zu",
                          static_cast<int>(type),
                          static_cast<bsl::size_t>(payloadSize));

            ntsa::UdpOptionValue& unassigned = this->makeUnassigned();

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

ntsa::Error UdpOption::encode(ntsa::PacketEncoder* encoder, bool final) const
{
    ntsa::Error error;

    if (d_type == ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM) {
        const bsl::size_t payloadSize = sizeof(bdlb::BigEndianUint32);

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::UdpOptionType::e_PADDING));
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

        error = encoder->encodeUint32(d_additionalChecksum.object());
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::UdpOptionType::e_FRAGMENTATION) {
        const bsl::size_t payloadSize =
            d_fragmentation.object().rdos().has_value() ? 10 : 8;

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::UdpOptionType::e_PADDING));
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

        error = encoder->encodeUint16(
            static_cast<bsl::uint16_t>(d_fragmentation.object().start()));
        if (error) {
            return error;
        }

        error = encoder->encodeUint32(
            static_cast<bsl::uint32_t>(d_fragmentation.object().identifier()));
        if (error) {
            return error;
        }

        error = encoder->encodeUint16(
            static_cast<bsl::uint16_t>(d_fragmentation.object().offset()));
        if (error) {
            return error;
        }

        if (d_fragmentation.object().rdos().has_value()) {
            error = encoder->encodeUint16(
                static_cast<bsl::uint16_t>(
                    d_fragmentation.object().rdos().value()));
            if (error) {
                return error;
            }
        }
    }
    else if (d_type == ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE) {
        const bsl::size_t payloadSize = sizeof(bdlb::BigEndianUint32);

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::UdpOptionType::e_PADDING));
            if (error) {
                return error;
            }
        }

        error = encoder->encodeUint32(d_maxDatagramSize.object());
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE) {
        const bsl::size_t payloadSize = sizeof(bdlb::BigEndianUint16) + sizeof(bsl::uint8_t);

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::UdpOptionType::e_PADDING));
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

        error = encoder->encodeUint16(
            static_cast<bsl::uint16_t>(d_reassembly.object().maxSize()));
        if (error) {
            return error;
        }

        error = encoder->encodeUint8(
            static_cast<bsl::uint8_t>(d_reassembly.object().maxFragments()));
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::UdpOptionType::e_ECHO_REQUEST) {
        const bsl::size_t payloadSize = sizeof(bdlb::BigEndianUint32);

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::UdpOptionType::e_PADDING));
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

        error = encoder->encodeUint32(
            static_cast<bsl::uint32_t>(d_echoRequest.object()));
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::UdpOptionType::e_ECHO_RESPONSE) {
        const bsl::size_t payloadSize = sizeof(bdlb::BigEndianUint32);

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::UdpOptionType::e_PADDING));
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

        error = encoder->encodeUint32(
            static_cast<bsl::uint32_t>(d_echoResponse.object()));
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::UdpOptionType::e_TIMESTAMP) {
        const bsl::size_t payloadSize =
            sizeof(bdlb::BigEndianUint32) + sizeof(bdlb::BigEndianUint32);

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::UdpOptionType::e_PADDING));
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

        error = encoder->encodeUint32(d_timestamp.object().tx().value());
        if (error) {
            return error;
        }

        error = encoder->encodeUint32(d_timestamp.object().rx().value());
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::UdpOptionType::e_UNASSIGNED) {
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
                static_cast<bsl::uint8_t>(ntsa::UdpOptionType::e_PADDING));
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
    else if (d_type != ntsa::UdpOptionType::e_PADDING &&
             d_type != ntsa::UdpOptionType::e_UNDEFINED)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

bsl::uint32_t UdpOption::additionalChecksum() const
{
    BSLS_ASSERT(isAdditionalChecksum());
    return d_additionalChecksum.object();
}

const ntsa::UdpFragmentation& UdpOption::fragmentation() const
{
    BSLS_ASSERT(isFragmentation());
    return d_fragmentation.object();
}

bsl::uint32_t UdpOption::maxDatagramSize() const
{
    BSLS_ASSERT(isMaxDatagramSize());
    return d_maxDatagramSize.object();
}

const ntsa::UdpReassembly& UdpOption::reassembly() const
{
    BSLS_ASSERT(isReassembly());
    return d_reassembly.object();
}

bsl::uint32_t UdpOption::echoRequest() const
{
    BSLS_ASSERT(isEchoRequest());
    return d_echoRequest.object();
}

bsl::uint32_t UdpOption::echoResponse() const
{
    BSLS_ASSERT(isEchoResponse());
    return d_echoResponse.object();
}

const ntsa::UdpTimePointInterval& UdpOption::timestamp() const
{
    BSLS_ASSERT(isTimestamp());
    return d_timestamp.object();
}

const ntsa::UdpOptionValue& UdpOption::unassigned() const
{
    BSLS_ASSERT(isUnassigned());
    return d_unassigned.object();
}

ntsa::UdpOptionType::Value UdpOption::type() const
{
    return d_type;
}

const char* UdpOption::name() const
{
    switch (d_type) {
    case ntsa::UdpOptionType::e_UNDEFINED:
        return "end";
    case ntsa::UdpOptionType::e_PADDING:
        return "padding";
    case ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM:
        return "additionalChecksum";
    case ntsa::UdpOptionType::e_FRAGMENTATION:
        return "fragmentation";
    case ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE:
        return "maxDatagramSize";
    case ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE:
        return "reassembly";
    case ntsa::UdpOptionType::e_ECHO_REQUEST:
        return "echoRequest";
    case ntsa::UdpOptionType::e_ECHO_RESPONSE:
        return "echoResponse";
    case ntsa::UdpOptionType::e_TIMESTAMP:
        return "timestamp";
    case ntsa::UdpOptionType::e_UNASSIGNED:
        return "unassigned";
    default:
        return "???";
    }
}

bool UdpOption::isUndefined() const
{
    return d_type == ntsa::UdpOptionType::e_UNDEFINED;
}

bool UdpOption::isPadding() const
{
    return d_type == ntsa::UdpOptionType::e_PADDING;
}

bool UdpOption::isAdditionalChecksum() const
{
    return d_type == ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM;
}

bool UdpOption::isFragmentation() const
{
    return d_type == ntsa::UdpOptionType::e_FRAGMENTATION;
}

bool UdpOption::isMaxDatagramSize() const
{
    return d_type == ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE;
}

bool UdpOption::isReassembly() const
{
    return d_type == ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE;
}

bool UdpOption::isEchoRequest() const
{
    return d_type == ntsa::UdpOptionType::e_ECHO_REQUEST;
}

bool UdpOption::isEchoResponse() const
{
    return d_type == ntsa::UdpOptionType::e_ECHO_RESPONSE;
}

bool UdpOption::isTimestamp() const
{
    return d_type == ntsa::UdpOptionType::e_TIMESTAMP;
}

bool UdpOption::isUnassigned() const
{
    return d_type == ntsa::UdpOptionType::e_UNASSIGNED;
}

bool UdpOption::equals(const UdpOption& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::UdpOptionType::e_PADDING:
        return true;
    case ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM:
        return d_additionalChecksum.object() ==
               other.d_additionalChecksum.object();
    case ntsa::UdpOptionType::e_FRAGMENTATION:
        return d_fragmentation.object() == other.d_fragmentation.object();
    case ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE:
        return d_maxDatagramSize.object() == other.d_maxDatagramSize.object();
    case ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE:
        return d_reassembly.object() == other.d_reassembly.object();
    case ntsa::UdpOptionType::e_ECHO_REQUEST:
        return d_echoRequest.object() == other.d_echoRequest.object();
    case ntsa::UdpOptionType::e_ECHO_RESPONSE:
        return d_echoResponse.object() == other.d_echoResponse.object();
    case ntsa::UdpOptionType::e_TIMESTAMP:
        return d_timestamp.object() == other.d_timestamp.object();
    case ntsa::UdpOptionType::e_UNASSIGNED:
        return d_unassigned.object() == other.d_unassigned.object();
    default:
        BSLS_ASSERT(d_type == ntsa::UdpOptionType::e_UNDEFINED);
        return true;
    }
}

bool UdpOption::less(const UdpOption& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::UdpOptionType::e_PADDING:
        return false;
    case ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM:
        return d_additionalChecksum.object() <
               other.d_additionalChecksum.object();
    case ntsa::UdpOptionType::e_FRAGMENTATION:
        return d_fragmentation.object() < other.d_fragmentation.object();
    case ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE:
        return d_maxDatagramSize.object() < other.d_maxDatagramSize.object();
    case ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE:
        return d_reassembly.object() < other.d_reassembly.object();
    case ntsa::UdpOptionType::e_ECHO_REQUEST:
        return d_echoRequest.object() < other.d_echoRequest.object();
    case ntsa::UdpOptionType::e_ECHO_RESPONSE:
        return d_echoResponse.object() < other.d_echoResponse.object();
    case ntsa::UdpOptionType::e_TIMESTAMP:
        return d_timestamp.object() < other.d_timestamp.object();
    case ntsa::UdpOptionType::e_UNASSIGNED:
        return d_unassigned.object() < other.d_unassigned.object();
    default:
        BSLS_ASSERT(d_type == ntsa::UdpOptionType::e_UNDEFINED);
        return false;
    }
}

bsl::ostream& UdpOption::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    switch (d_type) {
    case ntsa::UdpOptionType::e_PADDING:
        printer.printAttribute("padding", true);
        break;
    case ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM:
        printer.printAttribute("additionalChecksum",
                               d_additionalChecksum.object());
        break;
    case ntsa::UdpOptionType::e_FRAGMENTATION:
        printer.printAttribute("fragmentation", d_fragmentation.object());
        break;
    case ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE:
        printer.printAttribute("maxDatagramSize", d_maxDatagramSize.object());
        break;
    case ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE:
        printer.printAttribute("reassembly", d_reassembly.object());
        break;
    case ntsa::UdpOptionType::e_ECHO_REQUEST:
        printer.printAttribute("echoRequest", d_echoRequest.object());
        break;
    case ntsa::UdpOptionType::e_ECHO_RESPONSE:
        printer.printAttribute("echoResponse", d_echoResponse.object());
        break;
    case ntsa::UdpOptionType::e_TIMESTAMP:
        printer.printAttribute("timestamp", d_timestamp.object());
        break;
    case ntsa::UdpOptionType::e_UNASSIGNED:
        printer.printAttribute("unassigned", d_unassigned.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::UdpOptionType::e_UNDEFINED);
        stream << "UNDEFINED";
        break;
    }

    printer.end();

    return stream;
}

void UdpOption::print(bslim::Printer* printer) const
{
    if (d_type == ntsa::UdpOptionType::e_PADDING) {
        printer->printAttribute("padding", true);
    }
    else if (d_type == ntsa::UdpOptionType::e_ADDITIONAL_PAYLOAD_CHECKSUM) {
        printer->printAttribute("additionalChecksum",
                                d_additionalChecksum.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_FRAGMENTATION) {
        printer->printAttribute("fragmentation", d_fragmentation.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_MAX_DATAGRAM_SIZE) {
        printer->printAttribute("maxDatagramSize", d_maxDatagramSize.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_MAX_REASSEMBLED_DATAGRAM_SIZE) {
        printer->printAttribute("reassembly", d_reassembly.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_ECHO_REQUEST) {
        printer->printAttribute("echoRequest", d_echoRequest.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_ECHO_RESPONSE) {
        printer->printAttribute("echoResponse", d_echoResponse.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_TIMESTAMP) {
        printer->printAttribute("timestamp", d_timestamp.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_UNASSIGNED) {
        printer->printAttribute("unassigned", d_unassigned.object());
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const UdpOption& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const UdpOption& lhs, const UdpOption& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const UdpOption& lhs, const UdpOption& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const UdpOption& lhs, const UdpOption& rhs)
{
    return lhs.less(rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
