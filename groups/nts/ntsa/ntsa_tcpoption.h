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

#ifndef INCLUDED_NTSA_TCPOPTION
#define INCLUDED_NTSA_TCPOPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_error.h>
#include <ntsa_tcpoptiontype.h>
#include <ntsa_tcpsequencenumber.h>
#include <ntsa_tcptimepoint.h>
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

/// Provide a union of TCP options.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of
/// TCP options.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b maxSegmentSize:
/// TODO
///
/// @li @b windowScale:
/// TODO
///
/// @li @b selectiveAckPermitted:
/// TODO
///
/// @li @b selectiveAck:
/// TODO
///
/// @li @b timestamp:
/// TODO
///
/// @li @b fastOpen:
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class TcpOption
{
    union {
        bsls::ObjectBuffer<bsl::size_t>                  d_maxSegmentSize;
        bsls::ObjectBuffer<bsl::size_t>                  d_windowScale;
        bsls::ObjectBuffer<ntsa::TcpSequenceRangeVector> d_selectiveAck;
        bsls::ObjectBuffer<ntsa::TcpTimePointInterval>   d_timestamp;
        bsls::ObjectBuffer<bdlb::Guid>                   d_fastOpen;
    };

    ntsa::TcpOptionType::Value d_type;
    bslma::Allocator*          d_allocator_p;

  public:
    /// Create a new TCP option having an undefined type. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    // currently installed default allocator is used.
    explicit TcpOption(bslma::Allocator* basicAllocator = 0);

    /// Create a new TCP option having the same value as the specified 'other'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    TcpOption(const TcpOption& other, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TcpOption();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    TcpOption& operator=(const TcpOption& other);

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
    ntsa::TcpSequenceRangeVector& makeSelectiveAck();

    /// Select the "selectiveAck" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::TcpSequenceRangeVector& makeSelectiveAck(
        const ntsa::TcpSequenceRangeVector& value);

    /// Select the "timestamp" representation. Return a reference to the
    /// modifiable representation.
    ntsa::TcpTimePointInterval& makeTimestamp();

    /// Select the "timestamp" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::TcpTimePointInterval& makeTimestamp(
        const ntsa::TcpTimePointInterval& value);

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
    ntsa::TcpSequenceRangeVector& selectiveAck();

    /// Return a reference to the modifiable "timestamp" representation. The
    /// behavior is undefined unless 'isTimestamp()' is true.
    ntsa::TcpTimePointInterval& timestamp();

    /// Return a reference to the modifiable "fastOpen" representation. The
    /// behavior is undefined unless 'isFastOpen()' is true.
    bdlb::Guid& fastOpen();

    /// Decode the option from the specified 'buffer'. Load into the specified
    /// 'size' the number of bytes decoded. Return the error.
    ntsa::Error decode(const ntsa::ConstBuffer& buffer, bsl::size_t* size);

    /// Encode the option to the specified 'buffer'. Load into the specified
    /// 'size' the number of bytes decoded. Return the error.
    ntsa::Error encode(ntsa::MutableBuffer* buffer, bsl::size_t* size) const;

    /// Return a reference to the non-modifiable "maxSegmentSize"
    /// representation. The behavior is undefined unless 'isMaxSegmentSize()'
    /// is true.
    bsl::size_t maxSegmentSize() const;

    /// Return a reference to the non-modifiable "windowScale" representation.
    /// The behavior is undefined unless 'isWindowScale()' is true.
    bsl::size_t windowScale() const;

    /// Return a reference to the non-modifiable "selectiveAck" representation.
    /// The behavior is undefined unless 'isSelectiveAck()' is true.
    const ntsa::TcpSequenceRangeVector& selectiveAck() const;

    /// Return a reference to the non-modifiable "timestamp" representation.
    /// The behavior is undefined unless 'isTimestamp()' is true.
    const ntsa::TcpTimePointInterval& timestamp() const;

    /// Return a reference to the non-modifiable "fastOpen" representation. The
    /// behavior is undefined unless 'isFastOpen()' is true.
    const bdlb::Guid& fastOpen() const;

    /// Return the type of the option representation.
    ntsa::TcpOptionType::Value type() const;

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
    bool equals(const TcpOption& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const TcpOption& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(TcpOption);
};

/// Defines a type alias for a vector of TCP options.
///
/// @ingroup module_ntsa_system
typedef bsl::vector<ntsa::TcpOption> TcpOptionVector;

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::TcpOption
bsl::ostream& operator<<(bsl::ostream& stream, const TcpOption& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::TcpOption
bool operator==(const TcpOption& lhs, const TcpOption& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::TcpOption
bool operator!=(const TcpOption& lhs, const TcpOption& rhs);

}  // close namespace ntsa
}  // close namespace BloombergLP
#endif
