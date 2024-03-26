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

#ifndef INCLUDED_NTCF_API
#define INCLUDED_NTCF_API

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

#include <ntca_acceptcontext.h>
#include <ntca_acceptevent.h>
#include <ntca_accepteventtype.h>
#include <ntca_acceptoptions.h>
#include <ntca_acceptqueuecontext.h>
#include <ntca_acceptqueueevent.h>
#include <ntca_acceptqueueeventtype.h>
#include <ntca_accepttoken.h>
#include <ntca_bindcontext.h>
#include <ntca_bindevent.h>
#include <ntca_bindeventtype.h>
#include <ntca_bindoptions.h>
#include <ntca_bindtoken.h>
#include <ntca_connectcontext.h>
#include <ntca_connectevent.h>
#include <ntca_connecteventtype.h>
#include <ntca_connectoptions.h>
#include <ntca_connecttoken.h>
#include <ntca_datagramsocketevent.h>
#include <ntca_datagramsocketeventtype.h>
#include <ntca_datagramsocketoptions.h>
#include <ntca_downgradecontext.h>
#include <ntca_downgradeevent.h>
#include <ntca_downgradeeventtype.h>
#include <ntca_driverconfig.h>
#include <ntca_drivermechanism.h>
#include <ntca_encryptionauthentication.h>
#include <ntca_encryptioncertificateoptions.h>
#include <ntca_encryptioncertificatestorageoptions.h>
#include <ntca_encryptioncertificatestoragetype.h>
#include <ntca_encryptionclientoptions.h>
#include <ntca_encryptionkeyoptions.h>
#include <ntca_encryptionkeystorageoptions.h>
#include <ntca_encryptionkeystoragetype.h>
#include <ntca_encryptionkeytype.h>
#include <ntca_encryptionmethod.h>
#include <ntca_encryptionrole.h>
#include <ntca_encryptionserveroptions.h>
#include <ntca_errorcontext.h>
#include <ntca_errorevent.h>
#include <ntca_erroreventtype.h>
#include <ntca_flowcontrolmode.h>
#include <ntca_flowcontroltype.h>
#include <ntca_getdomainnamecontext.h>
#include <ntca_getdomainnameevent.h>
#include <ntca_getdomainnameeventtype.h>
#include <ntca_getdomainnameoptions.h>
#include <ntca_getendpointcontext.h>
#include <ntca_getendpointevent.h>
#include <ntca_getendpointeventtype.h>
#include <ntca_getendpointoptions.h>
#include <ntca_getipaddresscontext.h>
#include <ntca_getipaddressevent.h>
#include <ntca_getipaddresseventtype.h>
#include <ntca_getipaddressoptions.h>
#include <ntca_getportcontext.h>
#include <ntca_getportevent.h>
#include <ntca_getporteventtype.h>
#include <ntca_getportoptions.h>
#include <ntca_getservicenamecontext.h>
#include <ntca_getservicenameevent.h>
#include <ntca_getservicenameeventtype.h>
#include <ntca_getservicenameoptions.h>
#include <ntca_interfaceconfig.h>
#include <ntca_listenersocketevent.h>
#include <ntca_listenersocketeventtype.h>
#include <ntca_listenersocketoptions.h>
#include <ntca_loadbalancingoptions.h>
#include <ntca_monitorablecollectorconfig.h>
#include <ntca_monitorableregistryconfig.h>
#include <ntca_proactorconfig.h>
#include <ntca_ratelimiterconfig.h>
#include <ntca_reactorconfig.h>
#include <ntca_reactorcontext.h>
#include <ntca_reactorevent.h>
#include <ntca_reactoreventoptions.h>
#include <ntca_reactoreventtrigger.h>
#include <ntca_reactoreventtype.h>
#include <ntca_readqueuecontext.h>
#include <ntca_readqueueevent.h>
#include <ntca_readqueueeventtype.h>
#include <ntca_receivecontext.h>
#include <ntca_receiveevent.h>
#include <ntca_receiveeventtype.h>
#include <ntca_receiveoptions.h>
#include <ntca_receivetoken.h>
#include <ntca_resolverconfig.h>
#include <ntca_resolversource.h>
#include <ntca_sendcontext.h>
#include <ntca_sendevent.h>
#include <ntca_sendeventtype.h>
#include <ntca_sendoptions.h>
#include <ntca_sendtoken.h>
#include <ntca_shutdowncontext.h>
#include <ntca_shutdownevent.h>
#include <ntca_shutdowneventtype.h>
#include <ntca_streamsocketevent.h>
#include <ntca_streamsocketeventtype.h>
#include <ntca_streamsocketoptions.h>
#include <ntca_threadconfig.h>
#include <ntca_timercontext.h>
#include <ntca_timerevent.h>
#include <ntca_timereventtype.h>
#include <ntca_timeroptions.h>
#include <ntca_upgradecontext.h>
#include <ntca_upgradeevent.h>
#include <ntca_upgradeeventtype.h>
#include <ntca_upgradeoptions.h>
#include <ntca_upgradetoken.h>
#include <ntca_waiteroptions.h>
#include <ntca_writequeuecontext.h>
#include <ntca_writequeueevent.h>
#include <ntca_writequeueeventtype.h>

