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

#include <ntsa_tcpoption.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcpoption_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsa {

bsl::size_t TcpOption::paddingSize(const bsl::uint8_t* cursor,
                                   bsl::size_t         optionSize)
{
    const bsl::size_t address = static_cast<bsl::size_t>(
        reinterpret_cast<bsl::uintptr_t>(cursor + optionSize));

    return (4 - (address & 3)) & 3;
}

TcpOption::TcpOption(bslma::Allocator* basicAllocator)
: d_type(ntsa::TcpOptionType::e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TcpOption::TcpOption(const TcpOption& other, bslma::Allocator* basicAllocator)
: d_type(other.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_type) {
    case ntsa::TcpOptionType::e_PADDING:
        break;
    case ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE:
        new (d_maxSegmentSize.buffer())
            bsl::size_t(other.d_maxSegmentSize.object());
        break;
    case ntsa::TcpOptionType::e_WINDOW_SCALE:
        new (d_windowScale.buffer()) bsl::size_t(other.d_windowScale.object());
        break;
    case ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED:
        break;
    case ntsa::TcpOptionType::e_SELECTIVE_ACK:
        new (d_selectiveAck.buffer())
            ntsa::TcpSequenceRangeVector(other.d_selectiveAck.object(),
                                         d_allocator_p);
        break;
    case ntsa::TcpOptionType::e_TIMESTAMP:
        new (d_timestamp.buffer())
            ntsa::TcpTimePointInterval(other.d_timestamp.object());
        break;
    case ntsa::TcpOptionType::e_FAST_OPEN:
        new (d_fastOpen.buffer()) bdlb::Guid(other.d_fastOpen.object());
        break;
    case ntsa::TcpOptionType::e_UNASSIGNED:
        new (d_unassigned.buffer())
            ntsa::TcpOptionValue(other.d_unassigned.object(), d_allocator_p);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::TcpOptionType::e_UNDEFINED);
    }
}

TcpOption::~TcpOption()
{
    if (isSelectiveAck()) {
        typedef ntsa::TcpSequenceRangeVector Type;
        d_selectiveAck.object().~Type();
    }
    else if (isUnassigned()) {
        typedef ntsa::TcpOptionValue Type;
        d_unassigned.object().~Type();
    }
}

TcpOption& TcpOption::operator=(const TcpOption& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    switch (other.d_type) {
    case ntsa::TcpOptionType::e_PADDING:
        break;
    case ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE:
        new (d_maxSegmentSize.buffer())
            bsl::size_t(other.d_maxSegmentSize.object());
        break;
    case ntsa::TcpOptionType::e_WINDOW_SCALE:
        new (d_windowScale.buffer()) bsl::size_t(other.d_windowScale.object());
        break;
    case ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED:
        break;
    case ntsa::TcpOptionType::e_SELECTIVE_ACK:
        new (d_selectiveAck.buffer())
            ntsa::TcpSequenceRangeVector(other.d_selectiveAck.object(),
                                         d_allocator_p);
        break;
    case ntsa::TcpOptionType::e_TIMESTAMP:
        new (d_timestamp.buffer())
            ntsa::TcpTimePointInterval(other.d_timestamp.object());
        break;
    case ntsa::TcpOptionType::e_FAST_OPEN:
        new (d_fastOpen.buffer()) bdlb::Guid(other.d_fastOpen.object());
        break;
    case ntsa::TcpOptionType::e_UNASSIGNED:
        new (d_unassigned.buffer())
            ntsa::TcpOptionValue(other.d_unassigned.object(), d_allocator_p);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::TcpOptionType::e_UNDEFINED);
    }

    d_type = other.d_type;

    return *this;
}

void TcpOption::reset()
{
    if (isSelectiveAck()) {
        typedef ntsa::TcpSequenceRangeVector Type;
        d_selectiveAck.object().~Type();
    }
    else if (isUnassigned()) {
        typedef ntsa::TcpOptionValue Type;
        d_unassigned.object().~Type();
    }

    d_type = ntsa::TcpOptionType::e_UNDEFINED;
}

void TcpOption::makePadding()
{
    if (d_type != ntsa::TcpOptionType::e_PADDING) {
        reset();
        d_type = ntsa::TcpOptionType::e_PADDING;
    }
}

bsl::size_t& TcpOption::makeMaxSegmentSize()
{
    if (d_type == ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE) {
        d_maxSegmentSize.object() = 0;
    }
    else {
        this->reset();
        new (d_maxSegmentSize.buffer()) bsl::size_t(0);
        d_type = ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE;
    }

    return d_maxSegmentSize.object();
}

