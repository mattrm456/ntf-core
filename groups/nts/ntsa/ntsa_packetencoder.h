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

#ifndef INCLUDED_NTSA_PACKETENCODER
#define INCLUDED_NTSA_PACKETENCODER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_packetencodercontext.h>
#include <ntsa_packetencoderoptions.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bsls_assert.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_memory.h>
#include <bsl_streambuf.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a raw packet decoder.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class PacketEncoder
{
    bdlbb::BlobBuffer* d_buffer;
    bsl::uint8_t*      d_begin;
    bsl::uint8_t*      d_current;
    bsl::uint8_t*      d_end;
    bsl::size_t        d_capacity;

  private:
    PacketEncoder(const PacketEncoder&) BSLS_KEYWORD_DELETED;
    PacketEncoder& operator=(const PacketEncoder&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new packet encoder that writes to the specified 'blobBuffer'.
    explicit PacketEncoder(bdlbb::BlobBuffer* blobBuffer);

    /// Destroy this object.
    ~PacketEncoder();

    /// Encode the specified signed 8-bit integer 'value'. Return the error.
    ntsa::Error encodeInt8(bsl::int8_t value);

    /// Encode the specified unsigned 8-bit integer 'value'. Return the error.
    ntsa::Error encodeUint8(bsl::uint8_t value);

    /// Encode the specified signed 16-bit integer 'value'. Return the error.
    ntsa::Error encodeInt16(bsl::int16_t value);

    /// Encode the specified signed 16-bit integer 'value'. Return the error.
    ntsa::Error encodeInt16(bdlb::BigEndianInt16 value);

    /// Encode the specified unsigned 16-bit integer 'value'. Return the error.
    ntsa::Error encodeUint16(bsl::uint16_t value);

    /// Encode the specified unsigned 16-bit integer 'value'. Return the error.
    ntsa::Error encodeUint16(bdlb::BigEndianUint16 value);

    /// Encode the specified signed 32-bit integer 'value'. Return the error.
    ntsa::Error encodeInt32(bsl::int32_t value);

    /// Encode the specified signed 32-bit integer 'value'. Return the error.
    ntsa::Error encodeInt32(bdlb::BigEndianInt32 value);

    /// Encode the specified unsigned 32-bit integer 'value'. Return the error.
    ntsa::Error encodeUint32(bsl::uint32_t value);

    /// Encode the specified unsigned 32-bit integer 'value'. Return the error.
    ntsa::Error encodeUint32(bdlb::BigEndianUint32 value);

    /// Encode the specified signed 64-bit integer 'value'. Return the error.
    ntsa::Error encodeInt64(bsl::int64_t value);

    /// Encode the specified signed 64-bit integer 'value'. Return the error.
    ntsa::Error encodeInt64(bdlb::BigEndianInt64 value);

    /// Encode the specified unsigned 64-bit integer 'value'. Return the error.
    ntsa::Error encodeUint64(bsl::uint64_t value);

    /// Encode the specified unsigned 64-bit integer 'value'. Return the error.
    ntsa::Error encodeUint64(bdlb::BigEndianUint64 value);

    /// Encode the specified 'size' bytes verbatim from the specified 'source'.
    /// Return the error.
    ntsa::Error encodeRaw(const void* source, bsl::size_t size);

    /// Encode the specified 'size' bytes verbatim from the specified 'source'.
    /// Return the error.
    ntsa::Error encodeRaw(const bdlbb::BlobBuffer& source, bsl::size_t size);

    /// Set the position of the next byte to be encoded to the specified
    /// 'position'. Return the error.
    ntsa::Error seek(bsl::size_t position);

    /// Increment the position of the next byte to be encoded by the specified
    /// 'amount'. Return the error.
    ntsa::Error advance(bsl::size_t amount);

    /// Decrement the position of the next byte to be encoded by the specified
    /// 'amount'. Return the error.
    ntsa::Error rewind(bsl::size_t amount);

    /// Synchronize the underlying device. Return the error.
    ntsa::Error flush();

    /// Return the address of the next byte to be encoded.
    bsl::uint8_t* next() const;

    /// Return the offset from the beginning of the buffer to the next byte to
    /// be encoded.
    bsl::size_t position() const;

    /// Return the capacity of the packet buffer.
    bsl::size_t capacity() const;
};

NTSCFG_INLINE
PacketEncoder::PacketEncoder(bdlbb::BlobBuffer* blobBuffer)
: d_buffer(blobBuffer)
, d_begin(reinterpret_cast<bsl::uint8_t*>(d_buffer->data()))
, d_current(d_begin)
, d_end(d_begin + d_buffer->size())
, d_capacity(static_cast<bsl::size_t>(blobBuffer->size()))
{
}

NTSCFG_INLINE
PacketEncoder::~PacketEncoder()
{
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeInt8(bsl::int8_t value)
{
    if (d_capacity < sizeof(bsl::int8_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    *d_current = value;

    d_current  += sizeof(bsl::int8_t);
    d_capacity -= sizeof(bsl::int8_t);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeUint8(bsl::uint8_t value)
{
    if (d_capacity < sizeof(bsl::uint8_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    *d_current = static_cast<char>(value);

    d_current  += sizeof(bsl::uint8_t);
    d_capacity -= sizeof(bsl::uint8_t);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeInt16(bsl::int16_t value)
{
    BSLMF_ASSERT(sizeof(bsl::int16_t) == sizeof(bdlb::BigEndianInt16));

    if (d_capacity < sizeof(bsl::int16_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bdlb::BigEndianInt16 bigEndianValue;
    bigEndianValue = value;

    bsl::memcpy(d_current, &bigEndianValue, sizeof(bigEndianValue));

    d_current  += sizeof(bigEndianValue);
    d_capacity -= sizeof(bigEndianValue);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeInt16(bdlb::BigEndianInt16 value)
{
    BSLMF_ASSERT(sizeof(bsl::int16_t) == sizeof(bdlb::BigEndianInt16));

    if (d_capacity < sizeof(bsl::int16_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bsl::memcpy(d_current, &value, sizeof(value));

    d_current  += sizeof(value);
    d_capacity -= sizeof(value);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeUint16(bsl::uint16_t value)
{
    BSLMF_ASSERT(sizeof(bsl::uint16_t) == sizeof(bdlb::BigEndianUint16));

    if (d_capacity < sizeof(bsl::uint16_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bdlb::BigEndianUint16 bigEndianValue;
    bigEndianValue = value;

    bsl::memcpy(d_current, &bigEndianValue, sizeof(bigEndianValue));

    d_current  += sizeof(bigEndianValue);
    d_capacity -= sizeof(bigEndianValue);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeUint16(bdlb::BigEndianUint16 value)
{
    BSLMF_ASSERT(sizeof(bsl::uint16_t) == sizeof(bdlb::BigEndianUint16));

    if (d_capacity < sizeof(bsl::uint16_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bsl::memcpy(d_current, &value, sizeof(value));

    d_current  += sizeof(value);
    d_capacity -= sizeof(value);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeInt32(bsl::int32_t value)
{
    BSLMF_ASSERT(sizeof(bsl::int32_t) == sizeof(bdlb::BigEndianInt32));

    if (d_capacity < sizeof(bsl::int32_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bdlb::BigEndianInt32 bigEndianValue;
    bigEndianValue = value;

    bsl::memcpy(d_current, &bigEndianValue, sizeof(bigEndianValue));

    d_current  += sizeof(bigEndianValue);
    d_capacity -= sizeof(bigEndianValue);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeInt32(bdlb::BigEndianInt32 value)
{
    BSLMF_ASSERT(sizeof(bsl::int32_t) == sizeof(bdlb::BigEndianInt32));

    if (d_capacity < sizeof(bsl::int32_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bsl::memcpy(d_current, &value, sizeof(value));

    d_current  += sizeof(value);
    d_capacity -= sizeof(value);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeUint32(bsl::uint32_t value)
{
    BSLMF_ASSERT(sizeof(bsl::uint32_t) == sizeof(bdlb::BigEndianUint32));

    if (d_capacity < sizeof(bsl::uint32_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bdlb::BigEndianUint32 bigEndianValue;
    bigEndianValue = value;

    bsl::memcpy(d_current, &bigEndianValue, sizeof(bigEndianValue));

    d_current  += sizeof(bigEndianValue);
    d_capacity -= sizeof(bigEndianValue);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeUint32(bdlb::BigEndianUint32 value)
{
    BSLMF_ASSERT(sizeof(bsl::uint32_t) == sizeof(bdlb::BigEndianUint32));

    if (d_capacity < sizeof(bsl::uint32_t)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bsl::memcpy(d_current, &value, sizeof(value));

    d_current  += sizeof(value);
    d_capacity -= sizeof(value);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeInt64(bsl::int64_t value)
{
    BSLMF_ASSERT(sizeof(bsls::Types::Int64) == sizeof(bdlb::BigEndianInt64));

    if (d_capacity < sizeof(bsls::Types::Int64)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bdlb::BigEndianInt64 bigEndianValue;
    bigEndianValue = static_cast<bsls::Types::Int64>(value);

    bsl::memcpy(d_current, &bigEndianValue, sizeof(bigEndianValue));

    d_current  += sizeof(bigEndianValue);
    d_capacity -= sizeof(bigEndianValue);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeInt64(bdlb::BigEndianInt64 value)
{
    BSLMF_ASSERT(sizeof(bsls::Types::Int64) == sizeof(bdlb::BigEndianInt64));

    if (d_capacity < sizeof(bsls::Types::Int64)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bsl::memcpy(d_current, &value, sizeof(value));

    d_current  += sizeof(value);
    d_capacity -= sizeof(value);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeUint64(bsl::uint64_t value)
{
    BSLMF_ASSERT(sizeof(bsls::Types::Uint64) == sizeof(bdlb::BigEndianUint64));

    if (d_capacity < sizeof(bsls::Types::Uint64)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bdlb::BigEndianUint64 bigEndianValue;
    bigEndianValue = static_cast<bsls::Types::Uint64>(value);

    bsl::memcpy(d_current, &bigEndianValue, sizeof(bigEndianValue));

    d_current  += sizeof(bigEndianValue);
    d_capacity -= sizeof(bigEndianValue);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeUint64(bdlb::BigEndianUint64 value)
{
    BSLMF_ASSERT(sizeof(bsls::Types::Uint64) == sizeof(bdlb::BigEndianUint64));

    if (d_capacity < sizeof(bsls::Types::Uint64)) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bsl::memcpy(d_current, &value, sizeof(value));

    d_current  += sizeof(value);
    d_capacity -= sizeof(value);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeRaw(const void* source, bsl::size_t size)
{
    if (d_capacity < size) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bsl::memcpy(d_current, source, size);

    d_current  += size;
    d_capacity -= size;

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::encodeRaw(const bdlbb::BlobBuffer& source,
                                     bsl::size_t              size)
{
    if (d_capacity < size) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    if (size > static_cast<bsl::size_t>(source.size())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::memcpy(d_current, source.data(), size);

    d_current  += size;
    d_capacity -= size;

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::seek(bsl::size_t position)
{
    if (d_begin + position <= d_end) {
        d_current  = d_begin + position;
        d_capacity  = static_cast<bsl::size_t>(d_end - d_current);
        return ntsa::Error();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::advance(bsl::size_t amount)
{
    if (d_current + amount <= d_end) {
        d_current  += amount;
        d_capacity  = static_cast<bsl::size_t>(d_end - d_current);
        return ntsa::Error();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::rewind(bsl::size_t amount)
{
    if (d_current - amount >= d_begin) {
        d_current  -= amount;
        d_capacity  = static_cast<bsl::size_t>(d_end - d_current);
        return ntsa::Error();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
ntsa::Error PacketEncoder::flush()
{
    d_buffer->setSize(static_cast<int>(d_current - d_begin));
    return ntsa::Error();
}

NTSCFG_INLINE
bsl::uint8_t* PacketEncoder::next() const
{
    return d_current;
}

NTSCFG_INLINE
bsl::size_t PacketEncoder::position() const
{
    return static_cast<bsl::size_t>(d_current - d_begin);
}

NTSCFG_INLINE
bsl::size_t PacketEncoder::capacity() const
{
    return static_cast<bsl::size_t>(d_end - d_begin);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
