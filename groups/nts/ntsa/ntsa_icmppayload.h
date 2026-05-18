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
#include <ntsa_icmpechorequest.h>
#include <ntsa_icmpechoresponse.h>
#include <ntsa_icmpproblem.h>
#include <ntsa_icmpredirect.h>
#include <ntsa_icmprouterrequest.h>
#include <ntsa_icmprouterresponse.h>
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
/// @li @b echoRequest:
/// The body of an ICMP type 8 echo request message, containing the
/// identifier and sequence number. Active when 'type()' is
/// 'ntsa::IcmpType::e_ECHO_REQUEST'.
///
/// @li @b echoResponse:
/// The body of an ICMP type 0 echo reply message, containing the identifier
/// and sequence number echoed back from the echo request. Active when
/// 'type()' is 'ntsa::IcmpType::e_ECHO_RESPONSE'.
///
/// @li @b routerRequest:
/// The body of an ICMP type 10 router soliticitation message. Active when
/// 'type()' is 'ntsa::IcmpType::e_ROUTER_REQUEST'.
///
/// @li @b routerResponse:
/// The body of an ICMP type 9 router advertisement message, containing the the
/// available router addresss, their priorities, and the time to live for the
/// router address list. Active when 'type()' is
/// 'ntsa::IcmpType::e_ROUTER_RESPONSE'.
///
/// @li @b redirect:
/// The body of an ICMP type 5 redirect message, containing the gateway
/// address toward which traffic should be redirected. Active when 'type()'
/// is 'ntsa::IcmpType::e_REDIRECT'.
///
/// @li @b unreachable:
/// The reserved body of an ICMP type 3 destination unreachable message.
/// Active when 'type()' is 'ntsa::IcmpType::e_UNREACHABLE'.
///
/// @li @b timeout:
/// The reserved body of an ICMP type 11 time exceeded message. Active when
/// 'type()' is 'ntsa::IcmpType::e_TIMEOUT'.
///
/// @li @b problem:
/// The body of an ICMP type 12 parameter problem message, containing the
/// pointer field that identifies the offending octet in the IP header.
/// Active when 'type()' is 'ntsa::IcmpType::e_PROBLEM'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_protocol
class IcmpPayload
{
    union {
        bsls::ObjectBuffer<ntsa::IcmpEchoRequest>    d_echoRequest;
        bsls::ObjectBuffer<ntsa::IcmpEchoResponse>   d_echoResponse;
        bsls::ObjectBuffer<ntsa::IcmpRouterRequest>  d_routerRequest;
        bsls::ObjectBuffer<ntsa::IcmpRouterResponse> d_routerResponse;
        bsls::ObjectBuffer<ntsa::IcmpUnreachable>    d_unreachable;
        bsls::ObjectBuffer<ntsa::IcmpRedirect>       d_redirect;
        bsls::ObjectBuffer<ntsa::IcmpTimeout>        d_timeout;
        bsls::ObjectBuffer<ntsa::IcmpProblem>        d_problem;
    };

    ntsa::IcmpType::Value d_type;
    bslma::Allocator*     d_allocator_p;

  public:
    /// Create a new ICMP payload having an undefined type. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    IcmpPayload(bslma::Allocator* basicAllocator = 0);

    /// Create a new ICMP payload having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' object.
    IcmpPayload(bslmf::MovableRef<IcmpPayload> original) NTSCFG_NOEXCEPT;

