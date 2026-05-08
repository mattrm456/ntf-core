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

#ifndef INCLUDED_NTSA_ICMPPAYLOAD
#define INCLUDED_NTSA_ICMPPAYLOAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_icmpping.h>
#include <ntsa_icmppong.h>
#include <ntsa_icmpproblem.h>
#include <ntsa_icmpredirect.h>
#include <ntsa_icmptimeout.h>
#include <ntsa_icmptype.h>
#include <ntsa_icmpunreachable.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a discriminated union of ICMP message payloads.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of
/// ICMP message-body structures. The active representation is selected by
/// calling the corresponding 'make*' method and is identified by the
/// 'ntsa::IcmpType::Value' discriminator returned by 'type()'.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b echoReply:
/// The body of an ICMP type 0 echo reply message, containing the identifier
/// and sequence number echoed back from the echo request. Active when
/// 'type()' is 'ntsa::IcmpType::e_ECHO_REPLY'.
///
/// @li @b destinationUnreachable:
/// The reserved body of an ICMP type 3 destination unreachable message.
/// Active when 'type()' is 'ntsa::IcmpType::e_DESTINATION_UNREACHABLE'.
///
/// @li @b redirect:
/// The body of an ICMP type 5 redirect message, containing the gateway
/// address toward which traffic should be redirected. Active when 'type()'
/// is 'ntsa::IcmpType::e_REDIRECT'.
///
/// @li @b echo:
/// The body of an ICMP type 8 echo request message, containing the
/// identifier and sequence number. Active when 'type()' is
/// 'ntsa::IcmpType::e_ECHO'.
///
/// @li @b timeExceeded:
/// The reserved body of an ICMP type 11 time exceeded message. Active when
/// 'type()' is 'ntsa::IcmpType::e_TIME_EXCEEDED'.
///
/// @li @b parameterProblem:
/// The body of an ICMP type 12 parameter problem message, containing the
/// pointer field that identifies the offending octet in the IP header.
/// Active when 'type()' is 'ntsa::IcmpType::e_PARAMETER_PROBLEM'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpPayload
{
    union {
        bsls::ObjectBuffer<ntsa::IcmpPong>        d_echoReply;
        bsls::ObjectBuffer<ntsa::IcmpUnreachable> d_destinationUnreachable;
        bsls::ObjectBuffer<ntsa::IcmpRedirect>    d_redirect;
        bsls::ObjectBuffer<ntsa::IcmpPing>        d_echo;
        bsls::ObjectBuffer<ntsa::IcmpTimeout>     d_timeExceeded;
        bsls::ObjectBuffer<ntsa::IcmpProblem>     d_parameterProblem;
    };

    ntsa::IcmpType::Value d_type;

  public:
    /// Create a new ICMP payload having an undefined type.
    IcmpPayload();

    /// Create a new ICMP payload having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' object.
    IcmpPayload(bslmf::MovableRef<IcmpPayload> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP payload having the same value as the specified
    /// 'original' object.
    IcmpPayload(const IcmpPayload& original);

    /// Destroy this object.
    ~IcmpPayload();

    /// Assign the value of the specified 'other' object to this object.
    /// Assign an unspecified but valid value to the 'other' object. Return a
    /// reference to this modifiable object.
    IcmpPayload& operator=(bslmf::MovableRef<IcmpPayload> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IcmpPayload& operator=(const IcmpPayload& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Select the "echoReply" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IcmpPong& makeEchoReply();

    /// Select the "echoReply" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpPong& makeEchoReply(const ntsa::IcmpPong& value);

    /// Select the "destinationUnreachable" representation.
    ntsa::IcmpUnreachable& makeDestinationUnreachable();

    /// Select the "redirect" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IcmpRedirect& makeRedirect();

    /// Select the "redirect" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpRedirect& makeRedirect(const ntsa::IcmpRedirect& value);

    /// Select the "echo" representation. Return a reference to the modifiable
    /// representation.
    ntsa::IcmpPing& makeEcho();

    /// Select the "echo" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpPing& makeEcho(const ntsa::IcmpPing& value);

    /// Select the "timeExceeded" representation.
    ntsa::IcmpTimeout& makeTimeExceeded();

    /// Select the "parameterProblem" representation. Return a reference to
    /// the modifiable representation.
    ntsa::IcmpProblem& makeParameterProblem();

    /// Select the "parameterProblem" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpProblem& makeParameterProblem(const ntsa::IcmpProblem& value);

    /// Return a reference to the modifiable "echoReply" representation. The
    /// behavior is undefined unless 'isEchoReply()' is true.
    ntsa::IcmpPong& echoReply();

    /// Return a reference to the modifiable "destinationUnreachable"
    /// representation. The behavior is undefined unless
    /// 'isDestinationUnreachable()' is true.
    ntsa::IcmpUnreachable& destinationUnreachable();

    /// Return a reference to the modifiable "redirect" representation. The
    /// behavior is undefined unless 'isRedirect()' is true.
    ntsa::IcmpRedirect& redirect();

    /// Return a reference to the modifiable "echo" representation. The
    /// behavior is undefined unless 'isEcho()' is true.
    ntsa::IcmpPing& echo();

    /// Return a reference to the modifiable "timeExceeded" representation.
    /// The behavior is undefined unless 'isTimeExceeded()' is true.
    ntsa::IcmpTimeout& timeExceeded();

    /// Return a reference to the modifiable "parameterProblem" representation.
    /// The behavior is undefined unless 'isParameterProblem()' is true.
    ntsa::IcmpProblem& parameterProblem();

    /// Decode the payload from the specified 'buffer' starting at the
    /// specified 'offset' inside the framing packet having the specified
    /// 'packetSize'. Use the specified 'type' to select the active
    /// representation. Return the error.
    ntsa::Error decode(const bdlbb::BlobBuffer& buffer,
                       bsl::size_t              offset,
                       bsl::size_t              packetSize,
                       ntsa::IcmpType::Value    type);

    /// Encode the payload to the specified 'buffer' starting at the specified
    /// 'offset'. Return the error.
    ntsa::Error encode(bdlbb::BlobBuffer* buffer, bsl::size_t offset) const;

    /// Return a reference to the non-modifiable "echoReply" representation.
    /// The behavior is undefined unless 'isEchoReply()' is true.
    const ntsa::IcmpPong& echoReply() const;

    /// Return a reference to the non-modifiable "destinationUnreachable"
    /// representation. The behavior is undefined unless
    /// 'isDestinationUnreachable()' is true.
    const ntsa::IcmpUnreachable& destinationUnreachable() const;

    /// Return a reference to the non-modifiable "redirect" representation.
    /// The behavior is undefined unless 'isRedirect()' is true.
    const ntsa::IcmpRedirect& redirect() const;

    /// Return a reference to the non-modifiable "echo" representation. The
    /// behavior is undefined unless 'isEcho()' is true.
    const ntsa::IcmpPing& echo() const;

    /// Return a reference to the non-modifiable "timeExceeded" representation.
    /// The behavior is undefined unless 'isTimeExceeded()' is true.
    const ntsa::IcmpTimeout& timeExceeded() const;

    /// Return a reference to the non-modifiable "parameterProblem"
    /// representation. The behavior is undefined unless
    /// 'isParameterProblem()' is true.
    const ntsa::IcmpProblem& parameterProblem() const;

    /// Return the type of the active representation.
    ntsa::IcmpType::Value type() const;

    /// Return the name of the active representation.
    const char* name() const;

    /// Return true if the payload representation is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the "echoReply" representation is currently selected,
    /// otherwise return false.
    bool isEchoReply() const;

    /// Return true if the "destinationUnreachable" representation is
    /// currently selected, otherwise return false.
    bool isDestinationUnreachable() const;

    /// Return true if the "redirect" representation is currently selected,
    /// otherwise return false.
    bool isRedirect() const;

    /// Return true if the "echo" representation is currently selected,
    /// otherwise return false.
    bool isEcho() const;

    /// Return true if the "timeExceeded" representation is currently
    /// selected, otherwise return false.
    bool isTimeExceeded() const;

    /// Return true if the "parameterProblem" representation is currently
    /// selected, otherwise return false.
    bool isParameterProblem() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const IcmpPayload& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const IcmpPayload& other) const;

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
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::IcmpPayload
bsl::ostream& operator<<(bsl::ostream& stream, const IcmpPayload& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IcmpPayload
bool operator==(const IcmpPayload& lhs, const IcmpPayload& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IcmpPayload
bool operator!=(const IcmpPayload& lhs, const IcmpPayload& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IcmpPayload
bool operator<(const IcmpPayload& lhs, const IcmpPayload& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IcmpPayload
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IcmpPayload& value);

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void IcmpPayload::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    hashAppend(algorithm, static_cast<int>(d_type));

    switch (d_type) {
    case ntsa::IcmpType::e_ECHO_REPLY:
        d_echoReply.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_DESTINATION_UNREACHABLE:
        d_destinationUnreachable.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_REDIRECT:
        d_redirect.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_ECHO:
        d_echo.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_TIME_EXCEEDED:
        d_timeExceeded.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_PARAMETER_PROBLEM:
        d_parameterProblem.object().hash(algorithm);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IcmpType::e_UNDEFINED);
    }
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&    algorithm,
                               const IcmpPayload& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
