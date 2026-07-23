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
#include <bsl_fstream.h>
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
#include <linux/rtnetlink.h>
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

#define NTSU_ROUTEUTIL_LOG_ERROR_SYSCTL(error)                                \
    do {                                                                      \
        BALL_LOG_ERROR << "Failed to get routing table: " << (error)          \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_HEADER(error)                   \
    do {                                                                      \
        BALL_LOG_ERROR << "Failed to decode "                                 \
                       << "route message header: " << (error)                 \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_PAYLOAD(error)                  \
    do {                                                                      \
        BALL_LOG_ERROR << "Failed to decode "                                 \
                       << "route message payload: " << (error)                \
                       << BALL_LOG_END;                                       \
    } while (false)

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX(type, sa, error)                 \
    do {                                                                      \
        BALL_LOG_ERROR << "Failed to decode route message payload [ type = "  \
                       << (type) << " sa_family = "                           \
                       << static_cast<bsl::uint32_t>((sa)->sa_family)         \
                       << " sa_len = "                                        \
                       << static_cast<bsl::uint32_t>((sa)->sa_len)            \
                       << " ]: " << (error) << BALL_LOG_END;                  \
    } while (false)

#else

#define NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX(type, sa, error)                 \
    do {                                                                      \
        BALL_LOG_ERROR << "Failed to decode route message payload [ type = "  \
                       << (type) << " sa_family = "                           \
                       << static_cast<bsl::uint32_t>((sa)->sa_family)         \
                       << " ]: " << (error) << BALL_LOG_END;                  \
    } while (false)

#endif

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

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

#define NTSU_ROUTEUTIL_LOG_RTAX(type, sa)                                     \
    do {                                                                      \
        BALL_LOG_TRACE << "Decoding route message payload [ type = "          \
                       << (type) << " sa_family = "                           \
                       << static_cast<bsl::uint32_t>((sa)->sa_family)         \
                       << " sa_len = "                                        \
                       << static_cast<bsl::uint32_t>((sa)->sa_len) << " ]"    \
                       << BALL_LOG_END;                                       \
    } while (false)

#else

#define NTSU_ROUTEUTIL_LOG_RTAX(type, sa)                                     \
    do {                                                                      \
        BALL_LOG_TRACE << "Decoding route message payload [ type = "          \
                       << (type) << " sa_family = "                           \
                       << static_cast<bsl::uint32_t>((sa)->sa_family)         \
                       << BALL_LOG_END;                                       \
    } while (false)

#endif

#define NTSU_ROUTEUTIL_LOG_RTAX_DST(sa) NTSU_ROUTEUTIL_LOG_RTAX("RTAX_DST", sa)

#define NTSU_ROUTEUTIL_LOG_RTAX_NETMASK(sa)                                   \
    NTSU_ROUTEUTIL_LOG_RTAX("RTAX_NETMASK", sa)

#define NTSU_ROUTEUTIL_LOG_RTAX_GATEWAY(sa)                                   \
    NTSU_ROUTEUTIL_LOG_RTAX("RTAX_GATEWAY", sa)

#define NTSU_ROUTEUTIL_LOG_RTAX_IFA(sa) NTSU_ROUTEUTIL_LOG_RTAX("RTAX_IFA", sa)

#define NTSU_ROUTEUTIL_LOG_RTAX_IFP(sa) NTSU_ROUTEUTIL_LOG_RTAX("RTAX_IFP", sa)

namespace BloombergLP {
namespace ntsu {

#if defined(BSLS_PLATFORM_OS_UNIX)

/// Provide a private, platform-specific implementation of utilities for
/// discovering route tables.
class RouteUtil::Impl
{
    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.ROUTEUTIL");

  public:
    /// Provide a private, platform-specific implementation of utilities for
    /// discovering route tables native to the operating system.
    class Native;

    /// Defines a type alias for a nullable Ethernet address.
    typedef bdlb::NullableValue<ntsa::EthernetAddress> NullableEthernetAddress;

    /// Defines a type alias for a nullable IP address.
    typedef bdlb::NullableValue<ntsa::IpAddress> NullableIpAddress;

    /// Defines a type alias for a nullable IPv4 address.
    typedef bdlb::NullableValue<ntsa::Ipv4Address> NullableIpv4Address;

    /// Defines a type alias for a nullable IPv6 address.
    typedef bdlb::NullableValue<ntsa::Ipv6Address> NullableIpv6Address;

    /// Defines a type alias for a nullable string.
    typedef bdlb::NullableValue<bsl::string> NullableString;

    /// Defines a type alias for a nullable 32-bit unsigned integer.
    typedef bdlb::NullableValue<bsl::uint32_t> NullableUint32;

    /// Decode the specified 'sa' socket address as a route destination.
    /// Load the IP address into the specified 'ipAddress'. Return the error.
    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::IpAddress>* ipAddress,
        const sockaddr*                       sa);

    /// Decode the specified 'sa' socket address as a route destination.
    /// Load the IPv4 address into the specified 'ipv4Address'. Return the
    /// error.
    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr*                         sa);

    /// Decode the specified 'sa' socket address as a route destination.
    /// Load the IPv6 address into the specified 'ipv6Address'. Return the
    /// error.
    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr*                         sa);

    /// Decode the specified 'sa' socket address as a route destination.
    /// Load the IPv4 address into the specified 'ipv4Address'. Return the
    /// error.
    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    /// Decode the specified 'sa' socket address as a route destination.
    /// Load the IPv6 address into the specified 'ipv6Address'. Return the
    /// error.
    static ntsa::Error decodeDestination(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr_in6*                     sa);

    /// Decode the specified 'sa' socket address as a network mask. Load the
    /// IPv4 network mask into the specified 'ipv4Mask'. Return the error.
    static ntsa::Error decodeNetMask(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Mask,
        const sockaddr*                         sa);

    /// Decode the specified 'sa' socket address as a network mask. Load the
    /// IPv6 network mask into the specified 'ip64Mask'. Return the error.
    static ntsa::Error decodeNetMask(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Mask,
        const sockaddr*                         sa);

    /// Decode the specified 'sa' socket address as a route gateway. Load the
    /// adapter into the specified 'adapterName' and 'adapterIndex'. Load the
    /// Ethernet address into the specified 'ethernetAddress'. Load the IP
    /// address into the specified 'ipAddress'. Return the error.
    static ntsa::Error decodeGateway(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::IpAddress>*       ipAddress,
        const sockaddr*                             sa);

    /// Decode the specified 'sa' socket address as a route gateway. Load the
    /// adapter into the specified 'adapterName' and 'adapterIndex'. Load the
    /// Ethernet address into the specified 'ethernetAddress'. Load the IPv4
    /// address into the specified 'ipv4Address'. Return the error.
    static ntsa::Error decodeGateway(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv4Address>*     ipv4Address,
        const sockaddr*                             sa);

    /// Decode the specified 'sa' socket address as a route gateway. Load the
    /// adapter into the specified 'adapterName' and 'adapterIndex'. Load the
    /// Ethernet address into the specified 'ethernetAddress'. Load the IPv6
    /// address into the specified 'ipv6Address'. Return the error.
    static ntsa::Error decodeGateway(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv6Address>*     ipv6Address,
        const sockaddr*                             sa);

    /// Decode the specified 'sa' socket address as a route gateway. Load the
    /// IPv4 address into the specified 'ipv4Address'. Return the error.
    static ntsa::Error decodeGateway(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    /// Decode the specified 'sa' socket address as a route gateway. Load the
    /// IPv6 address into the specified 'ipv6Address'. Return the error.
    static ntsa::Error decodeGateway(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr_in6*                     sa);

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

    /// Decode the specified 'sa' socket address as a route gateway. Load the
    /// adapter into the specified 'adapterName' and 'adapterIndex'. Load the
    /// Ethernet address into the specified 'ethernetAddress'. Return the
    /// error.
    static ntsa::Error decodeGateway(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_dl*                          sa);

#elif defined(BSLS_PLATFORM_OS_LINUX)

    /// Decode the specified 'sa' socket address as a route gateway. Load the
    /// adapter into the specified 'adapterName' and 'adapterIndex'. Load the
    /// Ethernet address into the specified 'ethernetAddress'. Return the
    /// error.
    static ntsa::Error decodeGateway(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_ll*                          sa);

#endif

    /// Decode the specified 'sa' socket address as a route interface name.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Load
    /// the IP address into the specified 'ipAddress'. Return the error.
    static ntsa::Error decodeIfa(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::IpAddress>*       ipAddress,
        const sockaddr*                             sa);

    /// Decode the specified 'sa' socket address as a route interface name.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Load
    /// the IPv4 address into the specified 'ipv4Address'. Return the error.
    static ntsa::Error decodeIfa(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv4Address>*     ipv4Address,
        const sockaddr*                             sa);

    /// Decode the specified 'sa' socket address as a route interface name.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Load
    /// the IPv6 address into the specified 'ipv6Address'. Return the error.
    static ntsa::Error decodeIfa(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv6Address>*     ipv6Address,
        const sockaddr*                             sa);

    /// Decode the specified 'sa' socket address as a route interface name.
    /// Load the IPv4 address into the specified 'ipv4Address'. Return the
    /// error.
    static ntsa::Error decodeIfa(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    /// Decode the specified 'sa' socket address as a route interface name.
    /// Load the IPv6 address into the specified 'ipv6Address'. Return the
    /// error.
    static ntsa::Error decodeIfa(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr_in6*                     sa);

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

    /// Decode the specified 'sa' socket address as a route interface name.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Return
    /// the error.
    static ntsa::Error decodeIfa(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_dl*                          sa);

#elif defined(BSLS_PLATFORM_OS_LINUX)

    /// Decode the specified 'sa' socket address as a route interface name.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Return
    /// the error.
    static ntsa::Error decodeIfa(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_ll*                          sa);

#endif

    /// Decode the specified 'sa' socket address as a route interface address.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Load
    /// the IP address into the specified 'ipAddress'. Return the error.
    static ntsa::Error decodeIfp(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::IpAddress>*       ipAddress,
        const sockaddr*                             sa);

    /// Decode the specified 'sa' socket address as a route interface address.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Load
    /// the IPv4 address into the specified 'ipv4Address'. Return the error.
    static ntsa::Error decodeIfp(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv4Address>*     ipv4Address,
        const sockaddr*                             sa);

    /// Decode the specified 'sa' socket address as a route interface address.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Load
    /// the IPv6 address into the specified 'ipv6Address'. Return the error.
    static ntsa::Error decodeIfp(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        bdlb::NullableValue<ntsa::Ipv6Address>*     ipv6Address,
        const sockaddr*                             sa);

    /// Decode the specified 'sa' socket address as a route interface address.
    /// Load the IPv4 address into the specified 'ipv4Address'. Return the
    /// error.
    static ntsa::Error decodeIfp(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    /// Decode the specified 'sa' socket address as a route interface address.
    /// Load the IPv6 address into the specified 'ipv6Address'. Return the
    /// error.
    static ntsa::Error decodeIfp(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Address,
        const sockaddr_in6*                     sa);

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

    /// Decode the specified 'sa' socket address as a route interface address.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Return
    /// the error.
    static ntsa::Error decodeIfp(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_dl*                          sa);

#elif defined(BSLS_PLATFORM_OS_LINUX)

    /// Decode the specified 'sa' socket address as a route interface address.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Return
    /// the error.
    static ntsa::Error decodeIfp(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_ll*                          sa);

#endif

    /// Decode the specified 'sa' socket address as an IPv4 address. Load the
    /// IPv4 address into the specified 'ipv4address'. Return the error.
    static ntsa::Error decodeIpv4Address(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Address,
        const sockaddr_in*                      sa);

    /// Decode the specified 'sa' socket address as an IPv6 address. Load the
    /// IPv6 address into the specified 'ipv4address'. Return the error.
    static ntsa::Error decodeIpv6Address(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv4Address,
        const sockaddr_in6*                     sa);

    /// Decode the specified 'sa' socket address as an IPv4 network mask. Load
    /// the IPv4 network mask into the specified 'ipv4Mask'. Return the error.
    static ntsa::Error decodeIpv4Mask(
        bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Mask,
        const sockaddr_in*                      sa);

    /// Decode the specified 'sa' socket address as an IPv6 network mask. Load
    /// the IPv6 network mask into the specified 'ipv6Mask'. Return the error.
    static ntsa::Error decodeIpv6Mask(
        bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Mask,
        const sockaddr_in6*                     sa);

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

    /// Decode the specified 'sa' socket address as physical device address.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Return
    /// the error.
    static ntsa::Error decodeLink(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_dl*                          sa);

#elif defined(BSLS_PLATFORM_OS_LINUX)

    /// Decode the specified 'sa' socket address as physical device address.
    /// Load the adapter into the specified 'adapterName' and 'adapterIndex'.
    /// Load the Ethernet address into the specified 'ethernetAddress'. Return
    /// the error.
    static ntsa::Error decodeLink(
        bdlb::NullableValue<bsl::string>*           adapterName,
        bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
        bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
        const sockaddr_ll*                          sa);

#endif

    /// Normalize the specified 'route' by specifying undefined fields in the
    /// 'route' from the specified 'adapterVector' and 'ethernetRouteTable'
    /// according to the fields in the 'route' that are defined. Return the
    /// error.
    static ntsa::Error normalizeRoute(
        ntsa::Ipv4Route*                  route,
        const bsl::vector<ntsa::Adapter>& adapterVector,
        const ntsa::EthernetRouteTable&   ethernetRouteTable);

    /// Normalize the specified 'route' by specifying undefined fields in the
    /// 'route' from the specified 'adapterVector' and 'ethernetRouteTable'
    /// according to the fields in the 'route' that are defined. Return the
    /// error.
    static ntsa::Error normalizeRoute(
        ntsa::Ipv6Route*                  route,
        const bsl::vector<ntsa::Adapter>& adapterVector,
        const ntsa::EthernetRouteTable&   ethernetRouteTable);
};

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
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    else if (sa->sa_family == AF_PACKET) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_ll*>(sa));
        if (error) {
            return error;
        }
    }
#endif
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
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    else if (sa->sa_family == AF_PACKET) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_ll*>(sa));
        if (error) {
            return error;
        }
    }
