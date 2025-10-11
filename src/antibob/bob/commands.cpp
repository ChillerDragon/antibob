#include <bob/antibob.h>
#include <bob/version.h>
#include <polybob/base/log.h>

void CAntibob::ComTest(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->SendChat(-1, polybob::TEAM_ALL, "HELLO FROM TEST COMAMNDDO");
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
	int MinConfidence = pResult->NumArguments() > 0 ? pResult->GetInteger(0) : pSelf->Config()->m_AbDumpConfidence;
	const char *pSearch = pResult->NumArguments() > 1 ? pResult->GetString(0) : "";
	pSelf->RconDump(pSearch, MinConfidence);
}

void CAntibob::ComEvents(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->RconEvents(pResult->GetInteger(0));
}

void CAntibob::ComVersion(CBobResult *pResult, void *pUserData)
{
	log_info("antibot", "antibob v" ANTIBOB_VERSION " git rev: %s, built on: " ANTIBOB_BUILD_DATE, BOB_GIT_SHORTREV_HASH);
}

void CAntibob::ComPendingPunishments(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->m_PunishController.ListPendingPunishments();
}

void CAntibob::ComKnownCheaters(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;

	if(!pSelf->m_pRoundData)
	{
		log_error("antibot", "missing round data");
		return;
	}

	bool GotKnown = false;
	for(int i = 0; i < ANTIBOT_MAX_CLIENTS; i++)
	{
		CAntibotPlayer *pPlayer = pSelf->m_apPlayers[i];
		if(!pPlayer)
			continue;
		if(!pPlayer->m_KnownCheater)
			continue;

		log_info("antibot", "cid=%d name='%s' was caught cheating already", i, pSelf->ClientName(i));
		GotKnown = true;
	}

	if(!GotKnown)
		log_info("antibot", "no known cheaters currently on the server");
}

void CAntibob::ComRedirectKnownCheaters(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;

	if(!pSelf->m_pRoundData)
	{
		log_error("antibot", "missing round data");
		return;
	}

	int DstPort = pResult->GetInteger(0);

	bool GotKnown = false;
	for(int i = 0; i < ANTIBOT_MAX_CLIENTS; i++)
	{
		CAntibotPlayer *pPlayer = pSelf->m_apPlayers[i];
		if(!pPlayer)
			continue;
		if(!pPlayer->m_KnownCheater)
			continue;

		log_info("antibot", "cid=%d name='%s' was caught cheating already redirecting to port %d", i, pSelf->ClientName(i), DstPort);
		char aCmd[512];
		str_format(aCmd, sizeof(aCmd), "redirect %d %d", i, DstPort);
		if(!pSelf->m_BobAbi.Rcon(aCmd))
		{
			log_error("antibob", "antibob rcon abi not supported. redirect failed.");
		}

		GotKnown = true;
	}

	if(!GotKnown)
		log_info("antibot", "no known cheaters currently on the server");
}
