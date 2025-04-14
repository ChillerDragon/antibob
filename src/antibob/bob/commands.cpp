#include <bob/antibob.h>
#include <polybob/base/log.h>

void CAntibob::ComTest(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->SendChat(-1, TEAM_ALL, "HELLO FROM TEST COMAMNDDO");
}

void CAntibob::ComCmdHelp(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->m_Console.PrintCmdHelp(pResult->GetString(0));
}

void CAntibob::ComCmdlist(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->m_Console.PrintCmdlist();
}

void CAntibob::ComDump(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->RconDump(pResult->NumArguments() ? pResult->GetString(0) : "");
}

void CAntibob::ComEvents(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->RconEvents(pResult->GetInteger(0));
}
