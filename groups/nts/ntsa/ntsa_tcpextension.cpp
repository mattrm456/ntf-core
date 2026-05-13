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

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a mechanism to visit each option.
class TcpExtension::Visitor
{
    const bsl::uint8_t* d_current;
    const bsl::uint8_t* d_next;
    const bsl::uint8_t* d_begin;
    const bsl::uint8_t* d_end;
    bsl::uint8_t        d_type;
    const void*         d_data;
    bsl::size_t         d_size;

  private:
    Visitor(const Visitor&) BSLS_KEYWORD_DELETED;
    Visitor& operator=(const Visitor&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new options visitor over the specified 'data' having the
    /// specified 'dataSize'.
    Visitor(const bsl::uint8_t* data, bsl::size_t dataSize);

    /// Destroy this object.
    ~Visitor();

    /// Reset the visitor to its position upon construction.
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

    /// Return true if the visitor is stopped at a valid option, and false
    /// otherwise.
    bool isValid() const;
};

TcpExtension::Visitor::Visitor(const bsl::uint8_t* data, bsl::size_t dataSize)
: d_current(data)
, d_next(data)
, d_begin(data)
, d_end(data + dataSize)
, d_type(ntsa::TcpOptionType::e_PADDING)
, d_data(0)
, d_size(0)
{
}

TcpExtension::Visitor::~Visitor()
{
}

void TcpExtension::Visitor::reset()
{
    d_current = d_begin;
    d_type    = ntsa::TcpOptionType::e_PADDING;
    d_data    = 0;
    d_size    = 0;

    decode();
}

ntsa::Error TcpExtension::Visitor::decode()
{
    d_current = d_next;

    if (d_current >= d_end) {
        d_current = d_end;
        d_next    = d_end;
        d_type    = ntsa::TcpOptionType::e_UNDEFINED;
        d_data    = 0;
        d_size    = 0;
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    d_type = d_current[0];

    if (d_type == ntsa::TcpOptionType::e_UNDEFINED ||
        d_type == ntsa::TcpOptionType::e_PADDING)
    {
        d_next = d_current + 1;
        d_data = 0;
        d_size = 0;
    }
    else if (d_current[1] < 2) {
        d_type = ntsa::TcpOptionType::e_UNDEFINED;
        d_next = d_end;
        d_data = 0;
        d_size = 0;

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

bsl::uint8_t TcpExtension::Visitor::type() const
{
    return d_type;
}

const void* TcpExtension::Visitor::data() const
{
    return d_data;
}

bsl::size_t TcpExtension::Visitor::size() const
{
    return d_size;
}

bsl::size_t TcpExtension::Visitor::position() const
{
    return static_cast<bsl::size_t>(d_current - d_begin);
}

bsl::size_t TcpExtension::Visitor::next() const
{
    return static_cast<bsl::size_t>(d_next - d_begin);
}

bool TcpExtension::Visitor::isValid() const
{
    return d_current < d_end;
}

ntsa::Error TcpExtension::add(const ntsa::TcpOption& option, bool final)
{
    ntsa::Error error;

    bsl::size_t offset = this->size();

    if (offset > k_MAX_OPTIONS_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::MutableBuffer mutableBuffer(d_options + offset,
                                      k_MAX_OPTIONS_LENGTH - offset);

    bsl::size_t size = 0;

    error = option.encode(&mutableBuffer, &size, final);
    if (error) {
        return error;
    }

    offset += size;

    return ntsa::Error();
}

ntsa::Error TcpExtension::decode(ntsa::PacketDecoder* decoder,
                                 bsl::size_t          size)
{
    ntsa::Error error;

    error = decoder->decodeRaw(this, size);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TcpExtension::encode(ntsa::PacketEncoder* encoder,
                                 bsl::size_t          size) const
{
    ntsa::Error error;

    error = encoder->encodeRaw(this, size);
    if (error) {
        return error;
    }

    return ntsa::Error();
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
    Visitor visitor(d_options, k_MAX_OPTIONS_LENGTH);
    while (visitor.isValid()) {
        visitor.decode();

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

    if (offset + size > bufferCapacity) {
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

    Visitor visitor(d_options + offset, k_MAX_OPTIONS_LENGTH);

    while (visitor.isValid()) {
        error = visitor.decode();
        if (error) {
            return false;
        }

        if (visitor.type() == type) {
            *payload = visitor.data();
            *length  = visitor.size();
            *next    = visitor.next();

            return true;
        }
    }

    *payload = 0;
    *length  = 0;
    *next    = k_MAX_OPTIONS_LENGTH;

    return false;
}

void TcpExtension::load(ntsa::TcpOptionVector* result) const
{
    ntsa::Error error;

    result->clear();

    const bsl::uint8_t* current = d_options;
    const bsl::uint8_t* end     = d_options + k_MAX_OPTIONS_LENGTH;

    while (true) {
        bsl::size_t size = 0;

        ntsa::ConstBuffer buffer(current,
                                 static_cast<bsl::size_t>(end - current));

        result->resize(result->size() + 1);
        ntsa::TcpOption& option = result->back();

        error = option.decode(buffer, &size);
        if (error) {
            break;
        }

        current += size;
    }
}

bsl::size_t TcpExtension::size() const
{
    ntsa::Error error;

    bsl::size_t result = 0;

    Visitor visitor(d_options, k_MAX_OPTIONS_LENGTH);

    while (visitor.isValid()) {
        error = visitor.decode();
        if (error) {
            break;
        }

        if (visitor.type() == ntsa::TcpOptionType::e_UNDEFINED) {
            result = visitor.position();
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
    ntsa::Error error;

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    this->print(&printer);
    printer.end();

    return stream;
}

void TcpExtension::print(bslim::Printer* printer) const
{
    ntsa::Error error;

    ntsa::TcpOptionVector tcpOptionVector;
    this->load(&tcpOptionVector);

    for (bsl::size_t i = 0; i < tcpOptionVector.size(); ++i) {
        tcpOptionVector[i].print(printer);
    }
}

}  // close package namespace
}  // close enterprise namespace
