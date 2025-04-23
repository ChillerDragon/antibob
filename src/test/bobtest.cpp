#include <polybob/base/system.h>

#include <cstdio>

#include "bobtest.h"

bool CBobTest::ExpectEq(int Actual, int Expected, const char *pFilename, int LineNumber)
{
	if(Actual == Expected)
		return true;

	m_NumErrors++;
	fprintf(stderr, "[assert] expect eq failed in %s:%d\n", pFilename, LineNumber);
	fprintf(stderr, "[assert]   expected: %d\n", Expected);
	fprintf(stderr, "[assert]     actual: %d\n", Actual);
	return false;
}

bool CBobTest::ExpectStrEq(const char *pActual, const char *pExpected, const char *pFilename, int LineNumber)
{
	if(!str_comp(pActual, pExpected))
		return true;

	m_NumErrors++;
	fprintf(stderr, "[assert] expect eq failed in %s:%d\n", pFilename, LineNumber);
	fprintf(stderr, "[assert]   expected: \"%s\"\n", pExpected);
	fprintf(stderr, "[assert]     actual: \"%s\"\n", pActual);
	return false;
}

CBobTest::~CBobTest()
{
	if(m_NumErrors == 0)
	{
		puts("OK all tests passed");
		exit(0);
	}
	fprintf(stderr, "ERROR there are %d failed tests\n", m_NumErrors);
	exit(1);
}
