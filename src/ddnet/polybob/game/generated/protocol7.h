#pragma once

class CUnpacker;
#include <polybob/engine/message.h>
namespace antibob {
namespace protocol7 {

enum
{
	INPUT_STATE_MASK = 0x3f
};

enum
{
	TEAM_SPECTATORS = -1,
	TEAM_RED,
	TEAM_BLUE,
	NUM_TEAMS,

	FLAG_MISSING = -3,
	FLAG_ATSTAND,
	FLAG_TAKEN,

	SPEC_FREEVIEW = 0,
	SPEC_PLAYER,
	SPEC_FLAGRED,
	SPEC_FLAGBLUE,
	NUM_SPECMODES,

	SKINPART_BODY = 0,
	SKINPART_MARKING,
	SKINPART_DECORATION,
	SKINPART_HANDS,
	SKINPART_FEET,
	SKINPART_EYES,
	NUM_SKINPARTS,
};

enum
{
	PICKUP_HEALTH = 0,
	PICKUP_ARMOR,
	PICKUP_GRENADE,
	PICKUP_SHOTGUN,
	PICKUP_LASER,
	PICKUP_NINJA,
	PICKUP_GUN,
	PICKUP_HAMMER,
	NUM_PICKUPS
};

enum
{
	EMOTE_NORMAL = 0,
	EMOTE_PAIN,
	EMOTE_HAPPY,
	EMOTE_SURPRISE,
	EMOTE_ANGRY,
	EMOTE_BLINK,
	NUM_EMOTES
};

enum
{
	EMOTICON_OOP = 0,
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
	VOTE_UNKNOWN = 0,
	VOTE_START_OP,
	VOTE_START_KICK,
	VOTE_START_SPEC,
	VOTE_END_ABORT,
	VOTE_END_PASS,
	VOTE_END_FAIL,
	NUM_VOTES
};

enum
{
	CHAT_NONE = 0,
	CHAT_ALL,
	CHAT_TEAM,
	CHAT_WHISPER,
	NUM_CHATS
};

enum
{
	GAMEMSG_TEAM_SWAP = 0,
	GAMEMSG_SPEC_INVALIDID,
	GAMEMSG_TEAM_SHUFFLE,
	GAMEMSG_TEAM_BALANCE,
	GAMEMSG_CTF_DROP,
	GAMEMSG_CTF_RETURN,
	GAMEMSG_TEAM_ALL,
	GAMEMSG_TEAM_BALANCE_VICTIM,
	GAMEMSG_CTF_GRAB,
	GAMEMSG_CTF_CAPTURE,
	GAMEMSG_GAME_PAUSED,
	NUM_GAMEMSGS
};

enum
{
	PLAYERFLAG_ADMIN = 1 << 0,
	PLAYERFLAG_CHATTING = 1 << 1,
	PLAYERFLAG_SCOREBOARD = 1 << 2,
	PLAYERFLAG_READY = 1 << 3,
	PLAYERFLAG_DEAD = 1 << 4,
	PLAYERFLAG_WATCHING = 1 << 5,
	PLAYERFLAG_BOT = 1 << 6,
	PLAYERFLAG_AIM = 1 << 7,
};

enum
{
	GAMEFLAG_TEAMS = 1 << 0,
	GAMEFLAG_FLAGS = 1 << 1,
	GAMEFLAG_SURVIVAL = 1 << 2,
	GAMEFLAG_RACE = 1 << 3,
};

enum
{
	GAMESTATEFLAG_WARMUP = 1 << 0,
	GAMESTATEFLAG_SUDDENDEATH = 1 << 1,
	GAMESTATEFLAG_ROUNDOVER = 1 << 2,
	GAMESTATEFLAG_GAMEOVER = 1 << 3,
	GAMESTATEFLAG_PAUSED = 1 << 4,
	GAMESTATEFLAG_STARTCOUNTDOWN = 1 << 5,
};

enum
{
	COREEVENTFLAG_GROUND_JUMP = 1 << 0,
	COREEVENTFLAG_AIR_JUMP = 1 << 1,
	COREEVENTFLAG_HOOK_ATTACH_PLAYER = 1 << 2,
	COREEVENTFLAG_HOOK_ATTACH_GROUND = 1 << 3,
	COREEVENTFLAG_HOOK_HIT_NOHOOK = 1 << 4,
};

enum
{
	RACEFLAG_HIDE_KILLMSG = 1 << 0,
	RACEFLAG_FINISHMSG_AS_CHAT = 1 << 1,
	RACEFLAG_KEEP_WANTED_WEAPON = 1 << 2,
};

enum
{
	NETOBJ_INVALID = 0,
	NETOBJTYPE_PLAYERINPUT,
	NETOBJTYPE_PROJECTILE,
	NETOBJTYPE_LASER,
	NETOBJTYPE_PICKUP,
	NETOBJTYPE_FLAG,
	NETOBJTYPE_GAMEDATA,
	NETOBJTYPE_GAMEDATATEAM,
	NETOBJTYPE_GAMEDATAFLAG,
	NETOBJTYPE_CHARACTERCORE,
	NETOBJTYPE_CHARACTER,
	NETOBJTYPE_PLAYERINFO,
	NETOBJTYPE_SPECTATORINFO,
	NETOBJTYPE_DE_CLIENTINFO,
	NETOBJTYPE_DE_GAMEINFO,
	NETOBJTYPE_DE_TUNEPARAMS,
	NETEVENTTYPE_COMMON,
	NETEVENTTYPE_EXPLOSION,
	NETEVENTTYPE_SPAWN,
	NETEVENTTYPE_HAMMERHIT,
	NETEVENTTYPE_DEATH,
	NETEVENTTYPE_SOUNDWORLD,
	NETEVENTTYPE_DAMAGE,
	NETOBJTYPE_PLAYERINFORACE,
	NETOBJTYPE_GAMEDATARACE,
	NUM_NETOBJTYPES
};

enum
{
	NETMSG_INVALID = 0,
	NETMSGTYPE_SV_MOTD,
	NETMSGTYPE_SV_BROADCAST,
	NETMSGTYPE_SV_CHAT,
	NETMSGTYPE_SV_TEAM,
	NETMSGTYPE_SV_KILLMSG,
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
	NETMSGTYPE_SV_SERVERSETTINGS,
	NETMSGTYPE_SV_CLIENTINFO,
	NETMSGTYPE_SV_GAMEINFO,
	NETMSGTYPE_SV_CLIENTDROP,
	NETMSGTYPE_SV_GAMEMSG,
	NETMSGTYPE_DE_CLIENTENTER,
	NETMSGTYPE_DE_CLIENTLEAVE,
	NETMSGTYPE_CL_SAY,
	NETMSGTYPE_CL_SETTEAM,
	NETMSGTYPE_CL_SETSPECTATORMODE,
	NETMSGTYPE_CL_STARTINFO,
	NETMSGTYPE_CL_KILL,
	NETMSGTYPE_CL_READYCHANGE,
	NETMSGTYPE_CL_EMOTICON,
	NETMSGTYPE_CL_VOTE,
	NETMSGTYPE_CL_CALLVOTE,
	NETMSGTYPE_SV_SKINCHANGE,
	NETMSGTYPE_CL_SKINCHANGE,
	NETMSGTYPE_SV_RACEFINISH,
	NETMSGTYPE_SV_CHECKPOINT,
	NETMSGTYPE_SV_COMMANDINFO,
	NETMSGTYPE_SV_COMMANDINFOREMOVE,
	NETMSGTYPE_CL_COMMAND,
	NUM_NETMSGTYPES
};

template<typename... Ts>
struct make_void
{
	typedef void type;
};
template<typename... Ts>
using void_t = typename make_void<Ts...>::type;

template<typename T, typename = void>
struct is_sixup
{
	constexpr static bool value = false;
};

template<typename T>
struct is_sixup<T, void_t<typename T::is_sixup>>
{
	constexpr static bool value = true;
};

struct CNetObj_PlayerInput
{
	using is_sixup = char;
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
	using is_sixup = char;
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
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_LASER;
	int m_X;
	int m_Y;
	int m_FromX;
	int m_FromY;
	int m_StartTick;
};

struct CNetObj_Pickup
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_PICKUP;
	int m_X;
	int m_Y;
	int m_Type;
};

struct CNetObj_Flag
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_FLAG;
	int m_X;
	int m_Y;
	int m_Team;
};

