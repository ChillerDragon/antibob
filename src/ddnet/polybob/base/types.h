#pragma once

#include <cstdint>
#include <ctime>

typedef void *IOHANDLE;

enum
{
	/**
	 * The maximum bytes necessary to encode one Unicode codepoint with UTF-8.
	 */
	UTF8_BYTE_LENGTH = 4,

	IO_MAX_PATH_LENGTH = 512,

	NETADDR_MAXSTRSIZE = 1 + (8 * 4 + 7) + 1 + 1 + 5 + 1, // [XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX]:XXXXX

	NETTYPE_LINK_BROADCAST = 4,

	NETTYPE_INVALID = 0,
	NETTYPE_IPV4 = 1,
	NETTYPE_IPV6 = 2,
	NETTYPE_WEBSOCKET_IPV4 = 8,
	/**
	 * 0.7 address. This is a flag in NETADDR to avoid introducing a parameter to every networking function
	 * to differenciate between 0.6 and 0.7 connections.
	 */
	NETTYPE_TW7 = 16,

	NETTYPE_ALL = NETTYPE_IPV4 | NETTYPE_IPV6 | NETTYPE_WEBSOCKET_IPV4,
	NETTYPE_MASK = NETTYPE_ALL | NETTYPE_LINK_BROADCAST | NETTYPE_TW7,
};

