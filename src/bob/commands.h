// this file can be included multiple times

#ifndef CONSOLE_COMMAND
#define CONSOLE_COMMAND(name, params, callback, user, help)
#endif

CONSOLE_COMMAND("test", "", ComTest, this, "tests stuff")
CONSOLE_COMMAND("dump", "", ComDump, this, "shows current antibot datat for all players")