struct CNetObj_GameData
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_GAMEDATA;
	int m_GameStartTick;
	int m_GameStateFlags;
	int m_GameStateEndTick;
};

struct CNetObj_GameDataTeam
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_GAMEDATATEAM;
	int m_TeamscoreRed;
	int m_TeamscoreBlue;
};

struct CNetObj_GameDataFlag
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_GAMEDATAFLAG;
	int m_FlagCarrierRed;
	int m_FlagCarrierBlue;
	int m_FlagDropTickRed;
	int m_FlagDropTickBlue;
};

struct CNetObj_CharacterCore
{
	using is_sixup = char;
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
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_CHARACTER;
	int m_Health;
	int m_Armor;
	int m_AmmoCount;
	int m_Weapon;
	int m_Emote;
	int m_AttackTick;
	int m_TriggeredEvents;
};

struct CNetObj_PlayerInfo
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_PLAYERINFO;
	int m_PlayerFlags;
	int m_Score;
	int m_Latency;
};

struct CNetObj_SpectatorInfo
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_SPECTATORINFO;
	int m_SpecMode;
	int m_SpectatorId;
	int m_X;
	int m_Y;
};

struct CNetObj_De_ClientInfo
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_DE_CLIENTINFO;
	int m_Local;
	int m_Team;
	int m_aName[4];
	int m_aClan[3];
	int m_Country;
	int m_aaSkinPartNames[6][6];
	int m_aUseCustomColors[6];
	int m_aSkinPartColors[6];
};

