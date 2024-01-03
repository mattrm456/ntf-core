// Copyright 2023 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTCQ_ZEROCOPY
#define INCLUDED_NTCQ_ZEROCOPY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <bslma_allocator.h>
#include <bsls_keyword.h>
#include <bsl_vector.h>

#include <ntci_sendcallback.h>
#include <ntsa_data.h>
#include <ntsa_zerocopy.h>

namespace BloombergLP {
namespace ntcq {

/// @internal @brief
/// Describe an entry in a zero-copy queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class ZeroCopyEntry
{
    typedef bsl::vector<ntci::SendCallback> CallbackVector;

    bsl::uint32_t               d_id;
    bsl::shared_ptr<ntsa::Data> d_data_sp;
    ntca::SendEvent             d_event;
    CallbackVector              d_callbacks;
    bslma::Allocator*           d_allocator_p;

  public:
    /// Create a new zero-copy entry. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit ZeroCopyEntry(bslma::Allocator* basicAllocator = 0);

    /// Create a new zero-copy entry having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ZeroCopyEntry(const ZeroCopyEntry& original, 
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~ZeroCopyEntry();

    /// Set the identifier of the zero-copy entry to the specified 'id'.
    void setId(const bsl::uint32_t id);

    /// Set the data transmitted to the specified 'data'. 
    void setData(const bsl::shared_ptr<ntsa::Data>& data);

    /// Set the error encountered during transmission to the specified 'error'. 
    void setError(const ntsa::Error& error);

    /// Set the callback invoked when the data has been completely transmitted
    /// to the specified 'callback'.
    void addCallback(const ntci::SendCallback& callback);

    /// Invoke all callbacks for the specified 'sender'. If the specified
    /// 'defer' flag is false and the requirements of the strand of the
    /// specified 'entry' permits the callback to be invoked immediately by the
    /// 'strand', unlock the specified 'mutex', invoke the callback, then
    /// relock the 'mutex'. Otherwise, enqueue the invocation of the callback
    /// to be executed on the strand of the 'entry', if defined, or by the
    /// specified 'executor' otherwise.
    void dispatch(const bsl::shared_ptr<ntci::Sender>&   sender,
                  const bsl::shared_ptr<ntci::Strand>&   strand,
                  const bsl::shared_ptr<ntci::Executor>& executor,
                  bool                                   defer,
                  bslmt::Mutex*                          mutex);

    /// Return the identifier of the zero-copy entry. 
    bsl::uint32_t id() const;

    /// Return the send context.
    const ntca::SendContext& context() const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ZeroCopyEntry);
};

class ZeroCopyWaitList
{
    typedef bsl::list<ZeroCopyEntry> EntryList;

    EntryList                     d_entryList;
    bsl::shared_ptr<ntci::Strand> d_strand;
    bsl::uint32_t                 d_nextId;

    bool cancelled = false;

  private:
    ZeroCopyWaitList(const ZeroCopyWaitList&) BSLS_KEYWORD_DELETED;
    ZeroCopyWaitList& operator=(const ZeroCopyWaitList&) BSLS_KEYWORD_DELETED;

  public:
    explicit ZeroCopyWaitList(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ZeroCopyWaitList();

    void setStrand(const bsl::shared_ptr<ntci::Strand>& strand);

    void addEntry(ZeroCopyEntry& entry);

    bool zeroCopyAcknowledge(const ntsa::ZeroCopy&                  zc,
                             const bsl::shared_ptr<ntci::Sender>&   sender,
                             const bsl::shared_ptr<ntci::Executor>& executor);

    void cancelWait(const bsl::shared_ptr<ntci::Sender>&   sender,
                    const bsl::shared_ptr<ntci::Executor>& executor);

    //    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ZeroCopyWaitList);
};

}  // close package namespace
}  // close enterprise namespace

#endif  //INCLUDED_NTCQ_ZEROCOPY
