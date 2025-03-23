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

	log_info("dump", "num args %d ", pResult->NumArguments());
	if(pResult->NumArguments())
		log_info("dump", "arg '%s'", pResult->GetString(0));

	pSelf->DumpPlayers(pResult->NumArguments() ? pResult->GetString(0) : "");
}
