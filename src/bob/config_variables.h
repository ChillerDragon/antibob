// This file can be included several times.

#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc) ;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc) ;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc) ;
#endif

// TODO: this is weird
#ifndef CFGFLAG_SERVER
#define CFGFLAG_SERVER 4
#endif

// example of mirroring a server variable
// this is not synced automatically. Your autoexec should look like this then:
//
// sv_name "foo"
// antibot sv_name "foo"
//
MACRO_CONFIG_STR(SvName, sv_name, 128, "unnamed server", CFGFLAG_SERVER, "Server name");

// antibot internal variables are conventionally prefixed with ab
// short for anti bob
MACRO_CONFIG_INT(AbAutoKick, ab_auto_kick, 1, 1, 100, CFGFLAG_SERVER, "0=off 1=kick 2+=ban time in minutes")
MACRO_CONFIG_STR(AbKickReason, ab_kick_reason, 64, "antibob auto kick", CFGFLAG_SERVER, "shown to players that get kicked if ab_auto_kick is set")

//
// Add config variables for forks below this comment to avoid merge conflicts.
//
