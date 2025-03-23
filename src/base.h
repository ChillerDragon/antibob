#pragma once

#include <antibot/antibot_data.h>

class CBase
{
public:
	CAntibotData *m_pData = nullptr;

	CBase(CAntibotData *pData) :
		m_pData(pData){};

	void SendChatTarget(int ClientId, const char *pMessage);
};
