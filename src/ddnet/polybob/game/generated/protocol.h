#pragma once

#include <polybob/engine/message.h>
#include <polybob/engine/shared/teehistorian_ex.h>

namespace polybob
{

	class CUnpacker;

	enum
	{
		INPUT_STATE_MASK = 0x3f
	};

	enum
	{
		FLAG_MISSING = -3,
		FLAG_ATSTAND,
		FLAG_TAKEN,

		SPEC_FREEVIEW = -1,
		SPEC_FOLLOW = -2,
	};

	enum
	{
		GAMEINFO_CURVERSION = 10,
	};

	enum
	{
		EMOTE_NORMAL,
		EMOTE_PAIN,
		EMOTE_HAPPY,
		EMOTE_SURPRISE,
		EMOTE_ANGRY,
		EMOTE_BLINK,
		NUM_EMOTES
	};

	enum
	{
		POWERUP_HEALTH,
		POWERUP_ARMOR,
		POWERUP_WEAPON,
		POWERUP_NINJA,
		POWERUP_ARMOR_SHOTGUN,
		POWERUP_ARMOR_GRENADE,
		POWERUP_ARMOR_NINJA,
		POWERUP_ARMOR_LASER,
		NUM_POWERUPS
	};

	enum
	{
		EMOTICON_OOP,
		EMOTICON_EXCLAMATION,
		EMOTICON_HEARTS,
		EMOTICON_DROP,
		EMOTICON_DOTDOT,
		EMOTICON_MUSIC,
		EMOTICON_SORRY,
		EMOTICON_GHOST,
		EMOTICON_SUSHI,
		EMOTICON_SPLATTEE,
		EMOTICON_DEVILTEE,
		EMOTICON_ZOMG,
		EMOTICON_ZZZ,
		EMOTICON_WTF,
		EMOTICON_EYES,
		EMOTICON_QUESTION,
		NUM_EMOTICONS
	};

	enum
	{
		AUTHED_NO,
		AUTHED_HELPER,
		AUTHED_MOD,
		AUTHED_ADMIN,
		NUM_AUTHEDS
	};

	enum
	{
		ENTITYCLASS_PROJECTILE,
		ENTITYCLASS_DOOR,
		ENTITYCLASS_DRAGGER_WEAK,
		ENTITYCLASS_DRAGGER_NORMAL,
		ENTITYCLASS_DRAGGER_STRONG,
		ENTITYCLASS_GUN_NORMAL,
		ENTITYCLASS_GUN_EXPLOSIVE,
		ENTITYCLASS_GUN_FREEZE,
		ENTITYCLASS_GUN_UNFREEZE,
		ENTITYCLASS_LIGHT,
		ENTITYCLASS_PICKUP,
		NUM_ENTITYCLASSS
	};

	enum
	{
		LASERTYPE_RIFLE,
		LASERTYPE_SHOTGUN,
		LASERTYPE_DOOR,
		LASERTYPE_FREEZE,
		LASERTYPE_DRAGGER,
		LASERTYPE_GUN,
		LASERTYPE_PLASMA,
		NUM_LASERTYPES
	};

	enum
	{
		LASERDRAGGERTYPE_WEAK,
		LASERDRAGGERTYPE_WEAK_NW,
		LASERDRAGGERTYPE_NORMAL,
		LASERDRAGGERTYPE_NORMAL_NW,
		LASERDRAGGERTYPE_STRONG,
		LASERDRAGGERTYPE_STRONG_NW,
		NUM_LASERDRAGGERTYPES
	};

	enum
	{
		LASERGUNTYPE_UNFREEZE,
		LASERGUNTYPE_EXPLOSIVE,
		LASERGUNTYPE_FREEZE,
		LASERGUNTYPE_EXPFREEZE,
		NUM_LASERGUNTYPES
	};

	enum
	{
		TEAM_ALL = -2,
		TEAM_SPECTATORS,
		TEAM_RED,
		TEAM_BLUE,
		TEAM_WHISPER_SEND,
		TEAM_WHISPER_RECV,
		NUM_TEAMS
	};

	enum
	{
		PLAYERFLAG_PLAYING = 1 << 0,
		PLAYERFLAG_IN_MENU = 1 << 1,
		PLAYERFLAG_CHATTING = 1 << 2,
		PLAYERFLAG_SCOREBOARD = 1 << 3,
		PLAYERFLAG_AIM = 1 << 4,
		PLAYERFLAG_SPEC_CAM = 1 << 5,
	};

	enum
	{
		GAMEFLAG_TEAMS = 1 << 0,
		GAMEFLAG_FLAGS = 1 << 1,
	};

	enum
	{
		GAMESTATEFLAG_GAMEOVER = 1 << 0,
		GAMESTATEFLAG_SUDDENDEATH = 1 << 1,
		GAMESTATEFLAG_PAUSED = 1 << 2,
		GAMESTATEFLAG_RACETIME = 1 << 3,
	};

