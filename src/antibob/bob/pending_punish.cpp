#include "pending_punish.h"

#include <bob/antibob.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>

using namespace polybob;

CPendingPunish::CPendingPunish(int ClientId, const NETADDR &Ip, const char *pReason, int TimeInMinutes, EPunish Punish) :
	m_ClientId(ClientId), m_Ip(Ip), m_TimeInMinutes(TimeInMinutes), m_Punish(Punish)
{
	str_copy(m_aReason, pReason);
}

void CPunishController::ApplyPunish(CPendingPunish *pPunish)
{
	if(pPunish->m_Applied)
		return;
	pPunish->m_Applied = true;

	switch(pPunish->m_Punish)
	{
	case CPendingPunish::EPunish::KICK:
		m_pAntibob->Kick(pPunish->m_ClientId, pPunish->m_aReason);
		break;
	case CPendingPunish::EPunish::BAN:
	{
		if(m_pAntibob->m_apPlayers[pPunish->m_ClientId])
			m_pAntibob->Ban(pPunish->m_ClientId, pPunish->m_TimeInMinutes, pPunish->m_aReason);
		else
			m_pAntibob->Ban(pPunish->m_Ip, pPunish->m_TimeInMinutes, pPunish->m_aReason);
		break;
	}
	};
}

void CPunishController::OnInit(CAntibob *pAntibob)
{
	m_pAntibob = pAntibob;
}

void CPunishController::OnTick()
{
	if(m_NextPunishTime > time_get())
		return;

	m_NextPunishTime = time_get() + time_freq() * m_pAntibob->Config()->m_AbKickInterval;

	for(auto &Punish : m_vPendingPunishments)
		ApplyPunish(&Punish);

	m_vPendingPunishments.clear();
}

void CPunishController::OnPlayerDisconnect(int ClientId)
{
	int i = -1;
	for(auto &Punish : m_vPendingPunishments)
	{
		i++;
		if(Punish.m_ClientId != ClientId)
			continue;

		if(Punish.m_Punish == CPendingPunish::EPunish::BAN)
			ApplyPunish(&Punish);
		m_vPendingPunishments.erase(m_vPendingPunishments.begin() + i);
		break;
	}
}

void CPunishController::SchedulePunish(int ClientId, const char *pReason, int TimeInMinutes, CPendingPunish::EPunish Punish)
{
	for(auto &PendingPunish : m_vPendingPunishments)
	{
		if(PendingPunish.m_ClientId != ClientId)
			continue;

		// TODO: what exactly do we want to happen here?
		//       a player got two scheduled punishments
		//       ideally this depends on the punishment type
		//       for now any new punishments are ignored
		return;
	}

	m_vPendingPunishments.emplace_back(ClientId, m_pAntibob->m_apPlayers[ClientId]->m_Addr, pReason, TimeInMinutes, Punish);
}

void CPunishController::ListPendingPunishments() const
{
	if(m_vPendingPunishments.empty())
	{
		log_info("antibob", "no pending punishments");
		return;
	}

	int SecondsLeft = (m_NextPunishTime - time_get()) / time_freq();
	log_info("antibob", "Next punish in %d seconds:", SecondsLeft);
	for(const auto &Punish : m_vPendingPunishments)
		log_info("antibob", "  cid=%d reason='%s' name='%s'", Punish.m_ClientId, Punish.m_aReason, m_pAntibob->ClientName(Punish.m_ClientId));
}
