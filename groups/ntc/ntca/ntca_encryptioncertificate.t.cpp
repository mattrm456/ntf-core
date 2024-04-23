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

#include <ntca_encryptioncertificate.h>

#include <ntccfg_test.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

namespace test {

// clang-format off
enum {
    k_USER_CERTIFICATE_ASN1_SIZE = 614
};

const unsigned char k_USER_CERTIFICATE_ASN1[614] = {
    0x30, 0x82, 0x02, 0x62, 0x30, 0x82, 0x02, 0x08, 0xA0, 0x03,
    0x02, 0x01, 0x02, 0x02, 0x01, 0x02, 0x30, 0x0A, 0x06, 0x08,
    0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x48,
    0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C,
    0x07, 0x54, 0x45, 0x53, 0x54, 0x2E, 0x43, 0x41, 0x31, 0x15,
    0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42,
    0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x20, 0x4C,
    0x50, 0x31, 0x1D, 0x30, 0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B,
    0x0C, 0x14, 0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65, 0x77, 0x61,
    0x72, 0x65, 0x20, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F,
    0x72, 0x74, 0x30, 0x1E, 0x17, 0x0D, 0x32, 0x34, 0x30, 0x34,
    0x32, 0x33, 0x31, 0x34, 0x33, 0x34, 0x32, 0x31, 0x5A, 0x17,
    0x0D, 0x32, 0x35, 0x30, 0x34, 0x32, 0x33, 0x31, 0x34, 0x33,
    0x34, 0x32, 0x31, 0x5A, 0x30, 0x7C, 0x31, 0x12, 0x30, 0x10,
    0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x09, 0x54, 0x45, 0x53,
    0x54, 0x2E, 0x55, 0x53, 0x45, 0x52, 0x31, 0x15, 0x30, 0x13,
    0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42, 0x6C, 0x6F,
    0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x20, 0x4C, 0x50, 0x31,
    0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x0B,
    0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65, 0x65, 0x72, 0x69, 0x6E,
    0x67, 0x31, 0x1A, 0x30, 0x18, 0x06, 0x03, 0x55, 0x04, 0x0B,
    0x0C, 0x11, 0x50, 0x6C, 0x61, 0x74, 0x66, 0x6F, 0x72, 0x6D,
    0x20, 0x53, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x73, 0x31,
    0x1D, 0x30, 0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x14,
    0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65, 0x77, 0x61, 0x72, 0x65,
    0x20, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F, 0x72, 0x74,
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE,
    0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
    0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xDF, 0x0D, 0x91,
    0xDD, 0x48, 0x1B, 0xF5, 0xE0, 0x5E, 0x93, 0x1C, 0x54, 0x20,
    0x9E, 0xAD, 0xB6, 0xA2, 0x86, 0x10, 0x3F, 0x8F, 0x29, 0x9A,
    0xB0, 0x05, 0xF9, 0x84, 0x34, 0x34, 0x7F, 0x08, 0x30, 0xCB,
    0x65, 0x62, 0x84, 0x17, 0x87, 0x72, 0x59, 0x99, 0xF0, 0x76,
    0x82, 0xD2, 0x4B, 0x56, 0xBF, 0x7E, 0x9A, 0x2D, 0xF6, 0x89,
    0xE0, 0x06, 0x05, 0x45, 0xCF, 0xD7, 0x69, 0xFF, 0x1B, 0x84,
    0x6F, 0xA3, 0x81, 0xAE, 0x30, 0x81, 0xAB, 0x30, 0x09, 0x06,
    0x03, 0x55, 0x1D, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 0x1D,
    0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0xB0,
    0xF6, 0xEA, 0x19, 0x1E, 0xFE, 0xA6, 0xC2, 0x36, 0xF5, 0x8C,
    0x9C, 0x38, 0xD2, 0xD9, 0x04, 0x62, 0x55, 0x02, 0x6F, 0x30,
    0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18, 0x30, 0x16,
    0x80, 0x14, 0x2C, 0x27, 0xDB, 0x5A, 0x05, 0xC8, 0xA1, 0x49,
    0xFB, 0xC8, 0x1C, 0xFB, 0x40, 0x68, 0xC7, 0xA2, 0xBA, 0x42,
    0xD8, 0xDA, 0x30, 0x1C, 0x06, 0x03, 0x55, 0x1D, 0x11, 0x04,
    0x15, 0x30, 0x13, 0x82, 0x11, 0x6E, 0x74, 0x66, 0x2E, 0x62,
    0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x2E, 0x63,
    0x6F, 0x6D, 0x30, 0x1E, 0x06, 0x03, 0x55, 0x1D, 0x11, 0x04,
    0x17, 0x30, 0x15, 0x82, 0x13, 0x2A, 0x2E, 0x64, 0x65, 0x76,
    0x2E, 0x62, 0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67,
    0x2E, 0x63, 0x6F, 0x6D, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x1D,
    0x11, 0x04, 0x08, 0x30, 0x06, 0x87, 0x04, 0x0A, 0x22, 0x06,
    0x17, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x1D, 0x11, 0x04, 0x08,
    0x30, 0x06, 0x87, 0x04, 0x0A, 0x22, 0x07, 0x57, 0x30, 0x0A,
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02,
    0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x20, 0x37, 0x2E, 0xA7,
    0x2E, 0xC0, 0x63, 0x26, 0x60, 0x46, 0xB7, 0x37, 0xD9, 0x19,
    0xD6, 0x60, 0x89, 0x19, 0xBA, 0x43, 0xB8, 0x61, 0x90, 0x77,
    0x5B, 0x7F, 0x6D, 0xB5, 0xD8, 0xAE, 0x7B, 0x93, 0xC2, 0x02,
    0x21, 0x00, 0xD5, 0xB2, 0x8C, 0x90, 0xA1, 0x68, 0x5D, 0xA8,
    0xB5, 0xAF, 0xA2, 0xAF, 0x30, 0x08, 0x11, 0xC4, 0xCC, 0x73,
    0x9D, 0x60, 0xB2, 0xEF, 0x9D, 0xC5, 0xA9, 0x63, 0x71, 0xB3,
    0xF4, 0x5A, 0x89, 0xF4
};

class Indentation 
{
public:
    bsl::size_t d_numSpaces;

