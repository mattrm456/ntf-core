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
    case ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE:
        new (d_maxSegmentSize.buffer())
            bsl::size_t(other.d_maxSegmentSize.object());
        break;
    case ntsa::UdpOptionType::e_WINDOW_SCALE:
        new (d_windowScale.buffer()) bsl::size_t(other.d_windowScale.object());
        break;
    case ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED:
        break;
    case ntsa::UdpOptionType::e_SELECTIVE_ACK:
        new (d_selectiveAck.buffer())
            ntsa::UdpSequenceRangeVector(other.d_selectiveAck.object(),
                                         d_allocator_p);
        break;
    case ntsa::UdpOptionType::e_TIMESTAMP:
        new (d_timestamp.buffer())
            ntsa::UdpTimePointInterval(other.d_timestamp.object());
        break;
    case ntsa::UdpOptionType::e_FAST_OPEN:
        new (d_fastOpen.buffer()) bdlb::Guid(other.d_fastOpen.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::UdpOptionType::e_UNDEFINED);
    }
}

UdpOption::~UdpOption()
{
    if (isSelectiveAck()) {
        typedef ntsa::UdpSequenceRangeVector Type;
        d_selectiveAck.object().~Type();
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
    case ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE:
        new (d_maxSegmentSize.buffer())
            bsl::size_t(other.d_maxSegmentSize.object());
        break;
    case ntsa::UdpOptionType::e_WINDOW_SCALE:
        new (d_windowScale.buffer()) bsl::size_t(other.d_windowScale.object());
        break;
    case ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED:
        break;
    case ntsa::UdpOptionType::e_SELECTIVE_ACK:
        new (d_selectiveAck.buffer())
            ntsa::UdpSequenceRangeVector(other.d_selectiveAck.object(),
                                         d_allocator_p);
        break;
    case ntsa::UdpOptionType::e_TIMESTAMP:
        new (d_timestamp.buffer())
            ntsa::UdpTimePointInterval(other.d_timestamp.object());
        break;
    case ntsa::UdpOptionType::e_FAST_OPEN:
        new (d_fastOpen.buffer()) bdlb::Guid(other.d_fastOpen.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::UdpOptionType::e_UNDEFINED);
    }

    d_type = other.d_type;

    return *this;
}

