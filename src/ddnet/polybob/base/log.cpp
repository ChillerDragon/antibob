// We intentionally do not ship a logger implementation
// the ddnet-server provides it
// and the unit test framework provides one too
//
// polybob used to provide a logger aswell but that was fragile
// as in some cases the production antibot module would use its own
// implementation instead of the server one
//
// checkout this issue for more context
// https://github.com/ChillerDragon/antibob/issues/50
