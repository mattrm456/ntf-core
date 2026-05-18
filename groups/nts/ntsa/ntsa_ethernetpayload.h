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

#ifndef INCLUDED_NTSA_ETHERNETPAYLOAD
#define INCLUDED_NTSA_ETHERNETPAYLOAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv4packet.h>
#include <ntsa_ipv6packet.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>

#include <bdlbb_blob.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Ethernet payload.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class EthernetPayload
{
    /// Enumerates the representations of the payload.
    enum Type {
        /// The payload type is not defined.
        e_UNDEFINED = 0,

        /// The payload type is represented as a raw sequence of bytes.
        e_RAW = 1,

        /// The payload type is an IPv4 packet.
        e_IPV4 = 2,

        /// The payload type is an IPv6 packet.
        e_IPV6 = 3
    };

    Type d_type;

    union {
        bsls::ObjectBuffer<bdlbb::BlobBuffer> d_raw;
        bsls::ObjectBuffer<ntsa::Ipv4Packet>  d_ipv4;
        bsls::ObjectBuffer<ntsa::Ipv6Packet>  d_ipv6;
    };

    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new Ethernet payload having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EthernetPayload(bslma::Allocator* basicAllocator = 0);

    /// Create a new Ethernet payload having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    EthernetPayload(bslmf::MovableRef<EthernetPayload> original)
        NTSCFG_NOEXCEPT;

    /// Create a new Ethernet payload having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    EthernetPayload(const EthernetPayload& original,
                    bslma::Allocator*      basicAllocator = 0);

    /// Destroy this object.
    ~EthernetPayload();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    EthernetPayload& operator=(bslmf::MovableRef<EthernetPayload> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EthernetPayload& operator=(const EthernetPayload& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Make the representation of the payload a raw sequence of bytes. Return
    /// a reference to the modifable representation.
    bdlbb::BlobBuffer& makeRaw();

    /// Make the representation of the payload a raw sequence of bytes having
    /// the specified 'value'. Return a reference to the modifable
    /// representation.
    bdlbb::BlobBuffer& makeRaw(const bdlbb::BlobBuffer& value);

    /// Make the representation of the payload a raw sequence of bytes having
    /// the specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    bdlbb::BlobBuffer& makeRaw(bslmf::MovableRef<bdlbb::BlobBuffer> value)
        NTSCFG_NOEXCEPT;

    /// Make the representation of the payload an IPv4 packet. Return a
    /// reference to the modifable representation.
    ntsa::Ipv4Packet& makeIpv4();

    /// Make the representation of the payload an IPv4 packet having the
    /// specified 'value'. Return a reference to the modifable representation.
    ntsa::Ipv4Packet& makeIpv4(const ntsa::Ipv4Packet& value);

    /// Make the representation of the payload an IPv4 packet having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    ntsa::Ipv4Packet& makeIpv4(bslmf::MovableRef<ntsa::Ipv4Packet> value)
        NTSCFG_NOEXCEPT;

    /// Make the representation of the payload an IPv6 packet. Return a
    /// reference to the modifable representation.
    ntsa::Ipv6Packet& makeIpv6();

    /// Make the representation of the payload an IPv6 packet having the
    /// specified 'value'. Return a reference to the modifable representation.
    ntsa::Ipv6Packet& makeIpv6(const ntsa::Ipv6Packet& value);

    /// Make the representation of the payload an IPv6 packet having the
    /// specified 'value'. Assign an unspecified but valid value to the
    /// 'value'. Return a reference to the modifable representation.
    ntsa::Ipv6Packet& makeIpv6(bslmf::MovableRef<ntsa::Ipv6Packet> value)
        NTSCFG_NOEXCEPT;

    /// Return a reference to the modifiable raw sequence of bytes. The
    /// behavior is undefined unless 'isRaw()' is true.
    bdlbb::BlobBuffer& raw();

    /// Return a reference to the modifiable IPv4 packet. The behavior is
    /// undefined unless 'isIpv4()' is true.
    ntsa::Ipv4Packet& ipv4();

    /// Return a reference to the modifiable IPv6 packet. The behavior is
    /// undefined unless 'isIpv6()' is true.
    ntsa::Ipv6Packet& ipv6();

    /// Return a reference to the non-modifiable raw sequence of bytes. The
    /// behavior is undefined unless 'isRaw()' is true.
    const bdlbb::BlobBuffer& raw() const;

    /// Return a reference to the non-modifiable IPv4 packet. The behavior is
    /// undefined unless 'isTcp()' is true.
    const ntsa::Ipv4Packet& ipv4() const;

    /// Return a reference to the non-modifiable IPv6 packet. The behavior is
    /// undefined unless 'isUdp()' is true.
    const ntsa::Ipv6Packet& ipv6() const;

    /// Return the allocator.
    bslma::Allocator* allocator() const;

    /// Return true if the representation is not defined, otherwise return
    /// false.
    bool isUndefined() const;

    /// Return true if the representation is a raw sequence of bytes, otherwise
    /// return false.
    bool isRaw() const;

    /// Return true if the representation is an IPv4 packet, otherwise return
    /// false.
    bool isIpv4() const;

    /// Return true if the representation is an IPv6 packet, otherwise return
    /// false.
    bool isIpv6() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EthernetPayload& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(EthernetPayload);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::EthernetPayload
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetPayload& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::EthernetPayload
bool operator==(const EthernetPayload& lhs, const EthernetPayload& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::EthernetPayload
bool operator!=(const EthernetPayload& lhs, const EthernetPayload& rhs);

NTSCFG_INLINE
EthernetPayload::EthernetPayload(bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
bdlbb::BlobBuffer& EthernetPayload::raw()
{
    BSLS_ASSERT(isRaw());
    return d_raw.object();
}

NTSCFG_INLINE
ntsa::Ipv4Packet& EthernetPayload::ipv4()
{
    BSLS_ASSERT(isIpv4());
    return d_ipv4.object();
}

NTSCFG_INLINE
ntsa::Ipv6Packet& EthernetPayload::ipv6()
{
    BSLS_ASSERT(isIpv6());
    return d_ipv6.object();
}

NTSCFG_INLINE
const bdlbb::BlobBuffer& EthernetPayload::raw() const
{
    BSLS_ASSERT(isRaw());
    return d_raw.object();
}

NTSCFG_INLINE
const ntsa::Ipv4Packet& EthernetPayload::ipv4() const
{
    BSLS_ASSERT(isIpv6());
    return d_ipv4.object();
}

NTSCFG_INLINE
const ntsa::Ipv6Packet& EthernetPayload::ipv6() const
{
    BSLS_ASSERT(isIpv6());
    return d_ipv6.object();
}

NTSCFG_INLINE
bslma::Allocator* EthernetPayload::allocator() const
{
    return d_allocator_p;
}

NTSCFG_INLINE
bool EthernetPayload::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

NTSCFG_INLINE
bool EthernetPayload::isRaw() const
{
    return d_type == e_RAW;
}

NTSCFG_INLINE
bool EthernetPayload::isIpv4() const
{
    return d_type == e_IPV4;
}

NTSCFG_INLINE
bool EthernetPayload::isIpv6() const
{
    return d_type == e_IPV6;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
