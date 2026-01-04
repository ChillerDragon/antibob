#pragma once

#include <polybob/base/types.h>

#include <cstdint>
#include <vector>

class CPendingPunish
{
public:
	enum class EPunish
	{
		KICK,
		BAN,
	};

	CPendingPunish(int ClientId, const polybob::NETADDR &Ip, const char *pReason, int TimeInMinutes, EPunish Punish);

	int m_ClientId;
	polybob::NETADDR m_Ip;
	char m_aReason[512];
	int m_TimeInMinutes;
	EPunish m_Punish;
	bool m_Applied = false;
};

class CPunishController
{
	std::vector<CPendingPunish> m_vPendingPunishments;
	int64_t m_NextPunishTime = 0;
	class CAntibob *m_pAntibob = nullptr;
	void ApplyPunish(CPendingPunish *pPunish);

public:
	void OnInit(class CAntibob *pAntibob);
	void OnTick();
	void OnPlayerDisconnect(int ClientId);
	void SchedulePunish(int ClientId, const char *pReason, int TimeInMinutes, CPendingPunish::EPunish Punish);
	void ListPendingPunishments() const;
};
