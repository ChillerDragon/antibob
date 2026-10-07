#pragma once

#include <polybob/base/types.h>

namespace polybob
{

	/**
	 * Looks up the ip of a hostname.
	 *
	 * @ingroup Network-General
	 *
	 * @param hostname Host name to look up.
	 * @param addr The output address to write to.
	 * @param types The type of IP that should be returned.
	 *
	 * @return `0` on success.
	 */
	int net_host_lookup(const char *hostname, NETADDR *addr, int types);

	/**
	 * Compares two network addresses.
	 *
	 * @ingroup Network-General
	 *
	 * @param a Address to compare.
	 * @param b Address to compare to.
	 *
	 * @return `< 0` if address a is less than address b.
	 * @return `0` if address a is equal to address b.
	 * @return `> 0` if address a is greater than address b.
	 */
	int net_addr_comp(const NETADDR *a, const NETADDR *b);

	/**
	 * Compares two network addresses ignoring port.
	 *
	 * @ingroup Network-General
	 *
	 * @param a Address to compare.
	 * @param b Address to compare to.
	 *
	 * @return `< 0` if address a is less than address b.
	 * @return `0` if address a is equal to address b.
	 * @return `> 0` if address a is greater than address b.
	 */
	int net_addr_comp_noport(const NETADDR *a, const NETADDR *b);

	/**
	 * Turns a network address into a representative string.
	 *
	 * @ingroup Network-General
	 *
	 * @param addr Address to turn into a string.
	 * @param string Buffer to fill with the string.
	 * @param max_length Maximum size of the string.
	 * @param add_port Whether to add the port to the string.
	 *
	 * @remark The string will always be null-terminated.
	 */
	void net_addr_str(const NETADDR *addr, char *string, int max_length, bool add_port);

	/**
	 * Checks if an address is local.
	 *
	 * @ingroup Network-General
	 *
	 * @param addr Address to check.
	 *
	 * @return `true` if the address is local, `false` otherwise.
	 */
	bool net_addr_is_local(const NETADDR *addr);

	/**
	 * Turns string into a network address.
	 *
	 * @ingroup Network-General
	 *
	 * @param addr Address to fill in.
	 * @param string String to parse.
	 *
	 * @return `0` on success.
	 */
	int net_addr_from_str(NETADDR *addr, const char *string);

	/**
	 * If a network operation failed, the error code.
	 *
	 * @ingroup Network-General
	 *
	 * @returns The error code.
	 */
	int net_errno();

	/**
	 * If a network operation failed, the platform-specific error code and string.
	 *
	 * @ingroup Network-General
	 *
	 * @returns The error code and string combined into one string.
	 */
	std::string net_error_message();

	/**
	 * Creates a TCP socket.
	 *
	 * In case a port is already in use on any of the protocol families, the whole
	 * bind operation fails. Otherwise, if binding at least one protocol family
	 * succeeds, the operation is treated as a success: the host may not support all
	 * protocol families.
	 *
	 * @ingroup Network-TCP
	 *
	 * @param bindaddr Address to bind the socket to.
	 *
	 * @return On success it returns an handle to the socket. On failure it returns `nullptr`.
	 */
	NETSOCKET net_tcp_create(NETADDR bindaddr);

	/**
	 * Make a socket not block on operations.
	 *
	 * @ingroup Network-General
	 *
	 * @param sock The socket to set non-blocking mode on.
	 *
	 * @returns `0` on success.
	 */
	int net_set_non_blocking(NETSOCKET sock);

	/**
	 * Make a socket block on operations.
	 *
	 * @ingroup Network-General
	 *
	 * @param sock The socket to set blocking mode on.
	 *
	 * @returns `0` on success.
	 */
	int net_set_blocking(NETSOCKET sock);

	/**
	 * Determines whether a network operation would block.
	 *
	 * @ingroup Network-General
	 *
	 * @returns `0` if wouldn't block, `1` if would block.
	 */
	int net_would_block();

	/**
	 * Makes the socket start listening for new connections.
	 *
	 * @ingroup Network-TCP
	 *
	 * @param sock Socket to start listen to.
	 * @param backlog Size of the queue of incoming connections to keep.
	 *
	 * @return `0` on success.
	 */
	int net_tcp_listen(NETSOCKET sock, int backlog);

	/**
	 * Polls a listening socket for a new connection.
	 *
	 * @ingroup Network-TCP
	 *
	 * @param sock Listening socket to poll.
	 * @param new_sock Pointer to a socket to fill in with the new socket.
	 * @param addr Pointer to an address that will be filled in the remote address, can be `nullptr`.
	 *
	 * @return A non-negative integer on success. Negative integer on failure.
	 */
	int net_tcp_accept(NETSOCKET sock, NETSOCKET *new_sock, NETADDR *addr);

	/**
	 * Connects one socket to another.
	 *
	 * @ingroup Network-TCP
	 *
	 * @param sock Socket to connect.
	 * @param addr Address to connect to.
	 *
	 * @return `0` on success.
	 *
	 */
	int net_tcp_connect(NETSOCKET sock, const NETADDR *addr);

	/**
	 * Connect a socket to a TCP address without blocking.
	 *
	 * @ingroup Network-TCP
	 *
	 * @param sock The socket to connect with.
	 * @param bindaddr The address to connect to.
	 *
	 * @returns `0` on success.
	 */
	int net_tcp_connect_non_blocking(NETSOCKET sock, NETADDR bindaddr);

	/**
	 * Sends data to a TCP stream.
	 *
	 * @ingroup Network-TCP
	 *
	 * @param sock Socket to send data to.
	 * @param data Pointer to the data to send.
	 * @param size Size of the data to send.
	 *
	 * @return Number of bytes sent. Negative value on failure.
	 */
	int net_tcp_send(NETSOCKET sock, const void *data, int size);

	/**
	 * Receives data from a TCP stream.
	 *
	 * @ingroup Network-TCP
	 *
	 * @param sock Socket to recvive data from.
	 * @param data Pointer to a buffer to write the data to.
	 * @param maxsize Maximum of data to write to the buffer.
	 *
	 * @return Number of bytes recvived. Negative value on failure. When in
	 * non-blocking mode, it returns 0 when there is no more data to be fetched.
	 */
	int net_tcp_recv(NETSOCKET sock, void *data, int maxsize);

	/**
	 * Closes a TCP socket.
	 *
	 * @ingroup Network-TCP
	 *
	 * @param sock Socket to close.
	 */
	void net_tcp_close(NETSOCKET sock);

} // namespace polybob
