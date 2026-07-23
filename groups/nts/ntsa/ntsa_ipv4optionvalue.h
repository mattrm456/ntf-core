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

#ifndef INCLUDED_NTSA_IPV4OPTIONVALUE
#define INCLUDED_NTSA_IPV4OPTIONVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a IPv4 option value.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b kind:
/// The kind of option.
///
/// @li @b payload:
/// The payload of the option.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class Ipv4OptionValue
{
    /// The option kind.
    bsl::uint8_t d_kind;

    /// The option payload.
    bsl::vector<bsl::uint8_t> d_payload;

  public:
    /// Create a new IPv4 option value. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit Ipv4OptionValue(bslma::Allocator* basicAllocator = 0);

    /// Create a new IPv4 option value having the same value as the specified
    /// 'other' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Ipv4OptionValue(const Ipv4OptionValue& other,
                    bslma::Allocator*      basicAllocator = 0);

    /// Destroy this object.
    ~Ipv4OptionValue();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Ipv4OptionValue& operator=(const Ipv4OptionValue& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the kind to the specified 'value'.
    void setKind(bsl::uint8_t value);

    /// Set the payload to the specified 'value'.
    void setPayload(const bsl::vector<bsl::uint8_t>& value);

    /// Return the kind.
    bsl::uint8_t kind() const;

    /// Return the payload.
    const bsl::vector<bsl::uint8_t>& payload() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Ipv4OptionValue& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Ipv4OptionValue& other) const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Ipv4OptionValue);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4OptionValue
bool operator==(const Ipv4OptionValue& lhs, const Ipv4OptionValue& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4OptionValue
bool operator!=(const Ipv4OptionValue& lhs, const Ipv4OptionValue& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv4OptionValue
bool operator<(const Ipv4OptionValue& lhs, const Ipv4OptionValue& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv4OptionValue
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4OptionValue& value);

template <typename HASH_ALGORITHM>
void Ipv4OptionValue::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_kind);
    hashAppend(algorithm, d_payload);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&        algorithm,
                              const Ipv4OptionValue& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
