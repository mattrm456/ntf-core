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

#ifndef INCLUDED_NTSA_IPV4ROUTE
#define INCLUDED_NTSA_IPV4ROUTE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_adapter.h>
#include <ntsa_error.h>
#include <ntsa_ethernetaddress.h>
#include <ntsa_ipv4address.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bitutil.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Protocol version 4 (IPv4) route entry.
///
/// @details
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class Ipv4Route
{
    /// Enumerates the constants used by this implementation.
    enum Constant {
        /// The route is up.
        k_UP = 1 << 0,

        /// The route is to a specific host.
        k_HOST = 1 << 1,

        /// The route is to a gateway to route the packet closer to the
        /// destination.
        k_GATEWAY = 1 << 2,

        /// Route is a multicast address.
        k_MULTICAST = 1 << 3
    };

    /// The target network or specific host IP address, or undefined to
    /// indicate this route is used for any traffic not matched by a more
    /// specific rule.
    bdlb::NullableValue<ntsa::Ipv4Address> d_destinationIpv4Address;

    /// The subnet mask.
    bdlb::NullableValue<ntsa::Ipv4Address> d_destinationIpv4Mask;

    /// The number of bits in the subnet mask.
    bsl::uint32_t d_destinationIpv4MaskLength;

    /// The gateway adapter name.
    bdlb::NullableValue<bsl::string> d_gatewayAdapterName;

    /// The gateway adapter index.
    bdlb::NullableValue<bsl::uint32_t> d_gatewayAdapterIndex;

    /// The gateway Ethernet address.
    bdlb::NullableValue<ntsa::EthernetAddress> d_gatewayEthernetAddress;

    /// The gateway IPv4 address.
    bdlb::NullableValue<ntsa::Ipv4Address> d_gatewayIpv4Address;

    /// The interface adapter name.
    bdlb::NullableValue<bsl::string> d_interfaceAdapterName;

    /// The interface adapter index.
    bdlb::NullableValue<bsl::uint32_t> d_interfaceAdapterIndex;

    /// The interface Ethernet address.
    bdlb::NullableValue<ntsa::EthernetAddress> d_interfaceEthernetAddress;

    /// The interface IPv4 address.
    bdlb::NullableValue<ntsa::Ipv4Address> d_interfaceIpv4Address;

    /// The time-to-live, in seconds.
    bdlb::NullableValue<bsl::uint32_t> d_timeToLive;

    /// The administrative distance (or expense of the routing protocol).
    bsl::uint64_t d_distance;

    /// The cost of the path.
    bsl::uint64_t d_cost;

    /// The flags.
    bsl::uint32_t d_flags;

  public:
    /// Create a new IPv4 route having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit Ipv4Route(bslma::Allocator* basicAllocator = 0);

    /// Create a new IPv4 route having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Ipv4Route(const Ipv4Route& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Ipv4Route();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv4Route& operator=(const Ipv4Route& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the destination IPv4 address to the specified 'value'.
    void setDestinationIpv4Address(const ntsa::Ipv4Address& value);

    /// Set the destination IPv4 sub-network mask to the specified 'value'.
    void setDestinationIpv4Mask(const ntsa::Ipv4Address& value);

    /// Set the gateway adapter name to the specified 'value'.
    void setGatewayAdapterName(const bsl::string& value);

    /// Set the gateway adapter index to the specified 'value'.
    void setGatewayAdapterIndex(bsl::uint32_t value);

    /// Set the gateway Ethernet address to the specified 'value'.
    void setGatewayEthernetAddress(const ntsa::EthernetAddress& value);

    /// Set the gateway IPv4 address to the specified 'value'.
    void setGatewayIpv4Address(const ntsa::Ipv4Address& value);

    /// Set the interface adapter name to the specified 'value'.
    void setInterfaceAdapterName(const bsl::string& value);

    /// Set the interface adapter index to the specified 'value'.
    void setInterfaceAdapterIndex(bsl::uint32_t value);

    /// Set the interface Ethernet address to the specified 'value'.
    void setInterfaceEthernetAddress(const ntsa::EthernetAddress& value);

    /// Set the interface IPv4 address to the specified 'value'.
    void setInterfaceIpv4Address(const ntsa::Ipv4Address& value);

    /// Set the time-to-live, in seconds, to the specified 'value'.
    void setTimeToLive(bsl::size_t value);

    /// Set the adminstrative distance (i.e. expense of the routing protocol)
    /// to the specified 'value'.
    void setDistance(bsl::uint64_t value);

    /// Set the cost of the path to the specified 'value'.
    void setCost(bsl::uint64_t value);

    /// Set the flags to the specified 'value'.
    void setFlags(bsl::uint32_t value);

    /// Return the destination IPv4 address.
    const bdlb::NullableValue<ntsa::Ipv4Address>& destinationIpv4Address()
        const;

    /// Return the destination IPv4 sub-network mask.
    const bdlb::NullableValue<ntsa::Ipv4Address>& destinationIpv4Mask() const;

    /// Return the number of bits in the IPv4 sub-network mask.
    bsl::uint32_t destinationIpv4MaskLength() const;

    /// Return the gateway adapter name.
    const bdlb::NullableValue<bsl::string>& gatewayAdapterName() const;

    /// Return the gateway adapter index.
    const bdlb::NullableValue<bsl::uint32_t>& gatewayAdapterIndex() const;

    /// Return the gateway Ethernet address.
    const bdlb::NullableValue<ntsa::EthernetAddress>& gatewayEthernetAddress()
        const;

    /// Return the gateway IPv4 address.
    const bdlb::NullableValue<ntsa::Ipv4Address>& gatewayIpv4Address() const;

    /// Return the interface adapter name.
    const bdlb::NullableValue<bsl::string>& interfaceAdapterName() const;

    /// Return the interface adapter index.
    const bdlb::NullableValue<bsl::uint32_t>& interfaceAdapterIndex() const;

    /// Return the interface Ethernet address.
    const bdlb::NullableValue<ntsa::EthernetAddress>& interfaceEthernetAddress()
        const;

    /// Return the interface IPv4 address.
    const bdlb::NullableValue<ntsa::Ipv4Address>& interfaceIpv4Address() const;

    /// Return the time-to-live, in seconds.
    const bdlb::NullableValue<bsl::uint32_t>& timeToLive() const;

    /// Return the adminstrative distance (i.e. expense of the routing protocol).
    bsl::uint64_t distance() const;

    /// Return the cost of the path.
    bsl::uint64_t cost() const;

    /// Return the flags.
    bsl::uint32_t flags() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Ipv4Route& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Ipv4Route& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Ipv4Route);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::Ipv4Route
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Route& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Route
bool operator==(const Ipv4Route& lhs, const Ipv4Route& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Route
bool operator!=(const Ipv4Route& lhs, const Ipv4Route& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv4Route
bool operator<(const Ipv4Route& lhs, const Ipv4Route& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv4Route
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4Route& value);

NTSCFG_INLINE
void Ipv4Route::setDestinationIpv4Address(const ntsa::Ipv4Address& value)
{
    d_destinationIpv4Address = value;
}

NTSCFG_INLINE
void Ipv4Route::setDestinationIpv4Mask(const ntsa::Ipv4Address& value)
{
    d_destinationIpv4Mask = value;
    d_destinationIpv4MaskLength = bdlb::BitUtil::numBitsSet(value.value());
}

NTSCFG_INLINE
void Ipv4Route::setGatewayAdapterName(const bsl::string& value)
{
    d_gatewayAdapterName = value;
}

NTSCFG_INLINE
void Ipv4Route::setGatewayAdapterIndex(bsl::uint32_t value)
{
    d_gatewayAdapterIndex = value;
}

NTSCFG_INLINE
void Ipv4Route::setGatewayEthernetAddress(const ntsa::EthernetAddress& value)
{
    d_gatewayEthernetAddress = value;
}

NTSCFG_INLINE
void Ipv4Route::setGatewayIpv4Address(const ntsa::Ipv4Address& value)
{
    d_gatewayIpv4Address = value;
}

NTSCFG_INLINE
void Ipv4Route::setInterfaceAdapterName(const bsl::string& value)
{
    d_interfaceAdapterName = value;
}

NTSCFG_INLINE
void Ipv4Route::setInterfaceAdapterIndex(bsl::uint32_t value)
{
    d_interfaceAdapterIndex = value;
}

NTSCFG_INLINE
void Ipv4Route::setInterfaceEthernetAddress(const ntsa::EthernetAddress& value)
{
    d_interfaceEthernetAddress = value;
}

NTSCFG_INLINE
void Ipv4Route::setInterfaceIpv4Address(const ntsa::Ipv4Address& value)
{
    d_interfaceIpv4Address = value;
}

NTSCFG_INLINE
void Ipv4Route::setTimeToLive(bsl::size_t value)
{
    d_timeToLive = value;
}

NTSCFG_INLINE
void Ipv4Route::setDistance(bsl::uint64_t value)
{
    d_distance = value;
}

NTSCFG_INLINE
void Ipv4Route::setCost(bsl::uint64_t value)
{
    d_cost = value;
}

NTSCFG_INLINE
void Ipv4Route::setFlags(bsl::uint32_t value)
{
    d_flags = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv4Address>& Ipv4Route::
    destinationIpv4Address() const
{
    return d_destinationIpv4Address;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv4Address>& Ipv4Route::destinationIpv4Mask()
    const
{
    return d_destinationIpv4Mask;
}

NTSCFG_INLINE
bsl::uint32_t Ipv4Route::destinationIpv4MaskLength() const
{
    return d_destinationIpv4MaskLength;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::string>& Ipv4Route::gatewayAdapterName() const
{
    return d_gatewayAdapterName;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::uint32_t>& Ipv4Route::gatewayAdapterIndex()
    const
{
    return d_gatewayAdapterIndex;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::EthernetAddress>& Ipv4Route::
    gatewayEthernetAddress() const
{
    return d_gatewayEthernetAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv4Address>& Ipv4Route::gatewayIpv4Address()
    const
{
    return d_gatewayIpv4Address;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::string>& Ipv4Route::interfaceAdapterName() const
{
    return d_interfaceAdapterName;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::uint32_t>& Ipv4Route::interfaceAdapterIndex()
    const
{
    return d_interfaceAdapterIndex;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::EthernetAddress>& Ipv4Route::
    interfaceEthernetAddress() const
{
    return d_interfaceEthernetAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Ipv4Address>& Ipv4Route::interfaceIpv4Address()
    const
{
    return d_interfaceIpv4Address;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::uint32_t>& Ipv4Route::timeToLive() const
{
    return d_timeToLive;
}

NTSCFG_INLINE
bsl::uint64_t Ipv4Route::distance() const
{
    return d_distance;
}

NTSCFG_INLINE
bsl::uint64_t Ipv4Route::cost() const
{
    return d_cost;
}

NTSCFG_INLINE
bsl::uint32_t Ipv4Route::flags() const
{
    return d_flags;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Ipv4Route::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_destinationIpv4Address);
    hashAppend(algorithm, d_destinationIpv4Mask);
    hashAppend(algorithm, d_gatewayAdapterName);
    hashAppend(algorithm, d_gatewayAdapterIndex);
    hashAppend(algorithm, d_gatewayEthernetAddress);
    hashAppend(algorithm, d_gatewayIpv4Address);
    hashAppend(algorithm, d_interfaceAdapterName);
    hashAppend(algorithm, d_interfaceAdapterIndex);
    hashAppend(algorithm, d_interfaceEthernetAddress);
    hashAppend(algorithm, d_interfaceIpv4Address);
    hashAppend(algorithm, d_timeToLive);
    hashAppend(algorithm, d_flags);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Route& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Ipv4Route& lhs, const Ipv4Route& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Ipv4Route& lhs, const Ipv4Route& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Ipv4Route& lhs, const Ipv4Route& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&  algorithm,
                              const Ipv4Route& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
