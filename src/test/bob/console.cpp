#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/game/generated/protocolglue.h>

#include <bob/console.h>

#include "bobtest.h"

void ExpectEscape(CBobTest &BobTest, const char *pInput, const char *pEscaped)
{
	char aEscaped[512];
	CBobConsole::EscapeRconString(aEscaped, pInput);
	EXPECT_STREQ(aEscaped, pEscaped);
}

int main()
{
	CBobTest BobTest;
	EXPECT_STREQ("foo", "foo");
	EXPECT_EQ(10, 5 + 5);
	EXPECT_EQ(true, true);
	EXPECT_NE(true, false);

	log_info("test", "uwu");

	ExpectEscape(BobTest, "hello \" world", "hello \\\" world");
	ExpectEscape(BobTest, "foo\\", "foo\\\\");
	ExpectEscape(BobTest, "foo\\\"bar", "foo\\\\\\\"bar");
}