bsl::size_t& TcpOption::makeMaxSegmentSize(bsl::size_t value)
{
    if (d_type == ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE) {
        d_maxSegmentSize.object() = value;
    }
    else {
        this->reset();
        new (d_maxSegmentSize.buffer()) bsl::size_t(value);
        d_type = ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE;
    }

    return d_maxSegmentSize.object();
}

bsl::size_t& TcpOption::makeWindowScale()
{
    if (d_type == ntsa::TcpOptionType::e_WINDOW_SCALE) {
        d_windowScale.object() = 0;
    }
    else {
        this->reset();
        new (d_windowScale.buffer()) bsl::size_t(0);
        d_type = ntsa::TcpOptionType::e_WINDOW_SCALE;
    }

    return d_windowScale.object();
}

bsl::size_t& TcpOption::makeWindowScale(bsl::size_t value)
{
    if (d_type == ntsa::TcpOptionType::e_WINDOW_SCALE) {
        d_windowScale.object() = value;
    }
    else {
        this->reset();
        new (d_windowScale.buffer()) bsl::size_t(value);
        d_type = ntsa::TcpOptionType::e_WINDOW_SCALE;
    }

    return d_windowScale.object();
}

void TcpOption::makeSelectiveAckPermitted()
{
    if (d_type != ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED) {
        reset();
        d_type = ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED;
    }
}

ntsa::TcpSequenceRangeVector& TcpOption::makeSelectiveAck()
{
    if (d_type == ntsa::TcpOptionType::e_SELECTIVE_ACK) {
        d_selectiveAck.object().clear();
    }
    else {
        this->reset();
        new (d_selectiveAck.buffer())
            ntsa::TcpSequenceRangeVector(d_allocator_p);
        d_type = ntsa::TcpOptionType::e_SELECTIVE_ACK;
    }

    return d_selectiveAck.object();
}

ntsa::TcpSequenceRangeVector& TcpOption::makeSelectiveAck(
    const ntsa::TcpSequenceRangeVector& value)
{
    if (d_type == ntsa::TcpOptionType::e_SELECTIVE_ACK) {
        d_selectiveAck.object() = value;
    }
    else {
        this->reset();
        new (d_selectiveAck.buffer())
            ntsa::TcpSequenceRangeVector(value, d_allocator_p);
        d_type = ntsa::TcpOptionType::e_SELECTIVE_ACK;
    }

    return d_selectiveAck.object();
}

ntsa::TcpTimePointInterval& TcpOption::makeTimestamp()
{
    if (d_type == ntsa::TcpOptionType::e_TIMESTAMP) {
        d_timestamp.object().reset();
    }
    else {
        this->reset();
        new (d_timestamp.buffer()) ntsa::TcpTimePointInterval();
        d_type = ntsa::TcpOptionType::e_TIMESTAMP;
    }

    return d_timestamp.object();
}

ntsa::TcpTimePointInterval& TcpOption::makeTimestamp(
    const ntsa::TcpTimePointInterval& value)
{
    if (d_type == ntsa::TcpOptionType::e_TIMESTAMP) {
        d_timestamp.object() = value;
    }
    else {
        this->reset();
        new (d_timestamp.buffer()) ntsa::TcpTimePointInterval(value);
        d_type = ntsa::TcpOptionType::e_TIMESTAMP;
    }

    return d_timestamp.object();
}

bdlb::Guid& TcpOption::makeFastOpen()
{
    if (d_type == ntsa::TcpOptionType::e_FAST_OPEN) {
        d_fastOpen.object() = bdlb::Guid();
    }
    else {
        this->reset();
        new (d_fastOpen.buffer()) bdlb::Guid();
        d_type = ntsa::TcpOptionType::e_FAST_OPEN;
    }

    return d_fastOpen.object();
}

bdlb::Guid& TcpOption::makeFastOpen(const bdlb::Guid& value)
{
    if (d_type == ntsa::TcpOptionType::e_FAST_OPEN) {
        d_fastOpen.object() = value;
    }
    else {
        this->reset();
        new (d_fastOpen.buffer()) bdlb::Guid(value);
        d_type = ntsa::TcpOptionType::e_FAST_OPEN;
    }

    return d_fastOpen.object();
}

