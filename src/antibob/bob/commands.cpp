#include <bob/antibob.h>
#include <bob/version.h>
#include <polybob/base/log.h>
#include <polybob/base/system/str.h>
#include <polybob/engine/shared/jobs.h>

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

void CAntibob::ComConfigs(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->m_Console.PrintConfigs();
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
	const char *pSearch = pResult->NumArguments() > 1 ? pResult->GetString(1) : "";
	pSelf->RconDump(pSearch, MinConfidence);
}

void CAntibob::ComEvents(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	pSelf->RconEvents(pResult->GetInteger(0));
}

void CAntibob::ComKickEvents(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	const char *pEventIds = pResult->GetString(0);
	char aEventId[16];
	std::vector<int> vEventIds;
	while((pEventIds = str_next_token(pEventIds, ",", aEventId, sizeof(aEventId))))
	{
		if(!aEventId[0])
			continue;

		int EventId;
		if(!str_toint(aEventId, &EventId))
		{
			log_warn("antibot", "failed to parse event id '%s' not a valid number", aEventId);
			continue;
		}
		vEventIds.emplace_back(EventId);
	}

	if(vEventIds.empty())
	{
		log_error("antibob", "need to provide at least one valid event id");
		return;
	}

	int Matches = 0;
	for(CAntibotPlayer *pPlayer : pSelf->m_apPlayers)
	{
		if(!pPlayer)
			continue;
		if(pPlayer->m_DetectionEvents.empty())
			continue;

		bool MissingEvents = false;
		for(int EventId : vEventIds)
		{
			if(pPlayer->m_DetectionEvents.count(EventId))
				continue;

			MissingEvents = true;
			break;
		}
		if(MissingEvents)
			break;

		char aPlayerEvents[512];
		CDetectionEvent::EventsToIdStr(pPlayer->m_DetectionEvents, 0, aPlayerEvents, sizeof(aPlayerEvents));

		log_info(
			"antibot",
			"kicking cid=%d name='%s' with events: %s",
			pPlayer->GetCid(),
			pSelf->ClientName(pPlayer->GetCid()),
			aPlayerEvents);
		pSelf->Kick(pPlayer->GetCid(), "antibot");
		Matches++;
	}
	log_info("antibot", "kicked %d players based on matching events", Matches);
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

void CAntibob::ComPlayerJobs(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	if(!pSelf->m_pRoundData)
	{
		log_error("antibot", "missing round data");
		return;
	}
	int TotalQueued = 0;
	int TotalRunning = 0;
	for(const CAntibotPlayer *pPlayer : pSelf->m_apPlayers)
	{
		if(!pPlayer)
			continue;
		if(pPlayer->m_vpComputeJobs.empty())
			continue;

		int Queued = 0;
		int Running = 0;
		for(const auto &Job : pPlayer->m_vpComputeJobs)
		{
			if(Job->State() == polybob::IJob::STATE_RUNNING)
				Running++;
			if(Job->State() == polybob::IJob::STATE_QUEUED)
				Queued++;
		}

		log_info("antibot", "cid=%d queued=%d running=%d", pPlayer->GetCid(), Queued, Running);
	}
	log_info("antibot", "total queued=%d running=%d", TotalQueued, TotalRunning);
}
