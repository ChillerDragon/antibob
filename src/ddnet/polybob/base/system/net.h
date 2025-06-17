#pragma once

#include <polybob/base/types.h>

namespace polybob {

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

} // namespace polybob
