#include "../bobtest.h"

#include <bob/config.h>
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

	{
		CBobResult Result("foo \\\"bar", "vv");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, false);
		EXPECT_STREQ(aError, "argument 'at offset 1' has value 'foo' which is not a valid victim");
	}

	{
		CBobResult Result("1 2", "vv");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, true);
		EXPECT_STREQ(aError, "");
	}

	{
		CBobResult Result("-1", "v");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, false);
		EXPECT_STREQ(aError, "argument 'at offset 1' has value -1 which is not a valid victim (to small)");
	}

	{
		CBobResult Result("99999", "v");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, false);
		EXPECT_STREQ(aError, "argument 'at offset 1' has value 99999 which is not a valid victim (to big)");
	}

	{
		CBobResult Result("99999", "v[target]");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, false);
		EXPECT_STREQ(aError, "argument 'target' has value 99999 which is not a valid victim (to big)");
	}

	{
		CBobResult Result("all", "v");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, true);
		EXPECT_STREQ(aError, "");
	}

	{
		CBobResult Result("2", "v");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, true);
		EXPECT_STREQ(aError, "");
	}

	{
		CBobResult Result("2", "?v");
		Ok = Result.ParseArgs(aError, sizeof(aError));
		EXPECT_EQ(Ok, true);
		EXPECT_STREQ(aError, "");

		EXPECT_EQ(Result.GetVictim(0), 2);
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

	Ok = CBobConsole::ParseParams(vParams, "i?ss", aError, sizeof(aError));
	EXPECT_EQ(Ok, false);
	EXPECT_STREQ(aError, "got non optional param after optional param in 'i?ss'");
	EXPECT_EQ(vParams.size(), 2);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "sssisisiisisis", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 14);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "s s s", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 3);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "s s s  i      [age]", aError, sizeof(aError));
	EXPECT_EQ(Ok, false);
	EXPECT_STREQ(aError, "got invalid type for param in 's s s  i      [age]'");
	EXPECT_EQ(vParams.size(), 4);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "?", aError, sizeof(aError));
	EXPECT_EQ(Ok, false);
	EXPECT_STREQ(aError, "nameless optional parameter in '?'");
	EXPECT_EQ(vParams.size(), 0);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "\?\?\?", aError, sizeof(aError));
	EXPECT_EQ(Ok, false);
	EXPECT_STREQ(aError, "nameless optional parameter in '\?\?\?'");
	EXPECT_EQ(vParams.size(), 0);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "s[name]i[level]s['foo'|'bar'|'baz']", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 3);
	EXPECT_STREQ(vParams.at(2).m_aDescription, "'foo'|'bar'|'baz'");
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "s[name]r[password]", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 2);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "s[name]r[password]?i[pin]", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 3);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "   s[name]        r[password]             ?i[pin]", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 3);
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "i[client id] s[ HELLO   WORLD ]", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 2);
	EXPECT_STREQ(vParams.at(1).m_aDescription, " HELLO   WORLD ");
	vParams.clear();

	Ok = CBobConsole::ParseParams(vParams, "v[source]v[target]", aError, sizeof(aError));
	EXPECT_EQ(Ok, true);
	EXPECT_STREQ(aError, "");
	EXPECT_EQ(vParams.size(), 2);
	vParams.clear();
}

static void TestExec(CBobTest &BobTest)
{
	CBobConsole Console;
	CBobConfigManager Config;
	Config.OnInit();
	Console.OnInit(&Config);

	Console.Register(
		"expect_10",
		"i",
		[](CBobResult *pResult, void *pUserData) {
			CBobTest *pTest = (CBobTest *)pUserData;
			pTest->ExpectEq(pResult->GetInteger(0), 10, __FILE__, __LINE__);
		},
		&BobTest,
		"");

	Console.Register(
		"default_vic",
		"?v",
		[](CBobResult *pResult, void *pUserData) {
			CBobTest *pTest = (CBobTest *)pUserData;
			pTest->ExpectEq(pResult->GetVictim(0), 5, __FILE__, __LINE__);
		},
		&BobTest,
		"");

	Console.ExecuteCmd(-1, "expect_10 10");
	Console.ExecuteCmd(5, "default_vic");
}

int main()
{
	CBobTest BobTest;

	TestExpectEscape(BobTest);
	TestArgParse(BobTest);
	TestParseParams(BobTest);
	TestExec(BobTest);
}
