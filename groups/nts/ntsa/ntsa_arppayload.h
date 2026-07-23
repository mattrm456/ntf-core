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

#ifndef INCLUDED_NTSA_ARPPAYLOAD
#define INCLUDED_NTSA_ARPPAYLOAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_arprequest.h>
#include <ntsa_arpresponse.h>
#include <ntsa_arptype.h>
#include <ntsa_error.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a discriminated union of ARP message payloads.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of ARP
/// message-body structures. The active representation is selected by calling
/// the corresponding 'make*' method and is identified by the
/// 'ntsa::ArpType::Value' discriminator returned by 'type()'.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b request:
/// The body of an ARP type 1 request for an address resolution. Active when
/// 'type()' is 'ntsa::ArpType::e_REQUEST'.
///
/// @li @b response:
/// The body of an ARP type 2 response to a request for an address resolution.
/// Active when 'type()' is 'ntsa::ArpType::e_RESPONSE'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class ArpPayload
{
    union {
        bsls::ObjectBuffer<ntsa::ArpRequest>  d_request;
        bsls::ObjectBuffer<ntsa::ArpResponse> d_response;
    };

    ntsa::ArpType::Value d_type;
    bslma::Allocator*    d_allocator_p;

  public:
    /// Create a new ARP payload having an undefined type. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ArpPayload(bslma::Allocator* basicAllocator = 0);

    /// Create a new ARP payload having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' object.
    ArpPayload(bslmf::MovableRef<ArpPayload> original) NTSCFG_NOEXCEPT;

    /// Create a new ARP payload having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ArpPayload(const ArpPayload& original,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ArpPayload();

    /// Assign the value of the specified 'other' object to this object.
    /// Assign an unspecified but valid value to the 'other' object. Return a
    /// reference to this modifiable object.
    ArpPayload& operator=(bslmf::MovableRef<ArpPayload> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ArpPayload& operator=(const ArpPayload& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Select the "request" representation. Return a reference to the
    /// modifiable representation.
    ntsa::ArpRequest& makeRequest();

    /// Select the "request" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::ArpRequest& makeRequest(const ntsa::ArpRequest& value);

    /// Select the "response" representation. Return a reference to the
    /// modifiable representation.
    ntsa::ArpResponse& makeResponse();

    /// Select the "response" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::ArpResponse& makeResponse(const ntsa::ArpResponse& value);

    /// Return a reference to the modifiable "request" representation. The
    /// behavior is undefined unless 'isRequest()' is true.
    ntsa::ArpRequest& request();

    /// Return a reference to the modifiable "response" representation. The
    /// behavior is undefined unless 'isResponse()' is true.
    ntsa::ArpResponse& response();

    /// Return a reference to the non-modifiable "request" representation. The
    /// behavior is undefined unless 'isRequest()' is true.
    const ntsa::ArpRequest& request() const;

    /// Return a reference to the non-modifiable "response" representation. The
    /// behavior is undefined unless 'isResponse()' is true.
    const ntsa::ArpResponse& response() const;

    /// Return the type of the active representation.
    ntsa::ArpType::Value type() const;

    /// Return the name of the active representation.
    const char* name() const;

    /// Return true if the payload representation is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the "request" representation is currently selected,
    /// otherwise return false.
    bool isRequest() const;

    /// Return true if the "response" representation is currently selected,
    /// otherwise return false.
    bool isResponse() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const ArpPayload& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const ArpPayload& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Print this object using the specified 'printer'.
    void print(bslim::Printer* printer) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(ArpPayload);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::ArpPayload
bsl::ostream& operator<<(bsl::ostream& stream, const ArpPayload& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::ArpPayload
bool operator==(const ArpPayload& lhs, const ArpPayload& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::ArpPayload
bool operator!=(const ArpPayload& lhs, const ArpPayload& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::ArpPayload
bool operator<(const ArpPayload& lhs, const ArpPayload& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::ArpPayload
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ArpPayload& value);

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void ArpPayload::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, static_cast<int>(d_type));

    switch (d_type) {
    case ntsa::ArpType::e_REQUEST:
        d_request.object().hash(algorithm);
        break;
    case ntsa::ArpType::e_RESPONSE:
        d_response.object().hash(algorithm);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::ArpType::e_UNDEFINED);
    }
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&   algorithm,
                              const ArpPayload& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
