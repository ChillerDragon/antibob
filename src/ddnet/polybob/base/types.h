#pragma once

#include <cstdint>
#include <ctime>

namespace polybob
{

	typedef void *IOHANDLE;

	typedef int (*FS_LISTDIR_CALLBACK)(const char *name, int is_dir, int dir_type, void *user);

	typedef struct
	{
		const char *m_pName;
		time_t m_TimeCreated; // seconds since UNIX Epoch
		time_t m_TimeModified; // seconds since UNIX Epoch
	} CFsFileInfo;

	typedef int (*FS_LISTDIR_CALLBACK_FILEINFO)(const CFsFileInfo *info, int is_dir, int dir_type, void *user);

	enum
	{
		IO_MAX_PATH_LENGTH = 512,
	};

	/**
	 * The maximum bytes necessary to encode one Unicode codepoint with UTF-8.
	 *
	 * @ingroup Strings
	 */
	inline constexpr auto UTF8_BYTE_LENGTH = 4;

	/**
	 * @ingroup Network-General
	 */
	typedef struct NETSOCKET_INTERNAL *NETSOCKET;

	/**
	 * @ingroup Network-General
	 */
	inline constexpr auto NETTYPE_INVALID = 0;

	/**
	 * @ingroup Network-General
	 */
	inline constexpr auto NETTYPE_IPV4 = 1 << 0;

	/**
	 * @ingroup Network-General
	 */
	inline constexpr auto NETTYPE_IPV6 = 1 << 1;

	/**
	 * @ingroup Network-General
	 */
	inline constexpr auto NETTYPE_WEBSOCKET_IPV4 = 1 << 2;

	/**
	 * @ingroup Network-General
	 */
	inline constexpr auto NETTYPE_WEBSOCKET_IPV6 = 1 << 3;

	/**
	 * @ingroup Network-General
	 */
	inline constexpr auto NETTYPE_LINK_BROADCAST = 1 << 4;

	/**
	 * 0.7 address. This is a flag in NETADDR to avoid introducing a parameter to every networking function
	 * to differentiate between 0.6 and 0.7 connections.
	 *
	 * @ingroup Network-General
	 */
	inline constexpr auto NETTYPE_TW7 = 1 << 5;

	/**
	 * @ingroup Network-General
	 */
	inline constexpr auto NETTYPE_ALL = NETTYPE_IPV4 | NETTYPE_IPV6 | NETTYPE_WEBSOCKET_IPV4 | NETTYPE_WEBSOCKET_IPV6;

	/**
	 * @ingroup Network-General
	 */
	inline constexpr auto NETTYPE_MASK = NETTYPE_ALL | NETTYPE_LINK_BROADCAST | NETTYPE_TW7;

	/**
	 * @ingroup Network-Address
	 */
	inline constexpr auto NETADDR_MAXSTRSIZE = 1 + (8 * 4 + 7) + 1 + 1 + 5 + 1; // [XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX]:XXXXX

	/**
	 * @ingroup Network-General
	 */
	typedef struct NETADDR
	{
		unsigned int type;
		unsigned char ip[16];
		unsigned short port;

		bool operator==(const NETADDR &other) const;
		bool operator!=(const NETADDR &other) const { return !(*this == other); }
	} NETADDR;

} // namespace polybob
