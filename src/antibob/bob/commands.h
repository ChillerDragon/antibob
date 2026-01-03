// this file can be included multiple times

#ifndef CONSOLE_COMMAND
#define CONSOLE_COMMAND(name, params, callback, user, help)
#endif

CONSOLE_COMMAND("test", "", ComTest, this, "tests stuff")
CONSOLE_COMMAND("help", "s[command]", ComCmdHelp, this, "prints details for a given command (see also 'cmdlist')")
CONSOLE_COMMAND("configs", "", ComConfigs, this, "lists all antibot config variables (see also 'help' and 'cmdlist')")
CONSOLE_COMMAND("cmdlist", "", ComCmdlist, this, "lists all antibot commands (see also 'help' and 'configs')")
CONSOLE_COMMAND("dump", "?i[min confidence]?r[search]", ComDump, this, "shows current antibot data for all players")
CONSOLE_COMMAND("events", "i[client id]", ComEvents, this, "shows details about triggered detections for a given player")
CONSOLE_COMMAND("kick_events", "s[comma sep event ids]", ComKickEvents, this, "kick all players that triggered the given events or more")
CONSOLE_COMMAND("version", "", ComVersion, this, "shows version of the antibob module")
CONSOLE_COMMAND("pending", "", ComPendingPunishments, this, "list pending punishments")
CONSOLE_COMMAND("known", "", ComKnownCheaters, this, "list players that were already caught cheating")
CONSOLE_COMMAND("redirect_cheaters", "i[port]", ComRedirectKnownCheaters, this, "redirect all players caught cheating already")

//
// Add new commands for forks below this comment to avoid merge conflicts.
//