ntsa::TcpOptionValue& TcpOption::makeUnassigned()
{
    if (d_type == ntsa::TcpOptionType::e_UNASSIGNED) {
        d_unassigned.object().reset();
    }
    else {
        this->reset();
        new (d_unassigned.buffer()) ntsa::TcpOptionValue(d_allocator_p);
        d_type = ntsa::TcpOptionType::e_UNASSIGNED;
    }

    return d_unassigned.object();
}

ntsa::TcpOptionValue& TcpOption::makeUnassigned(
    const ntsa::TcpOptionValue& value)
{
    if (d_type == ntsa::TcpOptionType::e_UNASSIGNED) {
        d_unassigned.object() = value;
    }
    else {
        this->reset();
        new (d_unassigned.buffer()) ntsa::TcpOptionValue(value, d_allocator_p);
        d_type = ntsa::TcpOptionType::e_UNASSIGNED;
    }

    return d_unassigned.object();
}

bsl::size_t& TcpOption::maxSegmentSize()
{
    BSLS_ASSERT(isMaxSegmentSize());
    return d_maxSegmentSize.object();
}

bsl::size_t& TcpOption::windowScale()
{
    BSLS_ASSERT(isWindowScale());
    return d_windowScale.object();
}

ntsa::TcpSequenceRangeVector& TcpOption::selectiveAck()
{
    BSLS_ASSERT(isSelectiveAck());
    return d_selectiveAck.object();
}

ntsa::TcpTimePointInterval& TcpOption::timestamp()
{
    BSLS_ASSERT(isTimestamp());
    return d_timestamp.object();
}

bdlb::Guid& TcpOption::fastOpen()
{
    BSLS_ASSERT(isFastOpen());
    return d_fastOpen.object();
}

ntsa::TcpOptionValue& TcpOption::unassigned()
{
    BSLS_ASSERT(isUnassigned());
    return d_unassigned.object();
}

