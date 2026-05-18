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

#ifndef INCLUDED_NTSA_IGMPPAYLOAD
#define INCLUDED_NTSA_IGMPPAYLOAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_igmpjoin.h>
#include <ntsa_igmpleave.h>
#include <ntsa_igmpquery.h>
#include <ntsa_igmpreport.h>
#include <ntsa_igmptype.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a discriminated union of IGMP message payloads.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of
/// IGMP message-body structures. The active representation is selected by
/// calling the corresponding 'make*' method and is identified by the
/// 'ntsa::IgmpType::Value' discriminator returned by 'type()'.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b join:
/// The body of an IGMPv2 Membership Report message (type 0x16), containing
/// the multicast group address being joined. Active when 'type()' is
/// 'ntsa::IgmpType::e_REPORT_V2'.
///
/// @li @b leave:
/// The body of an IGMPv2 Leave Group message (type 0x17), containing the
/// multicast group address being left. Active when 'type()' is
/// 'ntsa::IgmpType::e_LEAVE'.
///
/// @li @b query:
/// The body of an IGMPv3 Membership Query message (type 0x11), containing
/// the group address, flags, QRV, QQIC, and source addresses. Active when
/// 'type()' is 'ntsa::IgmpType::e_QUERY'.
///
/// @li @b report:
/// The body of an IGMPv3 Membership Report message (type 0x22), containing
/// the flags and group records. Active when 'type()' is
/// 'ntsa::IgmpType::e_REPORT_V3'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IgmpPayload
{
    union {
        bsls::ObjectBuffer<ntsa::IgmpJoin>   d_join;
        bsls::ObjectBuffer<ntsa::IgmpLeave>  d_leave;
        bsls::ObjectBuffer<ntsa::IgmpQuery>  d_query;
        bsls::ObjectBuffer<ntsa::IgmpReport> d_report;
    };

    ntsa::IgmpType::Value d_type;
    bslma::Allocator*     d_allocator_p;

  public:
    /// Create a new IGMP payload having an undefined type. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    IgmpPayload(bslma::Allocator* basicAllocator = 0);

    /// Create a new IGMP payload having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' object.
    IgmpPayload(bslmf::MovableRef<IgmpPayload> original) NTSCFG_NOEXCEPT;

    /// Create a new IGMP payload having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    IgmpPayload(const IgmpPayload& original,
                bslma::Allocator*  basicAllocator = 0);

    /// Destroy this object.
    ~IgmpPayload();

    /// Assign the value of the specified 'other' object to this object.
    /// Assign an unspecified but valid value to the 'other' object. Return a
    /// reference to this modifiable object.
    IgmpPayload& operator=(bslmf::MovableRef<IgmpPayload> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IgmpPayload& operator=(const IgmpPayload& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Select the "join" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IgmpJoin& makeJoin();

    /// Select the "join" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IgmpJoin& makeJoin(const ntsa::IgmpJoin& value);

    /// Select the "leave" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IgmpLeave& makeLeave();

    /// Select the "leave" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IgmpLeave& makeLeave(const ntsa::IgmpLeave& value);

    /// Select the "query" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IgmpQuery& makeQuery();

    /// Select the "query" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IgmpQuery& makeQuery(const ntsa::IgmpQuery& value);

    /// Select the "report" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IgmpReport& makeReport();

    /// Select the "report" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IgmpReport& makeReport(const ntsa::IgmpReport& value);

    /// Return a reference to the modifiable "join" representation. The
    /// behavior is undefined unless 'isJoin()' is true.
    ntsa::IgmpJoin& join();

    /// Return a reference to the modifiable "leave" representation. The
    /// behavior is undefined unless 'isLeave()' is true.
    ntsa::IgmpLeave& leave();

    /// Return a reference to the modifiable "query" representation. The
    /// behavior is undefined unless 'isQuery()' is true.
    ntsa::IgmpQuery& query();

    /// Return a reference to the modifiable "report" representation. The
    /// behavior is undefined unless 'isReport()' is true.
    ntsa::IgmpReport& report();

    /// Return a reference to the non-modifiable "join" representation. The
    /// behavior is undefined unless 'isJoin()' is true.
    const ntsa::IgmpJoin& join() const;

    /// Return a reference to the non-modifiable "leave" representation. The
    /// behavior is undefined unless 'isLeave()' is true.
    const ntsa::IgmpLeave& leave() const;

    /// Return a reference to the non-modifiable "query" representation. The
    /// behavior is undefined unless 'isQuery()' is true.
    const ntsa::IgmpQuery& query() const;

    /// Return a reference to the non-modifiable "report" representation. The
    /// behavior is undefined unless 'isReport()' is true.
    const ntsa::IgmpReport& report() const;

    /// Return the type of the active representation.
    ntsa::IgmpType::Value type() const;

    /// Return the name of the active representation.
    const char* name() const;

    /// Return true if the payload representation is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the "join" representation is currently selected,
    /// otherwise return false.
    bool isJoin() const;

    /// Return true if the "leave" representation is currently selected,
    /// otherwise return false.
    bool isLeave() const;

    /// Return true if the "query" representation is currently selected,
    /// otherwise return false.
    bool isQuery() const;

    /// Return true if the "report" representation is currently selected,
    /// otherwise return false.
    bool isReport() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IgmpPayload& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IgmpPayload& other) const;

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

    /// Print this object using the specified 'printer'.
    void print(bslim::Printer* printer) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(IgmpPayload);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IgmpPayload
bsl::ostream& operator<<(bsl::ostream& stream, const IgmpPayload& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IgmpPayload
bool operator==(const IgmpPayload& lhs, const IgmpPayload& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IgmpPayload
bool operator!=(const IgmpPayload& lhs, const IgmpPayload& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IgmpPayload
bool operator<(const IgmpPayload& lhs, const IgmpPayload& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IgmpPayload
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IgmpPayload& value);

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IgmpPayload::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, static_cast<int>(d_type));

    switch (d_type) {
    case ntsa::IgmpType::e_REPORT_V2:
        d_join.object().hash(algorithm);
        break;
    case ntsa::IgmpType::e_LEAVE:
        d_leave.object().hash(algorithm);
        break;
    case ntsa::IgmpType::e_QUERY:
        d_query.object().hash(algorithm);
        break;
    case ntsa::IgmpType::e_REPORT_V3:
        d_report.object().hash(algorithm);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IgmpType::e_UNDEFINED);
    }
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&    algorithm,
                              const IgmpPayload& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