#include <ntci_acceptcallback.h>
#include <ntci_acceptcallbackfactory.h>
#include <ntci_acceptfuture.h>
#include <ntci_acceptor.h>
#include <ntci_acceptresult.h>
#include <ntci_authorization.h>
#include <ntci_bindable.h>
#include <ntci_bindcallback.h>
#include <ntci_bindcallbackfactory.h>
#include <ntci_bindfuture.h>
#include <ntci_bindresult.h>
#include <ntci_callback.h>
#include <ntci_cancellation.h>
#include <ntci_closable.h>
#include <ntci_closecallback.h>
#include <ntci_closecallbackfactory.h>
#include <ntci_closefuture.h>
#include <ntci_closeresult.h>
#include <ntci_connectcallback.h>
#include <ntci_connectcallbackfactory.h>
#include <ntci_connectfuture.h>
#include <ntci_connector.h>
#include <ntci_connectresult.h>
#include <ntci_datagramsocket.h>
#include <ntci_datagramsocketfactory.h>
#include <ntci_datagramsocketmanager.h>
#include <ntci_datagramsocketsession.h>
#include <ntci_datapool.h>
#include <ntci_driver.h>
#include <ntci_driverfactory.h>
#include <ntci_encryption.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptioncertificategenerator.h>
#include <ntci_encryptioncertificatestorage.h>
#include <ntci_encryptionclient.h>
#include <ntci_encryptionclientfactory.h>
#include <ntci_encryptiondriver.h>
#include <ntci_encryptionkey.h>
#include <ntci_encryptionkeygenerator.h>
#include <ntci_encryptionkeystorage.h>
#include <ntci_encryptionserver.h>
#include <ntci_encryptionserverfactory.h>
#include <ntci_executor.h>
#include <ntci_getdomainnamecallback.h>
#include <ntci_getdomainnamecallbackfactory.h>
#include <ntci_getendpointcallback.h>
#include <ntci_getendpointcallbackfactory.h>
#include <ntci_getipaddresscallback.h>
#include <ntci_getipaddresscallbackfactory.h>
#include <ntci_getportcallback.h>
#include <ntci_getportcallbackfactory.h>
#include <ntci_getservicenamecallback.h>
#include <ntci_getservicenamecallbackfactory.h>
#include <ntci_identifiable.h>
#include <ntci_interface.h>
#include <ntci_invoker.h>
#include <ntci_listenersocket.h>
#include <ntci_listenersocketfactory.h>
#include <ntci_listenersocketmanager.h>
#include <ntci_listenersocketsession.h>
#include <ntci_log.h>
#include <ntci_monitorable.h>
#include <ntci_proactor.h>
#include <ntci_proactorfactory.h>
#include <ntci_proactormetrics.h>
#include <ntci_proactorpool.h>
#include <ntci_proactorsocket.h>
#include <ntci_ratelimiter.h>
#include <ntci_ratelimiterfactory.h>
#include <ntci_reactor.h>
#include <ntci_reactorfactory.h>
#include <ntci_reactormetrics.h>
#include <ntci_reactorpool.h>
#include <ntci_reactorsocket.h>
#include <ntci_receivecallback.h>
#include <ntci_receivecallbackfactory.h>
#include <ntci_receivefuture.h>
#include <ntci_receiver.h>
#include <ntci_receiveresult.h>
#include <ntci_resolver.h>
#include <ntci_resolverfactory.h>
#include <ntci_scheduler.h>
#include <ntci_sendcallback.h>
#include <ntci_sendcallbackfactory.h>
#include <ntci_sender.h>
#include <ntci_sendfuture.h>
#include <ntci_sendresult.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>
#include <ntci_streamsocket.h>
#include <ntci_streamsocketfactory.h>
#include <ntci_streamsocketmanager.h>
#include <ntci_streamsocketsession.h>
#include <ntci_thread.h>
#include <ntci_threadfactory.h>
#include <ntci_threadpool.h>
#include <ntci_timer.h>
#include <ntci_timercallback.h>
#include <ntci_timercallbackfactory.h>
#include <ntci_timerfactory.h>
#include <ntci_timerfuture.h>
#include <ntci_timerresult.h>
#include <ntci_timersession.h>
#include <ntci_upgradable.h>
#include <ntci_upgradecallback.h>
#include <ntci_upgradecallbackfactory.h>
#include <ntci_user.h>
#include <ntci_waiter.h>

#endif
