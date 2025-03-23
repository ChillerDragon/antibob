#include "antibot/antibot_data.h"
#include <base/log.h>
#include <bob/antibob.h>

void CAntibob::ComTest(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->SendChat(-1, TEAM_ALL, "HELLO FROM TEST COMAMNDDO");
}

void CAntibob::ComDump(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->DumpPlayers();
}
