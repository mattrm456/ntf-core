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

#ifndef INCLUDED_NTSA_IPV4ROUTESEQUENCE
#define INCLUDED_NTSA_IPV4ROUTESEQUENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Protocol version 4 (IPv4) route record and filter
/// option structure.
///
/// @details
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class Ipv4RouteSequence
{
    /// The index of the next entry in the sequence.
    bsl::size_t d_index;

    /// The entries in the sequence.
    bsl::vector<ntsa::Ipv4Address> d_vector;

  public:
    /// Create a new IPv4 route sequence having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit Ipv4RouteSequence(bslma::Allocator* basicAllocator = 0);

    /// Create a new IPv4 route sequence having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    Ipv4RouteSequence(bslmf::MovableRef<Ipv4RouteSequence> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv4 route sequence having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Ipv4RouteSequence(const Ipv4RouteSequence& original,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Ipv4RouteSequence();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Ipv4RouteSequence& operator=(bslmf::MovableRef<Ipv4RouteSequence> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv4RouteSequence& operator=(const Ipv4RouteSequence& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the index of the next entry.
    void setIndex(bsl::size_t value);

    /// Set the maximum number of entries.
    void setCount(bsl::size_t value);

    /// Return a reference to the modifiable entry at the specified 'index'.
    /// The behavior is undefined unless 'index' is less than 'count()'.
    ntsa::Ipv4Address& entry(std::size_t index);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the index of the next entry.
    bsl::size_t index() const;

    /// Return the maximum number of entries.
    bsl::size_t count() const;

    /// Return a reference to the non-modifiable entry at the specified
    /// 'index'. The behavior is undefined unless 'index' is less than
    /// 'count()'.
    const ntsa::Ipv4Address& entry(std::size_t index) const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Ipv4RouteSequence& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Ipv4RouteSequence& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Ipv4RouteSequence);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::Ipv4RouteSequence
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4RouteSequence& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4RouteSequence
bool operator==(const Ipv4RouteSequence& lhs, const Ipv4RouteSequence& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4RouteSequence
bool operator!=(const Ipv4RouteSequence& lhs, const Ipv4RouteSequence& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv4RouteSequence
bool operator<(const Ipv4RouteSequence& lhs, const Ipv4RouteSequence& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv4RouteSequence
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4RouteSequence& value);

NTSCFG_INLINE
Ipv4RouteSequence::Ipv4RouteSequence(bslma::Allocator* basicAllocator)
: d_index(0)
, d_vector(basicAllocator)
{
}

NTSCFG_INLINE
Ipv4RouteSequence::Ipv4RouteSequence(bslmf::MovableRef<Ipv4RouteSequence> original) NTSCFG_NOEXCEPT
: d_index(NTSCFG_MOVE_FROM(original, d_index))
, d_vector(NTSCFG_MOVE_FROM(original, d_vector))
{
}

NTSCFG_INLINE
Ipv4RouteSequence::Ipv4RouteSequence(const Ipv4RouteSequence& original,
                       bslma::Allocator* basicAllocator)
: d_index(original.d_index)
, d_vector(original.d_vector, basicAllocator)
{
}

NTSCFG_INLINE
Ipv4RouteSequence::~Ipv4RouteSequence()
{
}

NTSCFG_INLINE
Ipv4RouteSequence& Ipv4RouteSequence::operator=(bslmf::MovableRef<Ipv4RouteSequence> other)
    NTSCFG_NOEXCEPT
{
    d_index   = NTSCFG_MOVE_FROM(other, d_index);
    d_vector = NTSCFG_MOVE_FROM(other, d_vector);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Ipv4RouteSequence& Ipv4RouteSequence::operator=(const Ipv4RouteSequence& other)
{
    d_index   = other.d_index;
    d_vector = other.d_vector;

    return *this;
}

NTSCFG_INLINE
void Ipv4RouteSequence::reset()
{
    d_index = 0;
    d_vector.clear();
}


NTSCFG_INLINE
void Ipv4RouteSequence::setIndex(bsl::size_t value)
{
    d_index = value;
}

NTSCFG_INLINE
void Ipv4RouteSequence::setCount(bsl::size_t value)
{
    d_vector.resize(value);
}

NTSCFG_INLINE
ntsa::Ipv4Address& Ipv4RouteSequence::entry(std::size_t index)
{
    return d_vector[index];
}

NTSCFG_INLINE
bsl::size_t Ipv4RouteSequence::index() const
{
    return d_index;
}

NTSCFG_INLINE
bsl::size_t Ipv4RouteSequence::count() const
{
    return d_vector.size();
}

NTSCFG_INLINE
const ntsa::Ipv4Address& Ipv4RouteSequence::entry(std::size_t index) const
{
    return d_vector[index];
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Ipv4RouteSequence::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_index);
    hashAppend(algorithm, d_vector);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4RouteSequence& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Ipv4RouteSequence& lhs, const Ipv4RouteSequence& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Ipv4RouteSequence& lhs, const Ipv4RouteSequence& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Ipv4RouteSequence& lhs, const Ipv4RouteSequence& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&   algorithm,
                              const Ipv4RouteSequence& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
