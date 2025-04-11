// UUID(name_in_code, name)
//
// When adding your own extended net messages, choose the name (third
// parameter) as `<name>@<domain>` where `<name>` is a name you can choose
// freely and `<domain>` is a domain you own. If you don't own a domain, try
// choosing a string that is not a domain and uniquely identifies you, e.g. use
// the name of the client/server you develop.
//
// Example:
//
// 1) `i-unfreeze-you@ddnet.org`
// 2) `creeper@minetee`
//
// The first example applies if you own the `ddnet.org` domain, that is, if you
// are adding this message on behalf of the DDNet team.
//
// The second example shows how you could add a message if you don't own a
// domain, but need a message for your minetee client/server.

// This file can be included several times.

// system
UUID(BOB_NETMSG_WHATIS, "what-is@ddnet.tw")
UUID(BOB_NETMSG_ITIS, "it-is@ddnet.tw")
UUID(BOB_NETMSG_IDONTKNOW, "i-dont-know@ddnet.tw")
UUID(BOB_NETMSG_RCONTYPE, "rcon-type@ddnet.tw")
UUID(BOB_NETMSG_MAP_DETAILS, "map-details@ddnet.tw")
UUID(BOB_NETMSG_CAPABILITIES, "capabilities@ddnet.tw")
UUID(BOB_NETMSG_CLIENTVER, "clientver@ddnet.tw")
UUID(BOB_NETMSG_PINGEX, "ping@ddnet.tw")
UUID(BOB_NETMSG_PONGEX, "pong@ddnet.tw")
UUID(BOB_NETMSG_CHECKSUM_REQUEST, "checksum-request@ddnet.tw")
UUID(BOB_NETMSG_CHECKSUM_RESPONSE, "checksum-response@ddnet.tw")
UUID(BOB_NETMSG_CHECKSUM_ERROR, "checksum-error@ddnet.tw")
UUID(BOB_NETMSG_REDIRECT, "redirect@ddnet.org")
UUID(BOB_NETMSG_RCON_CMD_GROUP_START, "rcon-cmd-group-start@ddnet.org")
UUID(BOB_NETMSG_RCON_CMD_GROUP_END, "rcon-cmd-group-end@ddnet.org")
UUID(BOB_NETMSG_MAP_RELOAD, "map-reload@ddnet.org")
UUID(BOB_NETMSG_RECONNECT, "reconnect@ddnet.org")
UUID(BOB_NETMSG_MAPLIST_ADD, "sv-maplist-add@ddnet.org")
UUID(BOB_NETMSG_MAPLIST_GROUP_START, "sv-maplist-start@ddnet.org")
UUID(BOB_NETMSG_MAPLIST_GROUP_END, "sv-maplist-end@ddnet.org")

// game (could also move these to some network.py)
UUID(BOB_NETMSGTYPE_SV_MYOWNMESSAGE, "my-own-message@heinrich5991.de")
UUID(BOB_NETMSGTYPE_CL_SHOWDISTANCE, "show-distance@netmsg.ddnet.tw")
UUID(BOB_NETMSGTYPE_CL_SHOWOTHERS, "showothers@netmsg.ddnet.tw")
UUID(BOB_NETMSGTYPE_CL_CAMERAINFO, "camera-info@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_TEAMSSTAE, "teamsstate@netmsg.ddnet.tw")
UUID(BOB_NETMSGTYPE_SV_DDRACETIME, "ddrace-time@netmsg.ddnet.tw")
UUID(BOB_NETMSGTYPE_SV_RECORD, "record@netmsg.ddnet.tw")
UUID(BOB_NETMSGTYPE_SV_KILLMSGTEAM, "killmsgteam@netmsg.ddnet.tw")
UUID(BOB_NETMSGTYPE_SV_YOURVOTE, "yourvote@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_RACEFINISH, "racefinish@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_COMMANDINFO, "commandinfo@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_COMMANDINFOREMOVE, "commandinfo-remove@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_VOTEOPTIONGROUPSTART, "sv-vote-option-group-start@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_VOTEOPTIONGROUPEND, "sv-vote-option-group-end@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_COMMANDINFOGROUPSTART, "sv-commandinfo-group-start@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_COMMANDINFOGROUPEND, "sv-commandinfo-group-end@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_CHANGEINFOCOOLDOWN, "change-info-cooldown@netmsg.ddnet.org")
UUID(BOB_NETMSGTYPE_SV_MAPSOUNDGLOBAL, "map-sound-global@netmsg.ddnet.org")

//
// Add new commands for forks below this comment to avoid merge conflicts.
//
