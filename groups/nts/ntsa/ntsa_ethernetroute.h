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

#ifndef INCLUDED_NTSA_ETHERNETROUTE
#define INCLUDED_NTSA_ETHERNETROUTE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_adapter.h>
#include <ntsa_domainname.h>
#include <ntsa_error.h>
#include <ntsa_ethernetaddress.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Ethernet route entry.
///
/// @details
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class EthernetRoute
{
    /// Enumerates the constants used by this implementation.
    enum Constant {
        /// The route is permanent.
        k_PERMANENT = 1 << 0,

        /// The route is a bridge.
        k_BRIDGE = 1 << 1
    };

    /// The remote host's fully qualified domain name.
    bdlb::NullableValue<ntsa::DomainName> d_domainName;

    /// The remote host's Ethernet address.
    ntsa::EthernetAddress d_ethernetAddress;

    /// The remote host's IPv4 address.
    bdlb::NullableValue<ntsa::Ipv4Address> d_ipv4Address;

    /// The remote host's IPv6 address.
    bdlb::NullableValue<ntsa::Ipv6Address> d_ipv6Address;

    /// The flags.
    bsl::uint32_t d_flags;

  public:
    /// Create a new Ethernet route having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EthernetRoute(bslma::Allocator* basicAllocator = 0);

    /// Create a new Ethernet route having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    EthernetRoute(const EthernetRoute& original,
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~EthernetRoute();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EthernetRoute& operator=(const EthernetRoute& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the domain name to the specified 'value'.
    void setDomainName(const ntsa::DomainName& value);

    /// Set the Ethernet address to the specified 'value'.
    void setEthernetAddress(const ntsa::EthernetAddress& value);

    /// Set the IPv4 address to the specified 'value'.
    void setIpv4Address(const ntsa::Ipv4Address& value);

    /// Set the IPv6 address to the specified 'value'.
    void setIpv6Address(const ntsa::Ipv6Address& value);

    /// Set the flags to the specified 'value'.
    void setFlags(bsl::uint32_t value);

    /// Return the domain name.
    const bdlb::NullableValue<ntsa::DomainName>& domainName() const;

    /// Return the Ethernet address.
    const ntsa::EthernetAddress& ethernetAddress() const;

    /// Return the IPv4 address.
    const bdlb::NullableValue<ntsa::Ipv4Address>& ipv4Address() const;

    /// Return the IPv6 address.
    const bdlb::NullableValue<ntsa::Ipv6Address>& ipv6Address() const;

    /// Return the flags.
    bsl::uint32_t flags() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const EthernetRoute& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const EthernetRoute& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(EthernetRoute);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::EthernetRoute
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetRoute& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::EthernetRoute
bool operator==(const EthernetRoute& lhs, const EthernetRoute& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::EthernetRoute
bool operator!=(const EthernetRoute& lhs, const EthernetRoute& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::EthernetRoute
bool operator<(const EthernetRoute& lhs, const EthernetRoute& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::EthernetRoute
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EthernetRoute& value);

NTSCFG_INLINE
void EthernetRoute::setDomainName(const ntsa::DomainName& value)
{
    d_domainName = value;
}

NTSCFG_INLINE
void EthernetRoute::setEthernetAddress(const ntsa::EthernetAddress& value)
{
    d_ethernetAddress = value;
}

NTSCFG_INLINE
void EthernetRoute::setIpv4Address(const ntsa::Ipv4Address& value)
{
    d_ipv4Address = value;
}

NTSCFG_INLINE
void EthernetRoute::setIpv6Address(const ntsa::Ipv6Address& value)
{
    d_ipv6Address = value;
}

NTSCFG_INLINE
void EthernetRoute::setFlags(bsl::uint32_t value)
{
    d_flags = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::DomainName>& EthernetRoute::domainName() const
{
    return d_domainName;
}

NTSCFG_INLINE
const ntsa::EthernetAddress& EthernetRoute::ethernetAddress() const
{
    return d_ethernetAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv4Address>& EthernetRoute::ipv4Address()
    const
{
    return d_ipv4Address;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv6Address>& EthernetRoute::ipv6Address()
    const
{
    return d_ipv6Address;
}

NTSCFG_INLINE
bsl::uint32_t EthernetRoute::flags() const
{
    return d_flags;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void EthernetRoute::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_domainName);
    hashAppend(algorithm, d_ethernetAddress);
    hashAppend(algorithm, d_ipv4Address);
    hashAppend(algorithm, d_ipv6Address);
    hashAppend(algorithm, d_flags);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetRoute& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const EthernetRoute& lhs, const EthernetRoute& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const EthernetRoute& lhs, const EthernetRoute& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const EthernetRoute& lhs, const EthernetRoute& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&      algorithm,
                              const EthernetRoute& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
