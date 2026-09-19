#include "bobtest.h"
#include "server_mock/log.h"

#include <bob/detection_event.h>

int main()
{
	CBobTest BobTest;
	EXPECT_STREQ("sample", "sample");
	EXPECT_EQ(10, 5 + 5);
	EXPECT_STREQ(CDetectionEvent::EventToStr(BOB_DE_SAMPLE), "sample");
}
