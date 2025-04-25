#include "bobtest.h"

#include <polybob/base/system.h>
#include <polybob/game/generated/protocolglue.h>

int main()
{
	CBobTest BobTest;
	EXPECT_STREQ("sample", "sample");
	EXPECT_EQ(10, 5 + 5);
}
