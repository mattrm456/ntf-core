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

#ifndef INCLUDED_NTSA_UDPOPTION
#define INCLUDED_NTSA_UDPOPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_error.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetencoder.h>
#include <ntsa_udpfragmentation.h>
#include <ntsa_udpreassembly.h>
#include <ntsa_udptimepoint.h>
#include <ntsa_udpoptiontype.h>
#include <ntsa_udpoptionvalue.h>
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

/// Provide a union of UDP options.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of
/// UDP options.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b additionalChecksum:
/// The additional checksum option.
///
/// @li @b fragmentation:
/// The fragmentation option.
///
/// @li @b maxDatagramSize:
/// The maximum datagram size option.
///
/// @li @b reassembly:
/// The maximum reassembled datagram characteristics option.
///
/// @li @b echoRequest:
/// The echo request option.
///
/// @li @b echoResponse:
/// The echo response option.
///
/// @li @b timestamp:
/// The timestamp option.
///
/// @li @b unassigned:
/// The option whose kind is not officially registered.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class UdpOption
{
    union {
        /// The additional checksum option.
        bsls::ObjectBuffer<bsl::uint32_t> d_additionalChecksum;

        /// The fragmentation option.
        bsls::ObjectBuffer<ntsa::UdpFragmentation> d_fragmentation;

        /// The maximum datagram size option.
        bsls::ObjectBuffer<bsl::uint32_t> d_maxDatagramSize;

        /// The maximum reassembled datagram characteristics option.
        bsls::ObjectBuffer<ntsa::UdpReassembly> d_reassembly;

        /// The echo request option.
        bsls::ObjectBuffer<bsl::uint32_t> d_echoRequest;

        /// The echo response option.
        bsls::ObjectBuffer<bsl::uint32_t> d_echoResponse;

        /// The timestamp option.
        bsls::ObjectBuffer<ntsa::UdpTimePointInterval> d_timestamp;

        /// The option whose kind is not officially registered.
        bsls::ObjectBuffer<ntsa::UdpOptionValue> d_unassigned;
    };

    /// The option type.
    ntsa::UdpOptionType::Value d_type;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

  private:
    /// Return the number of padding bytes that should preceed an option having
    /// the specified 'optionSize' so that after encoding 'cursor' is aligned
    /// to an address that is a multiple of 4.
    static bsl::size_t paddingSize(const bsl::uint8_t* cursor,
                                   bsl::size_t         optionSize);

  public:
    /// Create a new UDP option having an undefined type. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    // currently installed default allocator is used.
    explicit UdpOption(bslma::Allocator* basicAllocator = 0);

    /// Create a new UDP option having the same value as the specified 'other'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    UdpOption(const UdpOption& other, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~UdpOption();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    UdpOption& operator=(const UdpOption& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Select the "padding" representation.
    void makePadding();

    /// Select the "additionalChecksum" representation. Return a reference to the
    /// modifiable representation.
    bsl::uint32_t& makeAdditionalChecksum();

    /// Select the "additionalChecksum" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bsl::uint32_t& makeAdditionalChecksum(bsl::uint32_t value);

    /// Select the "fragmentation" representation. Return a reference to the
    /// modifiable representation.
    ntsa::UdpFragmentation& makeFragmentation();

    /// Select the "fragmentation" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::UdpFragmentation& makeFragmentation(
        const ntsa::UdpFragmentation& value);

    /// Select the "maxSegmentSize" representation. Return a reference to the
    /// modifiable representation.
    bsl::uint32_t& makeMaxDatagramSize();

    /// Select the "maxSegmentSize" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bsl::uint32_t& makeMaxDatagramSize(bsl::uint32_t value);

    /// Select the "reassembly" representation. Return a reference to the
    /// modifiable representation.
    ntsa::UdpReassembly& makeReassembly();

    /// Select the "reassembly" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::UdpReassembly& makeReassembly(
        const ntsa::UdpReassembly& value);

    /// Select the "echoRequest" representation. Return a reference to the
    /// modifiable representation.
    bsl::uint32_t& makeEchoRequest();

    /// Select the "echoRequest" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bsl::uint32_t& makeEchoRequest(bsl::uint32_t value);

    /// Select the "echoResponse" representation. Return a reference to the
    /// modifiable representation.
    bsl::uint32_t& makeEchoResponse();

    /// Select the "echoResponse" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bsl::uint32_t& makeEchoResponse(bsl::uint32_t value);

    /// Select the "timestamp" representation. Return a reference to the
    /// modifiable representation.
    ntsa::UdpTimePointInterval& makeTimestamp();

    /// Select the "timestamp" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::UdpTimePointInterval& makeTimestamp(
        const ntsa::UdpTimePointInterval& value);

    /// Select the "unassigned" representation. Return a reference to the
    /// modifiable representation.
    ntsa::UdpOptionValue& makeUnassigned();

    /// Select the "unassigned" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::UdpOptionValue& makeUnassigned(const ntsa::UdpOptionValue& value);

    /// Return a reference to the modifiable "additionalChecksum"
    /// representation. The behavior is undefined unless
    /// 'isAdditionalChecksum()' is true.
    bsl::uint32_t& additionalChecksum();

    /// Return a reference to the modifiable "fragmentation" representation.
    /// The behavior is undefined unless 'isFragmentation()' is true.
    ntsa::UdpFragmentation& fragmentation();

    /// Return a reference to the modifiable "maxDatagramSize" representation.
    /// The behavior is undefined unless 'isMaxDatagramSize()' is true.
    bsl::uint32_t& maxDatagramSize();

    /// Return a reference to the modifiable "reassembly" representation.
    /// The behavior is undefined unless 'isReassembly()' is true.
    ntsa::UdpReassembly& reassembly();

    /// Return a reference to the modifiable "echoRequest" representation. The
    /// behavior is undefined unless 'isEchoRequest()' is true.
    bsl::uint32_t& echoRequest();

    /// Return a reference to the modifiable "echoResponse" representation. The
    /// behavior is undefined unless 'isEchoResponse()' is true.
    bsl::uint32_t& echoResponse();

    /// Return a reference to the modifiable "timestamp" representation. The
    /// behavior is undefined unless 'isTimestamp()' is true.
    ntsa::UdpTimePointInterval& timestamp();

    /// Return a reference to the modifiable "unassigned" representation. The
    /// behavior is undefined unless 'isUnassigned()' is true.
    ntsa::UdpOptionValue& unassigned();

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder, bool final) const;

    /// Return a reference to the modifiable "additionalChecksum"
    /// representation. The behavior is undefined unless
    /// 'isAdditionalChecksum()' is true.
    bsl::uint32_t additionalChecksum() const;

    /// Return a reference to the modifiable "fragmentation" representation.
    /// The behavior is undefined unless 'isFragmentation()' is true.
    const ntsa::UdpFragmentation& fragmentation() const;

    /// Return a reference to the modifiable "maxDatagramSize" representation.
    /// The behavior is undefined unless 'isMaxDatagramSize()' is true.
    bsl::uint32_t maxDatagramSize() const;

    /// Return a reference to the modifiable "reassembly" representation.
    /// The behavior is undefined unless 'isReassembly()' is true.
    const ntsa::UdpReassembly& reassembly() const;

    /// Return a reference to the modifiable "echoRequest" representation. The
    /// behavior is undefined unless 'isEchoRequest()' is true.
    bsl::uint32_t echoRequest() const;

    /// Return a reference to the modifiable "echoResponse" representation. The
    /// behavior is undefined unless 'isEchoResponse()' is true.
    bsl::uint32_t echoResponse() const;

    /// Return a reference to the modifiable "timestamp" representation. The
    /// behavior is undefined unless 'isTimestamp()' is true.
    const ntsa::UdpTimePointInterval& timestamp() const;

    /// Return a reference to the non-modifiable "unassigned" representation.
    /// The behavior is undefined unless 'isUnassigned()' is true.
    const ntsa::UdpOptionValue& unassigned() const;

    /// Return the type of the option representation.
    ntsa::UdpOptionType::Value type() const;

    /// Return the name of the option representation.
    const char* name() const;

    /// Return true if the option representation is undefined, otherwise return
    /// false.
    bool isUndefined() const;

    /// Return true if the "padding" representation is currently selected,
    /// otherwise return false.
    bool isPadding() const;

    /// Return true if the "additionalChecksum" representation is currently
    /// selected, otherwise return false.
    bool isAdditionalChecksum() const;

    /// Return true if the "fragmentation" representation is currently
    /// selected, otherwise return false.
    bool isFragmentation() const;

    /// Return true if the "maxDatagramSize" representation is currently
    /// selected, otherwise return false.
    bool isMaxDatagramSize() const;

    /// Return true if the "reassembly" representation is currently selected,
    /// otherwise return false.
    bool isReassembly() const;

    /// Return true if the "echoRequest" representation is currently
    /// selected, otherwise return false.
    bool isEchoRequest() const;

    /// Return true if the "echoResponse" representation is currently
    /// selected, otherwise return false.
    bool isEchoResponse() const;

    /// Return true if the "timestamp" representation is currently selected,
    /// otherwise return false.
    bool isTimestamp() const;

    /// Return true if the "unassigned" representation is currently selected,
    /// otherwise return false.
    bool isUnassigned() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const UdpOption& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const UdpOption& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(UdpOption);
};

/// Defines a type alias for a vector of UDP options.
///
/// @ingroup module_ntsa_system
typedef bsl::vector<ntsa::UdpOption> UdpOptionVector;

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::UdpOption
bsl::ostream& operator<<(bsl::ostream& stream, const UdpOption& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::UdpOption
bool operator==(const UdpOption& lhs, const UdpOption& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::UdpOption
bool operator!=(const UdpOption& lhs, const UdpOption& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::UdpOption
bool operator<(const UdpOption& lhs, const UdpOption& rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
