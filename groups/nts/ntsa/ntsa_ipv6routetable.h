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

#ifndef INCLUDED_NTSA_IPV6ROUTETABLE
#define INCLUDED_NTSA_IPV6ROUTETABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipv6route.h>
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

/// Provide an Internet Protocol version 4 (IPv6) route table.
///
/// @details
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class Ipv6RouteTable
{
    /// Defines a type alias for a map of routes by IPv6 address.
    typedef bsl::unordered_map<ntsa::Ipv6Address,
                               bsl::shared_ptr<ntsa::Ipv6Route> >
        RouteCache;

    /// Defines a type alias for a vector of routes.
    typedef bsl::vector<bsl::shared_ptr<ntsa::Ipv6Route> > RouteVector;

    /// Predicate to sort routes by longest prefix, then by distance, then by
    /// cost.
    class Sorter;

    /// The route cache.
    RouteCache d_routeCache;

    /// The route entries.
    RouteVector d_routeVector;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

  private:
    Ipv6RouteTable(const Ipv6RouteTable&);
    Ipv6RouteTable& operator=(const Ipv6RouteTable&);

  public:
    /// Create a new IPv6 route table having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit Ipv6RouteTable(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Ipv6RouteTable();

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Add the specified 'route' to the route table.
    void add(const ntsa::Ipv6Route& route);

    /// Load into the specified 'sourceEthernetAddress' and
    /// 'destinationEthernetAddress' the source and destination Ethernet
    /// address, respectively, and load into the specifed 'sourceIpvAddress'
    /// the source IPv6 address for the route to the specified
    /// 'destinationIpv6Address'. Return true if such a route exists, and false
    /// otherwise.
    bool find(ntsa::EthernetAddress*   sourceEthernetAddress,
              ntsa::EthernetAddress*   destinationEthernetAddress,
              ntsa::Ipv6Address*       sourceIpv6Address,
              const ntsa::Ipv6Address& destinationIpv6Address) const;

    /// Load into the specified 'result' each route in the table.
    void load(bsl::vector<ntsa::Ipv6Route>* result) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Ipv6RouteTable);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::Ipv6RouteTable
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6RouteTable& object);

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6RouteTable& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
#endif
