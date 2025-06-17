#include <polybob/base/detect.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/base/system/net.h>
#include <polybob/base/system/str.h>

#include <arpa/inet.h>
#include <netdb.h>

namespace polybob {

#define AF_WEBSOCKET_INET (0xee)

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

static void sockaddr_to_netaddr(const struct sockaddr *src, NETADDR *dst)
{
	if(src->sa_family == AF_INET)
	{
		mem_zero(dst, sizeof(NETADDR));
		dst->type = NETTYPE_IPV4;
		dst->port = htons(((struct sockaddr_in *)src)->sin_port);
		mem_copy(dst->ip, &((struct sockaddr_in *)src)->sin_addr.s_addr, 4);
	}
	else if(src->sa_family == AF_WEBSOCKET_INET)
	{
		mem_zero(dst, sizeof(NETADDR));
		dst->type = NETTYPE_WEBSOCKET_IPV4;
		dst->port = htons(((struct sockaddr_in *)src)->sin_port);
		mem_copy(dst->ip, &((struct sockaddr_in *)src)->sin_addr.s_addr, 4);
	}
	else if(src->sa_family == AF_INET6)
	{
		mem_zero(dst, sizeof(NETADDR));
		dst->type = NETTYPE_IPV6;
		dst->port = htons(((struct sockaddr_in6 *)src)->sin6_port);
		mem_copy(dst->ip, &((struct sockaddr_in6 *)src)->sin6_addr.s6_addr, 16);
	}
	else
	{
		mem_zero(dst, sizeof(struct sockaddr));
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
		return -1;

	if(e != 0)
	{
		freeaddrinfo(result);
		return -1;
	}

	sockaddr_to_netaddr(result->ai_addr, addr);
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
		struct sockaddr_in6 sa6;
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
			if(WSAStringToAddressA(buf, AF_INET6, nullptr, (struct sockaddr *)&sa6, &size) != 0)
				return -1;
		}
#else
		sa6.sin6_family = AF_INET6;

		if(inet_pton(AF_INET6, buf, &sa6.sin6_addr) != 1)
			return -1;
#endif
		sockaddr_to_netaddr((struct sockaddr *)&sa6, addr);

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

} // namespace polybob