	enum
	{
		CHARACTERFLAG_SOLO = 1 << 0,
		CHARACTERFLAG_JETPACK = 1 << 1,
		CHARACTERFLAG_COLLISION_DISABLED = 1 << 2,
		CHARACTERFLAG_ENDLESS_HOOK = 1 << 3,
		CHARACTERFLAG_ENDLESS_JUMP = 1 << 4,
		CHARACTERFLAG_SUPER = 1 << 5,
		CHARACTERFLAG_HAMMER_HIT_DISABLED = 1 << 6,
		CHARACTERFLAG_SHOTGUN_HIT_DISABLED = 1 << 7,
		CHARACTERFLAG_GRENADE_HIT_DISABLED = 1 << 8,
		CHARACTERFLAG_LASER_HIT_DISABLED = 1 << 9,
		CHARACTERFLAG_HOOK_HIT_DISABLED = 1 << 10,
		CHARACTERFLAG_TELEGUN_GUN = 1 << 11,
		CHARACTERFLAG_TELEGUN_GRENADE = 1 << 12,
		CHARACTERFLAG_TELEGUN_LASER = 1 << 13,
		CHARACTERFLAG_WEAPON_HAMMER = 1 << 14,
		CHARACTERFLAG_WEAPON_GUN = 1 << 15,
		CHARACTERFLAG_WEAPON_SHOTGUN = 1 << 16,
		CHARACTERFLAG_WEAPON_GRENADE = 1 << 17,
		CHARACTERFLAG_WEAPON_LASER = 1 << 18,
		CHARACTERFLAG_WEAPON_NINJA = 1 << 19,
		CHARACTERFLAG_MOVEMENTS_DISABLED = 1 << 20,
		CHARACTERFLAG_IN_FREEZE = 1 << 21,
		CHARACTERFLAG_PRACTICE_MODE = 1 << 22,
		CHARACTERFLAG_LOCK_MODE = 1 << 23,
		CHARACTERFLAG_TEAM0_MODE = 1 << 24,
		CHARACTERFLAG_INVINCIBLE = 1 << 25,
	};

	enum
	{
		GAMEINFOFLAG_TIMESCORE = 1 << 0,
		GAMEINFOFLAG_GAMETYPE_RACE = 1 << 1,
		GAMEINFOFLAG_GAMETYPE_FASTCAP = 1 << 2,
		GAMEINFOFLAG_GAMETYPE_FNG = 1 << 3,
		GAMEINFOFLAG_GAMETYPE_DDRACE = 1 << 4,
		GAMEINFOFLAG_GAMETYPE_DDNET = 1 << 5,
		GAMEINFOFLAG_GAMETYPE_BLOCK_WORLDS = 1 << 6,
		GAMEINFOFLAG_GAMETYPE_VANILLA = 1 << 7,
		GAMEINFOFLAG_GAMETYPE_PLUS = 1 << 8,
		GAMEINFOFLAG_FLAG_STARTS_RACE = 1 << 9,
		GAMEINFOFLAG_RACE = 1 << 10,
		GAMEINFOFLAG_UNLIMITED_AMMO = 1 << 11,
		GAMEINFOFLAG_DDRACE_RECORD_MESSAGE = 1 << 12,
		GAMEINFOFLAG_RACE_RECORD_MESSAGE = 1 << 13,
		GAMEINFOFLAG_ALLOW_EYE_WHEEL = 1 << 14,
		GAMEINFOFLAG_ALLOW_HOOK_COLL = 1 << 15,
		GAMEINFOFLAG_ALLOW_ZOOM = 1 << 16,
		GAMEINFOFLAG_BUG_DDRACE_GHOST = 1 << 17,
		GAMEINFOFLAG_BUG_DDRACE_INPUT = 1 << 18,
		GAMEINFOFLAG_BUG_FNG_LASER_RANGE = 1 << 19,
		GAMEINFOFLAG_BUG_VANILLA_BOUNCE = 1 << 20,
		GAMEINFOFLAG_PREDICT_FNG = 1 << 21,
		GAMEINFOFLAG_PREDICT_DDRACE = 1 << 22,
		GAMEINFOFLAG_PREDICT_DDRACE_TILES = 1 << 23,
		GAMEINFOFLAG_PREDICT_VANILLA = 1 << 24,
		GAMEINFOFLAG_ENTITIES_DDNET = 1 << 25,
		GAMEINFOFLAG_ENTITIES_DDRACE = 1 << 26,
		GAMEINFOFLAG_ENTITIES_RACE = 1 << 27,
		GAMEINFOFLAG_ENTITIES_FNG = 1 << 28,
		GAMEINFOFLAG_ENTITIES_VANILLA = 1 << 29,
		GAMEINFOFLAG_DONT_MASK_ENTITIES = 1 << 30,
		GAMEINFOFLAG_ENTITIES_BW = 1 << 31,
	};

