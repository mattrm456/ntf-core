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

#ifndef INCLUDED_NTSA_UDPEXTENSION
#define INCLUDED_NTSA_UDPEXTENSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntsa_udpheader.h>
#include <ntsa_udpoption.h>
#include <ntsa_udpoptiontype.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslim_printer.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a Transmission Control Protocol (UDP) header extension area.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class UdpExtension
{
  public:
    /// Enumerate the constants used by the implementation.
    enum Constants {
        /// The minimum length of all options, in bytes.
        k_MIN_OPTIONS_LENGTH = 0,

        /// The maximum length of all options, in bytes.
        k_MAX_OPTIONS_LENGTH = 40
    };

  private:
    /// Define a type alias for a vector of options.
    typedef bsl::vector<ntsa::UdpOption> OptionVector;

    /// The options vector.
    OptionVector d_vector;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new UDP extension area having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit UdpExtension(bslma::Allocator* basicAllocator = 0);

    /// Create a new UDP extension area having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    UdpExtension(bslmf::MovableRef<UdpExtension> original) NTSCFG_NOEXCEPT;

    /// Create a new UDP extension area having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    UdpExtension(const UdpExtension& original,
                 bslma::Allocator*   basicAllocator = 0);

    /// Destroy this object.
    ~UdpExtension();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    UdpExtension& operator=(bslmf::MovableRef<UdpExtension> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    UdpExtension& operator=(const UdpExtension& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Add the specified 'option'.
    void add(const ntsa::UdpOption& option);

    /// Add the specified 'option'.
    void add(bslmf::MovableRef<ntsa::UdpOption> option);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder, bsl::size_t size);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Load into the specified 'result' each option.
    void load(ntsa::UdpOptionVector* result) const;

    /// Return true if no options are defined, otherwise return false.
    bool empty() const;

    /// Return the allocator.
    bslma::Allocator* allocator() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const UdpExtension& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const UdpExtension& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(UdpExtension);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::UdpExtension
bsl::ostream& operator<<(bsl::ostream& stream, const UdpExtension& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::UdpExtension
bool operator==(const UdpExtension& lhs, const UdpExtension& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::UdpExtension
bool operator!=(const UdpExtension& lhs, const UdpExtension& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::UdpExtension
bool operator<(const UdpExtension& lhs, const UdpExtension& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::UdpExtension
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UdpExtension& value);

NTSCFG_INLINE
UdpExtension::UdpExtension(bslma::Allocator* basicAllocator)
: d_vector(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
UdpExtension::UdpExtension(bslmf::MovableRef<UdpExtension> original)
    NTSCFG_NOEXCEPT : d_vector(NTSCFG_MOVE_FROM(original, d_vector)),
                      d_allocator_p(NTSCFG_MOVE_FROM(original, d_allocator_p))
{
}

NTSCFG_INLINE
UdpExtension::UdpExtension(const UdpExtension& original,
                           bslma::Allocator*   basicAllocator)
: d_vector(original.d_vector, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
UdpExtension::~UdpExtension()
{
}

NTSCFG_INLINE
UdpExtension& UdpExtension::operator=(bslmf::MovableRef<UdpExtension> other)
    NTSCFG_NOEXCEPT
{
    d_vector      = NTSCFG_MOVE_FROM(other, d_vector);
    d_allocator_p = NTSCFG_MOVE_FROM(other, d_allocator_p);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
UdpExtension& UdpExtension::operator=(const UdpExtension& other)
{
    d_vector = other.d_vector;

    return *this;
}

NTSCFG_INLINE
void UdpExtension::reset()
{
    d_vector.clear();
}

NTSCFG_INLINE
bool UdpExtension::empty() const
{
    return d_vector.empty();
}

NTSCFG_INLINE
bslma::Allocator* UdpExtension::allocator() const
{
    return d_allocator_p;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void UdpExtension::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_vector);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const UdpExtension& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const UdpExtension& lhs, const UdpExtension& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const UdpExtension& lhs, const UdpExtension& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const UdpExtension& lhs, const UdpExtension& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&     algorithm,
                              const UdpExtension& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
