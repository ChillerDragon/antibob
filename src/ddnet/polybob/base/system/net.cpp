#include <polybob/base/detect.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/base/system/net.h>
#include <polybob/base/system/str.h>

#if defined(CONF_FAMILY_UNIX)
#include <unistd.h> // close

// UNIX net includes
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#endif

namespace polybob
{

#define AF_WEBSOCKET_INET (0xee)

#ifdef CONF_PLATFORM_LINUX
	static constexpr size_t VLEN = 128;
#endif
	static constexpr size_t PACKETSIZE = 1400;

	typedef struct
	{
#ifdef CONF_PLATFORM_LINUX
		int pos;
		int size;
		struct mmsghdr msgs[VLEN];
		struct iovec iovecs[VLEN];
		char bufs[VLEN][PACKETSIZE];
		char sockaddrs[VLEN][128];
#else
		char buf[PACKETSIZE];
#endif
	} NETSOCKET_BUFFER;

	static constexpr const unsigned char LOOPBACKADDR_IPV4[16] = {127, 0, 0, 1};
	static constexpr const unsigned char LOOPBACKADDR_IPV6[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

	static void net_buffer_init(NETSOCKET_BUFFER *buffer)
	{
#if defined(CONF_PLATFORM_LINUX)
		buffer->pos = 0;
		buffer->size = 0;
		mem_zero(buffer->msgs, sizeof(buffer->msgs));
		mem_zero(buffer->iovecs, sizeof(buffer->iovecs));
		mem_zero(buffer->sockaddrs, sizeof(buffer->sockaddrs));
		for(size_t i = 0; i < VLEN; ++i)
		{
			buffer->iovecs[i].iov_base = buffer->bufs[i];
			buffer->iovecs[i].iov_len = PACKETSIZE;
			buffer->msgs[i].msg_hdr.msg_iov = &(buffer->iovecs[i]);
			buffer->msgs[i].msg_hdr.msg_iovlen = 1;
			buffer->msgs[i].msg_hdr.msg_name = &(buffer->sockaddrs[i]);
			buffer->msgs[i].msg_hdr.msg_namelen = sizeof(buffer->sockaddrs[i]);
		}
#endif
	}

#if defined(CONF_PLATFORM_LINUX)
	static void net_buffer_reinit(NETSOCKET_BUFFER *buffer)
	{
		for(size_t i = 0; i < VLEN; i++)
		{
			buffer->msgs[i].msg_hdr.msg_namelen = sizeof(buffer->sockaddrs[i]);
		}
	}
#endif

#if defined(CONF_WEBSOCKETS)
	static void net_buffer_simple(NETSOCKET_BUFFER *buffer, char **buf, int *size)
	{
#if defined(CONF_PLATFORM_LINUX)
		*buf = buffer->bufs[0];
		*size = sizeof(buffer->bufs[0]);
#else
		*buf = buffer->buf;
		*size = sizeof(buffer->buf);
#endif
	}
#endif

	struct NETSOCKET_INTERNAL
	{
		int type;
		int ipv4sock;
		int ipv6sock;
		int web_ipv4sock;
		int web_ipv6sock;
		bool broken;

		NETSOCKET_BUFFER buffer;
	};
	static NETSOCKET_INTERNAL invalid_socket = {NETTYPE_INVALID, -1, -1, -1, -1, false};

	const NETADDR NETADDR_ZEROED = {NETTYPE_INVALID, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0};

	static void netaddr_to_sockaddr_in(const NETADDR *src, struct sockaddr_in *dest)
	{
		dbg_assert((src->type & (NETTYPE_IPV4 | NETTYPE_WEBSOCKET_IPV4)) != 0, "Invalid address type '%d' for netaddr_to_sockaddr_in", src->type);
		mem_zero(dest, sizeof(struct sockaddr_in));
		dest->sin_family = AF_INET;
		dest->sin_port = htons(src->port);
		mem_copy(&dest->sin_addr.s_addr, src->ip, 4);
	}

	static void netaddr_to_sockaddr_in6(const NETADDR *src, struct sockaddr_in6 *dest)
	{
		dbg_assert((src->type & NETTYPE_IPV6) != 0, "Invalid address type '%d' for netaddr_to_sockaddr_in6", src->type);
		mem_zero(dest, sizeof(struct sockaddr_in6));
		dest->sin6_family = AF_INET6;
		dest->sin6_port = htons(src->port);
		mem_copy(&dest->sin6_addr.s6_addr, src->ip, 16);
	}

	static void sockaddr_to_netaddr(const sockaddr *src, socklen_t src_len, NETADDR *dst)
	{
		*dst = NETADDR_ZEROED;
		if(src->sa_family == AF_INET && src_len >= (socklen_t)sizeof(sockaddr_in))
		{
			const sockaddr_in *src_in = (const sockaddr_in *)src;
			dst->type = NETTYPE_IPV4;
			dst->port = htons(src_in->sin_port);
			static_assert(sizeof(dst->ip) >= sizeof(src_in->sin_addr.s_addr));
			mem_copy(dst->ip, &src_in->sin_addr.s_addr, sizeof(src_in->sin_addr.s_addr));
		}
		else if(src->sa_family == AF_INET6 && src_len >= (socklen_t)sizeof(sockaddr_in6))
		{
			const sockaddr_in6 *src_in6 = (const sockaddr_in6 *)src;
			dst->type = NETTYPE_IPV6;
			dst->port = htons(src_in6->sin6_port);
			static_assert(sizeof(dst->ip) >= sizeof(src_in6->sin6_addr.s6_addr));
			mem_copy(dst->ip, &src_in6->sin6_addr.s6_addr, sizeof(src_in6->sin6_addr.s6_addr));
		}
		else
		{
			log_warn("net", "Cannot convert sockaddr of family %d", src->sa_family);
		}
	}

	static int priv_net_extract(const char *hostname, char *host, int max_host, int *port)
	{
		*port = 0;
		host[0] = 0;

		if(hostname[0] == '[')
		{
			// ipv6 mode
			int i;
			for(i = 1; i < max_host && hostname[i] && hostname[i] != ']'; i++)
				host[i - 1] = hostname[i];
			host[i - 1] = 0;
			if(hostname[i] != ']') // malformatted
				return -1;

			i++;
			if(hostname[i] == ':')
				*port = str_toint(hostname + i + 1);
		}
		else
		{
			// generic mode (ipv4, hostname etc)
			int i;
			for(i = 0; i < max_host - 1 && hostname[i] && hostname[i] != ':'; i++)
				host[i] = hostname[i];
			host[i] = 0;

			if(hostname[i] == ':')
				*port = str_toint(hostname + i + 1);
		}

		return 0;
	}

	static int net_host_lookup_fallback(const char *hostname, NETADDR *addr, int types, int port)
	{
		if(str_comp_nocase(hostname, "localhost") == 0)
		{
			if(types == NETTYPE_IPV4)
			{
				addr->type = NETTYPE_IPV4;
				mem_copy(addr->ip, LOOPBACKADDR_IPV4, sizeof(LOOPBACKADDR_IPV4));
				addr->port = port;
				return 0;
			}
			else if(types == NETTYPE_IPV6)
			{
				addr->type = NETTYPE_IPV6;
				mem_copy(addr->ip, LOOPBACKADDR_IPV6, sizeof(LOOPBACKADDR_IPV6));
				addr->port = port;
				return 0;
			}
			else
			{
				// TODO: return both IPv4 and IPv6 address
				addr->type = NETTYPE_IPV4;
				mem_copy(addr->ip, LOOPBACKADDR_IPV4, sizeof(LOOPBACKADDR_IPV4));
				addr->port = port;
				return 0;
			}
		}
		return -1;
	}

	static int net_host_lookup_impl(const char *hostname, NETADDR *addr, int types)
	{
		char host[256];
		int port = 0;
		if(priv_net_extract(hostname, host, sizeof(host), &port))
			return -1;

		log_trace("host_lookup", "host='%s' port='%d' types='%d'", host, port, types);

		struct addrinfo hints;
		mem_zero(&hints, sizeof(hints));

		if(types == NETTYPE_IPV4)
			hints.ai_family = AF_INET;
		else if(types == NETTYPE_IPV6)
			hints.ai_family = AF_INET6;
		else
			hints.ai_family = AF_UNSPEC;

		struct addrinfo *result = nullptr;
		int e = getaddrinfo(host, nullptr, &hints, &result);
		if(!result)
		{
			return net_host_lookup_fallback(host, addr, types, port);
		}

		if(e != 0)
		{
			freeaddrinfo(result);
			return net_host_lookup_fallback(host, addr, types, port);
		}

		sockaddr_to_netaddr(result->ai_addr, result->ai_addrlen, addr);
		addr->port = port;
		freeaddrinfo(result);
		return 0;
	}

	int net_host_lookup(const char *hostname, NETADDR *addr, int types)
	{
		const char *ws_hostname = str_startswith(hostname, "ws://");
		if(ws_hostname)
		{
			if((types & NETTYPE_WEBSOCKET_IPV4) == 0)
			{
				return -1;
			}
			int result = net_host_lookup_impl(ws_hostname, addr, NETTYPE_IPV4);
			if(result == 0 && addr->type == NETTYPE_IPV4)
			{
				addr->type = NETTYPE_WEBSOCKET_IPV4;
			}
			return result;
		}
		return net_host_lookup_impl(hostname, addr, types & ~NETTYPE_WEBSOCKET_IPV4);
	}

	std::string net_error_message()
	{
		const int error = net_errno();
#if defined(CONF_FAMILY_WINDOWS)
		const std::string message = windows_format_system_message(error);
		return std::to_string(error) + " '" + message + "'";
#else
		return std::to_string(error) + " '" + strerror(error) + "'";
#endif
	}

	int net_addr_comp(const NETADDR *a, const NETADDR *b)
	{
		return mem_comp(a, b, sizeof(NETADDR));
	}

	bool NETADDR::operator==(const NETADDR &other) const
	{
		return net_addr_comp(this, &other) == 0;
	}

	int net_addr_comp_noport(const NETADDR *a, const NETADDR *b)
	{
		NETADDR ta = *a, tb = *b;
		ta.port = tb.port = 0;

		return net_addr_comp(&ta, &tb);
	}

	void net_addr_str_v6(const unsigned short ip[8], int port, char *buffer, int buffer_size)
	{
		int longest_seq_len = 0;
		int longest_seq_start = -1;
		int w = 0;
		int i;
		{
			int seq_len = 0;
			int seq_start = -1;
			// Determine longest sequence of zeros.
			for(i = 0; i < 8 + 1; i++)
			{
				if(seq_start != -1)
				{
					if(i == 8 || ip[i] != 0)
					{
						if(longest_seq_len < seq_len)
						{
							longest_seq_len = seq_len;
							longest_seq_start = seq_start;
						}
						seq_len = 0;
						seq_start = -1;
					}
					else
					{
						seq_len += 1;
					}
				}
				else
				{
					if(i != 8 && ip[i] == 0)
					{
						seq_start = i;
						seq_len = 1;
					}
				}
			}
		}
		if(longest_seq_len <= 1)
		{
			longest_seq_len = 0;
			longest_seq_start = -1;
		}
		w += str_copy(buffer + w, "[", buffer_size - w);
		for(i = 0; i < 8; i++)
		{
			if(longest_seq_start <= i && i < longest_seq_start + longest_seq_len)
			{
				if(i == longest_seq_start)
				{
					w += str_copy(buffer + w, "::", buffer_size - w);
				}
			}
			else
			{
				const char *colon = (i == 0 || i == longest_seq_start + longest_seq_len) ? "" : ":";
				w += str_format(buffer + w, buffer_size - w, "%s%x", colon, ip[i]);
			}
		}
		w += str_copy(buffer + w, "]", buffer_size - w);
		if(port >= 0)
		{
			str_format(buffer + w, buffer_size - w, ":%d", port);
		}
	}

	void net_addr_str(const NETADDR *addr, char *string, int max_length, bool add_port)
	{
		if(addr->type & NETTYPE_IPV4 || addr->type & NETTYPE_WEBSOCKET_IPV4)
		{
			if(add_port)
			{
				str_format(string, max_length, "%d.%d.%d.%d:%d", addr->ip[0], addr->ip[1], addr->ip[2], addr->ip[3], addr->port);
			}
			else
			{
				str_format(string, max_length, "%d.%d.%d.%d", addr->ip[0], addr->ip[1], addr->ip[2], addr->ip[3]);
			}
		}
		else if(addr->type & NETTYPE_IPV6)
		{
			unsigned short ip[8];
			for(int i = 0; i < 8; i++)
			{
				ip[i] = (addr->ip[i * 2] << 8) | (addr->ip[i * 2 + 1]);
			}
			int port = add_port ? addr->port : -1;
			net_addr_str_v6(ip, port, string, max_length);
		}
		else
		{
			dbg_assert(false, "unknown NETADDR type %d", addr->type);
		}
	}

	bool net_addr_is_local(const NETADDR *addr)
	{
		char addr_str[NETADDR_MAXSTRSIZE];
		net_addr_str(addr, addr_str, sizeof(addr_str), true);

		if(addr->ip[0] == 127 || addr->ip[0] == 10 || (addr->ip[0] == 192 && addr->ip[1] == 168) || (addr->ip[0] == 172 && (addr->ip[1] >= 16 && addr->ip[1] <= 31)))
			return true;

		if(str_startswith(addr_str, "[fe80:") || str_startswith(addr_str, "[::1"))
			return true;

		return false;
	}

	static int parse_int(int *out, const char **str)
	{
		int i = 0;
		*out = 0;
		if(!str_isnum(**str))
			return -1;

		i = **str - '0';
		(*str)++;

		while(true)
		{
			if(!str_isnum(**str))
			{
				*out = i;
				return 0;
			}

			i = (i * 10) + (**str - '0');
			(*str)++;
		}

		return 0;
	}

	static int parse_char(char c, const char **str)
	{
		if(**str != c)
			return -1;
		(*str)++;
		return 0;
	}

	static int parse_uint8(unsigned char *out, const char **str)
	{
		int i;
		if(parse_int(&i, str) != 0)
			return -1;
		if(i < 0 || i > 0xff)
			return -1;
		*out = i;
		return 0;
	}

	static int parse_uint16(unsigned short *out, const char **str)
	{
		int i;
		if(parse_int(&i, str) != 0)
			return -1;
		if(i < 0 || i > 0xffff)
			return -1;
		*out = i;
		return 0;
	}

	int net_addr_from_str(NETADDR *addr, const char *string)
	{
		const char *str = string;
		mem_zero(addr, sizeof(NETADDR));

		if(str[0] == '[')
		{
			/* ipv6 */
			sockaddr_in6 sa6;
			char buf[128];
			int i;
			str++;
			for(i = 0; i < 127 && str[i] && str[i] != ']'; i++)
				buf[i] = str[i];
			buf[i] = 0;
			str += i;
#if defined(CONF_FAMILY_WINDOWS)
			{
				int size;
				sa6.sin6_family = AF_INET6;
				size = (int)sizeof(sa6);
				if(WSAStringToAddressA(buf, AF_INET6, nullptr, (sockaddr *)&sa6, &size) != 0)
					return -1;
			}
#else
			sa6.sin6_family = AF_INET6;

			if(inet_pton(AF_INET6, buf, &sa6.sin6_addr) != 1)
				return -1;
#endif
			sockaddr_to_netaddr((sockaddr *)&sa6, sizeof(sa6), addr);

			if(*str == ']')
			{
				str++;
				if(*str == ':')
				{
					str++;
					if(parse_uint16(&addr->port, &str))
						return -1;
				}
				else
				{
					addr->port = 0;
				}
			}
			else
				return -1;

			return 0;
		}
		else
		{
			/* ipv4 */
			if(parse_uint8(&addr->ip[0], &str))
				return -1;
			if(parse_char('.', &str))
				return -1;
			if(parse_uint8(&addr->ip[1], &str))
				return -1;
			if(parse_char('.', &str))
				return -1;
			if(parse_uint8(&addr->ip[2], &str))
				return -1;
			if(parse_char('.', &str))
				return -1;
			if(parse_uint8(&addr->ip[3], &str))
				return -1;
			if(*str == ':')
			{
				str++;
				if(parse_uint16(&addr->port, &str))
					return -1;
			}
			if(*str != '\0')
				return -1;

			addr->type = NETTYPE_IPV4;
		}

		return 0;
	}

	int net_errno()
	{
#if defined(CONF_FAMILY_WINDOWS)
		return WSAGetLastError();
#else
		return errno;
#endif
	}

	static int net_set_blocking_impl(NETSOCKET sock, bool blocking)
	{
		unsigned long mode = blocking ? 0 : 1;
		const char *mode_str = blocking ? "blocking" : "non-blocking";
		int sockets[] = {sock->ipv4sock, sock->ipv6sock};
		const char *socket_str[] = {"IPv4", "IPv6"};

		for(size_t i = 0; i < std::size(sockets); ++i)
		{
			if(sockets[i] >= 0)
			{
#if defined(CONF_FAMILY_WINDOWS)
				if(ioctlsocket(sockets[i], FIONBIO, &mode) != NO_ERROR)
				{
					log_error("net", "Setting %s mode for %s socket failed (%s)", socket_str[i], mode_str, net_error_message().c_str());
				}
#else
				if(ioctl(sockets[i], FIONBIO, &mode) == -1)
				{
					log_error("net", "Setting %s mode for %s socket failed (%s)", socket_str[i], mode_str, net_error_message().c_str());
				}
#endif
			}
		}

		return 0;
	}

	int net_set_non_blocking(NETSOCKET sock)
	{
		return net_set_blocking_impl(sock, false);
	}

	int net_set_blocking(NETSOCKET sock)
	{
		return net_set_blocking_impl(sock, true);
	}

	int net_would_block()
	{
#if defined(CONF_FAMILY_WINDOWS)
		return net_errno() == WSAEWOULDBLOCK;
#else
		return net_errno() == EWOULDBLOCK;
#endif
	}

	static bool net_address_in_use()
	{
#if defined(CONF_FAMILY_WINDOWS)
		return net_errno() == WSAEADDRINUSE;
#else
		return net_errno() == EADDRINUSE;
#endif
	}

	static void priv_net_close_socket(int sock)
	{
#if defined(CONF_FAMILY_WINDOWS)
		dbg_assert(closesocket(sock) == 0, "closesocket failure (%s)", net_error_message().c_str());
#else
		dbg_assert(close(sock) == 0, "close failure (%s)", net_error_message().c_str());
#endif
	}

	static void priv_net_close_all_sockets(NETSOCKET sock)
	{
		if(sock->ipv4sock >= 0)
		{
			priv_net_close_socket(sock->ipv4sock);
			sock->ipv4sock = -1;
			sock->type &= ~NETTYPE_IPV4;
		}

#if defined(CONF_WEBSOCKETS)
		if(sock->web_ipv4sock >= 0)
		{
			websocket_destroy(sock->web_ipv4sock);
			sock->web_ipv4sock = -1;
			sock->type &= ~NETTYPE_WEBSOCKET_IPV4;
		}
#endif

		if(sock->ipv6sock >= 0)
		{
			priv_net_close_socket(sock->ipv6sock);
			sock->ipv6sock = -1;
			sock->type &= ~NETTYPE_IPV6;
		}

#if defined(CONF_WEBSOCKETS)
		if(sock->web_ipv6sock >= 0)
		{
			websocket_destroy(sock->web_ipv6sock);
			sock->web_ipv6sock = -1;
			sock->type &= ~NETTYPE_WEBSOCKET_IPV6;
		}
#endif

		free(sock);
	}

	// `address_in_use` is only ever set to `true`, so the same variable can be
	// passed for all sockets of one `NETSOCKET`.
	static int priv_net_create_socket(int domain, int type, const NETADDR *bindaddr, bool *address_in_use)
	{
		int sock = socket(domain, type, 0);
		if(sock < 0)
		{
			log_error("net", "Failed to create socket with domain %d and type %d (%s)", domain, type, net_error_message().c_str());
			return -1;
		}

#if defined(CONF_FAMILY_UNIX)
		// On TCP sockets set SO_REUSEADDR to fix port rebind on restart
		if(domain == AF_INET && type == SOCK_STREAM)
		{
			int reuse_addr = 1;
			if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse_addr, sizeof(reuse_addr)) != 0)
			{
				log_error("net", "Setting SO_REUSEADDR failed with domain %d and type %d (%s)", domain, type, net_error_message().c_str());
			}
		}
#elif defined(CONF_FAMILY_WINDOWS)
		{
			// Ensure exclusive use of address, otherwise it's possible on Windows to bind to the same address and port with another socket.
			// See https://learn.microsoft.com/en-us/windows/win32/winsock/using-so-reuseaddr-and-so-exclusiveaddruse (last update 06/14/2022)
			int exclusive_addr_use = 1;
			if(setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char *)&exclusive_addr_use, sizeof(exclusive_addr_use)) != 0)
			{
				log_error("net", "Setting SO_EXCLUSIVEADDRUSE failed with domain %d and type %d (%s)", domain, type, net_error_message().c_str());
			}
		}
