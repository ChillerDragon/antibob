#pragma once

#include <antibot/antibot_data.h>

class CAntibob
{
	CAntibotData *m_pData;

public:
	virtual ~CAntibob() = default;

	// TODO: add variadic log_info(level, fmt, ...) macro
	void Log(const char *pMessage);

	virtual void OnInit(CAntibotData *pData);
	virtual void OnRoundStart(CAntibotRoundData *pRoundData);
	virtual void OnRoundEnd();
	virtual void OnUpdateData();
	virtual void OnDestroy();
	// returns false on unknown command
	virtual bool OnConsoleCommand(const char *pCommand);
	virtual void OnPlayerInit(int ClientId);
	virtual void OnPlayerDestroy(int ClientId);
	virtual void OnSpawn(int ClientId);
	virtual void OnHammerFireReloading(int ClientId);
	virtual void OnHammerFire(int ClientId);
	virtual void OnHammerHit(int ClientId, int TargetId);
	virtual void OnDirectInput(int ClientId);
	virtual void OnCharacterTick(int ClientId);
	virtual void OnHookAttach(int ClientId, bool Player);
	virtual void OnEngineTick();
	virtual void OnEngineClientJoin(int ClientId, bool Sixup);
	virtual void OnEngineClientDrop(int ClientId, const char *pReason);
	virtual bool OnEngineClientMessage(int ClientId, const void *pData, int Size, int Flags);
	virtual bool OnEngineServerMessage(int ClientId, const void *pData, int Size, int Flags);
	virtual bool OnEngineSimulateClientMessage(int *pClientId, void *pBuffer, int BufferSize, int *pOutSize, int *pFlags);
};
