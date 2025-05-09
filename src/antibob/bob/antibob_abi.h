#pragma once

// custom antibob antibot api
// the server has to support it
// you can not depend on this abi being available
class CAntibobAbi
{
public:
	typedef int (*FVersion)();
	typedef void (*FRcon)(const char *pLine);

	void OnInit();

	// returns 0 if the server does not
	// support the antibob version abi
	int ServerAbiVersion();

	// can fail with error log if the server
	// does not support the abi
	// returns false if the server does not support it
	bool Rcon(const char *pLine);

	int OwnAbiVersion() { return 1100; }

private:
	FVersion m_pfnVersion = nullptr;
	FRcon m_pfnRcon = nullptr;
};
