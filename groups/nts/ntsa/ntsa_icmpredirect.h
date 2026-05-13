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

#ifndef INCLUDED_NTSA_ICMPREDIRECT
#define INCLUDED_NTSA_ICMPREDIRECT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide the body of an ICMP type 5 redirect message.
///
/// @details
/// The ICMP redirect message is sent by a gateway to a source host to inform
/// it that a better route to a destination exists and that subsequent
/// datagrams should be sent directly to a different gateway, as described in
/// RFC 792.
///
/// The full redirect wire format, carried inside an IP datagram, is:
///
///```
///  0                   1                   2                   3
///  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |     Type      |     Code      |          Checksum             |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |                 Gateway Internet Address                      |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// |      Internet Header + 64 bits of Original Data Datagram      |
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///```
///
/// The type (5), code, and checksum fields are represented by
/// 'ntsa::IcmpHeader'. This class represents the fixed message-specific body
/// that immediately follows the header: the gateway internet address. The
/// variable-length original datagram data that trails the body is not
/// represented by this class.
///
/// The code field in the accompanying 'ntsa::IcmpHeader' qualifies the scope
/// of the redirect:
///
///   0 - Redirect datagrams for the network.
///   1 - Redirect datagrams for the host.
///   2 - Redirect datagrams for the type of service and network.
///   3 - Redirect datagrams for the type of service and host.
///
/// Gateway Internet Address (32 bits): The IPv4 address of the gateway to
/// which the source host should send subsequent datagrams for the destination
/// network or host named in the original datagram. The address is stored in
/// network byte order.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpRedirect
{
    /// The IPv4 address of the gateway to redirect traffic through.
    ntsa::Ipv4Address d_gatewayAddress;

  public:
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The fixed length of the IcmpRedirect body in octets.
        k_LENGTH = 4
    };

    /// Create a new ICMP redirect having a default value.
    IcmpRedirect();

    /// Create a new ICMP redirect having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    IcmpRedirect(bslmf::MovableRef<IcmpRedirect> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP redirect having the same value as the specified
    /// 'original' object.
    IcmpRedirect(const IcmpRedirect& original);

    /// Destroy this object.
    ~IcmpRedirect();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    IcmpRedirect& operator=(
        bslmf::MovableRef<IcmpRedirect> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpRedirect& operator=(const IcmpRedirect& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the gateway address to the specified 'value'.
    void setGatewayAddress(const ntsa::Ipv4Address& value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Decode the body from the specified 'buffer' starting at the specified
    /// 'offset' inside the framing packet having the specified 'packetSize'.
    /// Return the error.
    ntsa::Error decode(const bdlbb::BlobBuffer& buffer,
                       bsl::size_t              offset,
                       bsl::size_t              packetSize);

    /// Encode the body to the specified 'buffer' starting at the specified
    /// 'offset'. Return the error.
    ntsa::Error encode(bdlbb::BlobBuffer* buffer, bsl::size_t offset) const;

    /// Return the gateway address.
    const ntsa::Ipv4Address& gatewayAddress() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IcmpRedirect& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IcmpRedirect& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(IcmpRedirect);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IcmpRedirect);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IcmpRedirect);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IcmpRedirect
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpRedirect& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpRedirect
bool operator==(const IcmpRedirect& lhs, const IcmpRedirect& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpRedirect
bool operator!=(const IcmpRedirect& lhs, const IcmpRedirect& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpRedirect
bool operator<(const IcmpRedirect& lhs, const IcmpRedirect& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpRedirect
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpRedirect& value);

NTSCFG_INLINE
IcmpRedirect::IcmpRedirect()
: d_gatewayAddress()
{
    BSLMF_ASSERT(sizeof(*this) == k_LENGTH);
}

NTSCFG_INLINE
IcmpRedirect::IcmpRedirect(
    bslmf::MovableRef<IcmpRedirect> original) NTSCFG_NOEXCEPT
: d_gatewayAddress(NTSCFG_MOVE_FROM(original, d_gatewayAddress))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
IcmpRedirect::IcmpRedirect(const IcmpRedirect& original)
: d_gatewayAddress(original.d_gatewayAddress)
{
}

NTSCFG_INLINE
IcmpRedirect::~IcmpRedirect()
{
}

NTSCFG_INLINE
IcmpRedirect& IcmpRedirect::operator=(
    bslmf::MovableRef<IcmpRedirect> other) NTSCFG_NOEXCEPT
{
    d_gatewayAddress = NTSCFG_MOVE_FROM(other, d_gatewayAddress);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
IcmpRedirect& IcmpRedirect::operator=(const IcmpRedirect& other)
{
    d_gatewayAddress = other.d_gatewayAddress;

    return *this;
}

NTSCFG_INLINE
void IcmpRedirect::reset()
{
    d_gatewayAddress = ntsa::Ipv4Address();
}

NTSCFG_INLINE
void IcmpRedirect::setGatewayAddress(const ntsa::Ipv4Address& value)
{
    d_gatewayAddress = value;
}

NTSCFG_INLINE
const ntsa::Ipv4Address& IcmpRedirect::gatewayAddress() const
{
    return d_gatewayAddress;
}

NTSCFG_INLINE
bool IcmpRedirect::equals(const IcmpRedirect& other) const
{
    return d_gatewayAddress == other.d_gatewayAddress;
}

NTSCFG_INLINE
bool IcmpRedirect::less(const IcmpRedirect& other) const
{
    return d_gatewayAddress < other.d_gatewayAddress;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpRedirect::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_gatewayAddress);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpRedirect& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IcmpRedirect& lhs, const IcmpRedirect& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IcmpRedirect& lhs, const IcmpRedirect& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IcmpRedirect& lhs, const IcmpRedirect& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&     algorithm,
                              const IcmpRedirect& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