void UdpOption::reset()
{
    if (isSelectiveAck()) {
        typedef ntsa::UdpSequenceRangeVector Type;
        d_selectiveAck.object().~Type();
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

bsl::size_t& UdpOption::makeMaxSegmentSize()
{
    if (d_type == ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE) {
        d_maxSegmentSize.object() = 0;
    }
    else {
        this->reset();
        new (d_maxSegmentSize.buffer()) bsl::size_t(0);
        d_type = ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE;
    }

    return d_maxSegmentSize.object();
}

bsl::size_t& UdpOption::makeMaxSegmentSize(bsl::size_t value)
{
    if (d_type == ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE) {
        d_maxSegmentSize.object() = value;
    }
    else {
        this->reset();
        new (d_maxSegmentSize.buffer()) bsl::size_t(value);
        d_type = ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE;
    }

    return d_maxSegmentSize.object();
}

bsl::size_t& UdpOption::makeWindowScale()
{
    if (d_type == ntsa::UdpOptionType::e_WINDOW_SCALE) {
        d_windowScale.object() = 0;
    }
    else {
        this->reset();
        new (d_windowScale.buffer()) bsl::size_t(0);
        d_type = ntsa::UdpOptionType::e_WINDOW_SCALE;
    }

    return d_windowScale.object();
}

bsl::size_t& UdpOption::makeWindowScale(bsl::size_t value)
{
    if (d_type == ntsa::UdpOptionType::e_WINDOW_SCALE) {
        d_windowScale.object() = value;
    }
    else {
        this->reset();
        new (d_windowScale.buffer()) bsl::size_t(value);
        d_type = ntsa::UdpOptionType::e_WINDOW_SCALE;
    }

    return d_windowScale.object();
}

void UdpOption::makeSelectiveAckPermitted()
{
    if (d_type != ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED) {
        reset();
        d_type = ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED;
    }
}

ntsa::UdpSequenceRangeVector& UdpOption::makeSelectiveAck()
{
    if (d_type == ntsa::UdpOptionType::e_SELECTIVE_ACK) {
        d_selectiveAck.object().clear();
    }
    else {
        this->reset();
        new (d_selectiveAck.buffer())
            ntsa::UdpSequenceRangeVector(d_allocator_p);
        d_type = ntsa::UdpOptionType::e_SELECTIVE_ACK;
    }

    return d_selectiveAck.object();
}

ntsa::UdpSequenceRangeVector& UdpOption::makeSelectiveAck(
    const ntsa::UdpSequenceRangeVector& value)
{
    if (d_type == ntsa::UdpOptionType::e_SELECTIVE_ACK) {
        d_selectiveAck.object() = value;
    }
    else {
        this->reset();
        new (d_selectiveAck.buffer())
            ntsa::UdpSequenceRangeVector(value, d_allocator_p);
        d_type = ntsa::UdpOptionType::e_SELECTIVE_ACK;
    }

    return d_selectiveAck.object();
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

bdlb::Guid& UdpOption::makeFastOpen()
{
    if (d_type == ntsa::UdpOptionType::e_FAST_OPEN) {
        d_fastOpen.object() = bdlb::Guid();
    }
    else {
        this->reset();
        new (d_fastOpen.buffer()) bdlb::Guid();
        d_type = ntsa::UdpOptionType::e_FAST_OPEN;
    }

    return d_fastOpen.object();
}

bdlb::Guid& UdpOption::makeFastOpen(const bdlb::Guid& value)
{
    if (d_type == ntsa::UdpOptionType::e_FAST_OPEN) {
        d_fastOpen.object() = value;
    }
    else {
        this->reset();
        new (d_fastOpen.buffer()) bdlb::Guid(value);
        d_type = ntsa::UdpOptionType::e_FAST_OPEN;
    }

    return d_fastOpen.object();
}

bsl::size_t& UdpOption::maxSegmentSize()
{
    BSLS_ASSERT(isMaxSegmentSize());
    return d_maxSegmentSize.object();
}

bsl::size_t& UdpOption::windowScale()
{
    BSLS_ASSERT(isWindowScale());
    return d_windowScale.object();
}

ntsa::UdpSequenceRangeVector& UdpOption::selectiveAck()
{
    BSLS_ASSERT(isSelectiveAck());
    return d_selectiveAck.object();
}

ntsa::UdpTimePointInterval& UdpOption::timestamp()
{
    BSLS_ASSERT(isTimestamp());
    return d_timestamp.object();
}

bdlb::Guid& UdpOption::fastOpen()
{
    BSLS_ASSERT(isFastOpen());
    return d_fastOpen.object();
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

        if (type == ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE) {
            if (payloadSize != sizeof(bsl::uint16_t)) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::uint16_t maxSegmentSize;
            error = decoder->decodeUint16(&maxSegmentSize);
            if (error) {
                return error;
            }

            this->makeMaxSegmentSize(static_cast<bsl::size_t>(maxSegmentSize));
        }
        else if (type == ntsa::UdpOptionType::e_WINDOW_SCALE) {
            if (payloadSize != sizeof(bsl::uint8_t)) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::uint8_t windowScale;
            error = decoder->decodeUint8(&windowScale);
            if (error) {
                return error;
            }

            this->makeWindowScale(static_cast<bsl::size_t>(windowScale));
        }
        else if (type == ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED) {
            if (payloadSize != 0) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            this->makeSelectiveAckPermitted();
        }
        else if (type == ntsa::UdpOptionType::e_SELECTIVE_ACK) {
            if (payloadSize > 4) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ntsa::UdpSequenceRangeVector& selectiveAck =
                this->makeSelectiveAck();

            selectiveAck.resize(payloadSize);

            for (bsl::size_t i = 0; i < selectiveAck.size(); ++i) {
                bsl::uint32_t oldest;
                error = decoder->decodeUint32(&oldest);
                if (error) {
                    return error;
                }

                bsl::uint32_t newest;
                error = decoder->decodeUint32(&newest);
                if (error) {
                    return error;
                }

                selectiveAck[i].setOldest(ntsa::UdpSequenceNumber(oldest));
                selectiveAck[i].setNewest(ntsa::UdpSequenceNumber(newest));
            }
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
        else if (type == ntsa::UdpOptionType::e_FAST_OPEN) {
            if (payloadSize != sizeof(bdlb::Guid)) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bdlb::Guid& guid = this->makeFastOpen();

            error = decoder->decodeRaw(&guid, sizeof(bdlb::Guid));
            if (error) {
                return error;
            }
        }
        else {
            BSLS_LOG_WARN("Unknown UDP option %d", static_cast<int>(type));
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error UdpOption::encode(ntsa::PacketEncoder* encoder, bool final) const
{
    ntsa::Error error;

    if (d_type == ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE) {
        if (d_maxSegmentSize.object() >
            bsl::numeric_limits<bsl::uint16_t>::max())
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const bsl::size_t payloadSize = sizeof(bdlb::BigEndianUint16);

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
            static_cast<bsl::uint16_t>(d_maxSegmentSize.object()));
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::UdpOptionType::e_WINDOW_SCALE) {
        if (d_windowScale.object() > bsl::numeric_limits<bsl::uint8_t>::max())
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const bsl::size_t payloadSize = sizeof(bsl::uint8_t);

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

        error = encoder->encodeUint8(
            static_cast<bsl::uint8_t>(d_windowScale.object()));
        if (error) {
            return error;
        }
    }
    else if (d_type == ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED) {
        const bsl::size_t payloadSize = 0;

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
    }
    else if (d_type == ntsa::UdpOptionType::e_SELECTIVE_ACK) {
        const bsl::size_t payloadSize =
            d_selectiveAck.object().size() *
            (sizeof(bdlb::BigEndianUint32) + sizeof(bdlb::BigEndianUint32));

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

        error = encoder->encodeUint8(
            static_cast<bsl::uint8_t>(2 + d_selectiveAck.object().size()));
        if (error) {
            return error;
        }

        for (bsl::size_t i = 0; i < d_selectiveAck.object().size(); ++i) {
            error = encoder->encodeUint32(
                d_selectiveAck.object()[i].oldest().value());
            if (error) {
                return error;
            }

            error = encoder->encodeUint32(
                d_selectiveAck.object()[i].newest().value());
            if (error) {
                return error;
            }
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
    else if (d_type == ntsa::UdpOptionType::e_FAST_OPEN) {
        const bsl::size_t payloadSize = sizeof(bdlb::Guid);

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

        error = encoder->encodeRaw(&d_fastOpen.object(), sizeof(bdlb::Guid));
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

bsl::size_t UdpOption::maxSegmentSize() const
{
    BSLS_ASSERT(isMaxSegmentSize());
    return d_maxSegmentSize.object();
}

bsl::size_t UdpOption::windowScale() const
{
    BSLS_ASSERT(isWindowScale());
    return d_windowScale.object();
}

const ntsa::UdpSequenceRangeVector& UdpOption::selectiveAck() const
{
    BSLS_ASSERT(isSelectiveAck());
    return d_selectiveAck.object();
}

const ntsa::UdpTimePointInterval& UdpOption::timestamp() const
{
    BSLS_ASSERT(isTimestamp());
    return d_timestamp.object();
}

const bdlb::Guid& UdpOption::fastOpen() const
{
    BSLS_ASSERT(isFastOpen());
    return d_fastOpen.object();
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
    case ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE:
        return "maxSegmentSize";
    case ntsa::UdpOptionType::e_WINDOW_SCALE:
        return "windowScale";
    case ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED:
        return "selectiveAckPermitted";
    case ntsa::UdpOptionType::e_SELECTIVE_ACK:
        return "selectiveAck";
    case ntsa::UdpOptionType::e_TIMESTAMP:
        return "timestamp";
    case ntsa::UdpOptionType::e_FAST_OPEN:
        return "fastOpen";
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

bool UdpOption::isMaxSegmentSize() const
{
    return d_type == ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE;
}

bool UdpOption::isWindowScale() const
{
    return d_type == ntsa::UdpOptionType::e_WINDOW_SCALE;
}

bool UdpOption::isSelectiveAckPermitted() const
{
    return d_type == ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED;
}

bool UdpOption::isSelectiveAck() const
{
    return d_type == ntsa::UdpOptionType::e_SELECTIVE_ACK;
}

bool UdpOption::isTimestamp() const
{
    return d_type == ntsa::UdpOptionType::e_TIMESTAMP;
}

bool UdpOption::isFastOpen() const
{
    return d_type == ntsa::UdpOptionType::e_FAST_OPEN;
}

bool UdpOption::equals(const UdpOption& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::UdpOptionType::e_PADDING:
        return true;
    case ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE:
        return d_maxSegmentSize.object() == other.d_maxSegmentSize.object();
    case ntsa::UdpOptionType::e_WINDOW_SCALE:
        return d_windowScale.object() == other.d_windowScale.object();
    case ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED:
        return true;
    case ntsa::UdpOptionType::e_SELECTIVE_ACK:
        return d_selectiveAck.object() == other.d_selectiveAck.object();
    case ntsa::UdpOptionType::e_TIMESTAMP:
        return d_timestamp.object() == other.d_timestamp.object();
    case ntsa::UdpOptionType::e_FAST_OPEN:
        return d_fastOpen.object() == other.d_fastOpen.object();
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
    case ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE:
        return d_maxSegmentSize.object() < other.d_maxSegmentSize.object();
    case ntsa::UdpOptionType::e_WINDOW_SCALE:
        return d_windowScale.object() < other.d_windowScale.object();
    case ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED:
        return false;
    case ntsa::UdpOptionType::e_SELECTIVE_ACK:
        return d_selectiveAck.object() < other.d_selectiveAck.object();
    case ntsa::UdpOptionType::e_TIMESTAMP:
        return d_timestamp.object() < other.d_timestamp.object();
    case ntsa::UdpOptionType::e_FAST_OPEN:
        return d_fastOpen.object() < other.d_fastOpen.object();
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
    case ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE:
        printer.printAttribute("maxSegmentSize", d_maxSegmentSize.object());
    case ntsa::UdpOptionType::e_WINDOW_SCALE:
        printer.printAttribute("windowScale", d_windowScale.object());
    case ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED:
        printer.printAttribute("selectiveAckPermitted", true);
    case ntsa::UdpOptionType::e_SELECTIVE_ACK:
        printer.printAttribute("selectiveAck", d_selectiveAck.object());
    case ntsa::UdpOptionType::e_TIMESTAMP:
        printer.printAttribute("timestamp", d_timestamp.object());
    case ntsa::UdpOptionType::e_FAST_OPEN:
        printer.printAttribute("fastOpen", d_fastOpen.object());
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
    else if (d_type == ntsa::UdpOptionType::e_MAX_SEGMENT_SIZE) {
        printer->printAttribute("maxSegmentSize", d_maxSegmentSize.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_WINDOW_SCALE) {
        printer->printAttribute("windowScale", d_windowScale.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_SELECTIVE_ACK_PERMITTED) {
        printer->printAttribute("selectiveAckPermitted", true);
    }
    else if (d_type == ntsa::UdpOptionType::e_SELECTIVE_ACK) {
        printer->printAttribute("selectiveAck", d_selectiveAck.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_TIMESTAMP) {
        printer->printAttribute("timestamp", d_timestamp.object());
    }
    else if (d_type == ntsa::UdpOptionType::e_FAST_OPEN) {
        printer->printAttribute("fastOpen", d_fastOpen.object());
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
