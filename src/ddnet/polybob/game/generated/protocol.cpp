#include "protocol.h"

#include <polybob/base/system.h>
#include <polybob/engine/shared/bob_uuid_manager.h>
#include <polybob/engine/shared/packer.h>
#include <polybob/engine/shared/protocol.h>
#include <polybob/game/gamecore.h>
#include <polybob/game/mapitems_ex.h>

namespace polybob
{

	CNetObjHandler::CNetObjHandler()
	{
		m_pMsgFailedOn = "";
		m_pObjFailedOn = "";
		m_pObjCorrectedOn = "";
		m_NumObjCorrections = 0;
	}

	int CNetObjHandler::NumObjCorrections() const { return m_NumObjCorrections; }
	const char *CNetObjHandler::CorrectedObjOn() const { return m_pObjCorrectedOn; }
	const char *CNetObjHandler::FailedObjOn() const { return m_pObjFailedOn; }
	const char *CNetObjHandler::FailedMsgOn() const { return m_pMsgFailedOn; }

	static const int max_int = 0x7fffffff;
	static const int min_int = 0x80000000;

	int CNetObjHandler::ClampInt(const char *pErrorMsg, int Value, int Min, int Max)
	{
		if(Value < Min)
		{
			m_pObjCorrectedOn = pErrorMsg;
			m_NumObjCorrections++;
			return Min;
		}
		if(Value > Max)
		{
			m_pObjCorrectedOn = pErrorMsg;
			m_NumObjCorrections++;
			return Max;
		}
		return Value;
	}

	const char *CNetObjHandler::ms_apObjNames[] = {
		"EX/UUID",
		"PlayerInput",
		"Projectile",
		"Laser",
		"Pickup",
		"Flag",
		"GameInfo",
		"GameData",
		"CharacterCore",
		"Character",
		"PlayerInfo",
		"ClientInfo",
		"SpectatorInfo",
		"Common",
		"Explosion",
		"Spawn",
		"HammerHit",
		"Death",
		"SoundGlobal",
		"SoundWorld",
		"DamageInd",
		""};

	const char *CNetObjHandler::ms_apExObjNames[] = {
		"invalid",
		"MyOwnObject",
		"DDNetCharacter",
		"DDNetPlayer",
		"GameInfoEx",
		"DDRaceProjectile",
		"DDNetLaser",
		"DDNetProjectile",
		"DDNetPickup",
		"DDNetSpectatorInfo",
		"SpectatorCount",
		"Birthday",
		"Finish",
		"MyOwnEvent",
		"SpecChar",
		"SwitchState",
		"EntityEx",
		"MapSoundWorld",
		""};

	int CNetObjHandler::ms_aObjSizes[] = {
		0,
		sizeof(CNetObj_PlayerInput),
		sizeof(CNetObj_Projectile),
		sizeof(CNetObj_Laser),
		sizeof(CNetObj_Pickup),
		sizeof(CNetObj_Flag),
		sizeof(CNetObj_GameInfo),
		sizeof(CNetObj_GameData),
		sizeof(CNetObj_CharacterCore),
		sizeof(CNetObj_Character),
		sizeof(CNetObj_PlayerInfo),
		sizeof(CNetObj_ClientInfo),
		sizeof(CNetObj_SpectatorInfo),
		sizeof(CNetEvent_Common),
		sizeof(CNetEvent_Explosion),
		sizeof(CNetEvent_Spawn),
		sizeof(CNetEvent_HammerHit),
		sizeof(CNetEvent_Death),
		sizeof(CNetEvent_SoundGlobal),
		sizeof(CNetEvent_SoundWorld),
		sizeof(CNetEvent_DamageInd),
		0};

	int CNetObjHandler::ms_aUnpackedObjSizes[] = {
		16,
		sizeof(CNetObj_PlayerInput),
		sizeof(CNetObj_Projectile),
		sizeof(CNetObj_Laser),
		sizeof(CNetObj_Pickup),
		sizeof(CNetObj_Flag),
		sizeof(CNetObj_GameInfo),
		sizeof(CNetObj_GameData),
		sizeof(CNetObj_CharacterCore),
		sizeof(CNetObj_Character),
		sizeof(CNetObj_PlayerInfo),
		sizeof(CNetObj_ClientInfo),
		sizeof(CNetObj_SpectatorInfo),
		sizeof(CNetEvent_Common),
		sizeof(CNetEvent_Explosion),
		sizeof(CNetEvent_Spawn),
		sizeof(CNetEvent_HammerHit),
		sizeof(CNetEvent_Death),
		sizeof(CNetEvent_SoundGlobal),
		sizeof(CNetEvent_SoundWorld),
		sizeof(CNetEvent_DamageInd),
	};

	int CNetObjHandler::ms_aUnpackedExObjSizes[] = {
		0,
		sizeof(CNetObj_MyOwnObject),
		sizeof(CNetObj_DDNetCharacter),
		sizeof(CNetObj_DDNetPlayer),
		sizeof(CNetObj_GameInfoEx),
		sizeof(CNetObj_DDRaceProjectile),
		sizeof(CNetObj_DDNetLaser),
		sizeof(CNetObj_DDNetProjectile),
		sizeof(CNetObj_DDNetPickup),
		sizeof(CNetObj_DDNetSpectatorInfo),
		sizeof(CNetObj_SpectatorCount),
		sizeof(CNetEvent_Birthday),
		sizeof(CNetEvent_Finish),
		sizeof(CNetObj_MyOwnEvent),
		sizeof(CNetObj_SpecChar),
		sizeof(CNetObj_SwitchState),
		sizeof(CNetObj_EntityEx),
		sizeof(CNetEvent_MapSoundWorld),
	};

	const char *CNetObjHandler::ms_apMsgNames[] = {
		"invalid",
		"Sv_Motd",
		"Sv_Broadcast",
		"Sv_Chat",
		"Sv_KillMsg",
		"Sv_SoundGlobal",
		"Sv_TuneParams",
		"Unused",
		"Sv_ReadyToEnter",
		"Sv_WeaponPickup",
		"Sv_Emoticon",
		"Sv_VoteClearOptions",
		"Sv_VoteOptionListAdd",
		"Sv_VoteOptionAdd",
		"Sv_VoteOptionRemove",
		"Sv_VoteSet",
		"Sv_VoteStatus",
		"Cl_Say",
		"Cl_SetTeam",
		"Cl_SetSpectatorMode",
		"Cl_StartInfo",
		"Cl_ChangeInfo",
		"Cl_Kill",
		"Cl_Emoticon",
		"Cl_Vote",
		"Cl_CallVote",
		"Cl_IsDDNetLegacy",
		"Sv_DDRaceTimeLegacy",
		"Sv_RecordLegacy",
		"Unused2",
		"Sv_TeamsStateLegacy",
		"Cl_ShowOthersLegacy",
		""};

	const char *CNetObjHandler::ms_apExMsgNames[] = {
		"invalid",
		"Sv_MyOwnMessage",
		"Cl_ShowDistance",
		"Cl_ShowOthers",
		"Cl_CameraInfo",
		"Sv_TeamsState",
		"Sv_DDRaceTime",
		"Sv_Record",
		"Sv_KillMsgTeam",
		"Sv_YourVote",
		"Sv_RaceFinish",
		"Sv_CommandInfo",
		"Sv_CommandInfoRemove",
		"Sv_VoteOptionGroupStart",
		"Sv_VoteOptionGroupEnd",
		"Sv_CommandInfoGroupStart",
		"Sv_CommandInfoGroupEnd",
		"Sv_ChangeInfoCooldown",
		"Sv_MapSoundGlobal",
		"Sv_PreInput",
		"Sv_SaveCode",
		"Sv_ServerAlert",
		"Sv_ModeratorAlert",
		"Cl_EnableSpectatorCount",
		""};

	const char *CNetObjHandler::GetObjName(int Type) const
	{
		if(Type >= 0 && Type < NUM_NETOBJTYPES)
		{
			return ms_apObjNames[Type];
		}
		else if(Type > __NETOBJTYPE_UUID_HELPER && Type < OFFSET_NETMSGTYPE_UUID)
		{
			return ms_apExObjNames[Type - __NETOBJTYPE_UUID_HELPER];
		}
		return "(out of range)";
	}

	int CNetObjHandler::GetObjSize(int Type) const
	{
		if(Type < 0 || Type >= NUM_NETOBJTYPES)
			return 0;
		return ms_aObjSizes[Type];
	}

	int CNetObjHandler::GetUnpackedObjSize(int Type) const
	{
		if(Type >= 0 && Type < NUM_NETOBJTYPES)
		{
			return ms_aUnpackedObjSizes[Type];
		}
		else if(Type > __NETOBJTYPE_UUID_HELPER && Type < OFFSET_NETMSGTYPE_UUID)
		{
			return ms_aUnpackedExObjSizes[Type - __NETOBJTYPE_UUID_HELPER];
		}
		return 0;
	}

	const char *CNetObjHandler::GetMsgName(int Type) const
	{
		if(Type >= 0 && Type < NUM_NETMSGTYPES)
		{
			return ms_apMsgNames[Type];
		}
		else if(Type > __NETMSGTYPE_UUID_HELPER && Type < OFFSET_MAPITEMTYPE_UUID)
		{
			return ms_apExMsgNames[Type - __NETMSGTYPE_UUID_HELPER];
		}
		return "(out of range)";
	}