ntsa::Error TcpOption::decode(ntsa::PacketDecoder* decoder)
{
    ntsa::Error error;

    this->reset();

    bsl::uint8_t type;
    error = decoder->decodeUint8(&type);
    if (error) {
        return error;
    }

    if (type == ntsa::TcpOptionType::e_UNDEFINED) {
        this->reset();
    }
    else if (type == ntsa::TcpOptionType::e_PADDING) {
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

        if (type == ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE) {
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
        else if (type == ntsa::TcpOptionType::e_WINDOW_SCALE) {
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
        else if (type == ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED) {
            if (payloadSize != 0) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            this->makeSelectiveAckPermitted();
        }
        else if (type == ntsa::TcpOptionType::e_SELECTIVE_ACK) {
            if (payloadSize > 4) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ntsa::TcpSequenceRangeVector& selectiveAck =
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

                selectiveAck[i].setOldest(ntsa::TcpSequenceNumber(oldest));
                selectiveAck[i].setNewest(ntsa::TcpSequenceNumber(newest));
            }
        }
        else if (type == ntsa::TcpOptionType::e_TIMESTAMP) {
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

            ntsa::TcpTimePointInterval& timestamp = this->makeTimestamp();

            timestamp.setTx(ntsa::TcpTimePoint(tx));
            timestamp.setRx(ntsa::TcpTimePoint(rx));
        }
        else if (type == ntsa::TcpOptionType::e_FAST_OPEN) {
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
            BSLS_LOG_WARN("Unknown TCP option %d size = %zu",
                          static_cast<int>(type),
                          static_cast<bsl::size_t>(payloadSize));

            ntsa::TcpOptionValue& unassigned = this->makeUnassigned();

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

ntsa::Error TcpOption::encode(ntsa::PacketEncoder* encoder, bool final) const
{
    ntsa::Error error;

    if (d_type == ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE) {
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
                static_cast<bsl::uint8_t>(ntsa::TcpOptionType::e_PADDING));
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
    else if (d_type == ntsa::TcpOptionType::e_WINDOW_SCALE) {
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
                static_cast<bsl::uint8_t>(ntsa::TcpOptionType::e_PADDING));
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
    else if (d_type == ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED) {
        const bsl::size_t payloadSize = 0;

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::TcpOptionType::e_PADDING));
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
    else if (d_type == ntsa::TcpOptionType::e_SELECTIVE_ACK) {
        const bsl::size_t payloadSize =
            d_selectiveAck.object().size() *
            (sizeof(bdlb::BigEndianUint32) + sizeof(bdlb::BigEndianUint32));

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::TcpOptionType::e_PADDING));
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
    else if (d_type == ntsa::TcpOptionType::e_TIMESTAMP) {
        const bsl::size_t payloadSize =
            sizeof(bdlb::BigEndianUint32) + sizeof(bdlb::BigEndianUint32);

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::TcpOptionType::e_PADDING));
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
    else if (d_type == ntsa::TcpOptionType::e_FAST_OPEN) {
        const bsl::size_t payloadSize = sizeof(bdlb::Guid);

        const bsl::size_t optionSize =
            sizeof(bsl::uint8_t) + sizeof(bsl::uint8_t) + payloadSize;

        const bsl::size_t paddingSize =
            final ? this->paddingSize(encoder->next(), optionSize) : 0;

        for (bsl::size_t i = 0; i < paddingSize; ++i) {
            error = encoder->encodeUint8(
                static_cast<bsl::uint8_t>(ntsa::TcpOptionType::e_PADDING));
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
    else if (d_type == ntsa::TcpOptionType::e_UNASSIGNED) {
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
                static_cast<bsl::uint8_t>(ntsa::TcpOptionType::e_PADDING));
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

        error = encoder->encodeRaw(&d_unassigned.object().payload().front(),
                                   payloadSize);
        if (error) {
            return error;
        }
    }
    else if (d_type != ntsa::TcpOptionType::e_PADDING &&
             d_type != ntsa::TcpOptionType::e_UNDEFINED)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

bsl::size_t TcpOption::maxSegmentSize() const
{
    BSLS_ASSERT(isMaxSegmentSize());
    return d_maxSegmentSize.object();
}

bsl::size_t TcpOption::windowScale() const
{
    BSLS_ASSERT(isWindowScale());
    return d_windowScale.object();
}

const ntsa::TcpSequenceRangeVector& TcpOption::selectiveAck() const
{
    BSLS_ASSERT(isSelectiveAck());
    return d_selectiveAck.object();
}

const ntsa::TcpTimePointInterval& TcpOption::timestamp() const
{
    BSLS_ASSERT(isTimestamp());
    return d_timestamp.object();
}

const bdlb::Guid& TcpOption::fastOpen() const
{
    BSLS_ASSERT(isFastOpen());
    return d_fastOpen.object();
}

const ntsa::TcpOptionValue& TcpOption::unassigned() const
{
    BSLS_ASSERT(isUnassigned());
    return d_unassigned.object();
}

ntsa::TcpOptionType::Value TcpOption::type() const
{
    return d_type;
}

const char* TcpOption::name() const
{
    switch (d_type) {
    case ntsa::TcpOptionType::e_UNDEFINED:
        return "end";
    case ntsa::TcpOptionType::e_PADDING:
        return "padding";
    case ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE:
        return "maxSegmentSize";
    case ntsa::TcpOptionType::e_WINDOW_SCALE:
        return "windowScale";
    case ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED:
        return "selectiveAckPermitted";
    case ntsa::TcpOptionType::e_SELECTIVE_ACK:
        return "selectiveAck";
    case ntsa::TcpOptionType::e_TIMESTAMP:
        return "timestamp";
    case ntsa::TcpOptionType::e_FAST_OPEN:
        return "fastOpen";
    case ntsa::TcpOptionType::e_UNASSIGNED:
        return "unassigned";
    default:
        return "???";
    }
}

bool TcpOption::isUndefined() const
{
    return d_type == ntsa::TcpOptionType::e_UNDEFINED;
}

bool TcpOption::isPadding() const
{
    return d_type == ntsa::TcpOptionType::e_PADDING;
}

bool TcpOption::isMaxSegmentSize() const
{
    return d_type == ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE;
}

bool TcpOption::isWindowScale() const
{
    return d_type == ntsa::TcpOptionType::e_WINDOW_SCALE;
}

bool TcpOption::isSelectiveAckPermitted() const
{
    return d_type == ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED;
}

bool TcpOption::isSelectiveAck() const
{
    return d_type == ntsa::TcpOptionType::e_SELECTIVE_ACK;
}

bool TcpOption::isTimestamp() const
{
    return d_type == ntsa::TcpOptionType::e_TIMESTAMP;
}

bool TcpOption::isFastOpen() const
{
    return d_type == ntsa::TcpOptionType::e_FAST_OPEN;
}

bool TcpOption::isUnassigned() const
{
    return d_type == ntsa::TcpOptionType::e_UNASSIGNED;
}

bool TcpOption::equals(const TcpOption& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::TcpOptionType::e_PADDING:
        return true;
    case ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE:
        return d_maxSegmentSize.object() == other.d_maxSegmentSize.object();
    case ntsa::TcpOptionType::e_WINDOW_SCALE:
        return d_windowScale.object() == other.d_windowScale.object();
    case ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED:
        return true;
    case ntsa::TcpOptionType::e_SELECTIVE_ACK:
        return d_selectiveAck.object() == other.d_selectiveAck.object();
    case ntsa::TcpOptionType::e_TIMESTAMP:
        return d_timestamp.object() == other.d_timestamp.object();
    case ntsa::TcpOptionType::e_FAST_OPEN:
        return d_fastOpen.object() == other.d_fastOpen.object();
    case ntsa::TcpOptionType::e_UNASSIGNED:
        return d_unassigned.object() == other.d_unassigned.object();
    default:
        BSLS_ASSERT(d_type == ntsa::TcpOptionType::e_UNDEFINED);
        return true;
    }
}

bool TcpOption::less(const TcpOption& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::TcpOptionType::e_PADDING:
        return false;
    case ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE:
        return d_maxSegmentSize.object() < other.d_maxSegmentSize.object();
    case ntsa::TcpOptionType::e_WINDOW_SCALE:
        return d_windowScale.object() < other.d_windowScale.object();
    case ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED:
        return false;
    case ntsa::TcpOptionType::e_SELECTIVE_ACK:
        return d_selectiveAck.object() < other.d_selectiveAck.object();
    case ntsa::TcpOptionType::e_TIMESTAMP:
        return d_timestamp.object() < other.d_timestamp.object();
    case ntsa::TcpOptionType::e_FAST_OPEN:
        return d_fastOpen.object() < other.d_fastOpen.object();
    case ntsa::TcpOptionType::e_UNASSIGNED:
        return d_unassigned.object() < other.d_unassigned.object();
    default:
        BSLS_ASSERT(d_type == ntsa::TcpOptionType::e_UNDEFINED);
        return false;
    }
}

bsl::ostream& TcpOption::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    switch (d_type) {
    case ntsa::TcpOptionType::e_PADDING:
        printer.printAttribute("padding", true);
        break;
    case ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE:
        printer.printAttribute("maxSegmentSize", d_maxSegmentSize.object());
        break;
    case ntsa::TcpOptionType::e_WINDOW_SCALE:
        printer.printAttribute("windowScale", d_windowScale.object());
        break;
    case ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED:
        printer.printAttribute("selectiveAckPermitted", true);
        break;
    case ntsa::TcpOptionType::e_SELECTIVE_ACK:
        printer.printAttribute("selectiveAck", d_selectiveAck.object());
        break;
    case ntsa::TcpOptionType::e_TIMESTAMP:
        printer.printAttribute("timestamp", d_timestamp.object());
        break;
    case ntsa::TcpOptionType::e_FAST_OPEN:
        printer.printAttribute("fastOpen", d_fastOpen.object());
        break;
    case ntsa::TcpOptionType::e_UNASSIGNED:
        printer.printAttribute("unassigned", d_unassigned.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::TcpOptionType::e_UNDEFINED);
        stream << "UNDEFINED";
        break;
    }

    printer.end();

    return stream;
}

