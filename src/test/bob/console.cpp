#include "../bobtest.h"

#include <bob/console.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>

static void ExpectEscape(CBobTest &BobTest, const char *pInput, const char *pEscaped)
{
	char aEscaped[512];
	CBobConsole::EscapeRconString(aEscaped, pInput);
	EXPECT_STREQ(aEscaped, pEscaped);
}

static void TestArgParse(CBobTest &BobTest)
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

	{
		CBobResult Result("foo \"bar", "ss");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, false);
		EXPECT_STREQ(aError, "Expected closing quote (\") but got end of input");
	}

	{
		CBobResult Result("foo \\\"bar", "ss");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, true);
		EXPECT_STREQ(aError, "");
	}
}

static void TestExpectEscape(CBobTest &BobTest)
{
	ExpectEscape(BobTest, "hello \" world", "hello \\\" world");
	ExpectEscape(BobTest, "foo\\", "foo\\\\");
	ExpectEscape(BobTest, "foo\\\"bar", "foo\\\\\\\"bar");
	ExpectEscape(BobTest, "'\"'", "'\\\"'");
	ExpectEscape(BobTest, "\";shutdown", "\\\";shutdown");
}

static void TestParseParams(CBobTest &BobTest)
{
	std::vector<CBobParam> vParams;
	char aError[512];
	bool Ok;

	Ok = CBobConsole::ParseParams(vParams, "iii", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 3);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "i", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 1);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 0);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "i[client_id]?s[name]", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 2);
	vParams.clear();

	//                                                   v missing "s" for string
	Ok = CBobConsole::ParseParams(vParams, "i[client_id]?[name]", aError, sizeof(aError));
	EXPECT_EQ(Ok, false);
	EXPECT_STREQ(aError, "missing type for optional parameter in i[client_id]?[name]");
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "i?s", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 2);
	vParams.clear();
}

int main()
{
	CBobTest BobTest;

	TestExpectEscape(BobTest);
	TestArgParse(BobTest);
	TestParseParams(BobTest);
}
