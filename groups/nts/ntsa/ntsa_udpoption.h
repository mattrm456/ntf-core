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
#include <ntsa_udpoptiontype.h>
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
/// @li @b additionalPayloadChecksum:
/// TODO
///
/// @li @b fragmentation:
/// TODO
///
/// @li @b maxDatagramSize:
/// TODO
///
/// @li @b maxReassembledDatagramSize:
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class UdpOption
{
    union {
        bsls::ObjectBuffer<bsl::uint32_t>                  d_additionalPayloadChecksum;
        bsls::ObjectBuffer<ntsa::UdpFragmentation>         d_fragmentation;


        bsls::ObjectBuffer<bsl::size_t>                  d_maxSegmentSize;
        bsls::ObjectBuffer<bsl::size_t>                  d_windowScale;
        bsls::ObjectBuffer<ntsa::UdpSequenceRangeVector> d_selectiveAck;
        bsls::ObjectBuffer<ntsa::UdpTimePointInterval>   d_timestamp;
        bsls::ObjectBuffer<bdlb::Guid>                   d_fastOpen;
    };

    ntsa::UdpOptionType::Value d_type;
    bslma::Allocator*          d_allocator_p;

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

    /// Select the "maxSegmentSize" representation. Return a reference to the
    /// modifiable representation.
    bsl::size_t& makeMaxSegmentSize();

    /// Select the "maxSegmentSize" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bsl::size_t& makeMaxSegmentSize(bsl::size_t value);

    /// Select the "windowScale" representation. Return a reference to the
    /// modifiable representation.
    bsl::size_t& makeWindowScale();

    /// Select the "windowScale" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bsl::size_t& makeWindowScale(bsl::size_t value);

    /// Select the "selectiveAckPermitted" representation.
    void makeSelectiveAckPermitted();

    /// Select the "selectiveAck" representation. Return a reference to the
    /// modifiable representation.
    ntsa::UdpSequenceRangeVector& makeSelectiveAck();

    /// Select the "selectiveAck" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::UdpSequenceRangeVector& makeSelectiveAck(
        const ntsa::UdpSequenceRangeVector& value);

    /// Select the "timestamp" representation. Return a reference to the
    /// modifiable representation.
    ntsa::UdpTimePointInterval& makeTimestamp();

    /// Select the "timestamp" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::UdpTimePointInterval& makeTimestamp(
        const ntsa::UdpTimePointInterval& value);

    /// Select the "fastOpen" representation. Return a reference to the
    /// modifiable representation.
    bdlb::Guid& makeFastOpen();

    /// Select the "fastOpen" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bdlb::Guid& makeFastOpen(const bdlb::Guid& value);

    /// Return a reference to the modifiable "maxSegmentSize" representation.
    /// The behavior is undefined unless 'isMaxSegmentSize()' is true.
    bsl::size_t& maxSegmentSize();

    /// Return a reference to the modifiable "windowScale" representation. The
    /// behavior is undefined unless 'isWindowScale()' is true.
    bsl::size_t& windowScale();

    /// Return a reference to the modifiable "selectiveAck" representation. The
    /// behavior is undefined unless 'isSelectiveAck()' is true.
    ntsa::UdpSequenceRangeVector& selectiveAck();

    /// Return a reference to the modifiable "timestamp" representation. The
    /// behavior is undefined unless 'isTimestamp()' is true.
    ntsa::UdpTimePointInterval& timestamp();

    /// Return a reference to the modifiable "fastOpen" representation. The
    /// behavior is undefined unless 'isFastOpen()' is true.
    bdlb::Guid& fastOpen();

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::PacketDecoder* decoder);

    /// Encode the object through the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::PacketEncoder* encoder, bool final) const;

    /// Return a reference to the non-modifiable "maxSegmentSize"
    /// representation. The behavior is undefined unless 'isMaxSegmentSize()'
    /// is true.
    bsl::size_t maxSegmentSize() const;

    /// Return a reference to the non-modifiable "windowScale" representation.
    /// The behavior is undefined unless 'isWindowScale()' is true.
    bsl::size_t windowScale() const;

    /// Return a reference to the non-modifiable "selectiveAck" representation.
    /// The behavior is undefined unless 'isSelectiveAck()' is true.
    const ntsa::UdpSequenceRangeVector& selectiveAck() const;

    /// Return a reference to the non-modifiable "timestamp" representation.
    /// The behavior is undefined unless 'isTimestamp()' is true.
    const ntsa::UdpTimePointInterval& timestamp() const;

    /// Return a reference to the non-modifiable "fastOpen" representation. The
    /// behavior is undefined unless 'isFastOpen()' is true.
    const bdlb::Guid& fastOpen() const;

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

    /// Return true if the "maxSegmentSize" representation is currently
    /// selected, otherwise return false.
    bool isMaxSegmentSize() const;

    /// Return true if the "windowScale" representation is currently
    /// selected, otherwise return false.
    bool isWindowScale() const;

    /// Return true if the "selectiveAckPermitted" representation is currently
    /// selected, otherwise return false.
    bool isSelectiveAckPermitted() const;

    /// Return true if the "selectiveAck" representation is currently selected,
    /// otherwise return false.
    bool isSelectiveAck() const;

    /// Return true if the "timestamp" representation is currently selected,
    /// otherwise return false.
    bool isTimestamp() const;

    /// Return true if the "fastOpen" representation is currently selected,
    /// otherwise return false.
    bool isFastOpen() const;

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
