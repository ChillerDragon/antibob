#include "bobtest.h"

#include <bob/console.h>
#include <bob/str.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/game/generated/protocolglue.h>

int main()
{
	CBobTest BobTest;

	int Minutes;
	bool Success = str_extract_ban_minutes(
		"You have been banned for 5 minutes (Too many remote console authentication tries)",
		&Minutes);
	EXPECT_EQ(Success, true);
	EXPECT_EQ(Minutes, 5);

	Success = str_extract_ban_minutes(
		"You have been banned for -5 minutes (Too many remote console authentication tries)",
		&Minutes);
	EXPECT_EQ(Success, true);
	EXPECT_EQ(Minutes, -5);

	Success = str_extract_ban_minutes(
		"You have been banned for 5 seconds",
		&Minutes);
	EXPECT_EQ(Success, false);
	EXPECT_EQ(Minutes, 5);
}