	enum
	{
		GAMEINFOFLAG2_ALLOW_X_SKINS = 1 << 0,
		GAMEINFOFLAG2_GAMETYPE_CITY = 1 << 1,
		GAMEINFOFLAG2_GAMETYPE_FDDRACE = 1 << 2,
		GAMEINFOFLAG2_ENTITIES_FDDRACE = 1 << 3,
		GAMEINFOFLAG2_HUD_HEALTH_ARMOR = 1 << 4,
		GAMEINFOFLAG2_HUD_AMMO = 1 << 5,
		GAMEINFOFLAG2_HUD_DDRACE = 1 << 6,
		GAMEINFOFLAG2_NO_WEAK_HOOK = 1 << 7,
		GAMEINFOFLAG2_NO_SKIN_CHANGE_FOR_FROZEN = 1 << 8,
		GAMEINFOFLAG2_DDRACE_TEAM = 1 << 9,
	};

	enum
	{
		EXPLAYERFLAG_AFK = 1 << 0,
		EXPLAYERFLAG_PAUSED = 1 << 1,
		EXPLAYERFLAG_SPEC = 1 << 2,
	};

	enum
	{
		LEGACYPROJECTILEFLAG_CLIENTID_BIT0 = 1 << 0,
		LEGACYPROJECTILEFLAG_CLIENTID_BIT1 = 1 << 1,
		LEGACYPROJECTILEFLAG_CLIENTID_BIT2 = 1 << 2,
		LEGACYPROJECTILEFLAG_CLIENTID_BIT3 = 1 << 3,
		LEGACYPROJECTILEFLAG_CLIENTID_BIT4 = 1 << 4,
		LEGACYPROJECTILEFLAG_CLIENTID_BIT5 = 1 << 5,
		LEGACYPROJECTILEFLAG_CLIENTID_BIT6 = 1 << 6,
		LEGACYPROJECTILEFLAG_CLIENTID_BIT7 = 1 << 7,
		LEGACYPROJECTILEFLAG_NO_OWNER = 1 << 8,
		LEGACYPROJECTILEFLAG_IS_DDNET = 1 << 9,
		LEGACYPROJECTILEFLAG_BOUNCE_HORIZONTAL = 1 << 10,
		LEGACYPROJECTILEFLAG_BOUNCE_VERTICAL = 1 << 11,
		LEGACYPROJECTILEFLAG_EXPLOSIVE = 1 << 12,
		LEGACYPROJECTILEFLAG_FREEZE = 1 << 13,
	};

	enum
	{
		PROJECTILEFLAG_BOUNCE_HORIZONTAL = 1 << 0,
		PROJECTILEFLAG_BOUNCE_VERTICAL = 1 << 1,
		PROJECTILEFLAG_EXPLOSIVE = 1 << 2,
		PROJECTILEFLAG_FREEZE = 1 << 3,
		PROJECTILEFLAG_NORMALIZE_VEL = 1 << 4,
	};

	enum
	{
		LASERFLAG_NO_PREDICT = 1 << 0,
	};

	enum
	{
		NETOBJTYPE_EX,
		NETOBJTYPE_PLAYERINPUT,
		NETOBJTYPE_PROJECTILE,
		NETOBJTYPE_LASER,
		NETOBJTYPE_PICKUP,
		NETOBJTYPE_FLAG,
		NETOBJTYPE_GAMEINFO,
		NETOBJTYPE_GAMEDATA,
		NETOBJTYPE_CHARACTERCORE,
		NETOBJTYPE_CHARACTER,
		NETOBJTYPE_PLAYERINFO,
		NETOBJTYPE_CLIENTINFO,
		NETOBJTYPE_SPECTATORINFO,
		NETEVENTTYPE_COMMON,
		NETEVENTTYPE_EXPLOSION,
		NETEVENTTYPE_SPAWN,
		NETEVENTTYPE_HAMMERHIT,
		NETEVENTTYPE_DEATH,
		NETEVENTTYPE_SOUNDGLOBAL,
		NETEVENTTYPE_SOUNDWORLD,
		NETEVENTTYPE_DAMAGEIND,
		NUM_NETOBJTYPES
	};
	enum
	{
		__NETOBJTYPE_UUID_HELPER = OFFSET_GAME_UUID - 1,
		NETOBJTYPE_MYOWNOBJECT,
		NETOBJTYPE_DDNETCHARACTER,
		NETOBJTYPE_DDNETPLAYER,
		NETOBJTYPE_GAMEINFOEX,
		NETOBJTYPE_DDRACEPROJECTILE,
		NETOBJTYPE_DDNETLASER,
		NETOBJTYPE_DDNETPROJECTILE,
		NETOBJTYPE_DDNETPICKUP,
		NETOBJTYPE_DDNETSPECTATORINFO,
		NETEVENTTYPE_BIRTHDAY,
		NETEVENTTYPE_FINISH,
		NETOBJTYPE_MYOWNEVENT,
		NETOBJTYPE_SPECCHAR,
		NETOBJTYPE_SWITCHSTATE,
		NETOBJTYPE_ENTITYEX,
		NETEVENTTYPE_MAPSOUNDWORLD,
		OFFSET_NETMSGTYPE_UUID
	};