#endif
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
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    else if (sa->sa_family == AF_PACKET) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_ll*>(sa));
        if (error) {
            return error;
        }
    }
#endif
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

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

ntsa::Error RouteUtil::Impl::decodeGateway(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_dl*                          sa)
{
    return Impl::decodeLink(adapterName, adapterIndex, ethernetAddress, sa);
}

#elif defined(BSLS_PLATFORM_OS_LINUX)

ntsa::Error RouteUtil::Impl::decodeGateway(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_ll*                          sa)
{
    return Impl::decodeLink(adapterName, adapterIndex, ethernetAddress, sa);
}

#endif

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
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    else if (sa->sa_family == AF_PACKET) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_ll*>(sa));
        if (error) {
            return error;
        }
    }
#endif
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
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    else if (sa->sa_family == AF_PACKET) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_ll*>(sa));
        if (error) {
            return error;
        }
    }
#endif
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
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    else if (sa->sa_family == AF_PACKET) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_ll*>(sa));
        if (error) {
            return error;
        }
    }
#endif
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

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

ntsa::Error RouteUtil::Impl::decodeIfa(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_dl*                          sa)
{
    return Impl::decodeLink(adapterName, adapterIndex, ethernetAddress, sa);
}

#elif defined(BSLS_PLATFORM_OS_LINUX)

