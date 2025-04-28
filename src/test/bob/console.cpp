#include "bobtest.h"

#include <polybob/base/system.h>
#include <polybob/game/generated/protocolglue.h>

int main()
{
	CBobTest BobTest;
	EXPECT_STREQ("foo", "foo");
	EXPECT_EQ(10, 5 + 5);
	EXPECT_EQ(true, true);
	EXPECT_NE(true, false);
}
