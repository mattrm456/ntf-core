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

#ifndef INCLUDED_NTSA_TCPEXTENSION
#define INCLUDED_NTSA_TCPEXTENSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_tcpheader.h>
#include <ntsa_tcpoptiontype.h>
#include <ntsa_tcpoption.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslim_printer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a Transmission Control Protocol (TCP) header extension area.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class TcpExtension
{
  public:
    /// Enumerate the constants used by the implementation.
    enum Constants {
        /// The minimum length of all options, in bytes.
        k_MIN_OPTIONS_LENGTH = 0,

        /// The maximum length of all options, in bytes.
        k_MAX_OPTIONS_LENGTH = 40,

        /// The maximum segment size (MSS) option type.
        k_MSS_TYPE = 2,

        /// The maximum segment size (MSS) option size, in bytes.
        k_MSS_SIZE = 2,

        /// The window scaling option type.
        k_WINDOW_SCALING_TYPE = 3,

        /// The window scaling option size, in bytes.
        k_WINDOW_SCALING_SIZE = 1,

        /// The selective acknowledgement (SACK) permitted option type.
        k_SACK_PERMITTED_TYPE = 4,

        /// The selective acknowledgement (SACK) permitted option size.
        k_SACK_PERMITTED_SIZE = 0,

        /// The selective acknowledgement (SACK) option type.
        k_SACK_TYPE = 5,

        /// The selective acknowledgement (SACK) option size, but note that
        /// the size indicates the count of 8-byte records.
        k_SACK_SIZE = 1,

        /// The timestamp option type.
        k_TIMESTAMP_TYPE = 8,

        /// The timestamp option size.
        k_TIMESTAMP_SIZE = 8,

        /// The fast open option type.
        k_FAST_OPEN_TYPE = 34,

        /// The fast open option size.
        k_FAST_OPEN_SIZE = 16,

        /// The padding option type. This option type indicates padding between
        /// two options or between an option and the end of the options list.
        k_PADDING_TYPE = 1,

        /// The padding option size.
        k_PADDING_SIZE = 0,

        /// The end option type. This option type indicates the end of the
        /// options list.
        k_END_TYPE = 0,

        /// The end option size.
        k_END_SIZE = 0
    };

  private:
    /// The options.
    bsl::uint8_t d_options[k_MAX_OPTIONS_LENGTH];

  private:
    /// Provide a mechanism to decode the options.
    class Decoder;

  public:
    /// Create a new TCP extension area having a default value.
    TcpExtension();

    /// Create a new TCP extension area having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    TcpExtension(bslmf::MovableRef<TcpExtension> original) NTSCFG_NOEXCEPT;

    /// Create a new TCP extension area having the same value as the specified
    /// 'original' object.
    TcpExtension(const TcpExtension& original);

    /// Destroy this object.
    ~TcpExtension();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    TcpExtension& operator=(bslmf::MovableRef<TcpExtension> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    TcpExtension& operator=(const TcpExtension& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Decode the TCP extension area having the specified 'size' from the
    /// specified 'buffer' starting at the specified 'offset'. Return the
    /// error.
    ntsa::Error decode(const bdlbb::BlobBuffer& buffer,
                       bsl::size_t              offset,
                       bsl::size_t              size);

    /// Encode the TCP extension area to the specified 'buffer' starting at the
    /// specified 'offset'. Return the error.
    ntsa::Error encode(bdlbb::BlobBuffer* buffer,
                       bsl::size_t        offset) const;

    #if 0
    ntsa::Error getMaxSegmentSize(bsl::size_t* result) const;

    ntsa::Error getWindowScaling(bsl::size_t* result) const;

    ntsa::Error getSelectiveAckPermitted(bool result) const;

    ntsa::Error getSelectiveAck(bsl::vector< bsl::pair<bsl::uint32_t, bsl::uint32_t> >* result) const;

    ntsa::Error getFastOpen(bdlb::Guid* result) const;
    #endif



    /// Find the TCP option having the specified 'type' starting at the
    /// specified 'offset'. If such an option is found, load into the specified
    /// 'payload' the start of the  option's payload, load into the specified
    /// 'length' the length of the option's payload, load into the specified
    /// 'next' the offset of the next option, and return true. Otherwise,
    /// return false.
    bool find(const void** payload,
              bsl::size_t* length,
              bsl::size_t* next,
              bsl::uint8_t type,
              bsl::size_t  offset) const;

    /// Return the extension data.
    const void* data() const;

    /// Return the extension size, in bytes.
    bsl::size_t size() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const TcpExtension& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const TcpExtension& other) const;

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

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(TcpExtension);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(TcpExtension);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(TcpExtension);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::TcpExtension
bsl::ostream& operator<<(bsl::ostream& stream, const TcpExtension& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::TcpExtension
bool operator==(const TcpExtension& lhs, const TcpExtension& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::TcpExtension
bool operator!=(const TcpExtension& lhs, const TcpExtension& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::TcpExtension
bool operator<(const TcpExtension& lhs, const TcpExtension& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::TcpExtension
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TcpExtension& value);

NTSCFG_INLINE
TcpExtension::TcpExtension()
{
    BSLMF_ASSERT(sizeof(*this) == k_MAX_OPTIONS_LENGTH);

    NTSCFG_WARNING_UNUSED(d_options);

    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
}

NTSCFG_INLINE
TcpExtension::TcpExtension(bslmf::MovableRef<TcpExtension> original)
    NTSCFG_NOEXCEPT
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(BSLS_UTIL_ADDRESSOF(
                    bslmf::MovableRefUtil::access(original))),
                sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
TcpExtension::TcpExtension(const TcpExtension& original)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&original),
                sizeof *this);
}

NTSCFG_INLINE
TcpExtension::~TcpExtension()
{
}

NTSCFG_INLINE
TcpExtension& TcpExtension::operator=(bslmf::MovableRef<TcpExtension> other)
    NTSCFG_NOEXCEPT
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(
                    BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(other))),
                sizeof *this);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
TcpExtension& TcpExtension::operator=(const TcpExtension& other)
{
    bsl::memcpy(reinterpret_cast<void*>(this),
                reinterpret_cast<const void*>(&other),
                sizeof *this);

    return *this;
}

NTSCFG_INLINE
void TcpExtension::reset()
{
    bsl::memset(reinterpret_cast<void*>(this), 0, sizeof *this);
}

NTSCFG_INLINE
const void* TcpExtension::data() const
{
    return d_options;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void TcpExtension::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(this), sizeof *this);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const TcpExtension& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const TcpExtension& lhs, const TcpExtension& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const TcpExtension& lhs, const TcpExtension& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const TcpExtension& lhs, const TcpExtension& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&     algorithm,
                              const TcpExtension& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