	enum
	{
		NETMSGTYPE_EX,
		NETMSGTYPE_SV_MOTD,
		NETMSGTYPE_SV_BROADCAST,
		NETMSGTYPE_SV_CHAT,
		NETMSGTYPE_SV_KILLMSG,
		NETMSGTYPE_SV_SOUNDGLOBAL,
		NETMSGTYPE_SV_TUNEPARAMS,
		NETMSGTYPE_UNUSED,
		NETMSGTYPE_SV_READYTOENTER,
		NETMSGTYPE_SV_WEAPONPICKUP,
		NETMSGTYPE_SV_EMOTICON,
		NETMSGTYPE_SV_VOTECLEAROPTIONS,
		NETMSGTYPE_SV_VOTEOPTIONLISTADD,
		NETMSGTYPE_SV_VOTEOPTIONADD,
		NETMSGTYPE_SV_VOTEOPTIONREMOVE,
		NETMSGTYPE_SV_VOTESET,
		NETMSGTYPE_SV_VOTESTATUS,
		NETMSGTYPE_CL_SAY,
		NETMSGTYPE_CL_SETTEAM,
		NETMSGTYPE_CL_SETSPECTATORMODE,
		NETMSGTYPE_CL_STARTINFO,
		NETMSGTYPE_CL_CHANGEINFO,
		NETMSGTYPE_CL_KILL,
		NETMSGTYPE_CL_EMOTICON,
		NETMSGTYPE_CL_VOTE,
		NETMSGTYPE_CL_CALLVOTE,
		NETMSGTYPE_CL_ISDDNETLEGACY,
		NETMSGTYPE_SV_DDRACETIMELEGACY,
		NETMSGTYPE_SV_RECORDLEGACY,
		NETMSGTYPE_UNUSED2,
		NETMSGTYPE_SV_TEAMSSTATELEGACY,
		NETMSGTYPE_CL_SHOWOTHERSLEGACY,
		NUM_NETMSGTYPES
	};

	enum
	{
		__NETMSGTYPE_UUID_HELPER = OFFSET_NETMSGTYPE_UUID - 1,
		NETMSGTYPE_SV_MYOWNMESSAGE,
		NETMSGTYPE_CL_SHOWDISTANCE,
		NETMSGTYPE_CL_SHOWOTHERS,
		NETMSGTYPE_CL_CAMERAINFO,
		NETMSGTYPE_SV_TEAMSSTATE,
		NETMSGTYPE_SV_DDRACETIME,
		NETMSGTYPE_SV_RECORD,
		NETMSGTYPE_SV_KILLMSGTEAM,
		NETMSGTYPE_SV_YOURVOTE,
		NETMSGTYPE_SV_RACEFINISH,
		NETMSGTYPE_SV_COMMANDINFO,
		NETMSGTYPE_SV_COMMANDINFOREMOVE,
		NETMSGTYPE_SV_VOTEOPTIONGROUPSTART,
		NETMSGTYPE_SV_VOTEOPTIONGROUPEND,
		NETMSGTYPE_SV_COMMANDINFOGROUPSTART,
		NETMSGTYPE_SV_COMMANDINFOGROUPEND,
		NETMSGTYPE_SV_CHANGEINFOCOOLDOWN,
		NETMSGTYPE_SV_MAPSOUNDGLOBAL,
		OFFSET_MAPITEMTYPE_UUID
	};

	struct CNetObj_PlayerInput
	{
		static constexpr int ms_MsgId = NETOBJTYPE_PLAYERINPUT;
		int m_Direction;
		int m_TargetX;
		int m_TargetY;
		int m_Jump;
		int m_Fire;
		int m_Hook;
		int m_PlayerFlags;
		int m_WantedWeapon;
		int m_NextWeapon;
		int m_PrevWeapon;
	};

	struct CNetObj_Projectile
	{
		static constexpr int ms_MsgId = NETOBJTYPE_PROJECTILE;
		int m_X;
		int m_Y;
		int m_VelX;
		int m_VelY;
		int m_Type;
		int m_StartTick;
	};

	struct CNetObj_Laser
	{
		static constexpr int ms_MsgId = NETOBJTYPE_LASER;
		int m_X;
		int m_Y;
		int m_FromX;
		int m_FromY;
		int m_StartTick;
	};

	struct CNetObj_Pickup
	{
		static constexpr int ms_MsgId = NETOBJTYPE_PICKUP;
		int m_X;
		int m_Y;
		int m_Type;
		int m_Subtype;
	};

	struct CNetObj_Flag
	{
		static constexpr int ms_MsgId = NETOBJTYPE_FLAG;
		int m_X;
		int m_Y;
		int m_Team;
	};

	struct CNetObj_GameInfo
	{
		static constexpr int ms_MsgId = NETOBJTYPE_GAMEINFO;
		int m_GameFlags;
		int m_GameStateFlags;
		int m_RoundStartTick;
		int m_WarmupTimer;
		int m_ScoreLimit;
		int m_TimeLimit;
		int m_RoundNum;
		int m_RoundCurrent;
	};