    /// Create a new ICMP payload having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    IcmpPayload(const IcmpPayload& original,
                bslma::Allocator*  basicAllocator = 0);

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

    /// Select the "echoRequest" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IcmpEchoRequest& makeEchoRequest();

    /// Select the "echoRequest" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpEchoRequest& makeEchoRequest(const ntsa::IcmpEchoRequest& value);

    /// Select the "echoResponse" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IcmpEchoResponse& makeEchoResponse();

    /// Select the "echoResponse" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpEchoResponse& makeEchoResponse(
        const ntsa::IcmpEchoResponse& value);

    /// Select the "routerRequest" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IcmpRouterRequest& makeRouterRequest();

    /// Select the "routerRequest" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpRouterRequest& makeRouterRequest(
        const ntsa::IcmpRouterRequest& value);

    /// Select the "routerResponse" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IcmpRouterResponse& makeRouterResponse();

    /// Select the "routerResponse" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpRouterResponse& makeRouterResponse(
        const ntsa::IcmpRouterResponse& value);

    /// Select the "redirect" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IcmpRedirect& makeRedirect();

    /// Select the "redirect" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpRedirect& makeRedirect(const ntsa::IcmpRedirect& value);

    /// Select the "unreachable" representation.
    ntsa::IcmpUnreachable& makeUnreachable();

    /// Select the "unreachable" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpUnreachable& makeUnreachable(const ntsa::IcmpUnreachable& value);

    /// Select the "timeout" representation.
    ntsa::IcmpTimeout& makeTimeout();

    /// Select the "timeout" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpTimeout& makeTimeout(const ntsa::IcmpTimeout& value);

    /// Select the "problem" representation. Return a reference to the
    /// modifiable representation.
    ntsa::IcmpProblem& makeProblem();

    /// Select the "problem" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IcmpProblem& makeProblem(const ntsa::IcmpProblem& value);

    /// Return a reference to the modifiable "echoRequest" representation. The
    /// behavior is undefined unless 'isEchoRequest()' is true.
    ntsa::IcmpEchoRequest& echoRequest();

    /// Return a reference to the modifiable "echoResponse" representation. The
    /// behavior is undefined unless 'isEchoResponse()' is true.
    ntsa::IcmpEchoResponse& echoResponse();

    /// Return a reference to the modifiable "routerRequest" representation.
    /// The behavior is undefined unless 'isRouterRequest()' is true.
    ntsa::IcmpRouterRequest& routerRequest();

    /// Return a reference to the modifiable "routerResponse" representation.
    /// The behavior is undefined unless 'isRouterResponse()' is true.
    ntsa::IcmpRouterResponse& routerResponse();

    /// Return a reference to the modifiable "redirect" representation. The
    /// behavior is undefined unless 'isRedirect()' is true.
    ntsa::IcmpRedirect& redirect();

    /// Return a reference to the modifiable "unreachable" representation. The
    /// behavior is undefined unless 'isUnreachable()' is true.
    ntsa::IcmpUnreachable& unreachable();

    /// Return a reference to the modifiable "timeout" representation. The
    /// behavior is undefined unless 'isTimeout()' is true.
    ntsa::IcmpTimeout& timeout();

    /// Return a reference to the modifiable "problem" representation. The
    /// behavior is undefined unless 'isProblem()' is true.
    ntsa::IcmpProblem& problem();

    /// Return a reference to the non-modifiable "echoRequest" representation.
    /// The behavior is undefined unless 'isEchoRequest()' is true.
    const ntsa::IcmpEchoRequest& echoRequest() const;

    /// Return a reference to the non-modifiable "echoResponse" representation.
    /// The behavior is undefined unless 'isEchoResponse()' is true.
    const ntsa::IcmpEchoResponse& echoResponse() const;

    /// Return a reference to the non-modifiable "routerRequest" representation.
    /// The behavior is undefined unless 'isRouterRequest()' is true.
    const ntsa::IcmpRouterRequest& routerRequest() const;

    /// Return a reference to the non-modifiable "routerResponse"
    /// representation. The behavior is undefined unless 'isRouterResponse()'
    /// is true.
    const ntsa::IcmpRouterResponse& routerResponse() const;

    /// Return a reference to the non-modifiable "redirect" representation.
    /// The behavior is undefined unless 'isRedirect()' is true.
    const ntsa::IcmpRedirect& redirect() const;

    /// Return a reference to the non-modifiable "unreachable" representation.
    /// The behavior is undefined unless 'isUnreachable()' is true.
    const ntsa::IcmpUnreachable& unreachable() const;

    /// Return a reference to the non-modifiable "timeout" representation. The
    /// behavior is undefined unless 'isTimeout()' is true.
    const ntsa::IcmpTimeout& timeout() const;

    /// Return a reference to the non-modifiable "problem" representation. The
    /// behavior is undefined unless 'isProblem()' is true.
    const ntsa::IcmpProblem& problem() const;

    /// Return the type of the active representation.
    ntsa::IcmpType::Value type() const;

    /// Return the name of the active representation.
    const char* name() const;

    /// Return true if the payload representation is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the "echoRequest" representation is currently selected,
    /// otherwise return false.
    bool isEchoRequest() const;

    /// Return true if the "echoResponse" representation is currently selected,
    /// otherwise return false.
    bool isEchoResponse() const;

    /// Return true if the "routerRequest" representation is currently
    /// selected, otherwise return false.
    bool isRouterRequest() const;

    /// Return true if the "routerResponse" representation is currently
    /// selected, otherwise return false.
    bool isRouterResponse() const;

    /// Return true if the "redirect" representation is currently selected,
    /// otherwise return false.
    bool isRedirect() const;

    /// Return true if the "unreachable" representation is currently selected,
    /// otherwise return false.
    bool isUnreachable() const;

    /// Return true if the "timeout" representation is currently selected,
    /// otherwise return false.
    bool isTimeout() const;

    /// Return true if the "problem" representation is currently selected,
    /// otherwise return false.
    bool isProblem() const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(IcmpPayload);
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
    case ntsa::IcmpType::e_ECHO_REQUEST:
        d_echoRequest.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_ECHO_RESPONSE:
        d_echoResponse.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_ROUTER_REQUEST:
        d_routerRequest.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_ROUTER_RESPONSE:
        d_routerResponse.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_REDIRECT:
        d_redirect.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_UNREACHABLE:
        d_unreachable.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_TIMEOUT:
        d_timeout.object().hash(algorithm);
        break;
    case ntsa::IcmpType::e_PROBLEM:
        d_problem.object().hash(algorithm);
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