    explicit Indentation(bsl::size_t numSpaces) : d_numSpaces(numSpaces) {}

    friend bsl::ostream& operator<<(bsl::ostream&      stream, 
                                    const Indentation& object)
    {
        for (bsl::size_t i = 0; i < object.d_numSpaces; ++i) {
            stream << "    ";
        }

        return stream;
    }
};

class TestUtil 
{
public:
    static void decodeCertificate(bsl::streambuf*   buffer, 
                                  bslma::Allocator* allocator);

    static void decodeConstructed(ntsa::AbstractSyntaxDecoder* decoder, 
                                  bslma::Allocator*            allocator);

    static void decodeSequence(ntsa::AbstractSyntaxDecoder* decoder, 
                               bslma::Allocator*            allocator);

    static void decodePrimitive(ntsa::AbstractSyntaxDecoder* decoder, 
                                bslma::Allocator*            allocator);
};

void TestUtil::decodeCertificate(bsl::streambuf*   buffer, 
                                 bslma::Allocator* allocator)
{
    ntsa::Error error;

    ntsa::AbstractSyntaxDecoder decoder(buffer, allocator);

    error = decoder.decodeContext();
    NTCCFG_TEST_OK(error);

    NTCCFG_TEST_LOG_DEBUG << test::Indentation(decoder.depth())
                          << "Context = " 
                          << decoder.current()
                          << NTCCFG_TEST_LOG_END;

    NTCCFG_TEST_EQ(decoder.current().tagClass(), 
                   ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

    NTCCFG_TEST_EQ(decoder.current().tagType(), 
                   ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

    NTCCFG_TEST_EQ(decoder.current().tagNumber(), 
                   ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);

    TestUtil::decodeConstructed(&decoder, allocator);

    error = decoder.decodeContextComplete();
    NTCCFG_TEST_OK(error);
}

void TestUtil::decodeConstructed(ntsa::AbstractSyntaxDecoder* decoder, 
                                 bslma::Allocator*            allocator)
{
    ntsa::Error error;

    NTCCFG_TEST_EQ(decoder->current().tagType(), 
                   ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

    if (decoder->current().tagClass() ==
        ntsa::AbstractSyntaxTagClass::e_UNIVERSAL &&
        decoder->current().tagNumber() == 
        ntsa::AbstractSyntaxTagNumber::e_SEQUENCE) 
    {
        test::TestUtil::decodeSequence(decoder, allocator);
    }
    else {
        error = decoder->skip();
        NTCCFG_TEST_OK(error);

        // MRM
        // NTCCFG_TEST_EQ(decoder->current().tagNumber(), 
        //                ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);
    }
}


void TestUtil::decodeSequence(ntsa::AbstractSyntaxDecoder* decoder, 
                              bslma::Allocator*            allocator)
{
    ntsa::Error error;

    NTCCFG_TEST_EQ(decoder->current().tagClass(), 
                   ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

    NTCCFG_TEST_EQ(decoder->current().tagType(), 
                   ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

    NTCCFG_TEST_EQ(decoder->current().tagNumber(), 
                   ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);

    const bsl::uint64_t endPosition = 
                        decoder->current().contentPosition() + 
                        decoder->current().contentLength().value();

    while (true) {
        const bsl::uint64_t currentPosition = decoder->position();

        if (currentPosition >= endPosition) {
            break;
        }

        error = decoder->decodeContext();
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_DEBUG << test::Indentation(decoder->depth()) 
                              << "Context = " 
                              << decoder->current()
                              << NTCCFG_TEST_LOG_END;

        if (decoder->current().tagType() == 
            ntsa::AbstractSyntaxTagType::e_CONSTRUCTED) 
        {
            test::TestUtil::decodeConstructed(decoder, allocator);
        }
        else {
            test::TestUtil::decodePrimitive(decoder, allocator);
        }

        error = decoder->decodeContextComplete();
        NTCCFG_TEST_OK(error);

    }
}

void TestUtil::decodePrimitive(ntsa::AbstractSyntaxDecoder* decoder, 
                              bslma::Allocator*            allocator)
{
    ntsa::Error error;

    if (decoder->current().tagClass() ==
        ntsa::AbstractSyntaxTagClass::e_UNIVERSAL &&
        decoder->current().tagType() ==
        ntsa::AbstractSyntaxTagType::e_PRIMITIVE)
    {

    }

    decoder->skip();

    // TODO
}

// clang-format on

}  // close namespace 'test'

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        NTCCFG_TEST_LOG_DEBUG
            << "Decoding: "
            << bdlb::PrintStringSingleLineHexDumper(
                   (const char*)test::k_USER_CERTIFICATE_ASN1,
                   test::k_USER_CERTIFICATE_ASN1_SIZE)
            << NTCCFG_TEST_LOG_END;

        ntca::EncryptionCertificate certificate(&ta);

        bdlsb::FixedMemInStreamBuf isb(
            reinterpret_cast<const char*>(test::k_USER_CERTIFICATE_ASN1),
            test::k_USER_CERTIFICATE_ASN1_SIZE);

        test::TestUtil::decodeCertificate(&isb, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        bdlsb::FixedMemInStreamBuf isb(
            reinterpret_cast<const char*>(test::k_USER_CERTIFICATE_ASN1),
            test::k_USER_CERTIFICATE_ASN1_SIZE);

        ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

        ntca::EncryptionCertificate certificate(&ta);
        error = certificate.decode(&decoder);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_DEBUG << "Certificate = " << certificate 
                              << NTCCFG_TEST_LOG_END;
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
}
NTCCFG_TEST_DRIVER_END;