struct CNetObj_De_GameInfo
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_DE_GAMEINFO;
	int m_GameFlags;
	int m_ScoreLimit;
	int m_TimeLimit;
	int m_MatchNum;
	int m_MatchCurrent;
};

struct CNetObj_De_TuneParams
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_DE_TUNEPARAMS;
	int m_aTuneParams[32];
};

struct CNetEvent_Common
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETEVENTTYPE_COMMON;
	int m_X;
	int m_Y;
};

struct CNetEvent_Explosion : public CNetEvent_Common
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETEVENTTYPE_EXPLOSION;
};

struct CNetEvent_Spawn : public CNetEvent_Common
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETEVENTTYPE_SPAWN;
};

struct CNetEvent_HammerHit : public CNetEvent_Common
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETEVENTTYPE_HAMMERHIT;
};

struct CNetEvent_Death : public CNetEvent_Common
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETEVENTTYPE_DEATH;
	int m_ClientId;
};

struct CNetEvent_SoundWorld : public CNetEvent_Common
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETEVENTTYPE_SOUNDWORLD;
	int m_SoundId;
};

struct CNetEvent_Damage : public CNetEvent_Common
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETEVENTTYPE_DAMAGE;
	int m_ClientId;
	int m_Angle;
	int m_HealthAmount;
	int m_ArmorAmount;
	int m_Self;
};

struct CNetObj_PlayerInfoRace
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_PLAYERINFORACE;
	int m_RaceStartTick;
};

struct CNetObj_GameDataRace
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETOBJTYPE_GAMEDATARACE;
	int m_BestTime;
	int m_Precision;
	int m_RaceFlags;
};

