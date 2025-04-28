#pragma once

#include <polybob/antibot/antibot_data.h>
#include <polybob/engine/shared/protocol_ex.h>

#include <bob/console.h>
#include <bob/gameserver.h>

class CAntibob : public CGameServer
{
public:
	CAntibob(CAntibotData *pData);
	virtual ~CAntibob() = default;

#define CONSOLE_COMMAND(name, params, callback, user, help) static void callback(CBobResult *pResult, void *pUserData);
#include <bob/commands.h>
#undef CONSOLE_COMMAND

	//
	// internal helpers
	//

	void RegisterCommands();

	//
	// rcon commands
	//

	// called on rcon command "antibot dump"
	virtual void RconDump(const char *pSearch);
	virtual void RconEvents(int ClientId);

	//
	// antibob special hooks
	//

	// return true to drop the message
	bool OnSayNetMessage(const CNetMsg_Cl_Say *pMsg, int ClientId, const CUnpacker *pUnpacker);

	// return true to drop the message
	bool OnSayNetMessage7(const antibob::protocol7::CNetMsg_Cl_Say *pMsg, int ClientId, const CUnpacker *pUnpacker);

	//
	// ddnet antibot interface hooks
	//

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
	virtual void OnEngineClientJoin(int ClientId);
	virtual void OnEngineClientDrop(int ClientId, const char *pReason);
	virtual bool OnEngineClientMessage(int ClientId, const void *pData, int Size, int Flags);
	virtual bool OnEngineServerMessage(int ClientId, const void *pData, int Size, int Flags);
	virtual bool OnEngineSimulateClientMessage(int *pClientId, void *pBuffer, int BufferSize, int *pOutSize, int *pFlags);
};