	int CNetObjHandler::DumpObj(int Type, const void *pData, int Size) const
	{
		char aRawData[512];
		char aStr[128];
		int aInts[2] = {0x0, (int)0x80808080};
		switch(Type)
		{
		case NETOBJTYPE_PLAYERINPUT:
		{
			CNetObj_PlayerInput *pObj = (CNetObj_PlayerInput *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_Direction=%d", aRawData, pObj->m_Direction);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_TargetX=%d", aRawData, pObj->m_TargetX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_TargetY=%d", aRawData, pObj->m_TargetY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_Jump=%d", aRawData, pObj->m_Jump);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_Fire=%d", aRawData, pObj->m_Fire);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_Hook=%d", aRawData, pObj->m_Hook);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 6, ((const int *)pData)[6], ((const int *)pData)[6]);
			dbg_msg("snapshot", "%s\tm_PlayerFlags=%d (min=0 max=256)", aRawData, pObj->m_PlayerFlags);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 7, ((const int *)pData)[7], ((const int *)pData)[7]);
			dbg_msg("snapshot", "%s\tm_WantedWeapon=%d", aRawData, pObj->m_WantedWeapon);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 8, ((const int *)pData)[8], ((const int *)pData)[8]);
			dbg_msg("snapshot", "%s\tm_NextWeapon=%d", aRawData, pObj->m_NextWeapon);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 9, ((const int *)pData)[9], ((const int *)pData)[9]);
			dbg_msg("snapshot", "%s\tm_PrevWeapon=%d", aRawData, pObj->m_PrevWeapon);
			return 0;
		};

		case NETOBJTYPE_PROJECTILE:
		{
			CNetObj_Projectile *pObj = (CNetObj_Projectile *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_VelX=%d", aRawData, pObj->m_VelX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_VelY=%d", aRawData, pObj->m_VelY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_Type=%d (min=0 max=NUM_WEAPONS-1(%d))", aRawData, pObj->m_Type, (int)NUM_WEAPONS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_StartTick=%d (NetTick)", aRawData, pObj->m_StartTick);
			return 0;
		};

		case NETOBJTYPE_LASER:
		{
			CNetObj_Laser *pObj = (CNetObj_Laser *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_FromX=%d", aRawData, pObj->m_FromX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_FromY=%d", aRawData, pObj->m_FromY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_StartTick=%d (NetTick)", aRawData, pObj->m_StartTick);
			return 0;
		};

		case NETOBJTYPE_PICKUP:
		{
			CNetObj_Pickup *pObj = (CNetObj_Pickup *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Type=%d (min=0 max=max_int(%d))", aRawData, pObj->m_Type, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_Subtype=%d (min=0 max=max_int(%d))", aRawData, pObj->m_Subtype, (int)max_int);
			return 0;
		};

		case NETOBJTYPE_FLAG:
		{
			CNetObj_Flag *pObj = (CNetObj_Flag *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Team=%d (min=TEAM_RED(%d) max=TEAM_BLUE(%d))", aRawData, pObj->m_Team, (int)TEAM_RED, (int)TEAM_BLUE);
			return 0;
		};

		case NETOBJTYPE_GAMEINFO:
		{
			CNetObj_GameInfo *pObj = (CNetObj_GameInfo *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_GameFlags=%d (min=0 max=256)", aRawData, pObj->m_GameFlags);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_GameStateFlags=%d (min=0 max=256)", aRawData, pObj->m_GameStateFlags);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_RoundStartTick=%d (NetTick)", aRawData, pObj->m_RoundStartTick);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_WarmupTimer=%d (min=min_int(%d) max=max_int(%d))", aRawData, pObj->m_WarmupTimer, (int)min_int, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_ScoreLimit=%d (min=0 max=max_int(%d))", aRawData, pObj->m_ScoreLimit, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_TimeLimit=%d (min=0 max=max_int(%d))", aRawData, pObj->m_TimeLimit, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 6, ((const int *)pData)[6], ((const int *)pData)[6]);
			dbg_msg("snapshot", "%s\tm_RoundNum=%d (min=0 max=max_int(%d))", aRawData, pObj->m_RoundNum, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 7, ((const int *)pData)[7], ((const int *)pData)[7]);
			dbg_msg("snapshot", "%s\tm_RoundCurrent=%d (min=0 max=max_int(%d))", aRawData, pObj->m_RoundCurrent, (int)max_int);
			return 0;
		};

		case NETOBJTYPE_GAMEDATA:
		{
			CNetObj_GameData *pObj = (CNetObj_GameData *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_TeamscoreRed=%d", aRawData, pObj->m_TeamscoreRed);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_TeamscoreBlue=%d", aRawData, pObj->m_TeamscoreBlue);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_FlagCarrierRed=%d (min=FLAG_MISSING(%d) max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_FlagCarrierRed, (int)FLAG_MISSING, (int)MAX_CLIENTS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_FlagCarrierBlue=%d (min=FLAG_MISSING(%d) max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_FlagCarrierBlue, (int)FLAG_MISSING, (int)MAX_CLIENTS - 1);
			return 0;
		};

		case NETOBJTYPE_CHARACTERCORE:
		{
			CNetObj_CharacterCore *pObj = (CNetObj_CharacterCore *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_Tick=%d", aRawData, pObj->m_Tick);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_VelX=%d", aRawData, pObj->m_VelX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_VelY=%d", aRawData, pObj->m_VelY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_Angle=%d", aRawData, pObj->m_Angle);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 6, ((const int *)pData)[6], ((const int *)pData)[6]);
			dbg_msg("snapshot", "%s\tm_Direction=%d (min=-1 max=1)", aRawData, pObj->m_Direction);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 7, ((const int *)pData)[7], ((const int *)pData)[7]);
			dbg_msg("snapshot", "%s\tm_Jumped=%d (min=0 max=3)", aRawData, pObj->m_Jumped);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 8, ((const int *)pData)[8], ((const int *)pData)[8]);
			dbg_msg("snapshot", "%s\tm_HookedPlayer=%d (min=-1 max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_HookedPlayer, (int)MAX_CLIENTS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 9, ((const int *)pData)[9], ((const int *)pData)[9]);
			dbg_msg("snapshot", "%s\tm_HookState=%d (min=-1 max=5)", aRawData, pObj->m_HookState);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 10, ((const int *)pData)[10], ((const int *)pData)[10]);
			dbg_msg("snapshot", "%s\tm_HookTick=%d", aRawData, pObj->m_HookTick);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 11, ((const int *)pData)[11], ((const int *)pData)[11]);
			dbg_msg("snapshot", "%s\tm_HookX=%d", aRawData, pObj->m_HookX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 12, ((const int *)pData)[12], ((const int *)pData)[12]);
			dbg_msg("snapshot", "%s\tm_HookY=%d", aRawData, pObj->m_HookY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 13, ((const int *)pData)[13], ((const int *)pData)[13]);
			dbg_msg("snapshot", "%s\tm_HookDx=%d", aRawData, pObj->m_HookDx);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 14, ((const int *)pData)[14], ((const int *)pData)[14]);
			dbg_msg("snapshot", "%s\tm_HookDy=%d", aRawData, pObj->m_HookDy);
			return 0;
		};

		case NETOBJTYPE_CHARACTER:
		{
			CNetObj_Character *pObj = (CNetObj_Character *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_Tick=%d", aRawData, pObj->m_Tick);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_VelX=%d", aRawData, pObj->m_VelX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_VelY=%d", aRawData, pObj->m_VelY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_Angle=%d", aRawData, pObj->m_Angle);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 6, ((const int *)pData)[6], ((const int *)pData)[6]);
			dbg_msg("snapshot", "%s\tm_Direction=%d (min=-1 max=1)", aRawData, pObj->m_Direction);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 7, ((const int *)pData)[7], ((const int *)pData)[7]);
			dbg_msg("snapshot", "%s\tm_Jumped=%d (min=0 max=3)", aRawData, pObj->m_Jumped);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 8, ((const int *)pData)[8], ((const int *)pData)[8]);
			dbg_msg("snapshot", "%s\tm_HookedPlayer=%d (min=-1 max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_HookedPlayer, (int)MAX_CLIENTS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 9, ((const int *)pData)[9], ((const int *)pData)[9]);
			dbg_msg("snapshot", "%s\tm_HookState=%d (min=-1 max=5)", aRawData, pObj->m_HookState);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 10, ((const int *)pData)[10], ((const int *)pData)[10]);
			dbg_msg("snapshot", "%s\tm_HookTick=%d", aRawData, pObj->m_HookTick);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 11, ((const int *)pData)[11], ((const int *)pData)[11]);
			dbg_msg("snapshot", "%s\tm_HookX=%d", aRawData, pObj->m_HookX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 12, ((const int *)pData)[12], ((const int *)pData)[12]);
			dbg_msg("snapshot", "%s\tm_HookY=%d", aRawData, pObj->m_HookY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 13, ((const int *)pData)[13], ((const int *)pData)[13]);
			dbg_msg("snapshot", "%s\tm_HookDx=%d", aRawData, pObj->m_HookDx);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 14, ((const int *)pData)[14], ((const int *)pData)[14]);
			dbg_msg("snapshot", "%s\tm_HookDy=%d", aRawData, pObj->m_HookDy);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 15, ((const int *)pData)[15], ((const int *)pData)[15]);
			dbg_msg("snapshot", "%s\tm_PlayerFlags=%d (min=0 max=256)", aRawData, pObj->m_PlayerFlags);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 16, ((const int *)pData)[16], ((const int *)pData)[16]);
			dbg_msg("snapshot", "%s\tm_Health=%d (min=0 max=10)", aRawData, pObj->m_Health);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 17, ((const int *)pData)[17], ((const int *)pData)[17]);
			dbg_msg("snapshot", "%s\tm_Armor=%d (min=0 max=10)", aRawData, pObj->m_Armor);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 18, ((const int *)pData)[18], ((const int *)pData)[18]);
			dbg_msg("snapshot", "%s\tm_AmmoCount=%d (min=-1 max=10)", aRawData, pObj->m_AmmoCount);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 19, ((const int *)pData)[19], ((const int *)pData)[19]);
			dbg_msg("snapshot", "%s\tm_Weapon=%d (min=-1 max=NUM_WEAPONS-1(%d))", aRawData, pObj->m_Weapon, (int)NUM_WEAPONS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 20, ((const int *)pData)[20], ((const int *)pData)[20]);
			dbg_msg("snapshot", "%s\tm_Emote=%d (min=0 max=6)", aRawData, pObj->m_Emote);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 21, ((const int *)pData)[21], ((const int *)pData)[21]);
			dbg_msg("snapshot", "%s\tm_AttackTick=%d (min=0 max=max_int(%d))", aRawData, pObj->m_AttackTick, (int)max_int);
			return 0;
		};

		case NETOBJTYPE_PLAYERINFO:
		{
			CNetObj_PlayerInfo *pObj = (CNetObj_PlayerInfo *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_Local=%d (min=0 max=1)", aRawData, pObj->m_Local);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_ClientId=%d (min=0 max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_ClientId, (int)MAX_CLIENTS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Team=%d (min=TEAM_SPECTATORS(%d) max=TEAM_BLUE(%d))", aRawData, pObj->m_Team, (int)TEAM_SPECTATORS, (int)TEAM_BLUE);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_Score=%d", aRawData, pObj->m_Score);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_Latency=%d", aRawData, pObj->m_Latency);
			return 0;
		};

		case NETOBJTYPE_CLIENTINFO:
		{
			CNetObj_ClientInfo *pObj = (CNetObj_ClientInfo *)pData;
			aInts[0] = pObj->m_aName[0];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_aName[0]=%d\tIntToStr: %s", aRawData, pObj->m_aName[0], aStr);
			aInts[0] = pObj->m_aName[1];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_aName[1]=%d\tIntToStr: %s", aRawData, pObj->m_aName[1], aStr);
			aInts[0] = pObj->m_aName[2];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_aName[2]=%d\tIntToStr: %s", aRawData, pObj->m_aName[2], aStr);
			aInts[0] = pObj->m_aName[3];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_aName[3]=%d\tIntToStr: %s", aRawData, pObj->m_aName[3], aStr);
			aInts[0] = pObj->m_aClan[0];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_aClan[0]=%d\tIntToStr: %s", aRawData, pObj->m_aClan[0], aStr);
			aInts[0] = pObj->m_aClan[1];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_aClan[1]=%d\tIntToStr: %s", aRawData, pObj->m_aClan[1], aStr);
			aInts[0] = pObj->m_aClan[2];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 6, ((const int *)pData)[6], ((const int *)pData)[6]);
			dbg_msg("snapshot", "%s\tm_aClan[2]=%d\tIntToStr: %s", aRawData, pObj->m_aClan[2], aStr);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 7, ((const int *)pData)[7], ((const int *)pData)[7]);
			dbg_msg("snapshot", "%s\tm_Country=%d", aRawData, pObj->m_Country);
			aInts[0] = pObj->m_aSkin[0];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 8, ((const int *)pData)[8], ((const int *)pData)[8]);
			dbg_msg("snapshot", "%s\tm_aSkin[0]=%d\tIntToStr: %s", aRawData, pObj->m_aSkin[0], aStr);
			aInts[0] = pObj->m_aSkin[1];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 9, ((const int *)pData)[9], ((const int *)pData)[9]);
			dbg_msg("snapshot", "%s\tm_aSkin[1]=%d\tIntToStr: %s", aRawData, pObj->m_aSkin[1], aStr);
			aInts[0] = pObj->m_aSkin[2];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 10, ((const int *)pData)[10], ((const int *)pData)[10]);
			dbg_msg("snapshot", "%s\tm_aSkin[2]=%d\tIntToStr: %s", aRawData, pObj->m_aSkin[2], aStr);
			aInts[0] = pObj->m_aSkin[3];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 11, ((const int *)pData)[11], ((const int *)pData)[11]);
			dbg_msg("snapshot", "%s\tm_aSkin[3]=%d\tIntToStr: %s", aRawData, pObj->m_aSkin[3], aStr);
			aInts[0] = pObj->m_aSkin[4];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 12, ((const int *)pData)[12], ((const int *)pData)[12]);
			dbg_msg("snapshot", "%s\tm_aSkin[4]=%d\tIntToStr: %s", aRawData, pObj->m_aSkin[4], aStr);
			aInts[0] = pObj->m_aSkin[5];
			IntsToStr(aInts, std::size(aInts), aStr, std::size(aStr));
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 13, ((const int *)pData)[13], ((const int *)pData)[13]);
			dbg_msg("snapshot", "%s\tm_aSkin[5]=%d\tIntToStr: %s", aRawData, pObj->m_aSkin[5], aStr);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 14, ((const int *)pData)[14], ((const int *)pData)[14]);
			dbg_msg("snapshot", "%s\tm_UseCustomColor=%d (min=0 max=1)", aRawData, pObj->m_UseCustomColor);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 15, ((const int *)pData)[15], ((const int *)pData)[15]);
			dbg_msg("snapshot", "%s\tm_ColorBody=%d", aRawData, pObj->m_ColorBody);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 16, ((const int *)pData)[16], ((const int *)pData)[16]);
			dbg_msg("snapshot", "%s\tm_ColorFeet=%d", aRawData, pObj->m_ColorFeet);
			return 0;
		};

		case NETOBJTYPE_SPECTATORINFO:
		{
			CNetObj_SpectatorInfo *pObj = (CNetObj_SpectatorInfo *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_SpectatorId=%d (min=SPEC_FREEVIEW(%d) max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_SpectatorId, (int)SPEC_FREEVIEW, (int)MAX_CLIENTS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			return 0;
		};

		case NETOBJTYPE_MYOWNOBJECT:
		{
			CNetObj_MyOwnObject *pObj = (CNetObj_MyOwnObject *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_Test=%d", aRawData, pObj->m_Test);
			return 0;
		};

		case NETOBJTYPE_DDNETCHARACTER:
		{
			CNetObj_DDNetCharacter *pObj = (CNetObj_DDNetCharacter *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_Flags=%d", aRawData, pObj->m_Flags);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_FreezeEnd=%d (NetTick)", aRawData, pObj->m_FreezeEnd);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Jumps=%d (min=-1 max=255)", aRawData, pObj->m_Jumps);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_TeleCheckpoint=%d", aRawData, pObj->m_TeleCheckpoint);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_StrongWeakId=%d (min=0 max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_StrongWeakId, (int)MAX_CLIENTS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_JumpedTotal=%d (min=-1 max=255)", aRawData, pObj->m_JumpedTotal);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 6, ((const int *)pData)[6], ((const int *)pData)[6]);
			dbg_msg("snapshot", "%s\tm_NinjaActivationTick=%d (NetTick)", aRawData, pObj->m_NinjaActivationTick);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 7, ((const int *)pData)[7], ((const int *)pData)[7]);
			dbg_msg("snapshot", "%s\tm_FreezeStart=%d (NetTick)", aRawData, pObj->m_FreezeStart);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 8, ((const int *)pData)[8], ((const int *)pData)[8]);
			dbg_msg("snapshot", "%s\tm_TargetX=%d", aRawData, pObj->m_TargetX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 9, ((const int *)pData)[9], ((const int *)pData)[9]);
			dbg_msg("snapshot", "%s\tm_TargetY=%d", aRawData, pObj->m_TargetY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 10, ((const int *)pData)[10], ((const int *)pData)[10]);
			dbg_msg("snapshot", "%s\tm_TuneZoneOverride=%d (min=TuneZone::OVERRIDE_NONE(%d) max=TuneZone::NUM-1(%d))", aRawData, pObj->m_TuneZoneOverride, (int)TuneZone::OVERRIDE_NONE, (int)TuneZone::NUM - 1);
			return 0;
		};

		case NETOBJTYPE_DDNETPLAYER:
		{
			CNetObj_DDNetPlayer *pObj = (CNetObj_DDNetPlayer *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_Flags=%d", aRawData, pObj->m_Flags);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_AuthLevel=%d (min=AUTHED_NO(%d) max=AUTHED_ADMIN(%d))", aRawData, pObj->m_AuthLevel, (int)AUTHED_NO, (int)AUTHED_ADMIN);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_FinishTimeSeconds=%d (min=FinishTime::UNSET(%d) max=max_int(%d))", aRawData, pObj->m_FinishTimeSeconds, (int)FinishTime::UNSET, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_FinishTimeMillis=%d (min=0 max=999)", aRawData, pObj->m_FinishTimeMillis);
			return 0;
		};

		case NETOBJTYPE_GAMEINFOEX:
		{
			CNetObj_GameInfoEx *pObj = (CNetObj_GameInfoEx *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_Flags=%d", aRawData, pObj->m_Flags);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Version=%d", aRawData, pObj->m_Version);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Flags2=%d", aRawData, pObj->m_Flags2);
			return 0;
		};

		case NETOBJTYPE_DDRACEPROJECTILE:
		{
			CNetObj_DDRaceProjectile *pObj = (CNetObj_DDRaceProjectile *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Angle=%d", aRawData, pObj->m_Angle);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_Data=%d", aRawData, pObj->m_Data);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_Type=%d (min=0 max=NUM_WEAPONS-1(%d))", aRawData, pObj->m_Type, (int)NUM_WEAPONS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_StartTick=%d (NetTick)", aRawData, pObj->m_StartTick);
			return 0;
		};

		case NETOBJTYPE_DDNETLASER:
		{
			CNetObj_DDNetLaser *pObj = (CNetObj_DDNetLaser *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_ToX=%d", aRawData, pObj->m_ToX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_ToY=%d", aRawData, pObj->m_ToY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_FromX=%d", aRawData, pObj->m_FromX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_FromY=%d", aRawData, pObj->m_FromY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_StartTick=%d (NetTick)", aRawData, pObj->m_StartTick);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_Owner=%d (min=-1 max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_Owner, (int)MAX_CLIENTS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 6, ((const int *)pData)[6], ((const int *)pData)[6]);
			dbg_msg("snapshot", "%s\tm_Type=%d", aRawData, pObj->m_Type);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 7, ((const int *)pData)[7], ((const int *)pData)[7]);
			dbg_msg("snapshot", "%s\tm_SwitchNumber=%d", aRawData, pObj->m_SwitchNumber);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 8, ((const int *)pData)[8], ((const int *)pData)[8]);
			dbg_msg("snapshot", "%s\tm_Subtype=%d", aRawData, pObj->m_Subtype);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 9, ((const int *)pData)[9], ((const int *)pData)[9]);
			dbg_msg("snapshot", "%s\tm_Flags=%d", aRawData, pObj->m_Flags);
			return 0;
		};

		case NETOBJTYPE_DDNETPROJECTILE:
		{
			CNetObj_DDNetProjectile *pObj = (CNetObj_DDNetProjectile *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_VelX=%d", aRawData, pObj->m_VelX);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_VelY=%d", aRawData, pObj->m_VelY);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_Type=%d (min=0 max=NUM_WEAPONS-1(%d))", aRawData, pObj->m_Type, (int)NUM_WEAPONS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_StartTick=%d (NetTick)", aRawData, pObj->m_StartTick);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 6, ((const int *)pData)[6], ((const int *)pData)[6]);
			dbg_msg("snapshot", "%s\tm_Owner=%d (min=-1 max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_Owner, (int)MAX_CLIENTS - 1);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 7, ((const int *)pData)[7], ((const int *)pData)[7]);
			dbg_msg("snapshot", "%s\tm_SwitchNumber=%d", aRawData, pObj->m_SwitchNumber);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 8, ((const int *)pData)[8], ((const int *)pData)[8]);
			dbg_msg("snapshot", "%s\tm_TuneZone=%d", aRawData, pObj->m_TuneZone);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 9, ((const int *)pData)[9], ((const int *)pData)[9]);
			dbg_msg("snapshot", "%s\tm_Flags=%d", aRawData, pObj->m_Flags);
			return 0;
		};

		case NETOBJTYPE_DDNETPICKUP:
		{
			CNetObj_DDNetPickup *pObj = (CNetObj_DDNetPickup *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Type=%d (min=0 max=max_int(%d))", aRawData, pObj->m_Type, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_Subtype=%d (min=0 max=max_int(%d))", aRawData, pObj->m_Subtype, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_SwitchNumber=%d", aRawData, pObj->m_SwitchNumber);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_Flags=%d", aRawData, pObj->m_Flags);
			return 0;
		};

		case NETOBJTYPE_DDNETSPECTATORINFO:
		{
			CNetObj_DDNetSpectatorInfo *pObj = (CNetObj_DDNetSpectatorInfo *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_HasCameraInfo=%d (min=0 max=1)", aRawData, pObj->m_HasCameraInfo);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Zoom=%d (min=0 max=max_int(%d))", aRawData, pObj->m_Zoom, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Deadzone=%d (min=0 max=max_int(%d))", aRawData, pObj->m_Deadzone, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_FollowFactor=%d (min=0 max=max_int(%d))", aRawData, pObj->m_FollowFactor, (int)max_int);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_SpectatorCount=%d (min=0 max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_SpectatorCount, (int)MAX_CLIENTS - 1);
			return 0;
		};

		case NETOBJTYPE_SPECTATORCOUNT:
		{
			CNetObj_SpectatorCount *pObj = (CNetObj_SpectatorCount *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_NumSpectators=%d (min=0 max=max_int(%d))", aRawData, pObj->m_NumSpectators, (int)max_int);
			return 0;
		};

		case NETEVENTTYPE_COMMON:
		{
			CNetEvent_Common *pObj = (CNetEvent_Common *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			return 0;
		};

		case NETEVENTTYPE_EXPLOSION:
		{
			CNetEvent_Explosion *pObj = (CNetEvent_Explosion *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			return 0;
		};

		case NETEVENTTYPE_SPAWN:
		{
			CNetEvent_Spawn *pObj = (CNetEvent_Spawn *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			return 0;
		};

		case NETEVENTTYPE_HAMMERHIT:
		{
			CNetEvent_HammerHit *pObj = (CNetEvent_HammerHit *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			return 0;
		};

		case NETEVENTTYPE_DEATH:
		{
			CNetEvent_Death *pObj = (CNetEvent_Death *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_ClientId=%d (min=0 max=MAX_CLIENTS-1(%d))", aRawData, pObj->m_ClientId, (int)MAX_CLIENTS - 1);
			return 0;
		};

		case NETEVENTTYPE_SOUNDGLOBAL:
		{
			CNetEvent_SoundGlobal *pObj = (CNetEvent_SoundGlobal *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_SoundId=%d (min=0 max=NUM_SOUNDS-1(%d))", aRawData, pObj->m_SoundId, (int)NUM_SOUNDS - 1);
			return 0;
		};

		case NETEVENTTYPE_SOUNDWORLD:
		{
			CNetEvent_SoundWorld *pObj = (CNetEvent_SoundWorld *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_SoundId=%d (min=0 max=NUM_SOUNDS-1(%d))", aRawData, pObj->m_SoundId, (int)NUM_SOUNDS - 1);
			return 0;
		};

		case NETEVENTTYPE_DAMAGEIND:
		{
			CNetEvent_DamageInd *pObj = (CNetEvent_DamageInd *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_Angle=%d", aRawData, pObj->m_Angle);
			return 0;
		};

		case NETEVENTTYPE_BIRTHDAY:
		{
			CNetEvent_Birthday *pObj = (CNetEvent_Birthday *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			return 0;
		};

		case NETEVENTTYPE_FINISH:
		{
			CNetEvent_Finish *pObj = (CNetEvent_Finish *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			return 0;
		};

		case NETOBJTYPE_MYOWNEVENT:
		{
			CNetObj_MyOwnEvent *pObj = (CNetObj_MyOwnEvent *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_Test=%d", aRawData, pObj->m_Test);
			return 0;
		};

		case NETOBJTYPE_SPECCHAR:
		{
			CNetObj_SpecChar *pObj = (CNetObj_SpecChar *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			return 0;
		};

		case NETOBJTYPE_SWITCHSTATE:
		{
			CNetObj_SwitchState *pObj = (CNetObj_SwitchState *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_HighestSwitchNumber=%d", aRawData, pObj->m_HighestSwitchNumber);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_aStatus[0]=%d", aRawData, pObj->m_aStatus[0]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_aStatus[1]=%d", aRawData, pObj->m_aStatus[1]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 3, ((const int *)pData)[3], ((const int *)pData)[3]);
			dbg_msg("snapshot", "%s\tm_aStatus[2]=%d", aRawData, pObj->m_aStatus[2]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 4, ((const int *)pData)[4], ((const int *)pData)[4]);
			dbg_msg("snapshot", "%s\tm_aStatus[3]=%d", aRawData, pObj->m_aStatus[3]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 5, ((const int *)pData)[5], ((const int *)pData)[5]);
			dbg_msg("snapshot", "%s\tm_aStatus[4]=%d", aRawData, pObj->m_aStatus[4]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 6, ((const int *)pData)[6], ((const int *)pData)[6]);
			dbg_msg("snapshot", "%s\tm_aStatus[5]=%d", aRawData, pObj->m_aStatus[5]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 7, ((const int *)pData)[7], ((const int *)pData)[7]);
			dbg_msg("snapshot", "%s\tm_aStatus[6]=%d", aRawData, pObj->m_aStatus[6]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 8, ((const int *)pData)[8], ((const int *)pData)[8]);
			dbg_msg("snapshot", "%s\tm_aStatus[7]=%d", aRawData, pObj->m_aStatus[7]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 9, ((const int *)pData)[9], ((const int *)pData)[9]);
			dbg_msg("snapshot", "%s\tm_aSwitchNumbers[0]=%d", aRawData, pObj->m_aSwitchNumbers[0]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 10, ((const int *)pData)[10], ((const int *)pData)[10]);
			dbg_msg("snapshot", "%s\tm_aSwitchNumbers[1]=%d", aRawData, pObj->m_aSwitchNumbers[1]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 11, ((const int *)pData)[11], ((const int *)pData)[11]);
			dbg_msg("snapshot", "%s\tm_aSwitchNumbers[2]=%d", aRawData, pObj->m_aSwitchNumbers[2]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 12, ((const int *)pData)[12], ((const int *)pData)[12]);
			dbg_msg("snapshot", "%s\tm_aSwitchNumbers[3]=%d", aRawData, pObj->m_aSwitchNumbers[3]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 13, ((const int *)pData)[13], ((const int *)pData)[13]);
			dbg_msg("snapshot", "%s\tm_aEndTicks[0]=%d", aRawData, pObj->m_aEndTicks[0]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 14, ((const int *)pData)[14], ((const int *)pData)[14]);
			dbg_msg("snapshot", "%s\tm_aEndTicks[1]=%d", aRawData, pObj->m_aEndTicks[1]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 15, ((const int *)pData)[15], ((const int *)pData)[15]);
			dbg_msg("snapshot", "%s\tm_aEndTicks[2]=%d", aRawData, pObj->m_aEndTicks[2]);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 16, ((const int *)pData)[16], ((const int *)pData)[16]);
			dbg_msg("snapshot", "%s\tm_aEndTicks[3]=%d", aRawData, pObj->m_aEndTicks[3]);
			return 0;
		};

		case NETOBJTYPE_ENTITYEX:
		{
			CNetObj_EntityEx *pObj = (CNetObj_EntityEx *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_SwitchNumber=%d", aRawData, pObj->m_SwitchNumber);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Layer=%d", aRawData, pObj->m_Layer);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_EntityClass=%d", aRawData, pObj->m_EntityClass);
			return 0;
		};

		case NETEVENTTYPE_MAPSOUNDWORLD:
		{
			CNetEvent_MapSoundWorld *pObj = (CNetEvent_MapSoundWorld *)pData;
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 0, ((const int *)pData)[0], ((const int *)pData)[0]);
			dbg_msg("snapshot", "%s\tm_X=%d", aRawData, pObj->m_X);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 1, ((const int *)pData)[1], ((const int *)pData)[1]);
			dbg_msg("snapshot", "%s\tm_Y=%d", aRawData, pObj->m_Y);
			str_format(aRawData, sizeof(aRawData), "\t\t%3d %12d\t%08x", 2, ((const int *)pData)[2], ((const int *)pData)[2]);
			dbg_msg("snapshot", "%s\tm_SoundId=%d", aRawData, pObj->m_SoundId);
			return 0;
		};
		}
		return -1;
	}

	void *CNetObjHandler::SecureUnpackObj(int Type, CUnpacker *pUnpacker)
	{
		m_pObjFailedOn = 0;
		switch(Type)
		{
		case NETOBJTYPE_EX:
		{
			const unsigned char *pPtr = pUnpacker->GetRaw(sizeof(CUuid));
			if(pPtr != 0)
			{
				mem_copy(m_aUnpackedData, pPtr, sizeof(CUuid));
			}
			break;
		}

		case NETOBJTYPE_PLAYERINPUT:
		{
			CNetObj_PlayerInput *pData = (CNetObj_PlayerInput *)m_aUnpackedData;
			pData->m_Direction = pUnpacker->GetUncompressedInt();
			pData->m_TargetX = pUnpacker->GetUncompressedInt();
			pData->m_TargetY = pUnpacker->GetUncompressedInt();
			pData->m_Jump = pUnpacker->GetUncompressedInt();
			pData->m_Fire = pUnpacker->GetUncompressedInt();
			pData->m_Hook = pUnpacker->GetUncompressedInt();
			pData->m_PlayerFlags = pUnpacker->GetUncompressedInt();
			pData->m_WantedWeapon = pUnpacker->GetUncompressedInt();
			pData->m_NextWeapon = pUnpacker->GetUncompressedInt();
			pData->m_PrevWeapon = pUnpacker->GetUncompressedInt();
			pData->m_PlayerFlags = ClampInt("m_PlayerFlags", pData->m_PlayerFlags, 0, 256);
		}
		break;

		case NETOBJTYPE_PROJECTILE:
		{
			CNetObj_Projectile *pData = (CNetObj_Projectile *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_VelX = pUnpacker->GetUncompressedInt();
			pData->m_VelY = pUnpacker->GetUncompressedInt();
			pData->m_Type = pUnpacker->GetUncompressedInt();
			pData->m_StartTick = pUnpacker->GetUncompressedInt();
			pData->m_Type = ClampInt("m_Type", pData->m_Type, 0, NUM_WEAPONS - 1);
		}
		break;

		case NETOBJTYPE_LASER:
		{
			CNetObj_Laser *pData = (CNetObj_Laser *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_FromX = pUnpacker->GetUncompressedInt();
			pData->m_FromY = pUnpacker->GetUncompressedInt();
			pData->m_StartTick = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETOBJTYPE_PICKUP:
		{
			CNetObj_Pickup *pData = (CNetObj_Pickup *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_Type = pUnpacker->GetUncompressedInt();
			pData->m_Subtype = pUnpacker->GetUncompressedInt();
			pData->m_Type = ClampInt("m_Type", pData->m_Type, 0, max_int);
			pData->m_Subtype = ClampInt("m_Subtype", pData->m_Subtype, 0, max_int);
		}
		break;

		case NETOBJTYPE_FLAG:
		{
			CNetObj_Flag *pData = (CNetObj_Flag *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_Team = pUnpacker->GetUncompressedInt();
			pData->m_Team = ClampInt("m_Team", pData->m_Team, TEAM_RED, TEAM_BLUE);
		}
		break;

		case NETOBJTYPE_GAMEINFO:
		{
			CNetObj_GameInfo *pData = (CNetObj_GameInfo *)m_aUnpackedData;
			pData->m_GameFlags = pUnpacker->GetUncompressedInt();
			pData->m_GameStateFlags = pUnpacker->GetUncompressedInt();
			pData->m_RoundStartTick = pUnpacker->GetUncompressedInt();
			pData->m_WarmupTimer = pUnpacker->GetUncompressedInt();
			pData->m_ScoreLimit = pUnpacker->GetUncompressedInt();
			pData->m_TimeLimit = pUnpacker->GetUncompressedInt();
			pData->m_RoundNum = pUnpacker->GetUncompressedInt();
			pData->m_RoundCurrent = pUnpacker->GetUncompressedInt();
			pData->m_GameFlags = ClampInt("m_GameFlags", pData->m_GameFlags, 0, 256);
			pData->m_GameStateFlags = ClampInt("m_GameStateFlags", pData->m_GameStateFlags, 0, 256);
			pData->m_WarmupTimer = ClampInt("m_WarmupTimer", pData->m_WarmupTimer, min_int, max_int);
			pData->m_ScoreLimit = ClampInt("m_ScoreLimit", pData->m_ScoreLimit, 0, max_int);
			pData->m_TimeLimit = ClampInt("m_TimeLimit", pData->m_TimeLimit, 0, max_int);
			pData->m_RoundNum = ClampInt("m_RoundNum", pData->m_RoundNum, 0, max_int);
			pData->m_RoundCurrent = ClampInt("m_RoundCurrent", pData->m_RoundCurrent, 0, max_int);
		}
		break;

		case NETOBJTYPE_GAMEDATA:
		{
			CNetObj_GameData *pData = (CNetObj_GameData *)m_aUnpackedData;
			pData->m_TeamscoreRed = pUnpacker->GetUncompressedInt();
			pData->m_TeamscoreBlue = pUnpacker->GetUncompressedInt();
			pData->m_FlagCarrierRed = pUnpacker->GetUncompressedInt();
			pData->m_FlagCarrierBlue = pUnpacker->GetUncompressedInt();
			pData->m_FlagCarrierRed = ClampInt("m_FlagCarrierRed", pData->m_FlagCarrierRed, FLAG_MISSING, MAX_CLIENTS - 1);
			pData->m_FlagCarrierBlue = ClampInt("m_FlagCarrierBlue", pData->m_FlagCarrierBlue, FLAG_MISSING, MAX_CLIENTS - 1);
		}
		break;

		case NETOBJTYPE_CHARACTERCORE:
		{
			CNetObj_CharacterCore *pData = (CNetObj_CharacterCore *)m_aUnpackedData;
			pData->m_Tick = pUnpacker->GetUncompressedInt();
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_VelX = pUnpacker->GetUncompressedInt();
			pData->m_VelY = pUnpacker->GetUncompressedInt();
			pData->m_Angle = pUnpacker->GetUncompressedInt();
			pData->m_Direction = pUnpacker->GetUncompressedInt();
			pData->m_Jumped = pUnpacker->GetUncompressedInt();
			pData->m_HookedPlayer = pUnpacker->GetUncompressedInt();
			pData->m_HookState = pUnpacker->GetUncompressedInt();
			pData->m_HookTick = pUnpacker->GetUncompressedInt();
			pData->m_HookX = pUnpacker->GetUncompressedInt();
			pData->m_HookY = pUnpacker->GetUncompressedInt();
			pData->m_HookDx = pUnpacker->GetUncompressedInt();
			pData->m_HookDy = pUnpacker->GetUncompressedInt();
			pData->m_Direction = ClampInt("m_Direction", pData->m_Direction, -1, 1);
			pData->m_Jumped = ClampInt("m_Jumped", pData->m_Jumped, 0, 3);
			pData->m_HookedPlayer = ClampInt("m_HookedPlayer", pData->m_HookedPlayer, -1, MAX_CLIENTS - 1);
			pData->m_HookState = ClampInt("m_HookState", pData->m_HookState, -1, 5);
		}
		break;

		case NETOBJTYPE_CHARACTER:
		{
			CNetObj_Character *pData = (CNetObj_Character *)m_aUnpackedData;
			pData->m_Tick = pUnpacker->GetUncompressedInt();
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_VelX = pUnpacker->GetUncompressedInt();
			pData->m_VelY = pUnpacker->GetUncompressedInt();
			pData->m_Angle = pUnpacker->GetUncompressedInt();
			pData->m_Direction = pUnpacker->GetUncompressedInt();
			pData->m_Jumped = pUnpacker->GetUncompressedInt();
			pData->m_HookedPlayer = pUnpacker->GetUncompressedInt();
			pData->m_HookState = pUnpacker->GetUncompressedInt();
			pData->m_HookTick = pUnpacker->GetUncompressedInt();
			pData->m_HookX = pUnpacker->GetUncompressedInt();
			pData->m_HookY = pUnpacker->GetUncompressedInt();
			pData->m_HookDx = pUnpacker->GetUncompressedInt();
			pData->m_HookDy = pUnpacker->GetUncompressedInt();
			pData->m_PlayerFlags = pUnpacker->GetUncompressedInt();
			pData->m_Health = pUnpacker->GetUncompressedInt();
			pData->m_Armor = pUnpacker->GetUncompressedInt();
			pData->m_AmmoCount = pUnpacker->GetUncompressedInt();
			pData->m_Weapon = pUnpacker->GetUncompressedInt();
			pData->m_Emote = pUnpacker->GetUncompressedInt();
			pData->m_AttackTick = pUnpacker->GetUncompressedInt();
			pData->m_Direction = ClampInt("m_Direction", pData->m_Direction, -1, 1);
			pData->m_Jumped = ClampInt("m_Jumped", pData->m_Jumped, 0, 3);
			pData->m_HookedPlayer = ClampInt("m_HookedPlayer", pData->m_HookedPlayer, -1, MAX_CLIENTS - 1);
			pData->m_HookState = ClampInt("m_HookState", pData->m_HookState, -1, 5);
			pData->m_PlayerFlags = ClampInt("m_PlayerFlags", pData->m_PlayerFlags, 0, 256);
			pData->m_Health = ClampInt("m_Health", pData->m_Health, 0, 10);
			pData->m_Armor = ClampInt("m_Armor", pData->m_Armor, 0, 10);
			pData->m_AmmoCount = ClampInt("m_AmmoCount", pData->m_AmmoCount, -1, 10);
			pData->m_Weapon = ClampInt("m_Weapon", pData->m_Weapon, -1, NUM_WEAPONS - 1);
			pData->m_Emote = ClampInt("m_Emote", pData->m_Emote, 0, 6);
			pData->m_AttackTick = ClampInt("m_AttackTick", pData->m_AttackTick, 0, max_int);
		}
		break;

		case NETOBJTYPE_PLAYERINFO:
		{
			CNetObj_PlayerInfo *pData = (CNetObj_PlayerInfo *)m_aUnpackedData;
			pData->m_Local = pUnpacker->GetUncompressedInt();
			pData->m_ClientId = pUnpacker->GetUncompressedInt();
			pData->m_Team = pUnpacker->GetUncompressedInt();
			pData->m_Score = pUnpacker->GetUncompressedInt();
			pData->m_Latency = pUnpacker->GetUncompressedInt();
			pData->m_Local = ClampInt("m_Local", pData->m_Local, 0, 1);
			pData->m_ClientId = ClampInt("m_ClientId", pData->m_ClientId, 0, MAX_CLIENTS - 1);
			pData->m_Team = ClampInt("m_Team", pData->m_Team, TEAM_SPECTATORS, TEAM_BLUE);
		}
		break;

		case NETOBJTYPE_CLIENTINFO:
		{
			CNetObj_ClientInfo *pData = (CNetObj_ClientInfo *)m_aUnpackedData;
			pData->m_aName[0] = pUnpacker->GetUncompressedInt();
			pData->m_aName[1] = pUnpacker->GetUncompressedInt();
			pData->m_aName[2] = pUnpacker->GetUncompressedInt();
			pData->m_aName[3] = pUnpacker->GetUncompressedInt();
			pData->m_aClan[0] = pUnpacker->GetUncompressedInt();
			pData->m_aClan[1] = pUnpacker->GetUncompressedInt();
			pData->m_aClan[2] = pUnpacker->GetUncompressedInt();
			pData->m_Country = pUnpacker->GetUncompressedInt();
			pData->m_aSkin[0] = pUnpacker->GetUncompressedInt();
			pData->m_aSkin[1] = pUnpacker->GetUncompressedInt();
			pData->m_aSkin[2] = pUnpacker->GetUncompressedInt();
			pData->m_aSkin[3] = pUnpacker->GetUncompressedInt();
			pData->m_aSkin[4] = pUnpacker->GetUncompressedInt();
			pData->m_aSkin[5] = pUnpacker->GetUncompressedInt();
			pData->m_UseCustomColor = pUnpacker->GetUncompressedInt();
			pData->m_ColorBody = pUnpacker->GetUncompressedInt();
			pData->m_ColorFeet = pUnpacker->GetUncompressedInt();
			pData->m_UseCustomColor = ClampInt("m_UseCustomColor", pData->m_UseCustomColor, 0, 1);
		}
		break;

		case NETOBJTYPE_SPECTATORINFO:
		{
			CNetObj_SpectatorInfo *pData = (CNetObj_SpectatorInfo *)m_aUnpackedData;
			pData->m_SpectatorId = pUnpacker->GetUncompressedInt();
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_SpectatorId = ClampInt("m_SpectatorId", pData->m_SpectatorId, SPEC_FREEVIEW, MAX_CLIENTS - 1);
		}
		break;

		case NETOBJTYPE_MYOWNOBJECT:
		{
			CNetObj_MyOwnObject *pData = (CNetObj_MyOwnObject *)m_aUnpackedData;
			pData->m_Test = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETOBJTYPE_DDNETCHARACTER:
		{
			CNetObj_DDNetCharacter *pData = (CNetObj_DDNetCharacter *)m_aUnpackedData;
			pData->m_Flags = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_FreezeEnd = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_Jumps = pUnpacker->GetUncompressedIntOrDefault(2);
			pData->m_TeleCheckpoint = pUnpacker->GetUncompressedIntOrDefault(-1);
			pData->m_StrongWeakId = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_JumpedTotal = pUnpacker->GetUncompressedIntOrDefault(-1);
			pData->m_NinjaActivationTick = pUnpacker->GetUncompressedIntOrDefault(-1);
			pData->m_FreezeStart = pUnpacker->GetUncompressedIntOrDefault(-1);
			pData->m_TargetX = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_TargetY = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_TuneZoneOverride = pUnpacker->GetUncompressedIntOrDefault(TuneZone::OVERRIDE_NONE);
			pData->m_Jumps = ClampInt("m_Jumps", pData->m_Jumps, -1, 255);
			pData->m_StrongWeakId = ClampInt("m_StrongWeakId", pData->m_StrongWeakId, 0, MAX_CLIENTS - 1);
			pData->m_JumpedTotal = ClampInt("m_JumpedTotal", pData->m_JumpedTotal, -1, 255);
			pData->m_TuneZoneOverride = ClampInt("m_TuneZoneOverride", pData->m_TuneZoneOverride, TuneZone::OVERRIDE_NONE, TuneZone::NUM - 1);
		}
		break;

		case NETOBJTYPE_DDNETPLAYER:
		{
			CNetObj_DDNetPlayer *pData = (CNetObj_DDNetPlayer *)m_aUnpackedData;
			pData->m_Flags = pUnpacker->GetUncompressedInt();
			pData->m_AuthLevel = pUnpacker->GetUncompressedInt();
			pData->m_FinishTimeSeconds = pUnpacker->GetUncompressedIntOrDefault(FinishTime::UNSET);
			pData->m_FinishTimeMillis = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_AuthLevel = ClampInt("m_AuthLevel", pData->m_AuthLevel, AUTHED_NO, AUTHED_ADMIN);
			pData->m_FinishTimeSeconds = ClampInt("m_FinishTimeSeconds", pData->m_FinishTimeSeconds, FinishTime::UNSET, max_int);
			pData->m_FinishTimeMillis = ClampInt("m_FinishTimeMillis", pData->m_FinishTimeMillis, 0, 999);
		}
		break;

		case NETOBJTYPE_GAMEINFOEX:
		{
			CNetObj_GameInfoEx *pData = (CNetObj_GameInfoEx *)m_aUnpackedData;
			pData->m_Flags = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_Version = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_Flags2 = pUnpacker->GetUncompressedIntOrDefault(0);
		}
		break;

		case NETOBJTYPE_DDRACEPROJECTILE:
		{
			CNetObj_DDRaceProjectile *pData = (CNetObj_DDRaceProjectile *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_Angle = pUnpacker->GetUncompressedInt();
			pData->m_Data = pUnpacker->GetUncompressedInt();
			pData->m_Type = pUnpacker->GetUncompressedInt();
			pData->m_StartTick = pUnpacker->GetUncompressedInt();
			pData->m_Type = ClampInt("m_Type", pData->m_Type, 0, NUM_WEAPONS - 1);
		}
		break;

		case NETOBJTYPE_DDNETLASER:
		{
			CNetObj_DDNetLaser *pData = (CNetObj_DDNetLaser *)m_aUnpackedData;
			pData->m_ToX = pUnpacker->GetUncompressedInt();
			pData->m_ToY = pUnpacker->GetUncompressedInt();
			pData->m_FromX = pUnpacker->GetUncompressedInt();
			pData->m_FromY = pUnpacker->GetUncompressedInt();
			pData->m_StartTick = pUnpacker->GetUncompressedInt();
			pData->m_Owner = pUnpacker->GetUncompressedInt();
			pData->m_Type = pUnpacker->GetUncompressedInt();
			pData->m_SwitchNumber = pUnpacker->GetUncompressedIntOrDefault(-1);
			pData->m_Subtype = pUnpacker->GetUncompressedIntOrDefault(-1);
			pData->m_Flags = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_Owner = ClampInt("m_Owner", pData->m_Owner, -1, MAX_CLIENTS - 1);
		}
		break;

		case NETOBJTYPE_DDNETPROJECTILE:
		{
			CNetObj_DDNetProjectile *pData = (CNetObj_DDNetProjectile *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_VelX = pUnpacker->GetUncompressedInt();
			pData->m_VelY = pUnpacker->GetUncompressedInt();
			pData->m_Type = pUnpacker->GetUncompressedInt();
			pData->m_StartTick = pUnpacker->GetUncompressedInt();
			pData->m_Owner = pUnpacker->GetUncompressedInt();
			pData->m_SwitchNumber = pUnpacker->GetUncompressedInt();
			pData->m_TuneZone = pUnpacker->GetUncompressedInt();
			pData->m_Flags = pUnpacker->GetUncompressedInt();
			pData->m_Type = ClampInt("m_Type", pData->m_Type, 0, NUM_WEAPONS - 1);
			pData->m_Owner = ClampInt("m_Owner", pData->m_Owner, -1, MAX_CLIENTS - 1);
		}
		break;

		case NETOBJTYPE_DDNETPICKUP:
		{
			CNetObj_DDNetPickup *pData = (CNetObj_DDNetPickup *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_Type = pUnpacker->GetUncompressedInt();
			pData->m_Subtype = pUnpacker->GetUncompressedInt();
			pData->m_SwitchNumber = pUnpacker->GetUncompressedInt();
			pData->m_Flags = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_Type = ClampInt("m_Type", pData->m_Type, 0, max_int);
			pData->m_Subtype = ClampInt("m_Subtype", pData->m_Subtype, 0, max_int);
		}
		break;

		case NETOBJTYPE_DDNETSPECTATORINFO:
		{
			CNetObj_DDNetSpectatorInfo *pData = (CNetObj_DDNetSpectatorInfo *)m_aUnpackedData;
			pData->m_HasCameraInfo = pUnpacker->GetUncompressedInt();
			pData->m_Zoom = pUnpacker->GetUncompressedInt();
			pData->m_Deadzone = pUnpacker->GetUncompressedInt();
			pData->m_FollowFactor = pUnpacker->GetUncompressedInt();
			pData->m_SpectatorCount = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_HasCameraInfo = ClampInt("m_HasCameraInfo", pData->m_HasCameraInfo, 0, 1);
			pData->m_Zoom = ClampInt("m_Zoom", pData->m_Zoom, 0, max_int);
			pData->m_Deadzone = ClampInt("m_Deadzone", pData->m_Deadzone, 0, max_int);
			pData->m_FollowFactor = ClampInt("m_FollowFactor", pData->m_FollowFactor, 0, max_int);
			pData->m_SpectatorCount = ClampInt("m_SpectatorCount", pData->m_SpectatorCount, 0, MAX_CLIENTS - 1);
		}
		break;

		case NETOBJTYPE_SPECTATORCOUNT:
		{
			CNetObj_SpectatorCount *pData = (CNetObj_SpectatorCount *)m_aUnpackedData;
			pData->m_NumSpectators = pUnpacker->GetUncompressedInt();
			pData->m_NumSpectators = ClampInt("m_NumSpectators", pData->m_NumSpectators, 0, max_int);
		}
		break;

		case NETEVENTTYPE_COMMON:
		{
			CNetEvent_Common *pData = (CNetEvent_Common *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETEVENTTYPE_EXPLOSION:
		{
			CNetEvent_Explosion *pData = (CNetEvent_Explosion *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETEVENTTYPE_SPAWN:
		{
			CNetEvent_Spawn *pData = (CNetEvent_Spawn *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETEVENTTYPE_HAMMERHIT:
		{
			CNetEvent_HammerHit *pData = (CNetEvent_HammerHit *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETEVENTTYPE_DEATH:
		{
			CNetEvent_Death *pData = (CNetEvent_Death *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_ClientId = pUnpacker->GetUncompressedInt();
			pData->m_ClientId = ClampInt("m_ClientId", pData->m_ClientId, 0, MAX_CLIENTS - 1);
		}
		break;

		case NETEVENTTYPE_SOUNDGLOBAL:
		{
			CNetEvent_SoundGlobal *pData = (CNetEvent_SoundGlobal *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_SoundId = pUnpacker->GetUncompressedInt();
			pData->m_SoundId = ClampInt("m_SoundId", pData->m_SoundId, 0, NUM_SOUNDS - 1);
		}
		break;

		case NETEVENTTYPE_SOUNDWORLD:
		{
			CNetEvent_SoundWorld *pData = (CNetEvent_SoundWorld *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_SoundId = pUnpacker->GetUncompressedInt();
			pData->m_SoundId = ClampInt("m_SoundId", pData->m_SoundId, 0, NUM_SOUNDS - 1);
		}
		break;

		case NETEVENTTYPE_DAMAGEIND:
		{
			CNetEvent_DamageInd *pData = (CNetEvent_DamageInd *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_Angle = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETEVENTTYPE_BIRTHDAY:
		{
			CNetEvent_Birthday *pData = (CNetEvent_Birthday *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETEVENTTYPE_FINISH:
		{
			CNetEvent_Finish *pData = (CNetEvent_Finish *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETOBJTYPE_MYOWNEVENT:
		{
			CNetObj_MyOwnEvent *pData = (CNetObj_MyOwnEvent *)m_aUnpackedData;
			pData->m_Test = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETOBJTYPE_SPECCHAR:
		{
			CNetObj_SpecChar *pData = (CNetObj_SpecChar *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETOBJTYPE_SWITCHSTATE:
		{
			CNetObj_SwitchState *pData = (CNetObj_SwitchState *)m_aUnpackedData;
			pData->m_HighestSwitchNumber = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aStatus[0] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aStatus[1] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aStatus[2] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aStatus[3] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aStatus[4] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aStatus[5] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aStatus[6] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aStatus[7] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aSwitchNumbers[0] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aSwitchNumbers[1] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aSwitchNumbers[2] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aSwitchNumbers[3] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aEndTicks[0] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aEndTicks[1] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aEndTicks[2] = pUnpacker->GetUncompressedIntOrDefault(0);
			pData->m_aEndTicks[3] = pUnpacker->GetUncompressedIntOrDefault(0);
		}
		break;

		case NETOBJTYPE_ENTITYEX:
		{
			CNetObj_EntityEx *pData = (CNetObj_EntityEx *)m_aUnpackedData;
			pData->m_SwitchNumber = pUnpacker->GetUncompressedInt();
			pData->m_Layer = pUnpacker->GetUncompressedInt();
			pData->m_EntityClass = pUnpacker->GetUncompressedInt();
		}
		break;

		case NETEVENTTYPE_MAPSOUNDWORLD:
		{
			CNetEvent_MapSoundWorld *pData = (CNetEvent_MapSoundWorld *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetUncompressedInt();
			pData->m_Y = pUnpacker->GetUncompressedInt();
			pData->m_SoundId = pUnpacker->GetUncompressedInt();
		}
		break;

		default:
			m_pObjFailedOn = "(type out of range)";
			break;
		}

		if(pUnpacker->Error())
			m_pObjFailedOn = "(unpack error)";

		if(m_pObjFailedOn)
			return 0;
		m_pObjFailedOn = "";
		return m_aUnpackedData;
	}

	void *CNetObjHandler::SecureUnpackMsg(int Type, CUnpacker *pUnpacker)
	{
		m_pMsgFailedOn = 0;
		switch(Type)
		{
		case NETMSGTYPE_SV_MOTD:
		{
			CNetMsg_Sv_Motd *pData = (CNetMsg_Sv_Motd *)m_aUnpackedData;
			pData->m_pMessage = pUnpacker->GetString();
		}
		break;

		case NETMSGTYPE_SV_BROADCAST:
		{
			CNetMsg_Sv_Broadcast *pData = (CNetMsg_Sv_Broadcast *)m_aUnpackedData;
			pData->m_pMessage = pUnpacker->GetString();
		}
		break;

		case NETMSGTYPE_SV_CHAT:
		{
			CNetMsg_Sv_Chat *pData = (CNetMsg_Sv_Chat *)m_aUnpackedData;
			pData->m_Team = pUnpacker->GetInt();
			pData->m_ClientId = pUnpacker->GetInt();
			pData->m_pMessage = pUnpacker->GetString(CUnpacker::SANITIZE_CC);
			if(pData->m_Team < -2 || pData->m_Team > 3)
			{
				m_pMsgFailedOn = "m_Team";
				break;
			}
			if(pData->m_ClientId < -1 || pData->m_ClientId > MAX_CLIENTS - 1)
			{
				m_pMsgFailedOn = "m_ClientId";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_KILLMSG:
		{
			CNetMsg_Sv_KillMsg *pData = (CNetMsg_Sv_KillMsg *)m_aUnpackedData;
			pData->m_Killer = pUnpacker->GetInt();
			pData->m_Victim = pUnpacker->GetInt();
			pData->m_Weapon = pUnpacker->GetInt();
			pData->m_ModeSpecial = pUnpacker->GetInt();
			if(pData->m_Killer < 0 || pData->m_Killer > MAX_CLIENTS - 1)
			{
				m_pMsgFailedOn = "m_Killer";
				break;
			}
			if(pData->m_Victim < 0 || pData->m_Victim > MAX_CLIENTS - 1)
			{
				m_pMsgFailedOn = "m_Victim";
				break;
			}
			if(pData->m_Weapon < WEAPON_GAME || pData->m_Weapon > NUM_WEAPONS - 1)
			{
				m_pMsgFailedOn = "m_Weapon";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_SOUNDGLOBAL:
		{
			CNetMsg_Sv_SoundGlobal *pData = (CNetMsg_Sv_SoundGlobal *)m_aUnpackedData;
			pData->m_SoundId = pUnpacker->GetInt();
			if(pData->m_SoundId < 0 || pData->m_SoundId > NUM_SOUNDS - 1)
			{
				m_pMsgFailedOn = "m_SoundId";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_TUNEPARAMS:
		{
			CNetMsg_Sv_TuneParams *pData = (CNetMsg_Sv_TuneParams *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_UNUSED:
		{
			CNetMsg_Unused *pData = (CNetMsg_Unused *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_READYTOENTER:
		{
			CNetMsg_Sv_ReadyToEnter *pData = (CNetMsg_Sv_ReadyToEnter *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_WEAPONPICKUP:
		{
			CNetMsg_Sv_WeaponPickup *pData = (CNetMsg_Sv_WeaponPickup *)m_aUnpackedData;
			pData->m_Weapon = pUnpacker->GetInt();
			if(pData->m_Weapon < 0 || pData->m_Weapon > NUM_WEAPONS - 1)
			{
				m_pMsgFailedOn = "m_Weapon";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_EMOTICON:
		{
			CNetMsg_Sv_Emoticon *pData = (CNetMsg_Sv_Emoticon *)m_aUnpackedData;
			pData->m_ClientId = pUnpacker->GetInt();
			pData->m_Emoticon = pUnpacker->GetInt();
			if(pData->m_ClientId < 0 || pData->m_ClientId > MAX_CLIENTS - 1)
			{
				m_pMsgFailedOn = "m_ClientId";
				break;
			}
			if(pData->m_Emoticon < 0 || pData->m_Emoticon > NUM_EMOTICONS - 1)
			{
				m_pMsgFailedOn = "m_Emoticon";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_VOTECLEAROPTIONS:
		{
			CNetMsg_Sv_VoteClearOptions *pData = (CNetMsg_Sv_VoteClearOptions *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_VOTEOPTIONLISTADD:
		{
			CNetMsg_Sv_VoteOptionListAdd *pData = (CNetMsg_Sv_VoteOptionListAdd *)m_aUnpackedData;
			pData->m_NumOptions = pUnpacker->GetInt();
			pData->m_pDescription0 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription1 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription2 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription3 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription4 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription5 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription6 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription7 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription8 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription9 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription10 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription11 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription12 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription13 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pDescription14 = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			if(pData->m_NumOptions < 1 || pData->m_NumOptions > 15)
			{
				m_pMsgFailedOn = "m_NumOptions";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_VOTEOPTIONADD:
		{
			CNetMsg_Sv_VoteOptionAdd *pData = (CNetMsg_Sv_VoteOptionAdd *)m_aUnpackedData;
			pData->m_pDescription = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
		}
		break;

		case NETMSGTYPE_SV_VOTEOPTIONREMOVE:
		{
			CNetMsg_Sv_VoteOptionRemove *pData = (CNetMsg_Sv_VoteOptionRemove *)m_aUnpackedData;
			pData->m_pDescription = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
		}
		break;

		case NETMSGTYPE_SV_VOTESET:
		{
			CNetMsg_Sv_VoteSet *pData = (CNetMsg_Sv_VoteSet *)m_aUnpackedData;
			pData->m_Timeout = pUnpacker->GetInt();
			pData->m_pDescription = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pReason = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			if(pData->m_Timeout < 0 || pData->m_Timeout > max_int)
			{
				m_pMsgFailedOn = "m_Timeout";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_VOTESTATUS:
		{
			CNetMsg_Sv_VoteStatus *pData = (CNetMsg_Sv_VoteStatus *)m_aUnpackedData;
			pData->m_Yes = pUnpacker->GetInt();
			pData->m_No = pUnpacker->GetInt();
			pData->m_Pass = pUnpacker->GetInt();
			pData->m_Total = pUnpacker->GetInt();
			if(pData->m_Yes < 0 || pData->m_Yes > MAX_CLIENTS)
			{
				m_pMsgFailedOn = "m_Yes";
				break;
			}
			if(pData->m_No < 0 || pData->m_No > MAX_CLIENTS)
			{
				m_pMsgFailedOn = "m_No";
				break;
			}
			if(pData->m_Pass < 0 || pData->m_Pass > MAX_CLIENTS)
			{
				m_pMsgFailedOn = "m_Pass";
				break;
			}
			if(pData->m_Total < 0 || pData->m_Total > MAX_CLIENTS)
			{
				m_pMsgFailedOn = "m_Total";
				break;
			}
		}
		break;

		case NETMSGTYPE_CL_SAY:
		{
			CNetMsg_Cl_Say *pData = (CNetMsg_Cl_Say *)m_aUnpackedData;
			pData->m_Team = pUnpacker->GetInt();
			pData->m_pMessage = pUnpacker->GetString(CUnpacker::SANITIZE_CC);
			if(pData->m_Team < 0 || pData->m_Team > 1)
			{
				m_pMsgFailedOn = "m_Team";
				break;
			}
		}
		break;

		case NETMSGTYPE_CL_SETTEAM:
		{
			CNetMsg_Cl_SetTeam *pData = (CNetMsg_Cl_SetTeam *)m_aUnpackedData;
			pData->m_Team = pUnpacker->GetInt();
			if(pData->m_Team < TEAM_SPECTATORS || pData->m_Team > TEAM_BLUE)
			{
				m_pMsgFailedOn = "m_Team";
				break;
			}
		}
		break;

		case NETMSGTYPE_CL_SETSPECTATORMODE:
		{
			CNetMsg_Cl_SetSpectatorMode *pData = (CNetMsg_Cl_SetSpectatorMode *)m_aUnpackedData;
			pData->m_SpectatorId = pUnpacker->GetInt();
			if(pData->m_SpectatorId < SPEC_FREEVIEW || pData->m_SpectatorId > MAX_CLIENTS - 1)
			{
				m_pMsgFailedOn = "m_SpectatorId";
				break;
			}
		}
		break;

		case NETMSGTYPE_CL_STARTINFO:
		{
			CNetMsg_Cl_StartInfo *pData = (CNetMsg_Cl_StartInfo *)m_aUnpackedData;
			pData->m_pName = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pClan = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_Country = pUnpacker->GetInt();
			pData->m_pSkin = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_UseCustomColor = pUnpacker->GetInt();
			pData->m_ColorBody = pUnpacker->GetInt();
			pData->m_ColorFeet = pUnpacker->GetInt();
			if(pData->m_UseCustomColor < 0 || pData->m_UseCustomColor > 1)
			{
				m_pMsgFailedOn = "m_UseCustomColor";
				break;
			}
		}
		break;

		case NETMSGTYPE_CL_CHANGEINFO:
		{
			CNetMsg_Cl_ChangeInfo *pData = (CNetMsg_Cl_ChangeInfo *)m_aUnpackedData;
			pData->m_pName = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pClan = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_Country = pUnpacker->GetInt();
			pData->m_pSkin = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_UseCustomColor = pUnpacker->GetInt();
			pData->m_ColorBody = pUnpacker->GetInt();
			pData->m_ColorFeet = pUnpacker->GetInt();
			if(pData->m_UseCustomColor < 0 || pData->m_UseCustomColor > 1)
			{
				m_pMsgFailedOn = "m_UseCustomColor";
				break;
			}
		}
		break;

		case NETMSGTYPE_CL_KILL:
		{
			CNetMsg_Cl_Kill *pData = (CNetMsg_Cl_Kill *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_CL_EMOTICON:
		{
			CNetMsg_Cl_Emoticon *pData = (CNetMsg_Cl_Emoticon *)m_aUnpackedData;
			pData->m_Emoticon = pUnpacker->GetInt();
			if(pData->m_Emoticon < 0 || pData->m_Emoticon > NUM_EMOTICONS - 1)
			{
				m_pMsgFailedOn = "m_Emoticon";
				break;
			}
		}
		break;

		case NETMSGTYPE_CL_VOTE:
		{
			CNetMsg_Cl_Vote *pData = (CNetMsg_Cl_Vote *)m_aUnpackedData;
			pData->m_Vote = pUnpacker->GetInt();
			if(pData->m_Vote < -1 || pData->m_Vote > 1)
			{
				m_pMsgFailedOn = "m_Vote";
				break;
			}
		}
		break;

		case NETMSGTYPE_CL_CALLVOTE:
		{
			CNetMsg_Cl_CallVote *pData = (CNetMsg_Cl_CallVote *)m_aUnpackedData;
			pData->m_pType = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pValue = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pReason = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
		}
		break;

		case NETMSGTYPE_CL_ISDDNETLEGACY:
		{
			CNetMsg_Cl_IsDDNetLegacy *pData = (CNetMsg_Cl_IsDDNetLegacy *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_DDRACETIMELEGACY:
		{
			CNetMsg_Sv_DDRaceTimeLegacy *pData = (CNetMsg_Sv_DDRaceTimeLegacy *)m_aUnpackedData;
			pData->m_Time = pUnpacker->GetInt();
			pData->m_Check = pUnpacker->GetInt();
			pData->m_Finish = pUnpacker->GetInt();
			if(pData->m_Finish < 0 || pData->m_Finish > 1)
			{
				m_pMsgFailedOn = "m_Finish";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_RECORDLEGACY:
		{
			CNetMsg_Sv_RecordLegacy *pData = (CNetMsg_Sv_RecordLegacy *)m_aUnpackedData;
			pData->m_ServerTimeBest = pUnpacker->GetInt();
			pData->m_PlayerTimeBest = pUnpacker->GetInt();
		}
		break;

		case NETMSGTYPE_UNUSED2:
		{
			CNetMsg_Unused2 *pData = (CNetMsg_Unused2 *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_TEAMSSTATELEGACY:
		{
			CNetMsg_Sv_TeamsStateLegacy *pData = (CNetMsg_Sv_TeamsStateLegacy *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_CL_SHOWOTHERSLEGACY:
		{
			CNetMsg_Cl_ShowOthersLegacy *pData = (CNetMsg_Cl_ShowOthersLegacy *)m_aUnpackedData;
			pData->m_Show = pUnpacker->GetInt();
			if(pData->m_Show < 0 || pData->m_Show > 1)
			{
				m_pMsgFailedOn = "m_Show";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_MYOWNMESSAGE:
		{
			CNetMsg_Sv_MyOwnMessage *pData = (CNetMsg_Sv_MyOwnMessage *)m_aUnpackedData;
			pData->m_Test = pUnpacker->GetInt();
		}
		break;

		case NETMSGTYPE_CL_SHOWDISTANCE:
		{
			CNetMsg_Cl_ShowDistance *pData = (CNetMsg_Cl_ShowDistance *)m_aUnpackedData;
			pData->m_X = pUnpacker->GetInt();
			pData->m_Y = pUnpacker->GetInt();
		}
		break;

		case NETMSGTYPE_CL_SHOWOTHERS:
		{
			CNetMsg_Cl_ShowOthers *pData = (CNetMsg_Cl_ShowOthers *)m_aUnpackedData;
			pData->m_Show = pUnpacker->GetInt();
			if(pData->m_Show < 0 || pData->m_Show > 2)
			{
				m_pMsgFailedOn = "m_Show";
				break;
			}
		}
		break;

		case NETMSGTYPE_CL_CAMERAINFO:
		{
			CNetMsg_Cl_CameraInfo *pData = (CNetMsg_Cl_CameraInfo *)m_aUnpackedData;
			pData->m_Zoom = pUnpacker->GetInt();
			pData->m_Deadzone = pUnpacker->GetInt();
			pData->m_FollowFactor = pUnpacker->GetInt();
		}
		break;

		case NETMSGTYPE_SV_TEAMSSTATE:
		{
			CNetMsg_Sv_TeamsState *pData = (CNetMsg_Sv_TeamsState *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_DDRACETIME:
		{
			CNetMsg_Sv_DDRaceTime *pData = (CNetMsg_Sv_DDRaceTime *)m_aUnpackedData;
			pData->m_Time = pUnpacker->GetInt();
			pData->m_Check = pUnpacker->GetInt();
			pData->m_Finish = pUnpacker->GetInt();
			if(pData->m_Finish < 0 || pData->m_Finish > 1)
			{
				m_pMsgFailedOn = "m_Finish";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_RECORD:
		{
			CNetMsg_Sv_Record *pData = (CNetMsg_Sv_Record *)m_aUnpackedData;
			pData->m_ServerTimeBest = pUnpacker->GetInt();
			pData->m_PlayerTimeBest = pUnpacker->GetInt();
		}
		break;

		case NETMSGTYPE_SV_KILLMSGTEAM:
		{
			CNetMsg_Sv_KillMsgTeam *pData = (CNetMsg_Sv_KillMsgTeam *)m_aUnpackedData;
			pData->m_Team = pUnpacker->GetInt();
			pData->m_First = pUnpacker->GetInt();
			if(pData->m_Team < 0 || pData->m_Team > MAX_CLIENTS - 1)
			{
				m_pMsgFailedOn = "m_Team";
				break;
			}
			if(pData->m_First < -1 || pData->m_First > MAX_CLIENTS - 1)
			{
				m_pMsgFailedOn = "m_First";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_YOURVOTE:
		{
			CNetMsg_Sv_YourVote *pData = (CNetMsg_Sv_YourVote *)m_aUnpackedData;
			pData->m_Voted = pUnpacker->GetInt();
			if(pData->m_Voted < -1 || pData->m_Voted > 1)
			{
				m_pMsgFailedOn = "m_Voted";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_RACEFINISH:
		{
			CNetMsg_Sv_RaceFinish *pData = (CNetMsg_Sv_RaceFinish *)m_aUnpackedData;
			pData->m_ClientId = pUnpacker->GetInt();
			pData->m_Time = pUnpacker->GetInt();
			pData->m_Diff = pUnpacker->GetInt();
			pData->m_RecordPersonal = pUnpacker->GetInt();
			pData->m_RecordServer = pUnpacker->GetIntOrDefault(0);
			if(pData->m_ClientId < 0 || pData->m_ClientId > MAX_CLIENTS - 1)
			{
				m_pMsgFailedOn = "m_ClientId";
				break;
			}
			if(pData->m_RecordPersonal < 0 || pData->m_RecordPersonal > 1)
			{
				m_pMsgFailedOn = "m_RecordPersonal";
				break;
			}
			if(pData->m_RecordServer < 0 || pData->m_RecordServer > 1)
			{
				m_pMsgFailedOn = "m_RecordServer";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_COMMANDINFO:
		{
			CNetMsg_Sv_CommandInfo *pData = (CNetMsg_Sv_CommandInfo *)m_aUnpackedData;
			pData->m_pName = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pArgsFormat = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pHelpText = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
		}
		break;

		case NETMSGTYPE_SV_COMMANDINFOREMOVE:
		{
			CNetMsg_Sv_CommandInfoRemove *pData = (CNetMsg_Sv_CommandInfoRemove *)m_aUnpackedData;
			pData->m_pName = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
		}
		break;

		case NETMSGTYPE_SV_VOTEOPTIONGROUPSTART:
		{
			CNetMsg_Sv_VoteOptionGroupStart *pData = (CNetMsg_Sv_VoteOptionGroupStart *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_VOTEOPTIONGROUPEND:
		{
			CNetMsg_Sv_VoteOptionGroupEnd *pData = (CNetMsg_Sv_VoteOptionGroupEnd *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_COMMANDINFOGROUPSTART:
		{
			CNetMsg_Sv_CommandInfoGroupStart *pData = (CNetMsg_Sv_CommandInfoGroupStart *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_COMMANDINFOGROUPEND:
		{
			CNetMsg_Sv_CommandInfoGroupEnd *pData = (CNetMsg_Sv_CommandInfoGroupEnd *)m_aUnpackedData;
			(void)pData;
		}
		break;

		case NETMSGTYPE_SV_CHANGEINFOCOOLDOWN:
		{
			CNetMsg_Sv_ChangeInfoCooldown *pData = (CNetMsg_Sv_ChangeInfoCooldown *)m_aUnpackedData;
			pData->m_WaitUntil = pUnpacker->GetInt();
		}
		break;

		case NETMSGTYPE_SV_MAPSOUNDGLOBAL:
		{
			CNetMsg_Sv_MapSoundGlobal *pData = (CNetMsg_Sv_MapSoundGlobal *)m_aUnpackedData;
			pData->m_SoundId = pUnpacker->GetInt();
		}
		break;

		case NETMSGTYPE_SV_PREINPUT:
		{
			CNetMsg_Sv_PreInput *pData = (CNetMsg_Sv_PreInput *)m_aUnpackedData;
			pData->m_Direction = pUnpacker->GetInt();
			pData->m_TargetX = pUnpacker->GetInt();
			pData->m_TargetY = pUnpacker->GetInt();
			pData->m_Jump = pUnpacker->GetInt();
			pData->m_Fire = pUnpacker->GetInt();
			pData->m_Hook = pUnpacker->GetInt();
			pData->m_WantedWeapon = pUnpacker->GetInt();
			pData->m_NextWeapon = pUnpacker->GetInt();
			pData->m_PrevWeapon = pUnpacker->GetInt();
			pData->m_Owner = pUnpacker->GetInt();
			pData->m_IntendedTick = pUnpacker->GetInt();
			if(pData->m_Owner < 0 || pData->m_Owner > MAX_CLIENTS - 1)
			{
				m_pMsgFailedOn = "m_Owner";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_SAVECODE:
		{
			CNetMsg_Sv_SaveCode *pData = (CNetMsg_Sv_SaveCode *)m_aUnpackedData;
			pData->m_State = pUnpacker->GetInt();
			pData->m_pError = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pSaveRequester = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pServerName = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pGeneratedCode = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pCode = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			pData->m_pTeamMembers = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			if(pData->m_State < SAVESTATE_PENDING || pData->m_State > SAVESTATE_ERROR)
			{
				m_pMsgFailedOn = "m_State";
				break;
			}
		}
		break;

		case NETMSGTYPE_SV_SERVERALERT:
		{
			CNetMsg_Sv_ServerAlert *pData = (CNetMsg_Sv_ServerAlert *)m_aUnpackedData;
			pData->m_pMessage = pUnpacker->GetString();
		}
		break;

		case NETMSGTYPE_SV_MODERATORALERT:
		{
			CNetMsg_Sv_ModeratorAlert *pData = (CNetMsg_Sv_ModeratorAlert *)m_aUnpackedData;
			pData->m_pMessage = pUnpacker->GetString();
		}
		break;

		case NETMSGTYPE_CL_ENABLESPECTATORCOUNT:
		{
			CNetMsg_Cl_EnableSpectatorCount *pData = (CNetMsg_Cl_EnableSpectatorCount *)m_aUnpackedData;
			pData->m_Enable = pUnpacker->GetInt();
			if(pData->m_Enable < 0 || pData->m_Enable > 1)
			{
				m_pMsgFailedOn = "m_Enable";
				break;
			}
		}
		break;

		default:
			m_pMsgFailedOn = "(type out of range)";
			break;
		}

		if(pUnpacker->Error())
			m_pMsgFailedOn = "(unpack error)";

		if(m_pMsgFailedOn)
			return 0;
		m_pMsgFailedOn = "";
		return m_aUnpackedData;
	}

	bool CNetObjHandler::TeeHistorianRecordMsg(int Type)
	{
		switch(Type)
		{
		case NETMSGTYPE_CL_SAY:
		case NETMSGTYPE_CL_VOTE:
		case NETMSGTYPE_CL_CALLVOTE:
			return false;
		default:
			return true;
		}
	}

	void RegisterGameUuids(CBobUuidManager *pManager)
	{
		pManager->RegisterName(NETOBJTYPE_MYOWNOBJECT, "my-own-object@heinrich5991.de");
		pManager->RegisterName(NETOBJTYPE_DDNETCHARACTER, "character@netobj.ddnet.tw");
		pManager->RegisterName(NETOBJTYPE_DDNETPLAYER, "player@netobj.ddnet.tw");
		pManager->RegisterName(NETOBJTYPE_GAMEINFOEX, "gameinfo@netobj.ddnet.tw");
		pManager->RegisterName(NETOBJTYPE_DDRACEPROJECTILE, "projectile@netobj.ddnet.tw");
		pManager->RegisterName(NETOBJTYPE_DDNETLASER, "laser@netobj.ddnet.tw");
		pManager->RegisterName(NETOBJTYPE_DDNETPROJECTILE, "ddnet-projectile@netobj.ddnet.tw");
		pManager->RegisterName(NETOBJTYPE_DDNETPICKUP, "pickup@netobj.ddnet.tw");
		pManager->RegisterName(NETOBJTYPE_DDNETSPECTATORINFO, "spectator-info@netobj.ddnet.org");
		pManager->RegisterName(NETOBJTYPE_SPECTATORCOUNT, "spectator-count@netobj.ddnet.org");
		pManager->RegisterName(NETEVENTTYPE_BIRTHDAY, "birthday@netevent.ddnet.org");
		pManager->RegisterName(NETEVENTTYPE_FINISH, "finish@netevent.ddnet.org");
		pManager->RegisterName(NETOBJTYPE_MYOWNEVENT, "my-own-event@heinrich5991.de");
		pManager->RegisterName(NETOBJTYPE_SPECCHAR, "spec-char@netobj.ddnet.tw");
		pManager->RegisterName(NETOBJTYPE_SWITCHSTATE, "switch-state@netobj.ddnet.tw");
		pManager->RegisterName(NETOBJTYPE_ENTITYEX, "entity-ex@netobj.ddnet.tw");
		pManager->RegisterName(NETEVENTTYPE_MAPSOUNDWORLD, "map-sound-world@netevent.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_MYOWNMESSAGE, "my-own-message@heinrich5991.de");
		pManager->RegisterName(NETMSGTYPE_CL_SHOWDISTANCE, "show-distance@netmsg.ddnet.tw");
		pManager->RegisterName(NETMSGTYPE_CL_SHOWOTHERS, "showothers@netmsg.ddnet.tw");
		pManager->RegisterName(NETMSGTYPE_CL_CAMERAINFO, "camera-info@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_TEAMSSTATE, "teamsstate@netmsg.ddnet.tw");
		pManager->RegisterName(NETMSGTYPE_SV_DDRACETIME, "ddrace-time@netmsg.ddnet.tw");
		pManager->RegisterName(NETMSGTYPE_SV_RECORD, "record@netmsg.ddnet.tw");
		pManager->RegisterName(NETMSGTYPE_SV_KILLMSGTEAM, "killmsgteam@netmsg.ddnet.tw");
		pManager->RegisterName(NETMSGTYPE_SV_YOURVOTE, "yourvote@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_RACEFINISH, "racefinish@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_COMMANDINFO, "commandinfo@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_COMMANDINFOREMOVE, "commandinfo-remove@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_VOTEOPTIONGROUPSTART, "sv-vote-option-group-start@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_VOTEOPTIONGROUPEND, "sv-vote-option-group-end@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_COMMANDINFOGROUPSTART, "sv-commandinfo-group-start@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_COMMANDINFOGROUPEND, "sv-commandinfo-group-end@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_CHANGEINFOCOOLDOWN, "change-info-cooldown@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_MAPSOUNDGLOBAL, "map-sound-global@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_PREINPUT, "preinput@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_SAVECODE, "save-code@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_SERVERALERT, "server-alert@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_SV_MODERATORALERT, "moderator-alert@netmsg.ddnet.org");
		pManager->RegisterName(NETMSGTYPE_CL_ENABLESPECTATORCOUNT, "enable-spectator-count@netmsg.ddnet.org");

		RegisterMapItemTypeUuids(pManager);
	}

} // namespace polybob