#endif

		// Set to IPv6-only if that's what we are creating, to ensure that dual-stack does not block the same IPv4 port.
#if defined(IPV6_V6ONLY)
		if(domain == AF_INET6)
		{
			int ipv6only = 1;
			if(setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&ipv6only, sizeof(ipv6only)) != 0)
			{
				log_error("net", "Setting IPV6_V6ONLY failed with domain %d and type %d (%s)", domain, type, net_error_message().c_str());
			}
		}
#endif

		sockaddr_storage addr;
		socklen_t addr_len;
		if(bindaddr->type == NETTYPE_IPV4)
		{
			netaddr_to_sockaddr_in(bindaddr, (sockaddr_in *)&addr);
			addr_len = sizeof(sockaddr_in);
		}
		else if(bindaddr->type == NETTYPE_IPV6)
		{
			netaddr_to_sockaddr_in6(bindaddr, (sockaddr_in6 *)&addr);
			addr_len = sizeof(sockaddr_in6);
		}
		else
		{
			dbg_assert(false, "socket type invalid: %d", type);
		}

		if(bind(sock, (sockaddr *)&addr, addr_len) != 0)
		{
			if(net_address_in_use())
			{
				*address_in_use = true;
			}
			log_error("net", "Failed to bind socket with domain %d and type %d (%s)", domain, type, net_error_message().c_str());
			priv_net_close_socket(sock);
			return -1;
		}

		return sock;
	}

	NETSOCKET net_tcp_create(NETADDR bindaddr)
	{
		NETSOCKET sock = (NETSOCKET_INTERNAL *)malloc(sizeof(*sock));
		*sock = invalid_socket;
		bool address_in_use = false;

		if(bindaddr.type & NETTYPE_IPV4)
		{
			NETADDR bindaddr_ipv4 = bindaddr;
			bindaddr_ipv4.type = NETTYPE_IPV4;
			const int socket4 = priv_net_create_socket(AF_INET, SOCK_STREAM, &bindaddr_ipv4, &address_in_use);
			if(socket4 >= 0)
			{
				sock->type |= NETTYPE_IPV4;
				sock->ipv4sock = socket4;
			}
		}

		if(bindaddr.type & NETTYPE_IPV6)
		{
			NETADDR bindaddr_ipv6 = bindaddr;
			bindaddr_ipv6.type = NETTYPE_IPV6;
			const int socket6 = priv_net_create_socket(AF_INET6, SOCK_STREAM, &bindaddr_ipv6, &address_in_use);
			if(socket6 >= 0)
			{
				sock->type |= NETTYPE_IPV6;
				sock->ipv6sock = socket6;
			}
		}

		if(sock->type == NETTYPE_INVALID || address_in_use)
		{
			priv_net_close_all_sockets(sock);
			sock = nullptr;
		}

		return sock;
	}

	int net_tcp_listen(NETSOCKET sock, int backlog)
	{
		int err = -1;
		if(sock->ipv4sock >= 0)
		{
			err = listen(sock->ipv4sock, backlog);
		}
		if(sock->ipv6sock >= 0)
		{
			err = listen(sock->ipv6sock, backlog);
		}
		return err;
	}

	int net_tcp_accept(NETSOCKET sock, NETSOCKET *new_sock, NETADDR *a)
	{
		*new_sock = nullptr;

		if(sock->ipv4sock >= 0)
		{
			sockaddr_storage addr;
			socklen_t sockaddr_len = sizeof(addr);

			int s = accept(sock->ipv4sock, (sockaddr *)&addr, &sockaddr_len);
			if(s != -1)
			{
				sockaddr_to_netaddr((sockaddr *)&addr, sockaddr_len, a);

				*new_sock = (NETSOCKET_INTERNAL *)malloc(sizeof(**new_sock));
				**new_sock = invalid_socket;
				(*new_sock)->type = NETTYPE_IPV4;
				(*new_sock)->ipv4sock = s;
				return s;
			}
		}

		if(sock->ipv6sock >= 0)
		{
			sockaddr_storage addr;
			socklen_t sockaddr_len = sizeof(addr);

			int s = accept(sock->ipv6sock, (sockaddr *)&addr, &sockaddr_len);
			if(s != -1)
			{
				*new_sock = (NETSOCKET_INTERNAL *)malloc(sizeof(**new_sock));
				**new_sock = invalid_socket;
				sockaddr_to_netaddr((sockaddr *)&addr, sockaddr_len, a);
				(*new_sock)->type = NETTYPE_IPV6;
				(*new_sock)->ipv6sock = s;
				return s;
			}
		}

		return -1;
	}

	int net_tcp_connect(NETSOCKET sock, const NETADDR *a)
	{
		if(a->type & NETTYPE_IPV4)
		{
			if(sock->ipv4sock < 0)
				return -2;
			sockaddr_in addr;
			netaddr_to_sockaddr_in(a, &addr);
			return connect(sock->ipv4sock, (sockaddr *)&addr, sizeof(addr));
		}

		if(a->type & NETTYPE_IPV6)
		{
			if(sock->ipv6sock < 0)
				return -2;
			sockaddr_in6 addr;
			netaddr_to_sockaddr_in6(a, &addr);
			return connect(sock->ipv6sock, (sockaddr *)&addr, sizeof(addr));
		}

		return -1;
	}

	int net_tcp_connect_non_blocking(NETSOCKET sock, NETADDR bindaddr)
	{
		net_set_non_blocking(sock);
		int res = net_tcp_connect(sock, &bindaddr);
		net_set_blocking(sock);
		return res;
	}

	int net_tcp_send(NETSOCKET sock, const void *data, int size)
	{
		int bytes = -1;

		if(sock->ipv4sock >= 0)
		{
			bytes = send(sock->ipv4sock, (const char *)data, size, 0);
		}
		if(sock->ipv6sock >= 0)
		{
			bytes = send(sock->ipv6sock, (const char *)data, size, 0);
		}

		return bytes;
	}

	int net_tcp_recv(NETSOCKET sock, void *data, int maxsize)
	{
		int bytes = -1;

		if(sock->ipv4sock >= 0)
		{
			bytes = recv(sock->ipv4sock, (char *)data, maxsize, 0);
		}
		if(sock->ipv6sock >= 0)
		{
			bytes = recv(sock->ipv6sock, (char *)data, maxsize, 0);
		}

		return bytes;
	}

	void net_tcp_close(NETSOCKET sock)
	{
		priv_net_close_all_sockets(sock);
	}

} // namespace polybob
