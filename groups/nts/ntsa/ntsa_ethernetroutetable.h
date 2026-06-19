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

#ifndef INCLUDED_NTSA_ETHERNETROUTETABLE
#define INCLUDED_NTSA_ETHERNETROUTETABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ethernetroute.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Ethernet route table.
///
/// @details
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class EthernetRouteTable
{
    /// Defines a type alias for a map of IPv4 addresses to Ethernet addresses.
    typedef bsl::unordered_map<ntsa::Ipv4Address, ntsa::EthernetAddress>
    EthernetAddressByIpv4Address;

    /// Defines a type alias for a map of IPv6 addresses to Ethernet addresses.
    typedef bsl::unordered_map<ntsa::Ipv6Address, ntsa::EthernetAddress>
    EthernetAddressByIpv6Address;

    /// Defines a type alias for a map of Ethernet addresses to IPv4 addresses.
    typedef bsl::unordered_map<ntsa::EthernetAddress, ntsa::Ipv4Address>
    Ipv4AddressByEthernetAddress;

    /// Defines a type alias for a map of Ethernet addresses to IPv4 addresses.
    typedef bsl::unordered_map<ntsa::EthernetAddress, ntsa::Ipv6Address>
    Ipv6AddressByEthernetAddress;

    /// The map of IPv4 addresses to Ethernet addresses.
    EthernetAddressByIpv4Address d_ethernetAddressByIpv4Address;

    /// The map of IPv6 addresses to Ethernet addresses.
    EthernetAddressByIpv6Address d_ethernetAddressByIpv6Address;

    /// The map of Ethernet addresses to IPv4 addresses.
    Ipv4AddressByEthernetAddress d_ipv4AddressByEthernetAddress;

    /// The map of Ethernet addresses to IPv4 addresses.
    Ipv6AddressByEthernetAddress d_ipv6AddressByEthernetAddress;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

  private:
    EthernetRouteTable(const EthernetRouteTable&);
    EthernetRouteTable& operator=(const EthernetRouteTable&);

  public:
    /// Create a new Ethernet route table having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EthernetRouteTable(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EthernetRouteTable();

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Add the specified 'route' to the route table.
    void add(const ntsa::EthernetRoute& route);

    /// Load into the specified 'result' the Ethernet address for the specified
    /// 'ipv4Address'. Return true if such an ethernet address exists, and
    /// false otherwise.
    bool find(ntsa::EthernetAddress*   result,
              const ntsa::Ipv4Address& ipv4Address) const;

    /// Load into the specified 'result' the Ethernet address for the specified
    /// 'ipv6Address'. Return true if such an ethernet address exists, and
    /// false otherwise.
    bool find(ntsa::EthernetAddress*   result,
              const ntsa::Ipv6Address& ipv6Address) const;

    /// Load into the specified 'result' the IPv4 address for the specified
    /// 'ethernetAddress'. Return true if such an ethernet address exists, and
    /// false otherwise.
    bool find(ntsa::Ipv4Address*           result,
              const ntsa::EthernetAddress& ethernetAddress);

    /// Load into the specified 'result' the IPv6 address for the specified
    /// 'ethernetAddress'. Return true if such an ethernet address exists, and
    /// false otherwise.
    bool find(ntsa::Ipv6Address*           result,
              const ntsa::EthernetAddress& ethernetAddress);

    /// Load into the specified 'result' each route in the table.
    void load(bsl::vector<ntsa::EthernetRoute>* result) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(EthernetRouteTable);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::EthernetRouteTable
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const EthernetRouteTable& object);

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const EthernetRouteTable& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
#endif