ntsa::Error RouteUtil::Impl::decodeIfa(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_ll*                          sa)
{
    return Impl::decodeLink(adapterName, adapterIndex, ethernetAddress, sa);
}

#endif

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
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    else if (sa->sa_family == AF_PACKET) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_ll*>(sa));
        if (error) {
            return error;
        }
    }
#endif
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
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    else if (sa->sa_family == AF_PACKET) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_ll*>(sa));
        if (error) {
            return error;
        }
    }
#endif
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
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
    else if (sa->sa_family == AF_LINK) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_dl*>(sa));
        if (error) {
            return error;
        }
    }
#elif defined(BSLS_PLATFORM_OS_LINUX)
    else if (sa->sa_family == AF_PACKET) {
        error = Impl::decodeGateway(adapterName,
                                    adapterIndex,
                                    ethernetAddress,
                                    reinterpret_cast<const sockaddr_ll*>(sa));
        if (error) {
            return error;
        }
    }
#endif
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

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

ntsa::Error RouteUtil::Impl::decodeIfp(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_dl*                          sa)
{
    return Impl::decodeLink(adapterName, adapterIndex, ethernetAddress, sa);
}

#elif defined(BSLS_PLATFORM_OS_LINUX)

ntsa::Error RouteUtil::Impl::decodeIfp(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_ll*                          sa)
{
    return Impl::decodeLink(adapterName, adapterIndex, ethernetAddress, sa);
}

#endif

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

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

    const bsl::uint8_t minimumLengthWithAddress =
        offsetof(struct sockaddr_in, sin_addr) + sizeof sa->sin_addr;

    if (sa->sin_len < minimumLengthWithAddress) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#endif

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

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

    const bsl::uint8_t minimumLengthWithAddress =
        offsetof(struct sockaddr_in6, sin6_addr) + sizeof sa->sin6_addr;

    if (sa->sin6_len < minimumLengthWithAddress) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#endif

    ipv6Address->makeValue();

    const bsl::size_t bytesCopied =
        ipv6Address->value().copyFrom(&sa->sin6_addr, sizeof sa->sin6_addr);

    if (bytesCopied != sizeof sa->sin6_addr) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

    const bsl::uint8_t minimumLengthWithScope =
        offsetof(struct sockaddr_in6, sin6_scope_id) +
        sizeof sa->sin6_scope_id;

    if (sa->sin6_len >= minimumLengthWithScope) {
        ipv6Address->value().setScopeId(sa->sin6_scope_id);
    }

#else

    ipv6Address->value().setScopeId(sa->sin6_scope_id);

#endif

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::decodeIpv4Mask(
    bdlb::NullableValue<ntsa::Ipv4Address>* ipv4Mask,
    const sockaddr_in*                      sa)
{
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

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

#else

    return RouteUtil::Impl::decodeIpv4Address(ipv4Mask, sa);

#endif
}

ntsa::Error RouteUtil::Impl::decodeIpv6Mask(
    bdlb::NullableValue<ntsa::Ipv6Address>* ipv6Mask,
    const sockaddr_in6*                     sa)
{
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

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

#else

    return RouteUtil::Impl::decodeIpv6Address(ipv6Mask, sa);

#endif
}

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

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

#elif defined(BSLS_PLATFORM_OS_LINUX)

