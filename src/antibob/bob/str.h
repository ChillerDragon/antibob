#pragma once

// reads the `pBanReason` string
// and searches for the ban time in minutes
// that value will be written to `pMinutes`
//
// returns false on parse error
bool str_extract_ban_minutes(const char *pBanReason, int *pMinutes);
