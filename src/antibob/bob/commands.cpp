#include <bob/antibob.h>
#include <bob/antibot_player.h>
#include <bob/detection_event.h>
#include <bob/pending_punish.h>
#include <bob/version.h>
#include <polybob/antibot/antibot_data.h>
#include <polybob/base/log.h>
#include <polybob/base/system/str.h>
#include <polybob/engine/shared/jobs.h>

#include <optional>

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

void CAntibob::ComAutoPunishEvent(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;

	const char *pName = pResult->GetString(0);
	const char *pPunishType = pResult->GetString(1);

	std::optional<int> CheckId = CDetectionEvent::EventNameToId(pName);
	if(!CheckId.has_value())
	{
		log_info("antibot", "ERROR: event with name '%s' not found!", pName);
		return;
	}
	int EventId = CheckId.value();

	CEventPunishConfig *pCfg = &pSelf->m_PunishController.m_aConfigs[EventId];

	if(str_comp(pPunishType, "off") == 0)
	{
		log_info("antibot", "successfully disabled auto punish for events of type '%s'", pName);
		pCfg->m_Punish = CEventPunishConfig::EPunish::OFF;
	}
	else if(str_comp(pPunishType, "kick") == 0)
	{
		log_info("antibot", "successfully enabled auto kick for events of type '%s'", pName);
		pCfg->m_Punish = CEventPunishConfig::EPunish::KICK;
	}
	else if(str_comp(pPunishType, "ban") == 0)
	{
		log_info("antibot", "successfully enabled auto ban for events of type '%s'", pName);
		pCfg->m_Punish = CEventPunishConfig::EPunish::BAN;
	}
	else
	{
		log_info("antibot", "ERROR: invalid punish type '%s' possible values are: off, kick, ban", pPunishType);
		return;
	}
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

void CAntibob::ComSampleJob(CBobResult *pResult, void *pUserData)
{
	CAntibob *pSelf = (CAntibob *)pUserData;
	if(!pSelf->m_pRoundData)
	{
		log_error("antibot", "missing round data");
		return;
	}
	int ClientId = pResult->GetInteger(0);
	if(ClientId < 0 || ClientId >= ANTIBOT_MAX_CLIENTS)
	{
		log_error("antibot", "invalid cid=%d", ClientId);
		return;
	}
	CAntibotPlayer *pPlayer = pSelf->m_apPlayers[ClientId];
	if(!pPlayer)
	{
		log_error("antibot", "cid=%d not online", ClientId);
		return;
	}
	CPlayerComputeRequest Request = {
		.m_Type = EPlayerJobType::BOB_SAMPLE,
	};

	// fill your input data here
	Request.m_Data.m_Bob.m_Jumped = false;

	log_info("COMMAND", "pResult->m_ClientId=%d", pResult->m_ClientId);

	if(!pSelf->StartComputeJob(pResult->m_ClientId, pPlayer, Request))
		log_warn("antibot", "failed to start job");
	else
		log_info("antibot", "started sample compute job for player with cid=%d", ClientId);
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