ntsa::Error RouteUtil::Impl::decodeLink(
    bdlb::NullableValue<bsl::string>*           adapterName,
    bdlb::NullableValue<bsl::uint32_t>*         adapterIndex,
    bdlb::NullableValue<ntsa::EthernetAddress>* ethernetAddress,
    const sockaddr_ll*                          sa)
{
    NTSCFG_WARNING_UNUSED(adapterName);

    adapterIndex->makeValue(static_cast<bsl::uint32_t>(sa->sll_ifindex));

    if (static_cast<bsl::size_t>(sa->sll_halen) ==
        sizeof(ntsa::EthernetAddress))
    {
        ethernetAddress->makeValue();

        const bsl::size_t bytesCopied = ethernetAddress->value().copyFrom(
            sa->sll_addr,
            static_cast<bsl::size_t>(sa->sll_halen));

        if (bytesCopied != static_cast<bsl::size_t>(sa->sll_halen)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

#endif

ntsa::Error RouteUtil::Impl::normalizeRoute(
    ntsa::Ipv4Route*                  route,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
{
    if (route->destinationIpv4Address().isNull()) {
        route->setDestinationIpv4Address(ntsa::Ipv4Address::any());
    }

    if (route->gatewayIpv4Address().isNull()) {
        const ntsa::Adapter* gatewayAdapter = 0;

        if (gatewayAdapter == 0) {
            if (route->gatewayAdapterName().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].name() ==
                        route->gatewayAdapterName().value())
                    {
                        gatewayAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (gatewayAdapter == 0) {
            if (route->gatewayAdapterIndex().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].index() ==
                        route->gatewayAdapterIndex().value())
                    {
                        gatewayAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (gatewayAdapter == 0) {
            if (route->gatewayEthernetAddress().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    ntsa::EthernetAddress ethernetAddress;
                    if (!ethernetAddress.parse(
                            adapterVector[i].ethernetAddress()))
                    {
                        continue;
                    }

                    if (ethernetAddress ==
                        route->gatewayEthernetAddress().value())
                    {
                        gatewayAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (gatewayAdapter != 0) {
            if (route->gatewayAdapterName().has_value()) {
                // TODO: compare
            }
            else {
                route->setGatewayAdapterName(gatewayAdapter->name());
            }

            if (route->gatewayAdapterIndex().has_value()) {
                // TODO: compare
            }
            else {
                route->setGatewayAdapterIndex(gatewayAdapter->index());
            }

            if (route->gatewayEthernetAddress().has_value()) {
                // TODO: compare
            }
            else {
                route->setGatewayEthernetAddress(
                    ntsa::EthernetAddress(gatewayAdapter->ethernetAddress()));
            }
        }
    }

    const ntsa::Adapter* interfaceAdapter = 0;

    if (interfaceAdapter == 0) {
        if (route->interfaceAdapterName().has_value()) {
            for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                if (adapterVector[i].name() ==
                    route->interfaceAdapterName().value())
                {
                    interfaceAdapter = &adapterVector[i];
                    break;
                }
            }
        }
    }

    if (interfaceAdapter == 0) {
        if (route->interfaceAdapterIndex().has_value()) {
            for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                if (adapterVector[i].index() ==
                    route->interfaceAdapterIndex().value())
                {
                    interfaceAdapter = &adapterVector[i];
                    break;
                }
            }
        }
    }

    if (interfaceAdapter == 0) {
        if (route->interfaceEthernetAddress().has_value()) {
            for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                ntsa::EthernetAddress ethernetAddress;
                if (!ethernetAddress.parse(adapterVector[i].ethernetAddress()))
                {
                    continue;
                }

                if (ethernetAddress ==
                    route->interfaceEthernetAddress().value())
                {
                    interfaceAdapter = &adapterVector[i];
                    break;
                }
            }
        }
    }

    if (interfaceAdapter == 0) {
        if (route->interfaceIpv4Address().has_value()) {
            for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                if (adapterVector[i].ipv4Address().has_value() &&
                    adapterVector[i].ipv4Address().value() ==
                        route->interfaceIpv4Address().value())
                {
                    interfaceAdapter = &adapterVector[i];
                    break;
                }
            }
        }
    }

    if (interfaceAdapter != 0) {
        if (route->interfaceAdapterName().has_value()) {
            // TODO: compare
        }
        else {
            route->setInterfaceAdapterName(interfaceAdapter->name());
        }

        if (route->interfaceAdapterIndex().has_value()) {
            // TODO: compare
        }
        else {
            route->setInterfaceAdapterIndex(interfaceAdapter->index());
        }

        if (route->interfaceEthernetAddress().has_value()) {
            // TODO: compare
        }
        else {
            route->setInterfaceEthernetAddress(
                ntsa::EthernetAddress(interfaceAdapter->ethernetAddress()));
        }

        if (route->interfaceIpv4Address().has_value()) {
            // TODO: compare
        }
        else if (interfaceAdapter->ipv4Address().has_value()) {
            route->setInterfaceIpv4Address(
                interfaceAdapter->ipv4Address().value());
        }
    }

    if (route->gatewayEthernetAddress().isNull()) {
        if (route->gatewayIpv4Address().has_value()) {
            if (route->gatewayIpv4Address().value().isLoopback()) {
                route->setGatewayEthernetAddress(ntsa::EthernetAddress());
            }
            else {
                ntsa::EthernetAddress ethernetAddress;
                if (ethernetRouteTable.find(
                        &ethernetAddress,
                        route->gatewayIpv4Address().value()))
                {
                    route->setGatewayEthernetAddress(ethernetAddress);
                }
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::normalizeRoute(
    ntsa::Ipv6Route*                  route,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
{
    if (route->destinationIpv6Address().isNull()) {
        route->setDestinationIpv6Address(ntsa::Ipv6Address::any());
    }

    if (route->gatewayIpv6Address().isNull()) {
        const ntsa::Adapter* gatewayAdapter = 0;

        if (gatewayAdapter == 0) {
            if (route->gatewayAdapterName().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].name() ==
                        route->gatewayAdapterName().value())
                    {
                        gatewayAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (gatewayAdapter == 0) {
            if (route->gatewayAdapterIndex().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    if (adapterVector[i].index() ==
                        route->gatewayAdapterIndex().value())
                    {
                        gatewayAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (gatewayAdapter == 0) {
            if (route->gatewayEthernetAddress().has_value()) {
                for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                    ntsa::EthernetAddress ethernetAddress;
                    if (!ethernetAddress.parse(
                            adapterVector[i].ethernetAddress()))
                    {
                        continue;
                    }

                    if (ethernetAddress ==
                        route->gatewayEthernetAddress().value())
                    {
                        gatewayAdapter = &adapterVector[i];
                        break;
                    }
                }
            }
        }

        if (gatewayAdapter != 0) {
            if (route->gatewayAdapterName().has_value()) {
                // TODO: compare
            }
            else {
                route->setGatewayAdapterName(gatewayAdapter->name());
            }

            if (route->gatewayAdapterIndex().has_value()) {
                // TODO: compare
            }
            else {
                route->setGatewayAdapterIndex(gatewayAdapter->index());
            }

            if (route->gatewayEthernetAddress().has_value()) {
                // TODO: compare
            }
            else {
                route->setGatewayEthernetAddress(
                    ntsa::EthernetAddress(gatewayAdapter->ethernetAddress()));
            }
        }
    }

    const ntsa::Adapter* interfaceAdapter = 0;

    if (interfaceAdapter == 0) {
        if (route->interfaceAdapterName().has_value()) {
            for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                if (adapterVector[i].name() ==
                    route->interfaceAdapterName().value())
                {
                    interfaceAdapter = &adapterVector[i];
                    break;
                }
            }
        }
    }

    if (interfaceAdapter == 0) {
        if (route->interfaceAdapterIndex().has_value()) {
            for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                if (adapterVector[i].index() ==
                    route->interfaceAdapterIndex().value())
                {
                    interfaceAdapter = &adapterVector[i];
                    break;
                }
            }
        }
    }

    if (interfaceAdapter == 0) {
        if (route->interfaceEthernetAddress().has_value()) {
            for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                ntsa::EthernetAddress ethernetAddress;
                if (!ethernetAddress.parse(adapterVector[i].ethernetAddress()))
                {
                    continue;
                }

                if (ethernetAddress ==
                    route->interfaceEthernetAddress().value())
                {
                    interfaceAdapter = &adapterVector[i];
                    break;
                }
            }
        }
    }

    if (interfaceAdapter == 0) {
        if (route->interfaceIpv6Address().has_value()) {
            for (bsl::size_t i = 0; i < adapterVector.size(); ++i) {
                if (adapterVector[i].ipv6Address().has_value() &&
                    adapterVector[i].ipv6Address().value() ==
                        route->interfaceIpv6Address().value())
                {
                    interfaceAdapter = &adapterVector[i];
                    break;
                }
            }
        }
    }

    if (interfaceAdapter != 0) {
        if (route->interfaceAdapterName().has_value()) {
            // TODO: compare
        }
        else {
            route->setInterfaceAdapterName(interfaceAdapter->name());
        }

        if (route->interfaceAdapterIndex().has_value()) {
            // TODO: compare
        }
        else {
            route->setInterfaceAdapterIndex(interfaceAdapter->index());
        }

        if (route->interfaceEthernetAddress().has_value()) {
            // TODO: compare
        }
        else {
            route->setInterfaceEthernetAddress(
                ntsa::EthernetAddress(interfaceAdapter->ethernetAddress()));
        }

        if (route->interfaceIpv6Address().has_value()) {
            // TODO: compare
        }
        else if (interfaceAdapter->ipv6Address().has_value()) {
            route->setInterfaceIpv6Address(
                interfaceAdapter->ipv6Address().value());
        }
    }

    if (route->gatewayEthernetAddress().isNull()) {
        if (route->gatewayIpv6Address().has_value()) {
            if (route->gatewayIpv6Address().value().isLoopback()) {
                route->setGatewayEthernetAddress(ntsa::EthernetAddress());
            }
            else {
                ntsa::EthernetAddress ethernetAddress;
                if (ethernetRouteTable.find(
                        &ethernetAddress,
                        route->gatewayIpv6Address().value()))
                {
                    route->setGatewayEthernetAddress(ethernetAddress);
                }
            }
        }
    }

    return ntsa::Error();
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

/// Provide a private, platform-specific implementation of utilities for
/// discovering route tables.
class RouteUtil::Impl
{
    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.ROUTEUTIL.IMPL.NATIVE");

  public:
    /// Provide a private, platform-specific implementation of utilities for
    /// discovering route tables native to the operating system.
    class Native;
};

#else
#error Not implemented
#endif

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)

/// Provide a private, platform-specific implementation of utilities for
/// discovering route tables.
class RouteUtil::Impl::Native
{
    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.ROUTEUTIL.IMPL.NATIVE");

    static ntsa::Error decodeRouteHeader(const rt_msghdr**    rtm,
                                         const bsl::uint8_t** current,
                                         const bsl::uint8_t*  end);

    static ntsa::Error decodeRoutePayload(const sockaddr** rti,
                                          const rt_msghdr* rtm);

    static const sockaddr* next(const sockaddr* sa);

  public:
    /// Load the Ethernet route table for the adapters in the specified
    /// 'adapterVector' into the specified 'result'. Return the error.
    static ntsa::Error load(ntsa::EthernetRouteTable*         result,
                            const bsl::vector<ntsa::Adapter>& adapterVector);

    /// Load the IPv4 route table into the specified 'result' using the
    /// specified 'ethernetRouteTable' for the adapters in the specified
    /// 'adapterVector'. Return the error.
    static ntsa::Error load(
        ntsa::Ipv4RouteTable*             result,
        const bsl::vector<ntsa::Adapter>& adapterVector,
        const ntsa::EthernetRouteTable&   ethernetRouteTable);

    /// Load the IPv6 route table into the specified 'result' using the
    /// specified 'ethernetRouteTable' for the adapters in the specified
    /// 'adapterVector'. Return the error.
    static ntsa::Error load(
        ntsa::Ipv6RouteTable*             result,
        const bsl::vector<ntsa::Adapter>& adapterVector,
        const ntsa::EthernetRouteTable&   ethernetRouteTable);
};

ntsa::Error RouteUtil::Impl::Native::decodeRouteHeader(
    const rt_msghdr**    rtm,
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

ntsa::Error RouteUtil::Impl::Native::decodeRoutePayload(const sockaddr** rti,
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

            sa = ntsu::RouteUtil::Impl::Native::next(sa);
        }
        else {
            rti[i] = 0;
        }
    }

    return ntsa::Error();
}

const sockaddr* RouteUtil::Impl::Native::next(const sockaddr* sa)
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

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::EthernetRouteTable*         result,
    const bsl::vector<ntsa::Adapter>& adapterVector)
{
    NTSCFG_WARNING_UNUSED(adapterVector);

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
        error = Impl::Native::decodeRouteHeader(&rtm, &current, bufferEnd);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                break;
            }
            else {
                NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_HEADER(error);
                return error;
            }
        }

        const sockaddr* rti_info[RTAX_MAX];
        NTSCFG_MEMORY_ZERO(rti_info, sizeof rti_info);

        error = Impl::Native::decodeRoutePayload(rti_info, rtm);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_PAYLOAD(error);
            return error;
        }

        NullableIpAddress       destinationIpAddress;
        NullableString          gatewayAdapterName;
        NullableUint32          gatewayAdapterIndex;
        NullableEthernetAddress gatewayEthernetAddress;
        NullableIpAddress       gatewayIpAddress;
        NullableString          interfaceAdapterName;
        NullableUint32          interfaceAdapterIndex;
        NullableEthernetAddress interfaceEthernetAddress;
        NullableIpAddress       interfaceIpAddress;

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

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::Ipv4RouteTable*             result,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
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
        error = Impl::Native::decodeRouteHeader(&rtm, &current, bufferEnd);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                break;
            }
            else {
                NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_HEADER(error);
                return error;
            }
        }

        const sockaddr* rti_info[RTAX_MAX];
        NTSCFG_MEMORY_ZERO(rti_info, sizeof rti_info);

        error = Impl::Native::decodeRoutePayload(rti_info, rtm);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_PAYLOAD(error);
            return error;
        }

        NullableIpv4Address     destinationIpv4Address;
        NullableIpv4Address     destinationIpv4Mask;
        NullableString          gatewayAdapterName;
        NullableUint32          gatewayAdapterIndex;
        NullableEthernetAddress gatewayEthernetAddress;
        NullableIpv4Address     gatewayIpv4Address;
        NullableString          interfaceAdapterName;
        NullableUint32          interfaceAdapterIndex;
        NullableEthernetAddress interfaceEthernetAddress;
        NullableIpv4Address     interfaceIpv4Address;

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
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_NETMASK(
                rti_info[RTAX_NETMASK],
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

        error = RouteUtil::Impl::normalizeRoute(&route,
                                                adapterVector,
                                                ethernetRouteTable);
        if (error) {
            return error;
        }

        result->add(route);
    }

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::Ipv6RouteTable*             result,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
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
        error = Impl::Native::decodeRouteHeader(&rtm, &current, bufferEnd);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                break;
            }
            else {
                NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_HEADER(error);
                return error;
            }
        }

        const sockaddr* rti_info[RTAX_MAX];
        NTSCFG_MEMORY_ZERO(rti_info, sizeof rti_info);

        error = Impl::Native::decodeRoutePayload(rti_info, rtm);
        if (error) {
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_ROUTE_PAYLOAD(error);
            return error;
        }

        NullableIpv6Address     destinationIpv6Address;
        NullableIpv6Address     destinationIpv6Mask;
        NullableString          gatewayAdapterName;
        NullableUint32          gatewayAdapterIndex;
        NullableEthernetAddress gatewayEthernetAddress;
        NullableIpv6Address     gatewayIpv6Address;
        NullableString          interfaceAdapterName;
        NullableUint32          interfaceAdapterIndex;
        NullableEthernetAddress interfaceEthernetAddress;
        NullableIpv6Address     interfaceIpv6Address;

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
            NTSU_ROUTEUTIL_LOG_ERROR_DECODE_RTAX_NETMASK(
                rti_info[RTAX_NETMASK],
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

        error = RouteUtil::Impl::normalizeRoute(&route,
                                                adapterVector,
                                                ethernetRouteTable);
        if (error) {
            return error;
        }

        result->add(route);
    }

    return ntsa::Error();
}

