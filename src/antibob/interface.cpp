#define ANTIBOTAPI DYNAMIC_EXPORT

#include <bob/antibob.h>
#include <polybob/antibot/antibot_interface.h>

#include <cstring>

extern "C" {

#define ANTIBOT_CLASS CAntibob // <= YOUR CLASS HERE

ANTIBOT_CLASS *pAntibob = nullptr;

int AntibotAbiVersion()
{
	return ANTIBOT_ABI_VERSION;
}
void AntibotInit(CAntibotData *pCallbackData)
{
	pAntibob = new ANTIBOT_CLASS(pCallbackData);
	pAntibob->OnInit(pCallbackData);
}
void AntibotRoundStart(CAntibotRoundData *pRoundData) { pAntibob->OnRoundStart(pRoundData); }
void AntibotRoundEnd(void) { pAntibob->OnRoundEnd(); }
void AntibotUpdateData(void) { pAntibob->OnUpdateData(); }
void AntibotDestroy(void)
{
	pAntibob->OnDestroy();
	delete pAntibob;
}
void AntibotConsoleCommand(const char *pCommand) { pAntibob->OnConsoleCommand(pCommand); }
void AntibotOnPlayerInit(int ClientId) { pAntibob->OnPlayerInit(ClientId); }
void AntibotOnPlayerDestroy(int ClientId) { pAntibob->OnPlayerDestroy(ClientId); }
void AntibotOnSpawn(int ClientId) { pAntibob->OnSpawn(ClientId); }
void AntibotOnHammerFireReloading(int ClientId) { pAntibob->OnHammerFireReloading(ClientId); }
void AntibotOnHammerFire(int ClientId) { pAntibob->OnHammerFire(ClientId); }
void AntibotOnHammerHit(int ClientId, int TargetId) { pAntibob->OnHammerHit(ClientId, TargetId); }
void AntibotOnDirectInput(int ClientId) { pAntibob->OnDirectInput(ClientId); }
void AntibotOnCharacterTick(int ClientId) { pAntibob->OnCharacterTick(ClientId); }
void AntibotOnHookAttach(int ClientId, bool Player) { pAntibob->OnHookAttach(ClientId, Player); }
void AntibotOnEngineTick(void) { pAntibob->OnEngineTick(); }
void AntibotOnEngineClientJoin(int ClientId) { pAntibob->OnEngineClientJoin(ClientId); }
void AntibotOnEngineClientDrop(int ClientId, const char *pReason) { pAntibob->OnEngineClientDrop(ClientId, pReason); }
bool AntibotOnEngineClientMessage(int ClientId, const void *pData, int Size, int Flags) { return pAntibob->OnEngineClientMessage(ClientId, pData, Size, Flags); }
bool AntibotOnEngineServerMessage(int ClientId, const void *pData, int Size, int Flags) { return pAntibob->OnEngineServerMessage(ClientId, pData, Size, Flags); }
bool AntibotOnEngineSimulateClientMessage(int *pClientId, void *pBuffer, int BufferSize, int *pOutSize, int *pFlags) { return pAntibob->OnEngineSimulateClientMessage(pClientId, pBuffer, BufferSize, pOutSize, pFlags); }
}
