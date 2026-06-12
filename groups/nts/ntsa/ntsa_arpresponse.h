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

#ifndef INCLUDED_NTSA_ARPRESPONSE
#define INCLUDED_NTSA_ARPRESPONSE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ethernetaddress.h>
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

/// Provide the body of an ARP type 2 response message.
///
/// @details
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class ArpResponse
{
    /// The hardware address of the device sending the ARP request.
    ntsa::EthernetAddress d_senderHardwareAddress;

    /// The protocol address of the device sending the ARP request.
    ntsa::Ipv4Address d_senderProtocolAddress;

    /// The hardware address of the device responding to the request.
    ntsa::EthernetAddress d_targetHardwareAddress;

    /// The protocol address of the device responding to the request.
    ntsa::Ipv4Address d_targetProtocolAddress;

  public:
    /// Create a new ARP redirect having a default value.
    ArpResponse();

    /// Create a new ARP redirect having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    ArpResponse(bslmf::MovableRef<ArpResponse> original) NTSCFG_NOEXCEPT;

    /// Create a new ARP redirect having the same value as the specified
    /// 'original' object.
    ArpResponse(const ArpResponse& original);

    /// Destroy this object.
    ~ArpResponse();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    ArpResponse& operator=(bslmf::MovableRef<ArpResponse> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ArpResponse& operator=(const ArpResponse& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the hardware address of the device sending the ARP request to the
    /// specified 'value'.
    void setSenderHardwareAddress(const ntsa::EthernetAddress& value);

    /// Set the protocol address of the device sending the ARP request to the
    /// specified 'value'.
    void setSenderProtocolAddress(const ntsa::Ipv4Address& value);

    /// Set the hardware address of the device responding to the request to the
    /// specified 'value'.
    void setTargetHardwareAddress(const ntsa::EthernetAddress& value);

    /// Set the protocol address of the device responding to the request to the
    /// specified 'value'.
    void setTargetProtocolAddress(const ntsa::Ipv4Address& value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder) const;

    /// Return the hardware address of the device sending the ARP request.
    const ntsa::EthernetAddress& senderHardwareAddress() const;

    /// Return the protocol address of the device sending the ARP request.
    const ntsa::Ipv4Address& senderProtocolAddress() const;

    /// Return the hardware address of the device responding to the request.
    const ntsa::EthernetAddress& targetHardwareAddress() const;

    /// Return the protocol address of the device responding to the request.
    const ntsa::Ipv4Address& targetProtocolAddress() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const ArpResponse& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const ArpResponse& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(ArpResponse);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ArpResponse);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ArpResponse);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::ArpResponse
bsl::ostream& operator<<(bsl::ostream& stream, const ArpResponse& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::ArpResponse
bool operator==(const ArpResponse& lhs, const ArpResponse& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::ArpResponse
bool operator!=(const ArpResponse& lhs, const ArpResponse& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::ArpResponse
bool operator<(const ArpResponse& lhs, const ArpResponse& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::ArpResponse
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ArpResponse& value);

NTSCFG_INLINE
ArpResponse::ArpResponse()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
ArpResponse::ArpResponse(bslmf::MovableRef<ArpResponse> original)
    NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(original)),
        sizeof *this);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
ArpResponse::ArpResponse(const ArpResponse& original)
{
    NTSCFG_MEMORY_COPY(this, &original, sizeof *this);
}

NTSCFG_INLINE
ArpResponse::~ArpResponse()
{
}

NTSCFG_INLINE
ArpResponse& ArpResponse::operator=(bslmf::MovableRef<ArpResponse> other)
    NTSCFG_NOEXCEPT
{
    NTSCFG_MEMORY_COPY(
        this,
        BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(other)),
        sizeof *this);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
ArpResponse& ArpResponse::operator=(const ArpResponse& other)
{
    NTSCFG_MEMORY_COPY(this, &other, sizeof *this);

    return *this;
}

NTSCFG_INLINE
void ArpResponse::reset()
{
    NTSCFG_MEMORY_ZERO(this, sizeof *this);
}

NTSCFG_INLINE
void ArpResponse::setSenderHardwareAddress(const ntsa::EthernetAddress& value)
{
    d_senderHardwareAddress = value;
}

NTSCFG_INLINE
void ArpResponse::setSenderProtocolAddress(const ntsa::Ipv4Address& value)
{
    d_senderProtocolAddress = value;
}

NTSCFG_INLINE
void ArpResponse::setTargetHardwareAddress(const ntsa::EthernetAddress& value)
{
    d_targetHardwareAddress = value;
}

NTSCFG_INLINE
void ArpResponse::setTargetProtocolAddress(const ntsa::Ipv4Address& value)
{
    d_targetProtocolAddress = value;
}

NTSCFG_INLINE
const ntsa::EthernetAddress& ArpResponse::senderHardwareAddress() const
{
    return d_senderHardwareAddress;
}

NTSCFG_INLINE
const ntsa::Ipv4Address& ArpResponse::senderProtocolAddress() const
{
    return d_senderProtocolAddress;
}

NTSCFG_INLINE
const ntsa::EthernetAddress& ArpResponse::targetHardwareAddress() const
{
    return d_targetHardwareAddress;
}

NTSCFG_INLINE
const ntsa::Ipv4Address& ArpResponse::targetProtocolAddress() const
{
    return d_targetProtocolAddress;
}

NTSCFG_INLINE
bool ArpResponse::equals(const ArpResponse& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) == 0;
}

NTSCFG_INLINE
bool ArpResponse::less(const ArpResponse& other) const
{
    return NTSCFG_MEMORY_COMPARE(this, &other, sizeof *this) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void ArpResponse::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const void*>(this), sizeof *this);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ArpResponse& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const ArpResponse& lhs, const ArpResponse& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const ArpResponse& lhs, const ArpResponse& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const ArpResponse& lhs, const ArpResponse& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&   algorithm,
                              const ArpResponse& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