	struct CNetObj_GameData
	{
		static constexpr int ms_MsgId = NETOBJTYPE_GAMEDATA;
		int m_TeamscoreRed;
		int m_TeamscoreBlue;
		int m_FlagCarrierRed;
		int m_FlagCarrierBlue;
	};

	struct CNetObj_CharacterCore
	{
		static constexpr int ms_MsgId = NETOBJTYPE_CHARACTERCORE;
		int m_Tick;
		int m_X;
		int m_Y;
		int m_VelX;
		int m_VelY;
		int m_Angle;
		int m_Direction;
		int m_Jumped;
		int m_HookedPlayer;
		int m_HookState;
		int m_HookTick;
		int m_HookX;
		int m_HookY;
		int m_HookDx;
		int m_HookDy;
	};

	struct CNetObj_Character : public CNetObj_CharacterCore
	{
		static constexpr int ms_MsgId = NETOBJTYPE_CHARACTER;
		int m_PlayerFlags;
		int m_Health;
		int m_Armor;
		int m_AmmoCount;
		int m_Weapon;
		int m_Emote;
		int m_AttackTick;
	};

	struct CNetObj_PlayerInfo
	{
		static constexpr int ms_MsgId = NETOBJTYPE_PLAYERINFO;
		int m_Local;
		int m_ClientId;
		int m_Team;
		int m_Score;
		int m_Latency;
	};

	struct CNetObj_ClientInfo
	{
		static constexpr int ms_MsgId = NETOBJTYPE_CLIENTINFO;
		int m_Name0;
		int m_Name1;
		int m_Name2;
		int m_Name3;
		int m_Clan0;
		int m_Clan1;
		int m_Clan2;
		int m_Country;
		int m_Skin0;
		int m_Skin1;
		int m_Skin2;
		int m_Skin3;
		int m_Skin4;
		int m_Skin5;
		int m_UseCustomColor;
		int m_ColorBody;
		int m_ColorFeet;
	};

	struct CNetObj_SpectatorInfo
	{
		static constexpr int ms_MsgId = NETOBJTYPE_SPECTATORINFO;
		int m_SpectatorId;
		int m_X;
		int m_Y;
	};

	struct CNetObj_MyOwnObject
	{
		static constexpr int ms_MsgId = NETOBJTYPE_MYOWNOBJECT;
		int m_Test;
	};

	struct CNetObj_DDNetCharacter
	{
		static constexpr int ms_MsgId = NETOBJTYPE_DDNETCHARACTER;
		int m_Flags;
		int m_FreezeEnd;
		int m_Jumps;
		int m_TeleCheckpoint;
		int m_StrongWeakId;
		int m_JumpedTotal;
		int m_NinjaActivationTick;
		int m_FreezeStart;
		int m_TargetX;
		int m_TargetY;
		int m_TuneZoneOverride;
	};

	struct CNetObj_DDNetPlayer
	{
		static constexpr int ms_MsgId = NETOBJTYPE_DDNETPLAYER;
		int m_Flags;
		int m_AuthLevel;
	};

	struct CNetObj_GameInfoEx
	{
		static constexpr int ms_MsgId = NETOBJTYPE_GAMEINFOEX;
		int m_Flags;
		int m_Version;
		int m_Flags2;
	};

	struct CNetObj_DDRaceProjectile
	{
		static constexpr int ms_MsgId = NETOBJTYPE_DDRACEPROJECTILE;
		int m_X;
		int m_Y;
		int m_Angle;
		int m_Data;
		int m_Type;
		int m_StartTick;
	};

	struct CNetObj_DDNetLaser
	{
		static constexpr int ms_MsgId = NETOBJTYPE_DDNETLASER;
		int m_ToX;
		int m_ToY;
		int m_FromX;
		int m_FromY;
		int m_StartTick;
		int m_Owner;
		int m_Type;
		int m_SwitchNumber;
		int m_Subtype;
		int m_Flags;
	};

	struct CNetObj_DDNetProjectile
	{
		static constexpr int ms_MsgId = NETOBJTYPE_DDNETPROJECTILE;
		int m_X;
		int m_Y;
		int m_VelX;
		int m_VelY;
		int m_Type;
		int m_StartTick;
		int m_Owner;
		int m_SwitchNumber;
		int m_TuneZone;
		int m_Flags;
	};

	struct CNetObj_DDNetPickup
	{
		static constexpr int ms_MsgId = NETOBJTYPE_DDNETPICKUP;
		int m_X;
		int m_Y;
		int m_Type;
		int m_Subtype;
		int m_SwitchNumber;
	};

	struct CNetObj_DDNetSpectatorInfo
	{
		static constexpr int ms_MsgId = NETOBJTYPE_DDNETSPECTATORINFO;
		int m_HasCameraInfo;
		int m_Zoom;
		int m_Deadzone;
		int m_FollowFactor;
		int m_SpectatorCount;
	};

