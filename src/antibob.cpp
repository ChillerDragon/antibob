#include "antibob.h"

#include <cstdio>
#include <cstring>

void CAntibob::Log(const char *pMessage)
{
	m_pData->m_pfnLog(pMessage, m_pData->m_pUser);
}

void CAntibob::OnInit(CAntibotData *pData)
{
	m_pData = pData;
	Log("antibob antibot initialized");
}

void CAntibob::OnRoundStart(CAntibotRoundData *pRoundData)
{
}

void CAntibob::OnRoundEnd()
{
}

void CAntibob::OnUpdateData()
{
}

void CAntibob::OnDestroy()
{
}

bool CAntibob::OnConsoleCommand(const char *pCommand)
{
	if(strcmp(pCommand, "dump") == 0)
	{
		Log("null antibot");
	}
	else
	{
		// Log("unknown command");
		return false;
	}
	return true;
}

void CAntibob::OnPlayerInit(int ClientId)
{
}

void CAntibob::OnPlayerDestroy(int ClientId)
{
}

void CAntibob::OnSpawn(int ClientId)
{
}

void CAntibob::OnHammerFireReloading(int ClientId)
{
}

void CAntibob::OnHammerFire(int ClientId)
{
}

void CAntibob::OnHammerHit(int ClientId, int TargetId)
{
}

void CAntibob::OnDirectInput(int ClientId)
{
}

void CAntibob::OnCharacterTick(int ClientId)
{
}

void CAntibob::OnHookAttach(int ClientId, bool Player)
{
}

void CAntibob::OnEngineTick()
{
}

void CAntibob::OnEngineClientJoin(int ClientId, bool Sixup)
{
}

void CAntibob::OnEngineClientDrop(int ClientId, const char *pReason)
{
}

bool CAntibob::OnEngineClientMessage(int ClientId, const void *pData, int Size, int Flags)
{
	return false;
}

bool CAntibob::OnEngineServerMessage(int ClientId, const void *pData, int Size, int Flags)
{
	return false;
}

bool CAntibob::OnEngineSimulateClientMessage(int *pClientId, void *pBuffer, int BufferSize, int *pOutSize, int *pFlags)
{
	return false;
}