void TcpOption::print(bslim::Printer* printer) const
{
    if (d_type == ntsa::TcpOptionType::e_PADDING) {
        printer->printAttribute("padding", true);
    }
    else if (d_type == ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE) {
        printer->printAttribute("maxSegmentSize", d_maxSegmentSize.object());
    }
    else if (d_type == ntsa::TcpOptionType::e_WINDOW_SCALE) {
        printer->printAttribute("windowScale", d_windowScale.object());
    }
    else if (d_type == ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED) {
        printer->printAttribute("selectiveAckPermitted", true);
    }
    else if (d_type == ntsa::TcpOptionType::e_SELECTIVE_ACK) {
        printer->printAttribute("selectiveAck", d_selectiveAck.object());
    }
    else if (d_type == ntsa::TcpOptionType::e_TIMESTAMP) {
        printer->printAttribute("timestamp", d_timestamp.object());
    }
    else if (d_type == ntsa::TcpOptionType::e_FAST_OPEN) {
        printer->printAttribute("fastOpen", d_fastOpen.object());
    }
    else if (d_type == ntsa::TcpOptionType::e_UNASSIGNED) {
        printer->printAttribute("unassigned", d_unassigned.object());
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const TcpOption& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const TcpOption& lhs, const TcpOption& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TcpOption& lhs, const TcpOption& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const TcpOption& lhs, const TcpOption& rhs)
{
    return lhs.less(rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
