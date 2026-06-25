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

#include <ntsu_routeutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_routeutil_cpp, "$Id$ $CSID$")

#include <ntsa_adapter.h>
#include <ntsa_packet.h>
#include <ntsa_packetdecoder.h>
#include <ntsa_packetdecodercontext.h>
#include <ntsa_packetdecoderoptions.h>
#include <ntsa_packetencoder.h>
#include <ntsa_packetencodercontext.h>
#include <ntsa_packetencoderoptions.h>
#include <ntsa_packetqueue.h>
#include <ntscfg_limits.h>
#include <ntscfg_platform.h>
#include <ntsu_adapterutil.h>

#include <bdlb_string.h>
#include <bdlbb_blob.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlcc_fixedqueue.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorguard.h>
#include <bslma_default.h>
#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_DARWIN)
#include <arpa/inet.h>
#include <net/bpf.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>
#include <netinet/if_ether.h>
#include <sys/sysctl.h>
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <linux/errqueue.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <winerror.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
// clang-format on
#ifdef interface
#undef interface
#endif
#pragma comment(lib, "ws2_32")
#endif

namespace BloombergLP {
namespace ntsu {

#if defined(BSLS_PLATFORM_OS_DARWIN)

#define NTSU_ROUTEUTIL_LOG_ERROR_SYSCTL(error)                                \
    do {                                                                      \
        BALL_LOG_ERROR << "Failed to get routing table: " << (error)          \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_HEADER(error)                   \
    do {                                                                      \
        BALL_LOG_ERROR << "Failed to decode route message header: " << (error)       \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_PAYLOAD(error)                   \
    do {                                                                      \
        BALL_LOG_ERROR << "Failed to decode route message payload: " << (error)       \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX(type, sa, error)                 \
    do {                                                                      \
        BALL_LOG_ERROR << "Failed to decode route message payload [ type = "  \
                       << (type) << " sa_family = "                           \
                       << static_cast<bsl::uint32_t>((sa)->sa_family)         \
                       << " sa_len = "                                        \
                       << static_cast<bsl::uint32_t>((sa)->sa_len)            \
                       << " ]: " << (error) << BALL_LOG_END;                  \
    } while (false)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_DST(sa, error)                   \
    NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX("RTAX_DST", sa, error)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_NETMASK(sa, error)               \
    NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX("RTAX_NETMASK", sa, error)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_GATEWAY(sa, error)               \
    NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX("RTAX_GATEWAY", sa, error)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_IFA(sa, error)                   \
    NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX("RTAX_IFA", sa, error)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_IFP(sa, error)                   \
    NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX("RTAX_IFP", sa, error)

#define NTSU_ROUTEUTIL_LOG_ROUTE_MESSAGE(rtm)                                 \
    do {                                                                      \
        BALL_LOG_TRACE << "Decoded route message header [ flags = "           \
                       << Impl::describesRtaxFlags(rtm) << " ]"               \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_ROUTEUTIL_LOG_RTAX(type, sa)                                     \
    do {                                                                      \
        BALL_LOG_ERROR << "Decoding route message payload [ type = "          \
                       << (type) << " sa_family = "                           \
                       << static_cast<bsl::uint32_t>((sa)->sa_family)         \
                       << " sa_len = "                                        \
                       << static_cast<bsl::uint32_t>((sa)->sa_len) << " ]"    \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_ROUTEUTIL_LOG_RTAX_DST(sa) NTSU_ROUTEUTIL_LOG_RTAX("RTAX_DST", sa)

#define NTSU_ROUTEUTIL_LOG_RTAX_NETMASK(sa)                                   \
    NTSU_ROUTEUTIL_LOG_RTAX("RTAX_NETMASK", sa)

#define NTSU_ROUTEUTIL_LOG_RTAX_GATEWAY(sa)                                   \
    NTSU_ROUTEUTIL_LOG_RTAX("RTAX_GATEWAY", sa)

#define NTSU_ROUTEUTIL_LOG_RTAX_IFA(sa) NTSU_ROUTEUTIL_LOG_RTAX("RTAX_IFA", sa)

#define NTSU_ROUTEUTIL_LOG_RTAX_IFP(sa) NTSU_ROUTEUTIL_LOG_RTAX("RTAX_IFP", sa)

#define NTSU_ROUTEUTIL_LOG_XYZ()                                              \
    do {                                                                      \
        BALL_LOG_TRACE << "XYZ" << BALL_LOG_END;                              \
    } while (false)

/// Provide a private, platform-specific implementation of utilities for
/// discovering route tables.
class RouteUtil::Impl
{
    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.ROUTEUTIL");

  public:
    static ntsa::Error decodeRouteHeader(const rt_msghdr**    rtm,
                                         const bsl::uint8_t** current,
                                         const bsl::uint8_t*  end);

    static ntsa::Error decodeRoutePayload(const sockaddr** rti,
                                          const rt_msghdr* rtm);

    static const sockaddr* next(const sockaddr* sa);

    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::IpAddress>* ipAddress,
        const sockaddr*                       sa);

    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr*                         sa);

    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr*                         sa);

    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr_in6*                     sa);

    static ntsa::Error decodeNetMask(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Mask,
        const sockaddr*                         sa);

    static ntsa::Error decodeNetMask(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Mask,
        const sockaddr*                         sa);

    static ntsa::Error decodeGateway(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::IpAddress>*       ipAddress,
        const sockaddr*                             sa);

    static ntsa::Error decodeGateway(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv4Address>*     ipv4Address,
        const sockaddr*                             sa);

    static ntsa::Error decodeGateway(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv6Address>*     ipv6Address,
        const sockaddr*                             sa);

    static ntsa::Error decodeGateway(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    static ntsa::Error decodeGateway(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr_in6*                     sa);

    static ntsa::Error decodeGateway(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_dl*                          sa);

    static ntsa::Error decodeIfa(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::IpAddress>*       ipAddress,
        const sockaddr*                             sa);

    static ntsa::Error decodeIfa(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv4Address>*     ipv4Address,
        const sockaddr*                             sa);

    static ntsa::Error decodeIfa(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv6Address>*     ipv6Address,
        const sockaddr*                             sa);

    static ntsa::Error decodeIfa(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    static ntsa::Error decodeIfa(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr_in6*                     sa);

    static ntsa::Error decodeIfa(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_dl*                          sa);

    static ntsa::Error decodeIfp(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::IpAddress>*       ipAddress,
        const sockaddr*                             sa);

    static ntsa::Error decodeIfp(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv4Address>*     ipv4Address,
        const sockaddr*                             sa);

    static ntsa::Error decodeIfp(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv6Address>*     ipv6Address,
        const sockaddr*                             sa);

    static ntsa::Error decodeIfp(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    static ntsa::Error decodeIfp(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr_in6*                     sa);

    static ntsa::Error decodeIfp(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_dl*                          sa);

    static ntsa::Error decodeIpv4Address(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    static ntsa::Error decodeIpv6Address(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv4Address,
        const sockaddr_in6*                     sa);

    static ntsa::Error decodeIpv4Mask(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Mask,
        const sockaddr_in*                      sa);

    static ntsa::Error decodeIpv6Mask(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Mask,
        const sockaddr_in6*                     sa);

    static ntsa::Error decodeLink(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_dl*                          sa);

    static bsl::string describesRtaxFlags(const rt_msghdr* rtm);
};

ntsa::Error RouteUtil::Impl::decodeRouteHeader(const rt_msghdr**    rtm,
                                               const bsl::uint8_t** current,
                                               const bsl::uint8_t*  end)
{
    *rtm = 0;

    while (true) {
        if (*current >= end) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        if (reinterpret_cast<bsl::uintptr_t>(*current) % 4 != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const rt_msghdr* header = reinterpret_cast<const rt_msghdr*>(*current);

        if (header->rtm_msglen == 0) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        *current += static_cast<bsl::size_t>(header->rtm_msglen);

        if (header->rtm_version != RTM_VERSION) {
            continue;
        }

        *rtm = header;
        break;
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeRoutePayload(const sockaddr** rti,
                                                const rt_msghdr* rtm)
{
    if (rtm->rtm_msglen == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (rtm->rtm_version != RTM_VERSION) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const sockaddr* sa = reinterpret_cast<const sockaddr*>(
        reinterpret_cast<const bsl::uint8_t*>(rtm) + sizeof(rt_msghdr));

    const sockaddr* saEnd = reinterpret_cast<const sockaddr*>(
        reinterpret_cast<const bsl::uint8_t*>(rtm) + rtm->rtm_msglen);

    for (int i = 0; i < RTAX_MAX; i++) {
        if ((rtm->rtm_addrs & (1 << i)) != 0) {
            if (reinterpret_cast<bsl::uintptr_t>(sa) % 4 != 0) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            if (sa >= saEnd) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            rti[i] = sa;

            sa = Impl::next(sa);
        }
        else {
            rti[i] = 0;
        }
    }

    return ntsa::Error();
}

const sockaddr* RouteUtil::Impl::next(const sockaddr* sa)
{
    bsl::size_t size;
    if (sa->sa_len > 0) {
        size = (sa->sa_len + sizeof(bsl::uint32_t) - 1) &
               ~(sizeof(bsl::uint32_t) - 1);
    }
    else {
        size = sizeof(bsl::uint32_t);
    }

    return reinterpret_cast<const sockaddr*>(
        reinterpret_cast<const bsl::uint8_t*>(sa) + size);
}

ntsa::Error RouteUtil::Impl::decodeDestination(
    bdlb::NullableValue<ntsa::IpAddress>* ipAddress,
    const sockaddr*                       sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_DST(sa);

    if (sa->sa_family == AF_INET) {
        bdlb::NullableValue<ntsa::Ipv4Address> ipv4Address;
        error =
            Impl::decodeDestination(&ipv4Address,
                                    reinterpret_cast<const sockaddr_in*>(sa));
        if (error) {
            return error;
        }

        if (ipv4Address.has_value()) {
            ipAddress->makeValue().makeV4(ipv4Address.value());
        }
    }
    else if (sa->sa_family == AF_INET6) {
        bdlb::NullableValue<ntsa::Ipv6Address> ipv6Address;
        error =
            Impl::decodeDestination(&ipv6Address,
                                    reinterpret_cast<const sockaddr_in6*>(sa));
        if (error) {
            return error;
        }

        if (ipv6Address.has_value()) {
            ipAddress->makeValue().makeV6(ipv6Address.value());
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeDestination(
    bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
    const sockaddr*                         sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_DST(sa);

    if (sa->sa_family == AF_INET) {
        error =
            Impl::decodeDestination(ipv4Address,
                                    reinterpret_cast<const sockaddr_in*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeDestination(
    bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
    const sockaddr*                         sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_DST(sa);

    if (sa->sa_family == AF_INET6) {
        error =
            Impl::decodeDestination(ipv6Address,
                                    reinterpret_cast<const sockaddr_in6*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeDestination(
    bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
    const sockaddr_in*                      sa)
{
    return Impl::decodeIpv4Address(ipv4Address, sa);
}

ntsa::Error RouteUtil::Impl::decodeDestination(
    bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
    const sockaddr_in6*                     sa)
{
    return Impl::decodeIpv6Address(ipv6Address, sa);
}

ntsa::Error RouteUtil::Impl::decodeNetMask(
    bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Mask,
    const sockaddr*                         sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_NETMASK(sa);

    error = Impl::decodeIpv4Mask(ipv4Mask,
                                 reinterpret_cast<const sockaddr_in*>(sa));
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeNetMask(
    bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Mask,
    const sockaddr*                         sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_NETMASK(sa);

    error = Impl::decodeIpv6Mask(ipv6Mask,
                                 reinterpret_cast<const sockaddr_in6*>(sa));
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeGateway(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    bdlb::NullableValue<ntsa::IpAddress>*       ipAddress,
    const sockaddr*                             sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_GATEWAY(sa);

    if (sa->sa_family == AF_INET) {
        bdlb::NullableValue<ntsa::Ipv4Address> ipv4Address;
        error = Impl::decodeGateway(&ipv4Address,
                                    reinterpret_cast<const sockaddr_in*>(sa));
        if (error) {
            return error;
        }

        if (ipv4Address.has_value()) {
            ipAddress->makeValue().makeV4(ipv4Address.value());
        }
    }
    else if (sa->sa_family == AF_INET6) {
        bdlb::NullableValue<ntsa::Ipv6Address> ipv6Address;
        error = Impl::decodeGateway(&ipv6Address,
                                    reinterpret_cast<const sockaddr_in6*>(sa));
        if (error) {
            return error;
        }

        if (ipv6Address.has_value()) {
            ipAddress->makeValue().makeV6(ipv6Address.value());
        }
    }
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeGateway(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    bdlb::NullableValue<ntsa::Ipv4Address>*     ipv4Address,
    const sockaddr*                             sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_GATEWAY(sa);

    if (sa->sa_family == AF_INET) {
        error = Impl::decodeGateway(ipv4Address,
                                    reinterpret_cast<const sockaddr_in*>(sa));
        if (error) {
            return error;
        }
    }
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeGateway(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    bdlb::NullableValue<ntsa::Ipv6Address>*     ipv6Address,
    const sockaddr*                             sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_GATEWAY(sa);

    if (sa->sa_family == AF_INET6) {
        error = Impl::decodeGateway(ipv6Address,
                                    reinterpret_cast<const sockaddr_in6*>(sa));
        if (error) {
            return error;
        }
    }
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeGateway(
    bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
    const sockaddr_in*                      sa)
{
    return Impl::decodeIpv4Address(ipv4Address, sa);
}

ntsa::Error RouteUtil::Impl::decodeGateway(
    bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
    const sockaddr_in6*                     sa)
{
    return Impl::decodeIpv6Address(ipv6Address, sa);
}

ntsa::Error RouteUtil::Impl::decodeGateway(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_dl*                          sa)
{
    return Impl::decodeLink(adapterName, adapterIndex, ethernetAddress, sa);
}

ntsa::Error RouteUtil::Impl::decodeIfa(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    bdlb::NullableValue<ntsa::IpAddress>*       ipAddress,
    const sockaddr*                             sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_IFA(sa);

    if (sa->sa_family == AF_INET) {
        bdlb::NullableValue<ntsa::Ipv4Address> ipv4Address;
        error = Impl::decodeGateway(&ipv4Address,
                                    reinterpret_cast<const sockaddr_in*>(sa));
        if (error) {
            return error;
        }

        if (ipv4Address.has_value()) {
            ipAddress->makeValue().makeV4(ipv4Address.value());
        }
    }
    else if (sa->sa_family == AF_INET6) {
        bdlb::NullableValue<ntsa::Ipv6Address> ipv6Address;
        error = Impl::decodeGateway(&ipv6Address,
                                    reinterpret_cast<const sockaddr_in6*>(sa));
        if (error) {
            return error;
        }

        if (ipv6Address.has_value()) {
            ipAddress->makeValue().makeV6(ipv6Address.value());
        }
    }
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIfa(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    bdlb::NullableValue<ntsa::Ipv4Address>*     ipv4Address,
    const sockaddr*                             sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_IFA(sa);

    if (sa->sa_family == AF_INET) {
        error = Impl::decodeGateway(ipv4Address,
                                    reinterpret_cast<const sockaddr_in*>(sa));
        if (error) {
            return error;
        }
    }
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIfa(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    bdlb::NullableValue<ntsa::Ipv6Address>*     ipv6Address,
    const sockaddr*                             sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_IFA(sa);

    if (sa->sa_family == AF_INET6) {
        error = Impl::decodeGateway(ipv6Address,
                                    reinterpret_cast<const sockaddr_in6*>(sa));
        if (error) {
            return error;
        }
    }
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIfa(
    bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
    const sockaddr_in*                      sa)
{
    return Impl::decodeIpv4Address(ipv4Address, sa);
}

ntsa::Error RouteUtil::Impl::decodeIfa(
    bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
    const sockaddr_in6*                     sa)
{
    return Impl::decodeIpv6Address(ipv6Address, sa);
}

ntsa::Error RouteUtil::Impl::decodeIfa(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_dl*                          sa)
{
    return Impl::decodeLink(adapterName, adapterIndex, ethernetAddress, sa);
}

ntsa::Error RouteUtil::Impl::decodeIfp(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    bdlb::NullableValue<ntsa::IpAddress>*       ipAddress,
    const sockaddr*                             sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_IFP(sa);

    if (sa->sa_family == AF_INET) {
        bdlb::NullableValue<ntsa::Ipv4Address> ipv4Address;
        error = Impl::decodeGateway(&ipv4Address,
                                    reinterpret_cast<const sockaddr_in*>(sa));
        if (error) {
            return error;
        }

        if (ipv4Address.has_value()) {
            ipAddress->makeValue().makeV4(ipv4Address.value());
        }
    }
    else if (sa->sa_family == AF_INET6) {
        bdlb::NullableValue<ntsa::Ipv6Address> ipv6Address;
        error = Impl::decodeGateway(&ipv6Address,
                                    reinterpret_cast<const sockaddr_in6*>(sa));
        if (error) {
            return error;
        }

        if (ipv6Address.has_value()) {
            ipAddress->makeValue().makeV6(ipv6Address.value());
        }
    }
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIfp(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    bdlb::NullableValue<ntsa::Ipv4Address>*     ipv4Address,
    const sockaddr*                             sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_IFP(sa);

    if (sa->sa_family == AF_INET) {
        error = Impl::decodeGateway(ipv4Address,
                                    reinterpret_cast<const sockaddr_in*>(sa));
        if (error) {
            return error;
        }
    }
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIfp(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    bdlb::NullableValue<ntsa::Ipv6Address>*     ipv6Address,
    const sockaddr*                             sa)
{
    ntsa::Error error;

    if (sa == 0) {
        return ntsa::Error();
    }

    NTSU_ROUTEUTIL_LOG_RTAX_IFP(sa);

    if (sa->sa_family == AF_INET6) {
        error = Impl::decodeGateway(ipv6Address,
                                    reinterpret_cast<const sockaddr_in6*>(sa));
        if (error) {
            return error;
        }
    }
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIfp(
    bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
    const sockaddr_in*                      sa)
{
    return Impl::decodeIpv4Address(ipv4Address, sa);
}

ntsa::Error RouteUtil::Impl::decodeIfp(
    bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
    const sockaddr_in6*                     sa)
{
    return Impl::decodeIpv6Address(ipv6Address, sa);
}

ntsa::Error RouteUtil::Impl::decodeIfp(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_dl*                          sa)
{
    return Impl::decodeLink(adapterName, adapterIndex, ethernetAddress, sa);
}

ntsa::Error RouteUtil::Impl::decodeIpv4Address(
    bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
    const sockaddr_in*                      sa)
{
    if (sa == 0) {
        return ntsa::Error();
    }

    if (sa->sin_family != AF_INET) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t minimumLengthWithAddress =
        offsetof(struct sockaddr_in, sin_addr) + sizeof sa->sin_addr;

    if (sa->sin_len < minimumLengthWithAddress) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ipv4Address->makeValue();

    const bsl::size_t bytesCopied =
        ipv4Address->value().copyFrom(&sa->sin_addr, sizeof sa->sin_addr);

    if (bytesCopied != sizeof sa->sin_addr) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIpv6Address(
    bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
    const sockaddr_in6*                     sa)
{
    if (sa == 0) {
        return ntsa::Error();
    }

    if (sa->sin6_family != AF_INET6) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t minimumLengthWithAddress =
        offsetof(struct sockaddr_in6, sin6_addr) + sizeof sa->sin6_addr;

    if (sa->sin6_len < minimumLengthWithAddress) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ipv6Address->makeValue();

    const bsl::size_t bytesCopied =
        ipv6Address->value().copyFrom(&sa->sin6_addr, sizeof sa->sin6_addr);

    if (bytesCopied != sizeof sa->sin6_addr) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t minimumLengthWithScope =
        offsetof(struct sockaddr_in6, sin6_scope_id) +
        sizeof sa->sin6_scope_id;

    if (sa->sin6_len >= minimumLengthWithScope) {
        ipv6Address->value().setScopeId(sa->sin6_scope_id);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIpv4Mask(
    bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Mask,
    const sockaddr_in*                      sa)
{
    if (sa == 0) {
        return ntsa::Error();
    }

    if (sa->sin_len == 0) {
        return ntsa::Error();
    }

    ipv4Mask->makeValue();

    const bsl::uint8_t minimumLengthWithAddress =
        offsetof(struct sockaddr_in, sin_addr) + sizeof sa->sin_addr;

    if (sa->sin_len >= minimumLengthWithAddress) {
        const bsl::size_t bytesCopied =
            ipv4Mask->value().copyFrom(&sa->sin_addr, sizeof sa->sin_addr);

        if (bytesCopied != sizeof sa->sin_addr) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        struct in_addr octets;
        NTSCFG_MEMORY_ZERO(&octets, sizeof octets);

        const bsl::size_t bytesToCopy = static_cast<bsl::size_t>(sa->sin_len) -
                                        offsetof(struct sockaddr_in, sin_addr);

        if (bytesToCopy > 0 && bytesToCopy <= sizeof(struct in_addr)) {
            NTSCFG_MEMORY_COPY(&octets, &sa->sin_addr, bytesToCopy);

            const bsl::size_t bytesCopied =
                ipv4Mask->value().copyFrom(&octets, sizeof octets);

            if (bytesCopied != sizeof octets) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIpv6Mask(
    bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Mask,
    const sockaddr_in6*                     sa)
{
    if (sa == 0) {
        return ntsa::Error();
    }

    if (sa->sin6_len == 0) {
        return ntsa::Error();
    }

    ipv6Mask->makeValue();

    const bsl::uint8_t minimumLengthWithAddress =
        offsetof(struct sockaddr_in6, sin6_addr) + sizeof sa->sin6_addr;

    if (sa->sin6_len >= minimumLengthWithAddress) {
        const bsl::size_t bytesCopied =
            ipv6Mask->value().copyFrom(&sa->sin6_addr, sizeof sa->sin6_addr);

        if (bytesCopied != sizeof sa->sin6_addr) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        struct in6_addr octets;
        NTSCFG_MEMORY_ZERO(&octets, sizeof octets);

        const bsl::size_t bytesToCopy =
            static_cast<bsl::size_t>(sa->sin6_len) -
            offsetof(struct sockaddr_in6, sin6_addr);

        if (bytesToCopy > 0 && bytesToCopy <= sizeof(struct in6_addr)) {
            NTSCFG_MEMORY_COPY(&octets, &sa->sin6_addr, bytesToCopy);

            const bsl::size_t bytesCopied =
                ipv6Mask->value().copyFrom(&octets, sizeof octets);

            if (bytesCopied != sizeof octets) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeLink(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_dl*                          sa)
{
    if (sa == 0) {
        return ntsa::Error();
    }

    if (sa->sdl_family != AF_LINK) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    adapterIndex->reset();
    adapterIndex->makeValue(static_cast<bsl::size_t>(sa->sdl_index));

    if (sa->sdl_nlen > 0) {
        adapterName->reset();
        adapterName->makeValue();
        adapterName->value().resize(static_cast<bsl::size_t>(sa->sdl_nlen));

        NTSCFG_MEMORY_COPY(&adapterName->value()[0],
                           sa->sdl_data,
                           sa->sdl_nlen);
    }

    if (sa->sdl_alen > 0 && sa->sdl_alen == 6) {
        ethernetAddress->reset();
        ethernetAddress->makeValue();

        NTSCFG_MEMORY_COPY(&ethernetAddress->value(),
                           sa->sdl_data + sa->sdl_nlen,
                           sa->sdl_alen);
    }

    return ntsa::Error();
}

bsl::string RouteUtil::Impl::describesRtaxFlags(const rt_msghdr* rtm)
{
    bsl::stringstream ss;
    ss << "[";
    if ((rtm->rtm_addrs & (1 << RTAX_DST)) != 0) {
        ss << " DST";
    }
    if ((rtm->rtm_addrs & (1 << RTAX_GATEWAY)) != 0) {
        ss << " GATEWAY";
    }
    if ((rtm->rtm_addrs & (1 << RTAX_NETMASK)) != 0) {
        ss << " NETMASK";
    }
    if ((rtm->rtm_addrs & (1 << RTAX_GENMASK)) != 0) {
        ss << " GENMASK";
    }
    if ((rtm->rtm_addrs & (1 << RTAX_IFP)) != 0) {
        ss << " IFP";
    }
    if ((rtm->rtm_addrs & (1 << RTAX_IFA)) != 0) {
        ss << " IFA";
    }
    if ((rtm->rtm_addrs & (1 << RTAX_AUTHOR)) != 0) {
        ss << " AUTHOR";
    }
    if ((rtm->rtm_addrs & (1 << RTAX_BRD)) != 0) {
        ss << " BRD";
    }
    ss << " ]";

    return ss.str();
}

ntsa::Error RouteUtil::load(ntsa::EthernetRouteTable* result)
{
    bsl::vector<ntsa::Adapter> adapterVector;
    ntsu::AdapterUtil::discoverAdapterList(&adapterVector);

    return RouteUtil::load(result, adapterVector);
}

ntsa::Error RouteUtil::load(ntsa::EthernetRouteTable*         result,
                            const bsl::vector<ntsa::Adapter>& adapterVector)
{
    ntsa::Error error;
    int         rc;

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    result->reset();

    int mib[6];

    mib[0] = CTL_NET;
    mib[1] = PF_ROUTE;
    mib[2] = 0;
    mib[3] = AF_UNSPEC;
    mib[4] = NET_RT_FLAGS;
    mib[5] = RTF_LLINFO;

    size_t bufferLength;
    rc = ::sysctl(mib, 6, 0, &bufferLength, 0, 0);

    if (rc < 0) {
        error = ntsa::Error::last();
        NTSU_ROUTEUTIL_LOG_ERROR_SYSCTL(error);
        return error;
    }

    bsl::uint8_t* buffer = reinterpret_cast<bsl::uint8_t*>(
        allocator->allocate(static_cast<bsl::size_t>(bufferLength)));

    const bsl::uint8_t* bufferEnd = buffer + bufferLength;

    bslma::DeallocatorGuard<bslma::Allocator> bufferGuard(buffer, allocator);

    rc = ::sysctl(mib, 6, buffer, &bufferLength, 0, 0);
    if (rc < 0) {
        error = ntsa::Error::last();
        NTSU_ROUTEUTIL_LOG_ERROR_SYSCTL(error);
        return error;
    }

    const bsl::uint8_t* current = buffer;

    while (current < bufferEnd) {
        const rt_msghdr* rtm = 0;
        error = Impl::decodeRouteHeader(&rtm, &current, bufferEnd);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                break;
            }
            else {
                NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_HEADER(error);
                return error;
            }
        }

        NTSU_ROUTEUTIL_LOG_ROUTE_MESSAGE(rtm);

        const sockaddr* rti_info[RTAX_MAX];
        NTSCFG_MEMORY_ZERO(rti_info, sizeof rti_info);

        error = Impl::decodeRoutePayload(rti_info, rtm);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_PAYLOAD(error);
            return error;
        }

        bdlb::NullableValue<ntsa::IpAddress>       destinationIpAddress;
        bdlb::NullableValue<bsl::string>           gatewayAdapterName;
        bdlb::NullableValue<bsl::uint32_t>         gatewayAdapterIndex;
        bdlb::NullableValue<ntsa::EthernetAddress> gatewayEthernetAddress;
        bdlb::NullableValue<ntsa::IpAddress>       gatewayIpAddress;
        bdlb::NullableValue<bsl::string>           interfaceAdapterName;
        bdlb::NullableValue<bsl::uint32_t>         interfaceAdapterIndex;
        bdlb::NullableValue<ntsa::EthernetAddress> interfaceEthernetAddress;
        bdlb::NullableValue<ntsa::IpAddress>       interfaceIpAddress;

        error =
            Impl::decodeDestination(&destinationIpAddress, rti_info[RTAX_DST]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_DST(rti_info[RTAX_DST],
                                                     error);
            return error;
        }

        error = Impl::decodeGateway(&gatewayAdapterName,
                                    &gatewayAdapterIndex,
                                    &gatewayEthernetAddress,
                                    &gatewayIpAddress,
                                    rti_info[RTAX_GATEWAY]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_GATEWAY(
                rti_info[RTAX_GATEWAY],
                error);
            return error;
        }

        error = Impl::decodeIfa(&interfaceAdapterName,
                                &interfaceAdapterIndex,
                                &interfaceEthernetAddress,
                                &interfaceIpAddress,
                                rti_info[RTAX_IFA]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_IFA(rti_info[RTAX_IFA],
                                                     error);
            return error;
        }

        error = Impl::decodeIfp(&interfaceAdapterName,
                                &interfaceAdapterIndex,
                                &interfaceEthernetAddress,
                                &interfaceIpAddress,
                                rti_info[RTAX_IFP]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_IFP(rti_info[RTAX_IFP],
                                                     error);
            return error;
        }

        ntsa::EthernetRoute route;

        if (destinationIpAddress.has_value()) {
            if (destinationIpAddress.value().isV4()) {
                route.setIpv4Address(destinationIpAddress.value().v4());
            }
            else if (destinationIpAddress.value().isV6()) {
                route.setIpv6Address(destinationIpAddress.value().v6());
            }
        }
        else {
            continue;
        }

        if (gatewayEthernetAddress.has_value()) {
            route.setEthernetAddress(gatewayEthernetAddress.value());
        }
        else {
            continue;
        }

        result->add(route);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::load(ntsa::Ipv4RouteTable* result)
{
    ntsa::Error error;

    bsl::vector<ntsa::Adapter> adapterVector;
    ntsu::AdapterUtil::discoverAdapterList(&adapterVector);

    ntsa::EthernetRouteTable ethernetRouteTable;
    error = RouteUtil::load(&ethernetRouteTable);
    if (error) {
        return error;
    }

    return RouteUtil::load(result, adapterVector, ethernetRouteTable);
}

ntsa::Error RouteUtil::load(ntsa::Ipv4RouteTable*             result,
                            const bsl::vector<ntsa::Adapter>& adapterVector,
                            const ntsa::EthernetRouteTable& ethernetRouteTable)
{
    ntsa::Error error;
    int         rc;

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    result->reset();

    int mib[6];

    mib[0] = CTL_NET;
    mib[1] = PF_ROUTE;
    mib[2] = 0;
    mib[3] = AF_INET;
    mib[4] = NET_RT_DUMP;
    mib[5] = 0;

    size_t bufferLength;
    rc = ::sysctl(mib, 6, 0, &bufferLength, 0, 0);

    if (rc < 0) {
        error = ntsa::Error::last();
        NTSU_ROUTEUTIL_LOG_ERROR_SYSCTL(error);
        return error;
    }

    bsl::uint8_t* buffer = reinterpret_cast<bsl::uint8_t*>(
        allocator->allocate(static_cast<bsl::size_t>(bufferLength)));

    const bsl::uint8_t* bufferEnd = buffer + bufferLength;

    bslma::DeallocatorGuard<bslma::Allocator> bufferGuard(buffer, allocator);

    rc = ::sysctl(mib, 6, buffer, &bufferLength, 0, 0);
    if (rc < 0) {
        error = ntsa::Error::last();
        NTSU_ROUTEUTIL_LOG_ERROR_SYSCTL(error);
        return error;
    }

    const bsl::uint8_t* current = buffer;

    while (current < bufferEnd) {
        const rt_msghdr* rtm = 0;
        error = Impl::decodeRouteHeader(&rtm, &current, bufferEnd);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                break;
            }
            else {
                NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_HEADER(error);
                return error;
            }
        }

        NTSU_ROUTEUTIL_LOG_ROUTE_MESSAGE(rtm);

        const sockaddr* rti_info[RTAX_MAX];
        NTSCFG_MEMORY_ZERO(rti_info, sizeof rti_info);

        error = Impl::decodeRoutePayload(rti_info, rtm);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_PAYLOAD(error);
            return error;
        }

        bdlb::NullableValue<ntsa::Ipv4Address>     destinationIpv4Address;
        bdlb::NullableValue<ntsa::Ipv4Address>     destinationIpv4Mask;
        bdlb::NullableValue<bsl::string>           gatewayAdapterName;
        bdlb::NullableValue<bsl::uint32_t>         gatewayAdapterIndex;
        bdlb::NullableValue<ntsa::EthernetAddress> gatewayEthernetAddress;
        bdlb::NullableValue<ntsa::Ipv4Address>     gatewayIpv4Address;
        bdlb::NullableValue<bsl::string>           interfaceAdapterName;
        bdlb::NullableValue<bsl::uint32_t>         interfaceAdapterIndex;
        bdlb::NullableValue<ntsa::EthernetAddress> interfaceEthernetAddress;
        bdlb::NullableValue<ntsa::Ipv4Address>     interfaceIpv4Address;

        error = Impl::decodeDestination(&destinationIpv4Address,
                                        rti_info[RTAX_DST]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_DST(rti_info[RTAX_DST],
                                                     error);
            return error;
        }

        error =
            Impl::decodeNetMask(&destinationIpv4Mask, rti_info[RTAX_NETMASK]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_NETMASK(rti_info[RTAX_NETMASK],
                                                     error);
            return error;
        }

        error = Impl::decodeGateway(&gatewayAdapterName,
                                    &gatewayAdapterIndex,
                                    &gatewayEthernetAddress,
                                    &gatewayIpv4Address,
                                    rti_info[RTAX_GATEWAY]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_GATEWAY(
                rti_info[RTAX_GATEWAY],
                error);
            return error;
        }

        error = Impl::decodeIfa(&interfaceAdapterName,
                                &interfaceAdapterIndex,
                                &interfaceEthernetAddress,
                                &interfaceIpv4Address,
                                rti_info[RTAX_IFA]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_IFA(rti_info[RTAX_IFA],
                                                     error);
            return error;
        }

        error = Impl::decodeIfp(&interfaceAdapterName,
                                &interfaceAdapterIndex,
                                &interfaceEthernetAddress,
                                &interfaceIpv4Address,
                                rti_info[RTAX_IFP]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_IFP(rti_info[RTAX_IFP],
                                                     error);
            return error;
        }

        ntsa::Ipv4Route route;

        if (destinationIpv4Address.has_value()) {
            route.setDestinationIpv4Address(destinationIpv4Address.value());
        }

        if (destinationIpv4Mask.has_value()) {
            route.setDestinationIpv4Mask(destinationIpv4Mask.value());
        }

        if (gatewayAdapterName.has_value()) {
            route.setGatewayAdapterName(gatewayAdapterName.value());
        }

        if (gatewayAdapterIndex.has_value()) {
            route.setGatewayAdapterIndex(gatewayAdapterIndex.value());
        }

        if (gatewayEthernetAddress.has_value()) {
            route.setGatewayEthernetAddress(gatewayEthernetAddress.value());
        }

        if (gatewayIpv4Address.has_value()) {
            route.setGatewayIpv4Address(gatewayIpv4Address.value());
        }

        if (interfaceAdapterName.has_value()) {
            route.setInterfaceAdapterName(interfaceAdapterName.value());
        }

        if (interfaceAdapterIndex.has_value()) {
            route.setInterfaceAdapterIndex(interfaceAdapterIndex.value());
        }

        if (interfaceEthernetAddress.has_value()) {
            route.setInterfaceEthernetAddress(
                interfaceEthernetAddress.value());
        }

        if (interfaceIpv4Address.has_value()) {
            route.setInterfaceIpv4Address(interfaceIpv4Address.value());
        }

        if (route.gatewayIpv4Address().isNull()) {
            const ntsa::Adapter* gatewayAdapter = 0;

            if (gatewayAdapter == 0) {
                if (route.gatewayAdapterName().has_value()) {
                    for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                        if (adapterVector[i].name() ==
                            route.gatewayAdapterName().value())
                        {
                            gatewayAdapter = &adapterVector[i];
                            break;
                        }
                    }
                }
            }

            if (gatewayAdapter == 0) {
                if (route.gatewayAdapterIndex().has_value()) {
                    for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                        if (adapterVector[i].index() ==
                            route.gatewayAdapterIndex().value())
                        {
                            gatewayAdapter = &adapterVector[i];
                            break;
                        }
                    }
                }
            }

            if (gatewayAdapter == 0) {
                if (route.gatewayEthernetAddress().has_value()) {
                    for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                        ntsa::EthernetAddress ethernetAddress;
                        if (!ethernetAddress.parse(
                                adapterVector[i].ethernetAddress()))
                        {
                            continue;
                        }

                        if (ethernetAddress ==
                            route.gatewayEthernetAddress().value())
                        {
                            gatewayAdapter = &adapterVector[i];
                            break;
                        }
                    }
                }
            }

            if (gatewayAdapter != 0) {
                if (route.gatewayAdapterName().has_value()) {
                    // TODO: compare
                }
                else {
                    route.setGatewayAdapterName(gatewayAdapter->name());
                }

                if (route.gatewayAdapterIndex().has_value()) {
                    // TODO: compare
                }
                else {
                    route.setGatewayAdapterIndex(gatewayAdapter->index());
                }

                if (route.gatewayEthernetAddress().has_value()) {
                    // TODO: compare
                }
                else {
                    route.setGatewayEthernetAddress(ntsa::EthernetAddress(
                        gatewayAdapter->ethernetAddress()));
                }
            }
        }

        const ntsa::Adapter* interfaceAdapter = 0;

        if (interfaceAdapter == 0) {
            if (route.interfaceAdapterName().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].name() ==
                        route.interfaceAdapterName().value())
                    {
                        interfaceAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (interfaceAdapter == 0) {
            if (route.interfaceAdapterIndex().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].index() ==
                        route.interfaceAdapterIndex().value())
                    {
                        interfaceAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (interfaceAdapter == 0) {
            if (route.interfaceEthernetAddress().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    ntsa::EthernetAddress ethernetAddress;
                    if (!ethernetAddress.parse(
                            adapterVector[i].ethernetAddress()))
                    {
                        continue;
                    }

                    if (ethernetAddress ==
                        route.interfaceEthernetAddress().value())
                    {
                        interfaceAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (interfaceAdapter == 0) {
            if (route.interfaceIpv4Address().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].ipv4Address().has_value() &&
                        adapterVector[i].ipv4Address().value() ==
                            route.interfaceIpv4Address().value())
                    {
                        interfaceAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (interfaceAdapter != 0) {
            if (route.interfaceAdapterName().has_value()) {
                // TODO: compare
            }
            else {
                route.setInterfaceAdapterName(interfaceAdapter->name());
            }

            if (route.interfaceAdapterIndex().has_value()) {
                // TODO: compare
            }
            else {
                route.setInterfaceAdapterIndex(interfaceAdapter->index());
            }

            if (route.interfaceEthernetAddress().has_value()) {
                // TODO: compare
            }
            else {
                route.setInterfaceEthernetAddress(ntsa::EthernetAddress(
                    interfaceAdapter->ethernetAddress()));
            }

            if (route.interfaceIpv4Address().has_value()) {
                // TODO: compare
            }
            else if (interfaceAdapter->ipv4Address().has_value()) {
                route.setInterfaceIpv4Address(
                    interfaceAdapter->ipv4Address().value());
            }
        }

        if (route.gatewayEthernetAddress().isNull()) {
            if (route.gatewayIpv4Address().has_value()) {
                if (route.gatewayIpv4Address().value().isLoopback()) {
                    route.setGatewayEthernetAddress(ntsa::EthernetAddress());
                }
                else {
                    ntsa::EthernetAddress ethernetAddress;
                    if (ethernetRouteTable.find(
                            &ethernetAddress,
                            route.gatewayIpv4Address().value()))
                    {
                        route.setGatewayEthernetAddress(ethernetAddress);
                    }
                }
            }
        }

        result->add(route);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::load(ntsa::Ipv6RouteTable* result)
{
    ntsa::Error error;

    bsl::vector<ntsa::Adapter> adapterVector;
    ntsu::AdapterUtil::discoverAdapterList(&adapterVector);

    ntsa::EthernetRouteTable ethernetRouteTable;
    error = RouteUtil::load(&ethernetRouteTable);
    if (error) {
        return error;
    }

    return RouteUtil::load(result, adapterVector, ethernetRouteTable);
}

ntsa::Error RouteUtil::load(ntsa::Ipv6RouteTable*             result,
                            const bsl::vector<ntsa::Adapter>& adapterVector,
                            const ntsa::EthernetRouteTable& ethernetRouteTable)
{
    ntsa::Error error;
    int         rc;

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    result->reset();

    int mib[6];

    mib[0] = CTL_NET;
    mib[1] = PF_ROUTE;
    mib[2] = 0;
    mib[3] = AF_INET6;
    mib[4] = NET_RT_DUMP;
    mib[5] = 0;

    size_t bufferLength;
    rc = ::sysctl(mib, 6, 0, &bufferLength, 0, 0);

    if (rc < 0) {
        error = ntsa::Error::last();
        NTSU_ROUTEUTIL_LOG_ERROR_SYSCTL(error);
        return error;
    }

    bsl::uint8_t* buffer = reinterpret_cast<bsl::uint8_t*>(
        allocator->allocate(static_cast<bsl::size_t>(bufferLength)));

    const bsl::uint8_t* bufferEnd = buffer + bufferLength;

    bslma::DeallocatorGuard<bslma::Allocator> bufferGuard(buffer, allocator);

    rc = ::sysctl(mib, 6, buffer, &bufferLength, 0, 0);
    if (rc < 0) {
        error = ntsa::Error::last();
        NTSU_ROUTEUTIL_LOG_ERROR_SYSCTL(error);
        return error;
    }

    const bsl::uint8_t* current = buffer;

    while (current < bufferEnd) {
        const rt_msghdr* rtm = 0;
        error = Impl::decodeRouteHeader(&rtm, &current, bufferEnd);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                break;
            }
            else {
                NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_HEADER(error);
                return error;
            }
        }

        NTSU_ROUTEUTIL_LOG_ROUTE_MESSAGE(rtm);

        const sockaddr* rti_info[RTAX_MAX];
        NTSCFG_MEMORY_ZERO(rti_info, sizeof rti_info);

        error = Impl::decodeRoutePayload(rti_info, rtm);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_PAYLOAD(error);
            return error;
        }

        bdlb::NullableValue<ntsa::Ipv6Address>     destinationIpv6Address;
        bdlb::NullableValue<ntsa::Ipv6Address>     destinationIpv6Mask;
        bdlb::NullableValue<bsl::string>           gatewayAdapterName;
        bdlb::NullableValue<bsl::uint32_t>         gatewayAdapterIndex;
        bdlb::NullableValue<ntsa::EthernetAddress> gatewayEthernetAddress;
        bdlb::NullableValue<ntsa::Ipv6Address>     gatewayIpv6Address;
        bdlb::NullableValue<bsl::string>           interfaceAdapterName;
        bdlb::NullableValue<bsl::uint32_t>         interfaceAdapterIndex;
        bdlb::NullableValue<ntsa::EthernetAddress> interfaceEthernetAddress;
        bdlb::NullableValue<ntsa::Ipv6Address>     interfaceIpv6Address;

        error = Impl::decodeDestination(&destinationIpv6Address,
                                        rti_info[RTAX_DST]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_DST(rti_info[RTAX_DST],
                                                     error);
            return error;
        }

        error =
            Impl::decodeNetMask(&destinationIpv6Mask, rti_info[RTAX_NETMASK]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_NETMASK(rti_info[RTAX_NETMASK],
                                                     error);
            return error;
        }

        error = Impl::decodeGateway(&gatewayAdapterName,
                                    &gatewayAdapterIndex,
                                    &gatewayEthernetAddress,
                                    &gatewayIpv6Address,
                                    rti_info[RTAX_GATEWAY]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_GATEWAY(
                rti_info[RTAX_GATEWAY],
                error);
            return error;
        }

        error = Impl::decodeIfa(&interfaceAdapterName,
                                &interfaceAdapterIndex,
                                &interfaceEthernetAddress,
                                &interfaceIpv6Address,
                                rti_info[RTAX_IFA]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_IFA(rti_info[RTAX_IFA],
                                                     error);
            return error;
        }

        error = Impl::decodeIfp(&interfaceAdapterName,
                                &interfaceAdapterIndex,
                                &interfaceEthernetAddress,
                                &interfaceIpv6Address,
                                rti_info[RTAX_IFP]);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_IFP(rti_info[RTAX_IFP],
                                                     error);
            return error;
        }

        ntsa::Ipv6Route route;

        if (destinationIpv6Address.has_value()) {
            route.setDestinationIpv6Address(destinationIpv6Address.value());
        }

        if (destinationIpv6Mask.has_value()) {
            route.setDestinationIpv6Mask(destinationIpv6Mask.value());
        }

        if (gatewayAdapterName.has_value()) {
            route.setGatewayAdapterName(gatewayAdapterName.value());
        }

        if (gatewayAdapterIndex.has_value()) {
            route.setGatewayAdapterIndex(gatewayAdapterIndex.value());
        }

        if (gatewayEthernetAddress.has_value()) {
            route.setGatewayEthernetAddress(gatewayEthernetAddress.value());
        }

        if (gatewayIpv6Address.has_value()) {
            route.setGatewayIpv6Address(gatewayIpv6Address.value());
        }

        if (interfaceAdapterName.has_value()) {
            route.setInterfaceAdapterName(interfaceAdapterName.value());
        }

        if (interfaceAdapterIndex.has_value()) {
            route.setInterfaceAdapterIndex(interfaceAdapterIndex.value());
        }

        if (interfaceEthernetAddress.has_value()) {
            route.setInterfaceEthernetAddress(
                interfaceEthernetAddress.value());
        }

        if (interfaceIpv6Address.has_value()) {
            route.setInterfaceIpv6Address(interfaceIpv6Address.value());
        }

        if (route.gatewayIpv6Address().isNull()) {
            const ntsa::Adapter* gatewayAdapter = 0;

            if (gatewayAdapter == 0) {
                if (route.gatewayAdapterName().has_value()) {
                    for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                        if (adapterVector[i].name() ==
                            route.gatewayAdapterName().value())
                        {
                            gatewayAdapter = &adapterVector[i];
                            break;
                        }
                    }
                }
            }

            if (gatewayAdapter == 0) {
                if (route.gatewayAdapterIndex().has_value()) {
                    for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                        if (adapterVector[i].index() ==
                            route.gatewayAdapterIndex().value())
                        {
                            gatewayAdapter = &adapterVector[i];
                            break;
                        }
                    }
                }
            }

            if (gatewayAdapter == 0) {
                if (route.gatewayEthernetAddress().has_value()) {
                    for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                        ntsa::EthernetAddress ethernetAddress;
                        if (!ethernetAddress.parse(
                                adapterVector[i].ethernetAddress()))
                        {
                            continue;
                        }

                        if (ethernetAddress ==
                            route.gatewayEthernetAddress().value())
                        {
                            gatewayAdapter = &adapterVector[i];
                            break;
                        }
                    }
                }
            }

            if (gatewayAdapter != 0) {
                if (route.gatewayAdapterName().has_value()) {
                    // TODO: compare
                }
                else {
                    route.setGatewayAdapterName(gatewayAdapter->name());
                }

                if (route.gatewayAdapterIndex().has_value()) {
                    // TODO: compare
                }
                else {
                    route.setGatewayAdapterIndex(gatewayAdapter->index());
                }

                if (route.gatewayEthernetAddress().has_value()) {
                    // TODO: compare
                }
                else {
                    route.setGatewayEthernetAddress(ntsa::EthernetAddress(
                        gatewayAdapter->ethernetAddress()));
                }
            }
        }

        const ntsa::Adapter* interfaceAdapter = 0;

        if (interfaceAdapter == 0) {
            if (route.interfaceAdapterName().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].name() ==
                        route.interfaceAdapterName().value())
                    {
                        interfaceAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (interfaceAdapter == 0) {
            if (route.interfaceAdapterIndex().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].index() ==
                        route.interfaceAdapterIndex().value())
                    {
                        interfaceAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (interfaceAdapter == 0) {
            if (route.interfaceEthernetAddress().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    ntsa::EthernetAddress ethernetAddress;
                    if (!ethernetAddress.parse(
                            adapterVector[i].ethernetAddress()))
                    {
                        continue;
                    }

                    if (ethernetAddress ==
                        route.interfaceEthernetAddress().value())
                    {
                        interfaceAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (interfaceAdapter == 0) {
            if (route.interfaceIpv6Address().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].ipv6Address().has_value() &&
                        adapterVector[i].ipv6Address().value() ==
                            route.interfaceIpv6Address().value())
                    {
                        interfaceAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (interfaceAdapter != 0) {
            if (route.interfaceAdapterName().has_value()) {
                // TODO: compare
            }
            else {
                route.setInterfaceAdapterName(interfaceAdapter->name());
            }

            if (route.interfaceAdapterIndex().has_value()) {
                // TODO: compare
            }
            else {
                route.setInterfaceAdapterIndex(interfaceAdapter->index());
            }

            if (route.interfaceEthernetAddress().has_value()) {
                // TODO: compare
            }
            else {
                route.setInterfaceEthernetAddress(ntsa::EthernetAddress(
                    interfaceAdapter->ethernetAddress()));
            }

            if (route.interfaceIpv6Address().has_value()) {
                // TODO: compare
            }
            else if (interfaceAdapter->ipv6Address().has_value()) {
                route.setInterfaceIpv6Address(
                    interfaceAdapter->ipv6Address().value());
            }
        }

        if (route.gatewayEthernetAddress().isNull()) {
            if (route.gatewayIpv6Address().has_value()) {
                if (route.gatewayIpv6Address().value().isLoopback()) {
                    route.setGatewayEthernetAddress(ntsa::EthernetAddress());
                }
                else {
                    ntsa::EthernetAddress ethernetAddress;
                    if (ethernetRouteTable.find(
                            &ethernetAddress,
                            route.gatewayIpv6Address().value()))
                    {
                        route.setGatewayEthernetAddress(ethernetAddress);
                    }
                }
            }
        }

        result->add(route);
    }

    return ntsa::Error();
}

#else

ntsa::Error RouteUtil::load(ntsa::EthernetRouteTable* result)
{
    result->reset();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error RouteUtil::load(ntsa::EthernetRouteTable*         result,
                            const bsl::vector<ntsa::Adapter>& adapterVector)
{
    result->reset();

    NTSCFG_WARNING_UNUSED(adapterVector);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error RouteUtil::load(ntsa::Ipv4RouteTable* result)
{
    result->reset();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error RouteUtil::load(ntsa::Ipv4RouteTable*             result,
                            const bsl::vector<ntsa::Adapter>& adapterVector,
                            const ntsa::EthernetRouteTable& ethernetRouteTable)
{
    result->reset();

    NTSCFG_WARNING_UNUSED(adapterVector);
    NTSCFG_WARNING_UNUSED(ethernetRouteTable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error RouteUtil::load(ntsa::Ipv6RouteTable* result)
{
    result->reset();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error RouteUtil::load(ntsa::Ipv6RouteTable*             result,
                            const bsl::vector<ntsa::Adapter>& adapterVector,
                            const ntsa::EthernetRouteTable& ethernetRouteTable)
{
    result->reset();

    NTSCFG_WARNING_UNUSED(adapterVector);
    NTSCFG_WARNING_UNUSED(ethernetRouteTable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

#endif

}  // close package namespace
}  // close enterprise namespace
