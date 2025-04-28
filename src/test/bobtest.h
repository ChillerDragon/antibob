#pragma once

#define EXPECT_NE(actual, expected) BobTest.ExpectNe(actual, expected, __FILE__, __LINE__);
#define EXPECT_EQ(actual, expected) BobTest.ExpectEq(actual, expected, __FILE__, __LINE__);
#define EXPECT_STREQ(actual, expected) BobTest.ExpectStrEq(actual, expected, __FILE__, __LINE__);

class CBobTest
{
	int m_NumErrors = 0;

public:
	bool ExpectNe(int Actual, int Expected, const char *pFilename, int LineNumber);
	bool ExpectEq(int Actual, int Expected, const char *pFilename, int LineNumber);
	bool ExpectStrEq(const char *pActual, const char *pExpected, const char *pFilename, int LineNumber);
	~CBobTest();
};
