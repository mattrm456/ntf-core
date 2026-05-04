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

ntsa::Error TcpOption::decode(const ntsa::ConstBuffer& buffer,
                              bsl::size_t*             size)
{
    const bsl::uint8_t* begin =
        static_cast<const bsl::uint8_t*>(buffer.data());

    const bsl::uint8_t* current = begin;
    const bsl::uint8_t* end     = begin + buffer.size();

    *size = 0;

    if (current == end) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    const bsl::uint8_t type = *current++;

    if (type == ntsa::TcpOptionType::e_UNDEFINED) {
        *size = current - begin;
        return ntsa::Error(ntsa::Error::e_EOF);
    }
    else if (type == ntsa::TcpOptionType::e_PADDING) {
        this->makePadding();
        *size = current - begin;
        return ntsa::Error();
    }
    else {
        if (current == end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bsl::uint8_t payloadSize = *current++;

        if (payloadSize < 2) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        payloadSize -= 2;

        if (payloadSize > static_cast<bsl::size_t>(end - current)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (type == ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE) {
            if (payloadSize != sizeof(bdlb::BigEndianUint16)) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bdlb::BigEndianUint16 maxSegmentSize;
            bsl::memcpy(reinterpret_cast<void*>(&maxSegmentSize),
                        current,
                        sizeof maxSegmentSize);
            current += sizeof maxSegmentSize;

            this->makeMaxSegmentSize(static_cast<bsl::size_t>(
                static_cast<bsl::uint16_t>(maxSegmentSize)));
        }
        else if (type == ntsa::TcpOptionType::e_WINDOW_SCALE) {
            if (payloadSize != sizeof(bsl::uint8_t)) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            const bsl::uint8_t windowScale = *current++;

            this->makeWindowScale(static_cast<bsl::size_t>(windowScale));
        }
        else if (type == ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED) {
            if (payloadSize != 0) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            this->makeSelectiveAckPermitted();
        }
        else if (type == ntsa::TcpOptionType::e_SELECTIVE_ACK) {
            payloadSize = payloadSize * (2 * sizeof(bdlb::BigEndianUint32));

            if (current + payloadSize >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            BSLS_LOG_WARN("Unsupported TCP option %d", static_cast<int>(type));
            return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
        }
        else if (type == ntsa::TcpOptionType::e_TIMESTAMP) {
            if (payloadSize != 2 * sizeof(bsl::uint32_t)) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bdlb::BigEndianUint32 txTimestamp;
            bsl::memcpy(reinterpret_cast<void*>(&txTimestamp),
                        current,
                        sizeof txTimestamp);
            current += sizeof txTimestamp;

            bdlb::BigEndianUint32 rxTimestamp;
            bsl::memcpy(reinterpret_cast<void*>(&rxTimestamp),
                        current,
                        sizeof rxTimestamp);
            current += sizeof rxTimestamp;

            ntsa::TcpTimePointInterval timestamp;

            timestamp.setTx(
                ntsa::TcpTimePoint(static_cast<bsl::uint32_t>(txTimestamp)));

            timestamp.setRx(
                ntsa::TcpTimePoint(static_cast<bsl::uint32_t>(rxTimestamp)));

            this->makeTimestamp(timestamp);
        }
        else if (type == ntsa::TcpOptionType::e_FAST_OPEN) {
            if (payloadSize != sizeof(bdlb::Guid)) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bdlb::Guid guid;
            bsl::memcpy(reinterpret_cast<void*>(&guid), current, sizeof guid);
            current += sizeof guid;

            this->makeFastOpen(guid);
        }
        else {
            BSLS_LOG_WARN("Unknown TCP option %d", static_cast<int>(type));
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *size = current - begin;
        return ntsa::Error();
    }
}

ntsa::Error TcpOption::encode(ntsa::MutableBuffer* buffer,
                              bsl::size_t*         size) const
{
    NTSCFG_WARNING_UNUSED(buffer);
    NTSCFG_WARNING_UNUSED(size);

    NTSCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
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
    case ntsa::TcpOptionType::e_MAX_SEGMENT_SIZE:
        printer.printAttribute("maxSegmentSize", d_maxSegmentSize.object());
    case ntsa::TcpOptionType::e_WINDOW_SCALE:
        printer.printAttribute("windowScale", d_windowScale.object());
    case ntsa::TcpOptionType::e_SELECTIVE_ACK_PERMITTED:
        printer.printAttribute("selectiveAckPermitted", true);
    case ntsa::TcpOptionType::e_SELECTIVE_ACK:
        printer.printAttribute("selectiveAck", d_selectiveAck.object());
    case ntsa::TcpOptionType::e_TIMESTAMP:
        printer.printAttribute("timestamp", d_timestamp.object());
    case ntsa::TcpOptionType::e_FAST_OPEN:
        printer.printAttribute("fastOpen", d_fastOpen.object());
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

}  // close namespace ntsa
}  // close namespace BloombergLP