#elif defined(BSLS_PLATFORM_OS_LINUX)

/// Provide a private, platform-specific implementation of utilities for
/// discovering route tables.
class RouteUtil::Impl::Native
{
    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.ROUTEUTIL.IMPL.NATIVE");

  public:
    /// Load the Ethernet route table for the adapters in the specified
    /// 'adapterVector' into the specified 'result'. Return the error.
    static ntsa::Error load(ntsa::EthernetRouteTable*         result,
                            const bsl::vector<ntsa::Adapter>& adapterVector);

    /// Load the IPv4 route table into the specified 'result' using the
    /// specified 'ethernetRouteTable' for the adapters in the specified
    /// 'adapterVector'. Return the error.
    static ntsa::Error load(
        ntsa::Ipv4RouteTable*             result,
        const bsl::vector<ntsa::Adapter>& adapterVector,
        const ntsa::EthernetRouteTable&   ethernetRouteTable);

    /// Load the IPv6 route table into the specified 'result' using the
    /// specified 'ethernetRouteTable' for the adapters in the specified
    /// 'adapterVector'. Return the error.
    static ntsa::Error load(
        ntsa::Ipv6RouteTable*             result,
        const bsl::vector<ntsa::Adapter>& adapterVector,
        const ntsa::EthernetRouteTable&   ethernetRouteTable);
};

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::EthernetRouteTable*         result,
    const bsl::vector<ntsa::Adapter>& adapterVector)
{
    NTSCFG_WARNING_UNUSED(adapterVector);

    result->reset();

    bsl::ifstream fs("/proc/net/arp", bsl::ios_base::in);
    if (!fs) {
        return ntsa::Error::last();
    }

    bsl::string line;

    bsl::getline(fs, line);

    while (fs) {
        line.clear();
        bsl::getline(fs, line);

        bsl::istringstream is(line);

        bsl::string ipv4AddressString;
        is >> ipv4AddressString;
        if (!is) {
            break;
        }

        bsl::string hardwareTypeString;
        is >> hardwareTypeString;
        if (!is) {
            break;
        }

        bsl::string flagsString;
        is >> flagsString;
        if (!is) {
            break;
        }

        bsl::string hardwareAddressString;
        is >> hardwareAddressString;
        if (!is) {
            break;
        }

        bsl::string maskString;
        is >> maskString;
        if (!is) {
            break;
        }

        bsl::string deviceString;
        is >> deviceString;
        if (!is) {
            break;
        }

        if (hardwareTypeString != "0x1") {
            continue;
        }

        ntsa::EthernetAddress ethernetAddress;
        if (!ethernetAddress.parse(hardwareAddressString)) {
            continue;
        }

        ntsa::Ipv4Address ipv4Address;
        if (!ipv4Address.parse(ipv4AddressString)) {
            continue;
        }

        ntsa::EthernetRoute route;
        route.setEthernetAddress(ethernetAddress);
        route.setIpv4Address(ipv4Address);

        result->add(route);
    }

    fs.close();

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::Ipv4RouteTable*             result,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
{
    ntsa::Error error;

    result->reset();

    int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        error = ntsa::Error::last();
        BALL_LOG_ERROR << "Failed to create netlink socket: " << error
                       << BALL_LOG_END;
        return error;
    }

    char requestBuffer[8192];
    NTSCFG_MEMORY_ZERO(requestBuffer, sizeof requestBuffer);

    struct nlmsghdr* requestHeader =
        reinterpret_cast<struct nlmsghdr*>(requestBuffer);

    struct rtmsg* requestPayload =
        reinterpret_cast<struct rtmsg*>(NLMSG_DATA(requestHeader));

    requestHeader->nlmsg_len   = NLMSG_LENGTH(sizeof(struct rtmsg));
    requestHeader->nlmsg_type  = RTM_GETROUTE;
    requestHeader->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;

    requestPayload->rtm_family = AF_INET;
    requestPayload->rtm_table  = RT_TABLE_MAIN;

    const ssize_t sendResult =
        ::send(fd, requestHeader, requestHeader->nlmsg_len, 0);

    if (sendResult < 0) {
        error = ntsa::Error::last();
        BALL_LOG_ERROR << "Failed to send to netlink socket: " << error
                       << BALL_LOG_END;

        ::close(fd);
        return error;
    }

    if (static_cast<bsl::size_t>(sendResult) !=
        static_cast<bsl::size_t>(requestHeader->nlmsg_len))
    {
        BALL_LOG_ERROR << "Failed to send to netlink socket: expected to send "
                       << requestHeader->nlmsg_len << " bytes but only sent "
                       << sendResult << " bytes" << BALL_LOG_END;

        ::close(fd);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    while (true) {
        char responseBuffer[8192];
        NTSCFG_MEMORY_ZERO(responseBuffer, sizeof responseBuffer);

        ssize_t receiveResult =
            ::recv(fd, responseBuffer, sizeof responseBuffer, 0);

        if (receiveResult < 0) {
            error = ntsa::Error::last();
            BALL_LOG_ERROR << "Failed to receive from netlink socket: "
                           << error << BALL_LOG_END;

            ::close(fd);
            return error;
        }

        struct nlmsghdr* responseHeader =
            reinterpret_cast<struct nlmsghdr*>(responseBuffer);

        while (NLMSG_OK(responseHeader, receiveResult)) {
            if (responseHeader->nlmsg_type == NLMSG_DONE) {
                break;
            }

            if (responseHeader->nlmsg_type == NLMSG_ERROR) {
                BALL_LOG_ERROR << "Failed to receive from netlink socket: "
                               << "NLMSG_ERROR" << BALL_LOG_END;

                ::close(fd);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            NullableIpv4Address     destinationIpv4Address;
            NullableIpv4Address     destinationIpv4Mask;
            NullableString          gatewayAdapterName;
            NullableUint32          gatewayAdapterIndex;
            NullableEthernetAddress gatewayEthernetAddress;
            NullableIpv4Address     gatewayIpv4Address;
            NullableString          interfaceAdapterName;
            NullableUint32          interfaceAdapterIndex;
            NullableEthernetAddress interfaceEthernetAddress;
            NullableIpv4Address     interfaceIpv4Address;

            struct rtmsg* responsePayload =
                reinterpret_cast<struct rtmsg*>(NLMSG_DATA(responseHeader));

            if (responsePayload->rtm_family != AF_INET) {
                ::close(fd);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            struct rtattr* rt_attr =
                reinterpret_cast<struct rtattr*>(RTM_RTA(responsePayload));

            int attr_len = RTM_PAYLOAD(responseHeader);

            while (RTA_OK(rt_attr, attr_len)) {
                if (rt_attr->rta_type == RTA_DST) {
                    if (rt_attr->rta_len !=
                        sizeof(struct rtattr) + sizeof(struct in_addr))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    const bsl::size_t bytesCopied =
                        destinationIpv4Address.makeValue().copyFrom(
                            RTA_DATA(rt_attr),
                            static_cast<bsl::size_t>(sizeof(struct in_addr)));

                    if (bytesCopied !=
                        static_cast<bsl::size_t>(sizeof(struct in_addr)))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }

                if (rt_attr->rta_type == RTA_GATEWAY) {
                    if (rt_attr->rta_len !=
                        sizeof(struct rtattr) + sizeof(struct in_addr))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    const bsl::size_t bytesCopied =
                        gatewayIpv4Address.makeValue().copyFrom(
                            RTA_DATA(rt_attr),
                            static_cast<bsl::size_t>(sizeof(struct in_addr)));

                    if (bytesCopied !=
                        static_cast<bsl::size_t>(sizeof(struct in_addr)))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }

                if (rt_attr->rta_type == RTA_OIF) {
                    if (rt_attr->rta_len !=
                        sizeof(struct rtattr) + sizeof(int))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    interfaceAdapterIndex.makeValue(static_cast<bsl::uint32_t>(
                        *reinterpret_cast<int*>(RTA_DATA(rt_attr))));
                }

                rt_attr = RTA_NEXT(rt_attr, attr_len);
            }

            const bsl::uint32_t netmaskPrefix =
                static_cast<bsl::uint32_t>(responsePayload->rtm_dst_len);

            if (netmaskPrefix > 0) {
                bsl::uint32_t netmaskValue =
                    ~0U << (32 - static_cast<int>(netmaskPrefix));

                destinationIpv4Mask.makeValue(
                    ntsa::Ipv4Address(htonl(netmaskValue)));
            }

            if (interfaceAdapterIndex.has_value() &&
                interfaceAdapterName.isNull())
            {
                char adapterName[IFNAMSIZ];
                NTSCFG_MEMORY_ZERO(adapterName, sizeof adapterName);

                const char* adapterNameResult =
                    if_indextoname(interfaceAdapterIndex.value(), adapterName);

                if (adapterNameResult == adapterName) {
                    interfaceAdapterName.makeValue().assign(adapterName);
                }
            }

            if (interfaceAdapterName.has_value() &&
                interfaceAdapterIndex.isNull())
            {
                unsigned int adapterIndex =
                    if_nametoindex(interfaceAdapterName.value().c_str());

                if (adapterIndex != 0) {
                    interfaceAdapterIndex.makeValue(
                        static_cast<bsl::uint32_t>(adapterIndex));
                }
            }

            ntsa::Ipv4Route route;

            if (destinationIpv4Address.has_value()) {
                route.setDestinationIpv4Address(
                    destinationIpv4Address.value());
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
                route.setGatewayEthernetAddress(
                    gatewayEthernetAddress.value());
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

            error = RouteUtil::Impl::normalizeRoute(&route,
                                                    adapterVector,
                                                    ethernetRouteTable);
            if (error) {
                return error;
            }

            result->add(route);

            responseHeader = NLMSG_NEXT(responseHeader, receiveResult);
        }

        if (responseHeader->nlmsg_type == NLMSG_DONE) {
            break;
        }
    }

    ::close(fd);

    return ntsa::Error();
}

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::Ipv6RouteTable*             result,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
{
    ntsa::Error error;

    result->reset();

    int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        error = ntsa::Error::last();
        BALL_LOG_ERROR << "Failed to create netlink socket: " << error
                       << BALL_LOG_END;
        return error;
    }

    char requestBuffer[8192];
    NTSCFG_MEMORY_ZERO(requestBuffer, sizeof requestBuffer);

    struct nlmsghdr* requestHeader =
        reinterpret_cast<struct nlmsghdr*>(requestBuffer);

    struct rtmsg* requestPayload =
        reinterpret_cast<struct rtmsg*>(NLMSG_DATA(requestHeader));

    requestHeader->nlmsg_len   = NLMSG_LENGTH(sizeof(struct rtmsg));
    requestHeader->nlmsg_type  = RTM_GETROUTE;
    requestHeader->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;

    requestPayload->rtm_family = AF_INET6;
    requestPayload->rtm_table  = RT_TABLE_MAIN;

    const ssize_t sendResult =
        ::send(fd, requestHeader, requestHeader->nlmsg_len, 0);

    if (sendResult < 0) {
        error = ntsa::Error::last();
        BALL_LOG_ERROR << "Failed to send to netlink socket: " << error
                       << BALL_LOG_END;

        ::close(fd);
        return error;
    }

    if (static_cast<bsl::size_t>(sendResult) !=
        static_cast<bsl::size_t>(requestHeader->nlmsg_len))
    {
        BALL_LOG_ERROR << "Failed to send to netlink socket: expected to send "
                       << requestHeader->nlmsg_len << " bytes but only sent "
                       << sendResult << " bytes" << BALL_LOG_END;

        ::close(fd);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    while (true) {
        char responseBuffer[8192];
        NTSCFG_MEMORY_ZERO(responseBuffer, sizeof responseBuffer);

        ssize_t receiveResult =
            ::recv(fd, responseBuffer, sizeof responseBuffer, 0);

        if (receiveResult < 0) {
            error = ntsa::Error::last();
            BALL_LOG_ERROR << "Failed to receive from netlink socket: "
                           << error << BALL_LOG_END;

            ::close(fd);
            return error;
        }

        struct nlmsghdr* responseHeader =
            reinterpret_cast<struct nlmsghdr*>(responseBuffer);

        while (NLMSG_OK(responseHeader, receiveResult)) {
            if (responseHeader->nlmsg_type == NLMSG_DONE) {
                break;
            }

            if (responseHeader->nlmsg_type == NLMSG_ERROR) {
                BALL_LOG_ERROR << "Failed to receive from netlink socket: "
                               << "NLMSG_ERROR" << BALL_LOG_END;

                ::close(fd);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            NullableIpv6Address     destinationIpv6Address;
            NullableIpv6Address     destinationIpv6Mask;
            NullableString          gatewayAdapterName;
            NullableUint32          gatewayAdapterIndex;
            NullableEthernetAddress gatewayEthernetAddress;
            NullableIpv6Address     gatewayIpv6Address;
            NullableString          interfaceAdapterName;
            NullableUint32          interfaceAdapterIndex;
            NullableEthernetAddress interfaceEthernetAddress;
            NullableIpv6Address     interfaceIpv6Address;

            struct rtmsg* responsePayload =
                reinterpret_cast<struct rtmsg*>(NLMSG_DATA(responseHeader));

            if (responsePayload->rtm_family != AF_INET6) {
                ::close(fd);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            struct rtattr* rt_attr =
                reinterpret_cast<struct rtattr*>(RTM_RTA(responsePayload));

            int attr_len = RTM_PAYLOAD(responseHeader);

            while (RTA_OK(rt_attr, attr_len)) {
                if (rt_attr->rta_type == RTA_DST) {
                    if (rt_attr->rta_len !=
                        sizeof(struct rtattr) + sizeof(struct in6_addr))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    const bsl::size_t bytesCopied =
                        destinationIpv6Address.makeValue().copyFrom(
                            RTA_DATA(rt_attr),
                            static_cast<bsl::size_t>(sizeof(struct in6_addr)));

                    if (bytesCopied !=
                        static_cast<bsl::size_t>(sizeof(struct in6_addr)))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }

                if (rt_attr->rta_type == RTA_GATEWAY) {
                    if (rt_attr->rta_len !=
                        sizeof(struct rtattr) + sizeof(struct in6_addr))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    const bsl::size_t bytesCopied =
                        gatewayIpv6Address.makeValue().copyFrom(
                            RTA_DATA(rt_attr),
                            static_cast<bsl::size_t>(sizeof(struct in6_addr)));

                    if (bytesCopied !=
                        static_cast<bsl::size_t>(sizeof(struct in6_addr)))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }

                if (rt_attr->rta_type == RTA_OIF) {
                    if (rt_attr->rta_len !=
                        sizeof(struct rtattr) + sizeof(int))
                    {
                        ::close(fd);
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    interfaceAdapterIndex.makeValue(static_cast<bsl::uint32_t>(
                        *reinterpret_cast<int*>(RTA_DATA(rt_attr))));
                }

                rt_attr = RTA_NEXT(rt_attr, attr_len);
            }

            const bsl::uint32_t netmaskPrefix =
                static_cast<bsl::uint32_t>(responsePayload->rtm_dst_len);

            if (netmaskPrefix > 0) {
                struct in6_addr m;
                NTSCFG_MEMORY_ZERO(&m, sizeof m);

                bsl::uint32_t n = netmaskPrefix;

                for (int i = 0; i < 16; i++) {
                    if (n >= 8) {
                        m.s6_addr[i]  = 0xFF;
                        n            -= 8;
                    }
                    else if (n > 0) {
                        m.s6_addr[i] = (0xFF << (8 - n)) & 0xFF;
                        n            = 0;
                    }
                    else {
                        break;
                    }
                }

                destinationIpv6Mask.makeValue().copyFrom(&m, sizeof m);
            }

            if (interfaceAdapterIndex.has_value() &&
                interfaceAdapterName.isNull())
            {
                char adapterName[IFNAMSIZ];
                NTSCFG_MEMORY_ZERO(adapterName, sizeof adapterName);

                const char* adapterNameResult =
                    if_indextoname(interfaceAdapterIndex.value(), adapterName);

                if (adapterNameResult == adapterName) {
                    interfaceAdapterName.makeValue().assign(adapterName);
                }
            }

            if (interfaceAdapterName.has_value() &&
                interfaceAdapterIndex.isNull())
            {
                unsigned int adapterIndex =
                    if_nametoindex(interfaceAdapterName.value().c_str());

                if (adapterIndex != 0) {
                    interfaceAdapterIndex.makeValue(
                        static_cast<bsl::uint32_t>(adapterIndex));
                }
            }

            ntsa::Ipv6Route route;

            if (destinationIpv6Address.has_value()) {
                route.setDestinationIpv6Address(
                    destinationIpv6Address.value());
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
                route.setGatewayEthernetAddress(
                    gatewayEthernetAddress.value());
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

            error = RouteUtil::Impl::normalizeRoute(&route,
                                                    adapterVector,
                                                    ethernetRouteTable);
            if (error) {
                return error;
            }

            result->add(route);

            responseHeader = NLMSG_NEXT(responseHeader, receiveResult);
        }

        if (responseHeader->nlmsg_type == NLMSG_DONE) {
            break;
        }
    }

    ::close(fd);

    return ntsa::Error();
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::EthernetRouteTable*         result,
    const bsl::vector<ntsa::Adapter>& adapterVector)
{
    result->reset();

    NTSCFG_WARNING_UNUSED(adapterVector);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::Ipv4RouteTable*             result,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
{
    result->reset();

    NTSCFG_WARNING_UNUSED(adapterVector);
    NTSCFG_WARNING_UNUSED(ethernetRouteTable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::Ipv6RouteTable*             result,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
{
    result->reset();

    NTSCFG_WARNING_UNUSED(adapterVector);
    NTSCFG_WARNING_UNUSED(ethernetRouteTable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

#else

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::EthernetRouteTable*         result,
    const bsl::vector<ntsa::Adapter>& adapterVector)
{
    result->reset();

    NTSCFG_WARNING_UNUSED(adapterVector);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::Ipv4RouteTable*             result,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
{
    result->reset();

    NTSCFG_WARNING_UNUSED(adapterVector);
    NTSCFG_WARNING_UNUSED(ethernetRouteTable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error RouteUtil::Impl::Native::load(
    ntsa::Ipv6RouteTable*             result,
    const bsl::vector<ntsa::Adapter>& adapterVector,
    const ntsa::EthernetRouteTable&   ethernetRouteTable)
{
    result->reset();

    NTSCFG_WARNING_UNUSED(adapterVector);
    NTSCFG_WARNING_UNUSED(ethernetRouteTable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

#endif

#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD) ||  \
    defined(BSLS_PLATFORM_OS_LINUX)

ntsa::Error RouteUtil::load(ntsa::EthernetRouteTable* result)
{
    bsl::vector<ntsa::Adapter> adapterVector;
    ntsu::AdapterUtil::discoverAdapterList(&adapterVector);

    return RouteUtil::load(result, adapterVector);
}

ntsa::Error RouteUtil::load(ntsa::EthernetRouteTable*         result,
                            const bsl::vector<ntsa::Adapter>& adapterVector)
{
    return RouteUtil::Impl::Native::load(result, adapterVector);
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
    return RouteUtil::Impl::Native::load(result,
                                         adapterVector,
                                         ethernetRouteTable);
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
    return RouteUtil::Impl::Native::load(result,
                                         adapterVector,
                                         ethernetRouteTable);
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
