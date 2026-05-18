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

#ifndef INCLUDED_NTSA_IGMPCHECKSUM
#define INCLUDED_NTSA_IGMPCHECKSUM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_cstddef.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Group Management Protocol (IGMP) checksum.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IgmpChecksum
{
    /// Enumerate the constants used by the implementation.
    enum Constant {
        /// The protocol number indicating IGMP.
        k_PROTOCOL_IGMP = 2
    };

    /// The accumulated value.
    bsl::uint32_t d_accumulator;

  public:
    /// Create a new IGMP checksum.
    IgmpChecksum();

    /// Create a new IGMP checksum having the same value as the specified
    /// 'original' object.
    IgmpChecksum(const IgmpChecksum& original);

    /// Destroy this object.
    ~IgmpChecksum();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    IgmpChecksum& operator=(const IgmpChecksum& other);

    /// Reset the object to its value upon default construction.
    void reset();

    /// Add the pseudo header for a packet from the specified 'sourceAddress'
    /// to the specified 'destinationAddress' having the specified 'length'
    /// of the IGMP packet, including the IGMP header.
    void add(const ntsa::Ipv4Address& sourceAddress,
             const ntsa::Ipv4Address& destinationAddress,
             bsl::size_t              length);

    /// Add the specified 'data' having the specified 'size' to the checksum.
    void add(const void* data, bsl::size_t size);

    /// Return the accumulator.
    bsl::uint32_t accumulator() const;

    /// Return the checksum value.
    bsl::uint16_t value() const;
};

}  // close package namespace
}  // close enterprise namespace
#endif
