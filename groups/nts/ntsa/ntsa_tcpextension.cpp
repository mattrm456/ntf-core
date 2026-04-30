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

#include <ntsa_tcpextension.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcpextension_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a mechanism to decode the options.
class TcpExtension::Decoder
{
    const bsl::uint8_t* d_current;
    const bsl::uint8_t* d_next;
    const bsl::uint8_t* d_begin;
    const bsl::uint8_t* d_end;
    bsl::uint8_t        d_type;
    const void*         d_data;
    bsl::size_t         d_size;

  private:
    Decoder(const Decoder&) BSLS_KEYWORD_DELETED;
    Decoder& operator=(const Decoder&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new options decoder over the specified 'data' having the
    /// specified 'dataSize'.
    Decoder(const bsl::uint8_t* data, bsl::size_t dataSize);

    /// Destroy this object.
    ~Decoder();

    /// Reset the decoder to its position upon construction.
    void reset();

    /// Decode the next option. Return the error.
    ntsa::Error decode();

    /// Return the option type.
    bsl::uint8_t type() const;

    /// Return the option data or 0 if no option data is defined.
    const void* data() const;

    /// Return the option data size, in bytes.
    bsl::size_t size() const;

    /// Return the offset of the option.
    bsl::size_t position() const;

    /// Return the offset of the next option.
    bsl::size_t next() const;

    /// Return true if the decoder is stopped at a valid option, and false
    /// otherwise.
    bool isValid() const;
};

TcpExtension::Decoder::Decoder(const bsl::uint8_t* data, bsl::size_t dataSize)
: d_current(data)
, d_next(data)
, d_begin(data)
, d_end(data + dataSize)
, d_type(k_PADDING_TYPE)
, d_data(0)
, d_size(0)
{
}

TcpExtension::Decoder::~Decoder()
{
}

void TcpExtension::Decoder::reset()
{
    d_current = d_begin;
    d_type    = k_PADDING_TYPE;
    d_data    = 0;
    d_size    = 0;

    decode();
}

ntsa::Error TcpExtension::Decoder::decode()
{
    d_current = d_next;

    if (d_current >= d_end) {
        d_current = d_end;
        d_next    = d_end;
        d_type    = k_END_TYPE;
        d_data    = 0;
        d_size    = 0;
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    d_type = d_current[0];

    if (d_type == k_END_TYPE || d_type == k_PADDING_TYPE) {
        d_next    = d_current + 1;
        d_data    = 0;
        d_size    = 0;
    }
    else if (d_current[1] < 2) {
        d_type    = k_END_TYPE;
        d_next    = d_end;
        d_data    = 0;
        d_size    = 0;

        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else {
        d_next = d_current + d_current[1];
        d_data = d_current + 2;
        d_size = d_current[1] - 2;
    }



    // should be types 2, 4, 8, 1, 3

    return ntsa::Error();
}

bsl::uint8_t TcpExtension::Decoder::type() const
{
    return d_type;
}

const void* TcpExtension::Decoder::data() const
{
    return d_data;
}

bsl::size_t TcpExtension::Decoder::size() const
{
    return d_size;
}

bsl::size_t TcpExtension::Decoder::position() const
{
    return static_cast<bsl::size_t>(d_current - d_begin);
}

bsl::size_t TcpExtension::Decoder::next() const
{
    return static_cast<bsl::size_t>(d_next - d_begin);
}

bool TcpExtension::Decoder::isValid() const
{
    return d_current < d_end;
}

ntsa::Error TcpExtension::decode(const bdlbb::BlobBuffer& buffer,
                                 bsl::size_t              offset,
                                 bsl::size_t              size)
{
    reset();

    if (buffer.data() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const char* bufferData = buffer.data();

    if (buffer.size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t bufferSize = static_cast<bsl::size_t>(buffer.size());

    if (offset + size > bufferSize) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::memcpy(reinterpret_cast<void*>(this), bufferData + offset, size);

    // MRM
#if 0
    Decoder decoder(d_options, k_MAX_OPTIONS_LENGTH);
    while (decoder.isValid()) {
        decoder.decode();

        BSLS_LOG_INFO("Decoded option offset %d type %d length %d",
                  (int)(d_current ? (bsl::uint8_t*)d_current - d_begin : 0),
                  (int)(d_type),
                  (int)(d_size));
    }
#endif

    return ntsa::Error();
}

ntsa::Error TcpExtension::encode(bdlbb::BlobBuffer* buffer,
                                 bsl::size_t        offset) const
{
    ntsa::Error error;

    if (buffer->data() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    char* bufferData = buffer->data();

    if (buffer->size() <= 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t bufferCapacity =
        static_cast<bsl::size_t>(buffer->size());

    const bsl::size_t size = this->size();

    if (offset + size > bufferCapacity)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::memcpy(reinterpret_cast<void*>(bufferData + offset),
                reinterpret_cast<const void*>(this),
                size);

    return ntsa::Error();
}

bool TcpExtension::find(const void** payload,
                        bsl::size_t* length,
                        bsl::size_t* next,
                        bsl::uint8_t type,
                        bsl::size_t  offset) const
{
    ntsa::Error error;

    Decoder decoder(d_options + offset, k_MAX_OPTIONS_LENGTH);

    while (decoder.isValid()) {
        error = decoder.decode();
        if (error) {
            return false;
        }

        if (decoder.type() == type) {
            *payload = decoder.data();
            *length  = decoder.size();
            *next    = decoder.next();

            return true;
        }
    }

    *payload = 0;
    *length  = 0;
    *next    = k_MAX_OPTIONS_LENGTH;

    return false;
}


bsl::size_t TcpExtension::size() const
{
    ntsa::Error error;

    bsl::size_t result = 0;

    Decoder decoder(d_options, k_MAX_OPTIONS_LENGTH);

    while (decoder.isValid()) {
        error = decoder.decode();
        if (error) {
            break;
        }

        if (decoder.type() == k_END_TYPE) {
            result = decoder.position();
            break;
        }
    }

    return result;
}

bool TcpExtension::equals(const TcpExtension& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) == 0;
}

bool TcpExtension::less(const TcpExtension& other) const
{
    return bsl::memcmp(reinterpret_cast<const void*>(this),
                       reinterpret_cast<const void*>(&other),
                       sizeof *this) < 0;
}

bsl::ostream& TcpExtension::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    // printer.printAttribute("mss", this->mss());

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