struct CNetMsg_Sv_Motd
{
	using is_sixup = char;
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
	using is_sixup = char;
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
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_CHAT;
	int m_Mode;
	int m_ClientId;
	int m_TargetId;
	const char *m_pMessage;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_Mode);
		pPacker->AddInt(m_ClientId);
		pPacker->AddInt(m_TargetId);
		pPacker->AddString(m_pMessage, -1);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_Team
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_TEAM;
	int m_ClientId;
	int m_Team;
	int m_Silent;
	int m_CooldownTick;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_ClientId);
		pPacker->AddInt(m_Team);
		pPacker->AddInt(m_Silent);
		pPacker->AddInt(m_CooldownTick);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_KillMsg
{
	using is_sixup = char;
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

struct CNetMsg_Sv_TuneParams
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_TUNEPARAMS;

	bool Pack(CMsgPacker *pPacker) const
	{
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Unused
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_UNUSED;

	bool Pack(CMsgPacker *pPacker) const
	{
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_ReadyToEnter
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_READYTOENTER;

	bool Pack(CMsgPacker *pPacker) const
	{
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_WeaponPickup
{
	using is_sixup = char;
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
	using is_sixup = char;
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
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTECLEAROPTIONS;

	bool Pack(CMsgPacker *pPacker) const
	{
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_VoteOptionListAdd
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTEOPTIONLISTADD;

	bool Pack(CMsgPacker *pPacker) const
	{
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_VoteOptionAdd
{
	using is_sixup = char;
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
	using is_sixup = char;
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
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_VOTESET;
	int m_ClientId;
	int m_Type;
	int m_Timeout;
	const char *m_pDescription;
	const char *m_pReason;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_ClientId);
		pPacker->AddInt(m_Type);
		pPacker->AddInt(m_Timeout);
		pPacker->AddString(m_pDescription, -1);
		pPacker->AddString(m_pReason, -1);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_VoteStatus
{
	using is_sixup = char;
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

struct CNetMsg_Sv_ServerSettings
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_SERVERSETTINGS;
	int m_KickVote;
	int m_KickMin;
	int m_SpecVote;
	int m_TeamLock;
	int m_TeamBalance;
	int m_PlayerSlots;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_KickVote);
		pPacker->AddInt(m_KickMin);
		pPacker->AddInt(m_SpecVote);
		pPacker->AddInt(m_TeamLock);
		pPacker->AddInt(m_TeamBalance);
		pPacker->AddInt(m_PlayerSlots);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_ClientInfo
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_CLIENTINFO;
	int m_ClientId;
	int m_Local;
	int m_Team;
	const char *m_pName;
	const char *m_pClan;
	int m_Country;
	const char *m_apSkinPartNames[6];
	int m_aUseCustomColors[6];
	int m_aSkinPartColors[6];
	int m_Silent;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_ClientId);
		pPacker->AddInt(m_Local);
		pPacker->AddInt(m_Team);
		pPacker->AddString(m_pName, -1);
		pPacker->AddString(m_pClan, -1);
		pPacker->AddInt(m_Country);
		pPacker->AddString(m_apSkinPartNames[0], -1);
		pPacker->AddString(m_apSkinPartNames[1], -1);
		pPacker->AddString(m_apSkinPartNames[2], -1);
		pPacker->AddString(m_apSkinPartNames[3], -1);
		pPacker->AddString(m_apSkinPartNames[4], -1);
		pPacker->AddString(m_apSkinPartNames[5], -1);
		pPacker->AddInt(m_aUseCustomColors[0]);
		pPacker->AddInt(m_aUseCustomColors[1]);
		pPacker->AddInt(m_aUseCustomColors[2]);
		pPacker->AddInt(m_aUseCustomColors[3]);
		pPacker->AddInt(m_aUseCustomColors[4]);
		pPacker->AddInt(m_aUseCustomColors[5]);
		pPacker->AddInt(m_aSkinPartColors[0]);
		pPacker->AddInt(m_aSkinPartColors[1]);
		pPacker->AddInt(m_aSkinPartColors[2]);
		pPacker->AddInt(m_aSkinPartColors[3]);
		pPacker->AddInt(m_aSkinPartColors[4]);
		pPacker->AddInt(m_aSkinPartColors[5]);
		pPacker->AddInt(m_Silent);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_GameInfo
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_GAMEINFO;
	int m_GameFlags;
	int m_ScoreLimit;
	int m_TimeLimit;
	int m_MatchNum;
	int m_MatchCurrent;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_GameFlags);
		pPacker->AddInt(m_ScoreLimit);
		pPacker->AddInt(m_TimeLimit);
		pPacker->AddInt(m_MatchNum);
		pPacker->AddInt(m_MatchCurrent);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_ClientDrop
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_CLIENTDROP;
	int m_ClientId;
	const char *m_pReason;
	int m_Silent;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_ClientId);
		pPacker->AddString(m_pReason, -1);
		pPacker->AddInt(m_Silent);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_GameMsg
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_GAMEMSG;

	bool Pack(CMsgPacker *pPacker) const
	{
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_De_ClientEnter
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_DE_CLIENTENTER;
	const char *m_pName;
	int m_ClientId;
	int m_Team;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddString(m_pName, -1);
		pPacker->AddInt(m_ClientId);
		pPacker->AddInt(m_Team);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_De_ClientLeave
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_DE_CLIENTLEAVE;
	const char *m_pName;
	int m_ClientId;
	const char *m_pReason;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddString(m_pName, -1);
		pPacker->AddInt(m_ClientId);
		pPacker->AddString(m_pReason, -1);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Cl_Say
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_CL_SAY;
	int m_Mode;
	int m_Target;
	const char *m_pMessage;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_Mode);
		pPacker->AddInt(m_Target);
		pPacker->AddString(m_pMessage, -1);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Cl_SetTeam
{
	using is_sixup = char;
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
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_CL_SETSPECTATORMODE;
	int m_SpecMode;
	int m_SpectatorId;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_SpecMode);
		pPacker->AddInt(m_SpectatorId);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Cl_StartInfo
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_CL_STARTINFO;
	const char *m_pName;
	const char *m_pClan;
	int m_Country;
	const char *m_apSkinPartNames[6];
	int m_aUseCustomColors[6];
	int m_aSkinPartColors[6];

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddString(m_pName, -1);
		pPacker->AddString(m_pClan, -1);
		pPacker->AddInt(m_Country);
		pPacker->AddString(m_apSkinPartNames[0], -1);
		pPacker->AddString(m_apSkinPartNames[1], -1);
		pPacker->AddString(m_apSkinPartNames[2], -1);
		pPacker->AddString(m_apSkinPartNames[3], -1);
		pPacker->AddString(m_apSkinPartNames[4], -1);
		pPacker->AddString(m_apSkinPartNames[5], -1);
		pPacker->AddInt(m_aUseCustomColors[0]);
		pPacker->AddInt(m_aUseCustomColors[1]);
		pPacker->AddInt(m_aUseCustomColors[2]);
		pPacker->AddInt(m_aUseCustomColors[3]);
		pPacker->AddInt(m_aUseCustomColors[4]);
		pPacker->AddInt(m_aUseCustomColors[5]);
		pPacker->AddInt(m_aSkinPartColors[0]);
		pPacker->AddInt(m_aSkinPartColors[1]);
		pPacker->AddInt(m_aSkinPartColors[2]);
		pPacker->AddInt(m_aSkinPartColors[3]);
		pPacker->AddInt(m_aSkinPartColors[4]);
		pPacker->AddInt(m_aSkinPartColors[5]);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Cl_Kill
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_CL_KILL;

	bool Pack(CMsgPacker *pPacker) const
	{
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Cl_ReadyChange
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_CL_READYCHANGE;

	bool Pack(CMsgPacker *pPacker) const
	{
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Cl_Emoticon
{
	using is_sixup = char;
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
	using is_sixup = char;
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
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_CL_CALLVOTE;
	const char *m_pType;
	const char *m_pValue;
	const char *m_pReason;
	int m_Force;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddString(m_pType, -1);
		pPacker->AddString(m_pValue, -1);
		pPacker->AddString(m_pReason, -1);
		pPacker->AddInt(m_Force);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_SkinChange
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_SKINCHANGE;
	int m_ClientId;
	const char *m_apSkinPartNames[6];
	int m_aUseCustomColors[6];
	int m_aSkinPartColors[6];

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_ClientId);
		pPacker->AddString(m_apSkinPartNames[0], -1);
		pPacker->AddString(m_apSkinPartNames[1], -1);
		pPacker->AddString(m_apSkinPartNames[2], -1);
		pPacker->AddString(m_apSkinPartNames[3], -1);
		pPacker->AddString(m_apSkinPartNames[4], -1);
		pPacker->AddString(m_apSkinPartNames[5], -1);
		pPacker->AddInt(m_aUseCustomColors[0]);
		pPacker->AddInt(m_aUseCustomColors[1]);
		pPacker->AddInt(m_aUseCustomColors[2]);
		pPacker->AddInt(m_aUseCustomColors[3]);
		pPacker->AddInt(m_aUseCustomColors[4]);
		pPacker->AddInt(m_aUseCustomColors[5]);
		pPacker->AddInt(m_aSkinPartColors[0]);
		pPacker->AddInt(m_aSkinPartColors[1]);
		pPacker->AddInt(m_aSkinPartColors[2]);
		pPacker->AddInt(m_aSkinPartColors[3]);
		pPacker->AddInt(m_aSkinPartColors[4]);
		pPacker->AddInt(m_aSkinPartColors[5]);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Cl_SkinChange
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_CL_SKINCHANGE;
	const char *m_apSkinPartNames[6];
	int m_aUseCustomColors[6];
	int m_aSkinPartColors[6];

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddString(m_apSkinPartNames[0], -1);
		pPacker->AddString(m_apSkinPartNames[1], -1);
		pPacker->AddString(m_apSkinPartNames[2], -1);
		pPacker->AddString(m_apSkinPartNames[3], -1);
		pPacker->AddString(m_apSkinPartNames[4], -1);
		pPacker->AddString(m_apSkinPartNames[5], -1);
		pPacker->AddInt(m_aUseCustomColors[0]);
		pPacker->AddInt(m_aUseCustomColors[1]);
		pPacker->AddInt(m_aUseCustomColors[2]);
		pPacker->AddInt(m_aUseCustomColors[3]);
		pPacker->AddInt(m_aUseCustomColors[4]);
		pPacker->AddInt(m_aUseCustomColors[5]);
		pPacker->AddInt(m_aSkinPartColors[0]);
		pPacker->AddInt(m_aSkinPartColors[1]);
		pPacker->AddInt(m_aSkinPartColors[2]);
		pPacker->AddInt(m_aSkinPartColors[3]);
		pPacker->AddInt(m_aSkinPartColors[4]);
		pPacker->AddInt(m_aSkinPartColors[5]);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_RaceFinish
{
	using is_sixup = char;
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

struct CNetMsg_Sv_Checkpoint
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_CHECKPOINT;
	int m_Diff;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddInt(m_Diff);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Sv_CommandInfo
{
	using is_sixup = char;
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
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_SV_COMMANDINFOREMOVE;
	const char *m_pName;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddString(m_pName, -1);
		return pPacker->Error() != 0;
	}
};

struct CNetMsg_Cl_Command
{
	using is_sixup = char;
	static constexpr int ms_MsgId = NETMSGTYPE_CL_COMMAND;
	const char *m_pName;
	const char *m_pArguments;

	bool Pack(CMsgPacker *pPacker) const
	{
		pPacker->AddString(m_pName, -1);
		pPacker->AddString(m_pArguments, -1);
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

class CNetObjHandler
{
	const char *m_pMsgFailedOn;
	char m_aMsgData[1024];
	const char *m_pObjFailedOn;
	int m_NumObjFailures;
	bool CheckInt(const char *pErrorMsg, int Value, int Min, int Max);
	bool CheckFlag(const char *pErrorMsg, int Value, int Mask);

	static const char *ms_apObjNames[];
	static int ms_aObjSizes[];
	static const char *ms_apMsgNames[];

public:
	CNetObjHandler();

	int ValidateObj(int Type, const void *pData, int Size);
	const char *GetObjName(int Type) const;
	int GetObjSize(int Type) const;
	const char *FailedObjOn() const;
	int NumObjFailures() const;

	const char *GetMsgName(int Type) const;
	void *SecureUnpackMsg(int Type, CUnpacker *pUnpacker);
	const char *FailedMsgOn() const;
};

} // namespace protocol7
}
