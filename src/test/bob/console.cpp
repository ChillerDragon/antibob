#include "bobtest.h"

#include <bob/console.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>

void ExpectEscape(CBobTest &BobTest, const char *pInput, const char *pEscaped)
{
	char aEscaped[512];
	CBobConsole::EscapeRconString(aEscaped, pInput);
	EXPECT_STREQ(aEscaped, pEscaped);
}

void TestArgParse(CBobTest &BobTest)
{
	bool Ok;
	char aError[512];

	{
		CBobResult Result("hello world", "ss");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, true);
	}

	{
		CBobResult Result("hello world too many args", "ss");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, false);
		EXPECT_STREQ(aError, "got 5 out of 2 arguments ss");
	}

	{
		CBobResult Result("foo \"hello world\" bar", "sss");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, true);
		EXPECT_STREQ(aError, "");
	}
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
	ExpectEscape(BobTest, "'\"'", "'\\\"'");
	ExpectEscape(BobTest, "\";shutdown", "\\\";shutdown");

	TestArgParse(BobTest);
}
