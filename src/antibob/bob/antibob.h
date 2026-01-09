#pragma once

#include "polybob/game/generated/protocol.h"

#include <bob/console.h>
#include <bob/gameserver.h>
#include <polybob/antibot/antibot_data.h>
#include <polybob/engine/shared/protocol_ex.h>

class CAntibotPlayer;

class CAntibob : public CGameServer
{
public:
	CAntibob(CAntibotData *pData);
	~CAntibob() override = default;

#define CONSOLE_COMMAND(name, params, callback, user, help) static void callback(CBobResult *pResult, void *pUserData);
#include <bob/commands.h>
#undef CONSOLE_COMMAND

	//
	// internal helpers
	//

	void RegisterCommands();
	void TrackBan(int ClientId, const char *pReason);

	//
	// helpers
	//

	bool IsTimeouted(int ClientId);
	void SendChatCommand(int ClientId, const char *pName, const char *pArgs, const char *pHelp);

	//
	// rcon commands
	//

	// called on rcon command "antibot dump"
	virtual void RconDump(const char *pSearch, int MinConfidence);
	virtual void RconEvents(int ClientId);

	//
	// antibob special hooks
	//

	// WARNING: this is only used by 0.6 connections!
	//          if you need to run code on any chat message sent by 0.6 or 0.7 use `OnChatMessage()` instead
	//
	// return true to drop the message
	virtual bool OnSayNetMessage(const polybob::CNetMsg_Cl_Say *pMsg, int ClientId, const polybob::CUnpacker *pUnpacker);

	// WARNING: this is only used by 0.7 connections!
	//          if you need to run code on any chat message sent by 0.6 or 0.7 use `OnChatMessage()` instead
	//
	// return true to drop the message
	virtual bool OnSayNetMessage7(const polybob::protocol7::CNetMsg_Cl_Say *pMsg, int ClientId, const polybob::CUnpacker *pUnpacker);

	// WARNING: this is only used by 0.6!
	//          if you need to run code on initial skin info or skin change use `OnSkinInfo()` instead
	virtual void OnStartInfoNetMessage(const polybob::CNetMsg_Cl_StartInfo *pMsg, int ClientId, const polybob::CUnpacker *pUnpacker);

	// WARNING: this is only used by 0.7!
	//          if you need to run code on initial skin info or skin change use `OnSkinInfo()` instead
	virtual void OnStartInfoNetMessage7(const polybob::protocol7::CNetMsg_Cl_StartInfo *pMsg, int ClientId, const polybob::CUnpacker *pUnpacker);

	virtual void OnInputNetMessage(int ClientId, int AckGameTick, int PredictionTick, int Size, CNetObj_PlayerInput *pInput);

	// WARNING: this is not fully implemented yet! Skin changes are not supported yet!
	//
	// called when the client sent a new skin info
	// this happens on join and when users request
	// a skin change.
	// It is called for 0.6 and 0.7 connections.
	// The new skin data is already written to
	//
	// `pPlayer->m_TeeInfos`
	//
	virtual void OnSkinInfo(CAntibotPlayer *pPlayer) {}

	// Called when someone used the /antibot chat command
	virtual void ChatCmdAntibot(int ClientId, const char *pArgs);

	virtual bool OnChatCommand(int ClientId, const char *pCommandWithArgs);

	// Called for both 0.7 and 0.7 chat messages
	// return true to drop the message
	virtual bool OnChatMessage(int ClientId, const char *pMessage, bool IsWhisper);

	// send http request with player name and ip
	// to backend configured by ab_cheater_api_url
	virtual void LookupPlayer(CAntibotPlayer *pPlayer);

	// same as IGameController::OnPlayerConnect(CPlayer *pPlayer)
	// in ddnet code base
	// is called when the client is fully online and
	// sent its name and skin info already
	virtual void OnPlayerConnect(CAntibotPlayer *pPlayer);

	// same as IGameController::OnPlayerDisconnect(CPlayer *pPlayer, const char *pReason)
	// in ddnet code base
	// only called for clients that properly joined first
	// this is not called for clients that abort during the connection phase
	virtual void OnPlayerDisconnect(CAntibotPlayer *pPlayer, const char *pReason) {}

	virtual void OnKnownCheaterJoin(CAntibotPlayer *pPlayer);

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