	struct CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_COMMON;
		int m_X;
		int m_Y;
	};

	struct CNetEvent_Explosion : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_EXPLOSION;
	};

	struct CNetEvent_Spawn : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_SPAWN;
	};

	struct CNetEvent_HammerHit : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_HAMMERHIT;
	};

	struct CNetEvent_Death : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_DEATH;
		int m_ClientId;
	};

	struct CNetEvent_SoundGlobal : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_SOUNDGLOBAL;
		int m_SoundId;
	};

	struct CNetEvent_SoundWorld : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_SOUNDWORLD;
		int m_SoundId;
	};

	struct CNetEvent_DamageInd : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_DAMAGEIND;
		int m_Angle;
	};

	struct CNetEvent_Birthday : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_BIRTHDAY;
	};

	struct CNetEvent_Finish : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_FINISH;
	};

	struct CNetObj_MyOwnEvent
	{
		static constexpr int ms_MsgId = NETOBJTYPE_MYOWNEVENT;
		int m_Test;
	};

	struct CNetObj_SpecChar
	{
		static constexpr int ms_MsgId = NETOBJTYPE_SPECCHAR;
		int m_X;
		int m_Y;
	};

	struct CNetObj_SwitchState
	{
		static constexpr int ms_MsgId = NETOBJTYPE_SWITCHSTATE;
		int m_HighestSwitchNumber;
		int m_aStatus[8];
		int m_aSwitchNumbers[4];
		int m_aEndTicks[4];
	};

	struct CNetObj_EntityEx
	{
		static constexpr int ms_MsgId = NETOBJTYPE_ENTITYEX;
		int m_SwitchNumber;
		int m_Layer;
		int m_EntityClass;
	};

	struct CNetEvent_MapSoundWorld : public CNetEvent_Common
	{
		static constexpr int ms_MsgId = NETEVENTTYPE_MAPSOUNDWORLD;
		int m_SoundId;
	};

	struct CNetMsg_Sv_Motd
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_MOTD;
		const char *m_pMessage;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddString(m_pMessage, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_Broadcast
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_BROADCAST;
		const char *m_pMessage;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddString(m_pMessage, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_Chat
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_CHAT;
		int m_Team;
		int m_ClientId;
		const char *m_pMessage;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Team);
			pPacker->AddInt(m_ClientId);
			pPacker->AddString(m_pMessage, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_KillMsg
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_KILLMSG;
		int m_Killer;
		int m_Victim;
		int m_Weapon;
		int m_ModeSpecial;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Killer);
			pPacker->AddInt(m_Victim);
			pPacker->AddInt(m_Weapon);
			pPacker->AddInt(m_ModeSpecial);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_SoundGlobal
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_SOUNDGLOBAL;
		int m_SoundId;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_SoundId);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_TuneParams
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_TUNEPARAMS;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Unused
	{
		static constexpr int ms_MsgId = NETMSGTYPE_UNUSED;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_ReadyToEnter
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_READYTOENTER;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_WeaponPickup
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_WEAPONPICKUP;
		int m_Weapon;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Weapon);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_Emoticon
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_EMOTICON;
		int m_ClientId;
		int m_Emoticon;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_ClientId);
			pPacker->AddInt(m_Emoticon);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_VoteClearOptions
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTECLEAROPTIONS;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_VoteOptionListAdd
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTEOPTIONLISTADD;
		int m_NumOptions;
		const char *m_pDescription0;
		const char *m_pDescription1;
		const char *m_pDescription2;
		const char *m_pDescription3;
		const char *m_pDescription4;
		const char *m_pDescription5;
		const char *m_pDescription6;
		const char *m_pDescription7;
		const char *m_pDescription8;
		const char *m_pDescription9;
		const char *m_pDescription10;
		const char *m_pDescription11;
		const char *m_pDescription12;
		const char *m_pDescription13;
		const char *m_pDescription14;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_NumOptions);
			pPacker->AddString(m_pDescription0, -1);
			pPacker->AddString(m_pDescription1, -1);
			pPacker->AddString(m_pDescription2, -1);
			pPacker->AddString(m_pDescription3, -1);
			pPacker->AddString(m_pDescription4, -1);
			pPacker->AddString(m_pDescription5, -1);
			pPacker->AddString(m_pDescription6, -1);
			pPacker->AddString(m_pDescription7, -1);
			pPacker->AddString(m_pDescription8, -1);
			pPacker->AddString(m_pDescription9, -1);
			pPacker->AddString(m_pDescription10, -1);
			pPacker->AddString(m_pDescription11, -1);
			pPacker->AddString(m_pDescription12, -1);
			pPacker->AddString(m_pDescription13, -1);
			pPacker->AddString(m_pDescription14, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_VoteOptionAdd
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTEOPTIONADD;
		const char *m_pDescription;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddString(m_pDescription, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_VoteOptionRemove
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTEOPTIONREMOVE;
		const char *m_pDescription;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddString(m_pDescription, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_VoteSet
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTESET;
		int m_Timeout;
		const char *m_pDescription;
		const char *m_pReason;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Timeout);
			pPacker->AddString(m_pDescription, -1);
			pPacker->AddString(m_pReason, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_VoteStatus
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTESTATUS;
		int m_Yes;
		int m_No;
		int m_Pass;
		int m_Total;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Yes);
			pPacker->AddInt(m_No);
			pPacker->AddInt(m_Pass);
			pPacker->AddInt(m_Total);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_Say
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_SAY;
		int m_Team;
		const char *m_pMessage;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Team);
			pPacker->AddString(m_pMessage, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_SetTeam
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_SETTEAM;
		int m_Team;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Team);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_SetSpectatorMode
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_SETSPECTATORMODE;
		int m_SpectatorId;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_SpectatorId);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_StartInfo
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_STARTINFO;
		const char *m_pName;
		const char *m_pClan;
		int m_Country;
		const char *m_pSkin;
		int m_UseCustomColor;
		int m_ColorBody;
		int m_ColorFeet;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddString(m_pName, -1);
			pPacker->AddString(m_pClan, -1);
			pPacker->AddInt(m_Country);
			pPacker->AddString(m_pSkin, -1);
			pPacker->AddInt(m_UseCustomColor);
			pPacker->AddInt(m_ColorBody);
			pPacker->AddInt(m_ColorFeet);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_ChangeInfo
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_CHANGEINFO;
		const char *m_pName;
		const char *m_pClan;
		int m_Country;
		const char *m_pSkin;
		int m_UseCustomColor;
		int m_ColorBody;
		int m_ColorFeet;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddString(m_pName, -1);
			pPacker->AddString(m_pClan, -1);
			pPacker->AddInt(m_Country);
			pPacker->AddString(m_pSkin, -1);
			pPacker->AddInt(m_UseCustomColor);
			pPacker->AddInt(m_ColorBody);
			pPacker->AddInt(m_ColorFeet);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_Kill
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_KILL;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_Emoticon
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_EMOTICON;
		int m_Emoticon;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Emoticon);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_Vote
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_VOTE;
		int m_Vote;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Vote);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_CallVote
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_CALLVOTE;
		const char *m_pType;
		const char *m_pValue;
		const char *m_pReason;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddString(m_pType, -1);
			pPacker->AddString(m_pValue, -1);
			pPacker->AddString(m_pReason, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_IsDDNetLegacy
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_ISDDNETLEGACY;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_DDRaceTimeLegacy
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_DDRACETIMELEGACY;
		int m_Time;
		int m_Check;
		int m_Finish;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Time);
			pPacker->AddInt(m_Check);
			pPacker->AddInt(m_Finish);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_RecordLegacy
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_RECORDLEGACY;
		int m_ServerTimeBest;
		int m_PlayerTimeBest;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_ServerTimeBest);
			pPacker->AddInt(m_PlayerTimeBest);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Unused2
	{
		static constexpr int ms_MsgId = NETMSGTYPE_UNUSED2;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_TeamsStateLegacy
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_TEAMSSTATELEGACY;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_ShowOthersLegacy
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_SHOWOTHERSLEGACY;
		int m_Show;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Show);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_MyOwnMessage
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_MYOWNMESSAGE;
		int m_Test;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Test);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_ShowDistance
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_SHOWDISTANCE;
		int m_X;
		int m_Y;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_X);
			pPacker->AddInt(m_Y);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_ShowOthers
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_SHOWOTHERS;
		int m_Show;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Show);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Cl_CameraInfo
	{
		static constexpr int ms_MsgId = NETMSGTYPE_CL_CAMERAINFO;
		int m_Zoom;
		int m_Deadzone;
		int m_FollowFactor;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Zoom);
			pPacker->AddInt(m_Deadzone);
			pPacker->AddInt(m_FollowFactor);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_TeamsState
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_TEAMSSTATE;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_DDRaceTime
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_DDRACETIME;
		int m_Time;
		int m_Check;
		int m_Finish;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Time);
			pPacker->AddInt(m_Check);
			pPacker->AddInt(m_Finish);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_Record
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_RECORD;
		int m_ServerTimeBest;
		int m_PlayerTimeBest;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_ServerTimeBest);
			pPacker->AddInt(m_PlayerTimeBest);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_KillMsgTeam
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_KILLMSGTEAM;
		int m_Team;
		int m_First;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Team);
			pPacker->AddInt(m_First);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_YourVote
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_YOURVOTE;
		int m_Voted;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_Voted);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_RaceFinish
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_RACEFINISH;
		int m_ClientId;
		int m_Time;
		int m_Diff;
		int m_RecordPersonal;
		int m_RecordServer;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_ClientId);
			pPacker->AddInt(m_Time);
			pPacker->AddInt(m_Diff);
			pPacker->AddInt(m_RecordPersonal);
			pPacker->AddInt(m_RecordServer);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_CommandInfo
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_COMMANDINFO;
		const char *m_pName;
		const char *m_pArgsFormat;
		const char *m_pHelpText;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddString(m_pName, -1);
			pPacker->AddString(m_pArgsFormat, -1);
			pPacker->AddString(m_pHelpText, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_CommandInfoRemove
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_COMMANDINFOREMOVE;
		const char *m_pName;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddString(m_pName, -1);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_VoteOptionGroupStart
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTEOPTIONGROUPSTART;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_VoteOptionGroupEnd
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTEOPTIONGROUPEND;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_CommandInfoGroupStart
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_COMMANDINFOGROUPSTART;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_CommandInfoGroupEnd
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_COMMANDINFOGROUPEND;

		bool Pack(CMsgPacker *pPacker) const
		{
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_ChangeInfoCooldown
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_CHANGEINFOCOOLDOWN;
		int m_WaitUntil;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_WaitUntil);
			return pPacker->Error() != 0;
		}
	};

	struct CNetMsg_Sv_MapSoundGlobal
	{
		static constexpr int ms_MsgId = NETMSGTYPE_SV_MAPSOUNDGLOBAL;
		int m_SoundId;

		bool Pack(CMsgPacker *pPacker) const
		{
			pPacker->AddInt(m_SoundId);
			return pPacker->Error() != 0;
		}
	};

	enum
	{
		SOUND_GUN_FIRE = 0,
		SOUND_SHOTGUN_FIRE,
		SOUND_GRENADE_FIRE,
		SOUND_HAMMER_FIRE,
		SOUND_HAMMER_HIT,
		SOUND_NINJA_FIRE,
		SOUND_GRENADE_EXPLODE,
		SOUND_NINJA_HIT,
		SOUND_LASER_FIRE,
		SOUND_LASER_BOUNCE,
		SOUND_WEAPON_SWITCH,
		SOUND_PLAYER_PAIN_SHORT,
		SOUND_PLAYER_PAIN_LONG,
		SOUND_BODY_LAND,
		SOUND_PLAYER_AIRJUMP,
		SOUND_PLAYER_JUMP,
		SOUND_PLAYER_DIE,
		SOUND_PLAYER_SPAWN,
		SOUND_PLAYER_SKID,
		SOUND_TEE_CRY,
		SOUND_HOOK_LOOP,
		SOUND_HOOK_ATTACH_GROUND,
		SOUND_HOOK_ATTACH_PLAYER,
		SOUND_HOOK_NOATTACH,
		SOUND_PICKUP_HEALTH,
		SOUND_PICKUP_ARMOR,
		SOUND_PICKUP_GRENADE,
		SOUND_PICKUP_SHOTGUN,
		SOUND_PICKUP_NINJA,
		SOUND_WEAPON_SPAWN,
		SOUND_WEAPON_NOAMMO,
		SOUND_HIT,
		SOUND_CHAT_SERVER,
		SOUND_CHAT_CLIENT,
		SOUND_CHAT_HIGHLIGHT,
		SOUND_CTF_DROP,
		SOUND_CTF_RETURN,
		SOUND_CTF_GRAB_PL,
		SOUND_CTF_GRAB_EN,
		SOUND_CTF_CAPTURE,
		SOUND_MENU,
		NUM_SOUNDS
	};
	enum
	{
		WEAPON_HAMMER = 0,
		WEAPON_GUN,
		WEAPON_SHOTGUN,
		WEAPON_GRENADE,
		WEAPON_LASER,
		WEAPON_NINJA,
		NUM_WEAPONS
	};

	enum
	{
		WEAPON_GAME = -3, // team switching etc
		WEAPON_SELF = -2, // console kill command
		WEAPON_WORLD = -1, // death tiles etc
	};

	class CNetObjHandler
	{
		const char *m_pMsgFailedOn;
		const char *m_pObjFailedOn;
		const char *m_pObjCorrectedOn;
		char m_aUnpackedData[1024 * 2];
		int m_NumObjCorrections;
		int ClampInt(const char *pErrorMsg, int Value, int Min, int Max);

		static const char *ms_apObjNames[];
		static const char *ms_apExObjNames[];
		static int ms_aObjSizes[];
		static int ms_aUnpackedObjSizes[];
		static int ms_aUnpackedExObjSizes[];
		static const char *ms_apMsgNames[];
		static const char *ms_apExMsgNames[];

	public:
		CNetObjHandler();

		void *SecureUnpackObj(int Type, CUnpacker *pUnpacker);
		const char *GetObjName(int Type) const;
		int GetObjSize(int Type) const;
		int GetUnpackedObjSize(int Type) const;
		int NumObjCorrections() const;
		const char *CorrectedObjOn() const;
		const char *FailedObjOn() const;

		const char *GetMsgName(int Type) const;
		void *SecureUnpackMsg(int Type, CUnpacker *pUnpacker);
		bool TeeHistorianRecordMsg(int Type);
		const char *FailedMsgOn() const;
	};

} // namespace polybob
