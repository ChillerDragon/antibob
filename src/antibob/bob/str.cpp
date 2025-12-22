#include "polybob/base/system.h"

#include <bob/str.h>

using namespace polybob;

bool str_extract_ban_minutes(const char *pBanReason, int *pMinutes)
{
	*pMinutes = 0;

	if(!str_startswith(pBanReason, "You have been banned for "))
		return false;

	const char *pStr = pBanReason;
	pStr += str_length("You have been banned for ");

	*pMinutes = atoi(pStr);

	pStr = str_find(pStr, " ");
	// missing space after minutes number
	if(!pStr)
		return false;

	// what ever we parsed as minutes was not what we expected
	// if it is not followed by " minutes (reason)"
	if(!str_startswith(pStr, " minutes ("))
		return false;
	return true;
}
