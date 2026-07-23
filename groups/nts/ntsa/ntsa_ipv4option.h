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

#ifndef INCLUDED_NTSA_IPV4OPTION
#define INCLUDED_NTSA_IPV4OPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_error.h>
#include <ntsa_ipv4optiontype.h>
#include <ntsa_ipv4optionvalue.h>
#include <ntsa_ipv4routeledger.h>
#include <ntsa_ipv4routesequence.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlb_guid.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a union of Internet Protocol version 4 (IPv4) options.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of
/// TCP options.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b alert:
/// This option to instruct a router to examine the packet more closely.
///
/// @li @b timestamp:
/// The option to instruct routers to record timestamps at each hop to measure
/// network latency and delays.
///
/// @li @b recordRoute:
/// The option to instruct routers to append their IP addresses to the packet
/// header, tracing the exact path taken.
///
/// @li @b looseSourceRoute:
/// The option to specify a list of routers the packet must visit, allowing for
/// intermediate nodes.
///
/// @li @b strictSourceRoute:
/// The option to force the packet to follow a rigidly defined list of routers.
///
/// @li @b unassigned:
/// The option whose kind is not officially registered.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class Ipv4Option
{
    union {
        /// The "timestamp" representation.
        bsls::ObjectBuffer<ntsa::Ipv4RouteLedger> d_timestamp;

        /// The "recordRoute" representation.
        bsls::ObjectBuffer<ntsa::Ipv4RouteSequence> d_recordRoute;

        /// The "sourceRouteLoose" representation.
        bsls::ObjectBuffer<ntsa::Ipv4RouteSequence> d_sourceRouteLoose;

        /// The "sourceRouteTight" representation.
        bsls::ObjectBuffer<ntsa::Ipv4RouteSequence> d_sourceRouteTight;

        /// The option whose kind is not officially registered.
        bsls::ObjectBuffer<ntsa::Ipv4OptionValue> d_unassigned;
    };

    /// The option type.
    ntsa::Ipv4OptionType::Value d_type;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

  private:
    /// Return the number of padding bytes that should preceed an option having
    /// the specified 'optionSize' so that after encoding 'cursor' is aligned
    /// to an address that is a multiple of 4.
    static bsl::size_t paddingSize(const bsl::uint8_t* cursor,
                                   bsl::size_t         optionSize);

  public:
    /// Create a new TCP option having an undefined type. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    // currently installed default allocator is used.
    explicit Ipv4Option(bslma::Allocator* basicAllocator = 0);

    /// Create a new TCP option having the same value as the specified 'other'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    Ipv4Option(const Ipv4Option& other, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Ipv4Option();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Ipv4Option& operator=(const Ipv4Option& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Select the "padding" representation.
    void makePadding();

    /// Select the "alert" representation.
    void makeAlert();

    /// Select the "timestamp" representation. Return a reference to the
    /// modifiable representation.
    ntsa::Ipv4RouteLedger& makeTimestamp();

    /// Select the "timestamp" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::Ipv4RouteLedger& makeTimestamp(const ntsa::Ipv4RouteLedger& value);

    /// Select the "recordRoute" representation. Return a reference to the
    /// modifiable representation.
    ntsa::Ipv4RouteSequence& makeRecordRoute();

    /// Select the "recordRoute" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::Ipv4RouteSequence& makeRecordRoute(
        const ntsa::Ipv4RouteSequence& value);

    /// Select the "sourceRouteLoose" representation. Return a reference to the
    /// modifiable representation.
    ntsa::Ipv4RouteSequence& makeSourceRouteLoose();

    /// Select the "sourceRouteLoose" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    ntsa::Ipv4RouteSequence& makeSourceRouteLoose(
        const ntsa::Ipv4RouteSequence& value);

    /// Select the "sourceRouteTight" representation. Return a reference to the
    /// modifiable representation.
    ntsa::Ipv4RouteSequence& makeSourceRouteTight();

    /// Select the "sourceRouteTight" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    ntsa::Ipv4RouteSequence& makeSourceRouteTight(
        const ntsa::Ipv4RouteSequence& value);

    /// Select the "unassigned" representation. Return a reference to the
    /// modifiable representation.
    ntsa::Ipv4OptionValue& makeUnassigned();

    /// Select the "unassigned" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::Ipv4OptionValue& makeUnassigned(const ntsa::Ipv4OptionValue& value);

    /// Return a reference to the modifiable "timestamp" representation. The
    /// behavior is undefined unless 'isTimestamp()' is true.
    ntsa::Ipv4RouteLedger& timestamp();

    /// Return a reference to the modifiable "recordRoute" representation. The
    /// behavior is undefined unless 'isRecordRoute()' is true.
    ntsa::Ipv4RouteSequence& recordRoute();

    /// Return a reference to the modifiable "sourceRouteLoose" representation.
    /// The behavior is undefined unless 'isSourceRouteLoose()' is true.
    ntsa::Ipv4RouteSequence& sourceRouteLoose();

    /// Return a reference to the modifiable "sourceRouteTight" representation.
    /// The behavior is undefined unless 'isSourceRouteTight()' is true.
    ntsa::Ipv4RouteSequence& sourceRouteTight();

    /// Return a reference to the modifiable "unassigned" representation. The
    /// behavior is undefined unless 'isUnassigned()' is true.
    ntsa::Ipv4OptionValue& unassigned();

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder, bool final) const;

    /// Return a reference to the non-modifiable "timestamp" representation.
    /// The behavior is undefined unless 'isTimestamp()' is true.
    const ntsa::Ipv4RouteLedger& timestamp() const;

    /// Return a reference to the non-modifiable "recordRoute" representation.
    /// The behavior is undefined unless 'isRecordRoute()' is true.
    const ntsa::Ipv4RouteSequence& recordRoute() const;

    /// Return a reference to the non-modifiable "sourceRouteLoose"
    /// representation. The behavior is undefined unless 'isSourceRouteLoose()'
    /// is true.
    const ntsa::Ipv4RouteSequence& sourceRouteLoose() const;

    /// Return a reference to the non-modifiable "sourceRouteTight"
    /// representation. The behavior is undefined unless 'isSourceRouteTight()'
    /// is true.
    const ntsa::Ipv4RouteSequence& sourceRouteTight() const;

    /// Return a reference to the non-modifiable "unassigned" representation.
    /// The behavior is undefined unless 'isUnassigned()' is true.
    const ntsa::Ipv4OptionValue& unassigned() const;

    /// Return the type of the option representation.
    ntsa::Ipv4OptionType::Value type() const;

    /// Return the name of the option representation.
    const char* name() const;

    /// Return true if the option representation is undefined, otherwise return
    /// false.
    bool isUndefined() const;

    /// Return true if the "padding" representation is currently selected,
    /// otherwise return false.
    bool isPadding() const;

    /// Return true if the "alert" representation is currently selected,
    /// otherwise return false.
    bool isAlert() const;

    /// Return true if the "timestamp" representation is currently selected,
    /// otherwise return false.
    bool isTimestamp() const;

    /// Return true if the "recordRoute" representation is currently selected,
    /// otherwise return false.
    bool isRecordRoute() const;

    /// Return true if the "sourceRouteLoose" representation is currently
    /// selected, otherwise return false.
    bool isSourceRouteLoose() const;

    /// Return true if the "sourceRouteTight" representation is currently
    /// selected, otherwise return false.
    bool isSourceRouteTight() const;

    /// Return true if the "unassigned" representation is currently selected,
    /// otherwise return false.
    bool isUnassigned() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Ipv4Option& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Ipv4Option& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Ipv4Option);
};

/// Defines a type alias for a vector of TCP options.
///
/// @ingroup module_ntsa_system
typedef bsl::vector<ntsa::Ipv4Option> Ipv4OptionVector;

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::Ipv4Option
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Option& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Option
bool operator==(const Ipv4Option& lhs, const Ipv4Option& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Option
bool operator!=(const Ipv4Option& lhs, const Ipv4Option& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv4Option
bool operator<(const Ipv4Option& lhs, const Ipv4Option& rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
