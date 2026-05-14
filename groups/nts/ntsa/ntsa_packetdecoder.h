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

#ifndef INCLUDED_NTSA_PACKETDECODER
#define INCLUDED_NTSA_PACKETDECODER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
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
class PacketDecoder
{
    const bdlbb::BlobBuffer* d_buffer;
    const bsl::uint8_t*      d_begin;
    const bsl::uint8_t*      d_current;
    const bsl::uint8_t*      d_end;
    bsl::size_t              d_size;

  private:
    PacketDecoder(const PacketDecoder&) BSLS_KEYWORD_DELETED;
    PacketDecoder& operator=(const PacketDecoder&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new packet decode that reads from the specified 'blobBuffer'.
    explicit PacketDecoder(const bdlbb::BlobBuffer* blobBuffer);

    /// Destroy this object.
    ~PacketDecoder();

    /// Decode a signed 8-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeInt8(bsl::int8_t* result);

    /// Decode an unsigned 8-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeUint8(bsl::uint8_t* result);

    /// Decode a signed 16-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeInt16(bsl::int16_t* result);

    /// Decode a signed 16-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeInt16(bdlb::BigEndianInt16* result);

    /// Decode an unsigned 16-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeUint16(bsl::uint16_t* result);

    /// Decode an unsigned 16-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeUint16(bdlb::BigEndianUint16* result);

    /// Decode a signed 32-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeInt32(bsl::int32_t* result);

    /// Decode a signed 32-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeInt32(bdlb::BigEndianInt32* result);

    /// Decode an unsigned 32-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeUint32(bsl::uint32_t* result);

    /// Decode an unsigned 32-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeUint32(bdlb::BigEndianUint32* result);

    /// Decode a signed 64-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeInt64(bsls::Types::Int64* result);

    /// Decode a signed 64-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeInt64(bdlb::BigEndianInt64* result);

    /// Decode an unsigned 64-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeUint64(bsls::Types::Uint64* result);

    /// Decode an unsigned 64-bit integer and load it into the specified
    /// 'result'. Return the error.
    ntsa::Error decodeUint64(bdlb::BigEndianUint64* result);

    /// Decode the specified 'size' bytes verbatim and load them into the
    /// specified 'destination'. Return the error.
    ntsa::Error decodeRaw(void* destination, bsl::size_t size);

    /// Decode the specified 'size' bytes verbatim and load them into the
    /// specified 'destination'. Return the error.
    ntsa::Error decodeRaw(bdlbb::BlobBuffer* destination, bsl::size_t size);

    /// Set the position of the next byte to be encoded to the specified
    /// 'position'. Return the error.
    ntsa::Error seek(bsl::size_t position);

    /// Increment the position of the next byte to be decoded by the specified
    /// 'amount'. Return the error.
    ntsa::Error advance(bsl::size_t amount);

    /// Decrement the position of the next byte to be decoded by the specified
    /// 'amount'. Return the error.
    ntsa::Error rewind(bsl::size_t amount);

    /// Return the address of the next byte to be decoded.
    const bsl::uint8_t* next() const;

    /// Return the offset from the beginning of the buffer to the next byte to
    /// be decoded.
    bsl::size_t position() const;

    /// Return the size of the packet buffer.
    bsl::size_t size() const;
};

NTSCFG_INLINE
PacketDecoder::PacketDecoder(const bdlbb::BlobBuffer* blobBuffer)
: d_buffer(blobBuffer)
, d_begin(reinterpret_cast<const bsl::uint8_t*>(
      const_cast<const char*>(d_buffer->data())))
, d_current(d_begin)
, d_end(d_begin + d_buffer->size())
, d_size(static_cast<bsl::size_t>(d_buffer->size()))
{
}

NTSCFG_INLINE
PacketDecoder::~PacketDecoder()
{
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeInt8(bsl::int8_t* result)
{
    if (d_size < sizeof(bsl::int8_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    *result = static_cast<bsl::int8_t>(*d_current);

    d_current += sizeof(bsl::int8_t);
    d_size    -= sizeof(bsl::int8_t);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeUint8(bsl::uint8_t* result)
{
    if (d_size < sizeof(bsl::uint8_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    *result = static_cast<bsl::uint8_t>(*d_current);

    d_current += sizeof(bsl::uint8_t);
    d_size    -= sizeof(bsl::uint8_t);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeInt16(bsl::int16_t* result)
{
    BSLMF_ASSERT(sizeof(bsl::int16_t) == sizeof(bdlb::BigEndianInt16));

    if (d_size < sizeof(bsl::int16_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bdlb::BigEndianInt16 bigEndianResult;
    bsl::memcpy(reinterpret_cast<void*>(&bigEndianResult),
                d_current,
                sizeof(bigEndianResult));

    *result = static_cast<bsl::int16_t>(bigEndianResult);

    d_current += sizeof(bigEndianResult);
    d_size    -= sizeof(bigEndianResult);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeInt16(bdlb::BigEndianInt16* result)
{
    BSLMF_ASSERT(sizeof(bsl::int16_t) == sizeof(bdlb::BigEndianInt16));

    if (d_size < sizeof(bsl::int16_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::memcpy(reinterpret_cast<void*>(result), d_current, sizeof(*result));

    d_current += sizeof(*result);
    d_size    -= sizeof(*result);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeUint16(bsl::uint16_t* result)
{
    BSLMF_ASSERT(sizeof(bsl::uint16_t) == sizeof(bdlb::BigEndianUint16));

    if (d_size < sizeof(bsl::uint16_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bdlb::BigEndianUint16 bigEndianResult;
    bsl::memcpy(reinterpret_cast<void*>(&bigEndianResult),
                d_current,
                sizeof(bigEndianResult));

    *result = static_cast<bsl::uint16_t>(bigEndianResult);

    d_current += sizeof(bigEndianResult);
    d_size    -= sizeof(bigEndianResult);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeUint16(bdlb::BigEndianUint16* result)
{
    BSLMF_ASSERT(sizeof(bsl::uint16_t) == sizeof(bdlb::BigEndianUint16));

    if (d_size < sizeof(bsl::uint16_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::memcpy(reinterpret_cast<void*>(result), d_current, sizeof(*result));

    d_current += sizeof(*result);
    d_size    -= sizeof(*result);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeInt32(bsl::int32_t* result)
{
    BSLMF_ASSERT(sizeof(bsl::int32_t) == sizeof(bdlb::BigEndianInt32));

    if (d_size < sizeof(bsl::int32_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bdlb::BigEndianInt32 bigEndianResult;
    bsl::memcpy(reinterpret_cast<void*>(&bigEndianResult),
                d_current,
                sizeof(bigEndianResult));

    *result = static_cast<bsl::int32_t>(bigEndianResult);

    d_current += sizeof(bigEndianResult);
    d_size    -= sizeof(bigEndianResult);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeInt32(bdlb::BigEndianInt32* result)
{
    BSLMF_ASSERT(sizeof(bsl::int32_t) == sizeof(bdlb::BigEndianInt32));

    if (d_size < sizeof(bsl::int32_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::memcpy(reinterpret_cast<void*>(result), d_current, sizeof(*result));

    d_current += sizeof(*result);
    d_size    -= sizeof(*result);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeUint32(bsl::uint32_t* result)
{
    BSLMF_ASSERT(sizeof(bsl::uint32_t) == sizeof(bdlb::BigEndianUint32));

    if (d_size < sizeof(bsl::uint32_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bdlb::BigEndianUint32 bigEndianResult;
    bsl::memcpy(reinterpret_cast<void*>(&bigEndianResult),
                d_current,
                sizeof(bigEndianResult));

    *result = static_cast<bsl::uint32_t>(bigEndianResult);

    d_current += sizeof(bigEndianResult);
    d_size    -= sizeof(bigEndianResult);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeUint32(bdlb::BigEndianUint32* result)
{
    BSLMF_ASSERT(sizeof(bsl::uint32_t) == sizeof(bdlb::BigEndianUint32));

    if (d_size < sizeof(bsl::uint32_t)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::memcpy(reinterpret_cast<void*>(result), d_current, sizeof(*result));

    d_current += sizeof(*result);
    d_size    -= sizeof(*result);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeInt64(bsls::Types::Int64* result)
{
    BSLMF_ASSERT(sizeof(bsls::Types::Int64) == sizeof(bdlb::BigEndianInt64));

    if (d_size < sizeof(bsls::Types::Int64)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bdlb::BigEndianInt64 bigEndianResult;
    bsl::memcpy(reinterpret_cast<void*>(&bigEndianResult),
                d_current,
                sizeof(bigEndianResult));

    *result = static_cast<bsls::Types::Int64>(bigEndianResult);

    d_current += sizeof(bigEndianResult);
    d_size    -= sizeof(bigEndianResult);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeInt64(bdlb::BigEndianInt64* result)
{
    BSLMF_ASSERT(sizeof(bsls::Types::Int64) == sizeof(bdlb::BigEndianInt64));

    if (d_size < sizeof(bsls::Types::Int64)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::memcpy(reinterpret_cast<void*>(result), d_current, sizeof(*result));

    d_current += sizeof(*result);
    d_size    -= sizeof(*result);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeUint64(bsls::Types::Uint64* result)
{
    BSLMF_ASSERT(sizeof(bsls::Types::Uint64) == sizeof(bdlb::BigEndianUint64));

    if (d_size < sizeof(bsls::Types::Uint64)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bdlb::BigEndianUint64 bigEndianResult;
    bsl::memcpy(reinterpret_cast<void*>(&bigEndianResult),
                d_current,
                sizeof(bigEndianResult));

    *result = static_cast<bsls::Types::Uint64>(bigEndianResult);

    d_current += sizeof(bigEndianResult);
    d_size    -= sizeof(bigEndianResult);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeUint64(bdlb::BigEndianUint64* result)
{
    BSLMF_ASSERT(sizeof(bsls::Types::Uint64) == sizeof(bdlb::BigEndianUint64));

    if (d_size < sizeof(bsls::Types::Uint64)) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::memcpy(reinterpret_cast<void*>(result), d_current, sizeof(*result));

    d_current += sizeof(*result);
    d_size    -= sizeof(*result);

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeRaw(void* destination, bsl::size_t size)
{
    if (d_size < size) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::memcpy(destination, d_current, size);

    d_current += size;
    d_size    -= size;

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::decodeRaw(bdlbb::BlobBuffer* destination,
                                     bsl::size_t        size)
{
    if (d_size < size) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    destination->reset(
        bsl::shared_ptr<char>(
            d_buffer->buffer(),
            reinterpret_cast<char*>(const_cast<bsl::uint8_t*>(d_current))),
        static_cast<int>(size));

    d_current += size;
    d_size    -= size;

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::seek(bsl::size_t position)
{
    if (d_begin + position <= d_end) {
        d_current  = d_begin + position;
        d_size  = static_cast<bsl::size_t>(d_end - d_current);
        return ntsa::Error();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::advance(bsl::size_t amount)
{
    if (d_current + amount <= d_end) {
        d_current += amount;
        d_size     = static_cast<bsl::size_t>(d_end - d_current);
        return ntsa::Error();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
ntsa::Error PacketDecoder::rewind(bsl::size_t amount)
{
    if (d_current - amount >= d_begin) {
        d_current -= amount;
        d_size     = static_cast<bsl::size_t>(d_end - d_current);
        return ntsa::Error();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
const bsl::uint8_t* PacketDecoder::next() const
{
    return d_current;
}

NTSCFG_INLINE
bsl::size_t PacketDecoder::position() const
{
    return static_cast<bsl::size_t>(d_current - d_begin);
}

NTSCFG_INLINE
bsl::size_t PacketDecoder::size() const
{
    return static_cast<bsl::size_t>(d_end - d_begin);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
