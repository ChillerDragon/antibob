#include "cmdline_arguments.h"

#include <polybob/base/log.h>
#include <polybob/base/system.h>

#include <cstdio>

#if defined(CONF_FAMILY_WINDOWS)
#include <windows.h>
#include <shellapi.h>
#endif

using polybob::str_copy;
using polybob::str_length;

int CCmdlineArguments::GetNumArguments()
{
#if defined(CONF_FAMILY_WINDOWS)
	int NumArgs = 0;
	LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &NumArgs);
	if (szArglist != nullptr) {
		LocalFree(szArglist);
	}
	return NumArgs;
#else
	FILE *pCmdline = fopen("/proc/self/cmdline", "rb");
	char *pArg = nullptr;
	size_t Size = 0;
	int NumArgs = 0;
	while(getdelim(&pArg, &Size, 0, pCmdline) != -1)
		NumArgs++;
	free(pArg);
	fclose(pCmdline);
	return NumArgs;
#endif
}

char **CCmdlineArguments::AllocateArguments()
{
#if defined(CONF_FAMILY_WINDOWS)
	int NumArgs = 0;
	LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &NumArgs);
	if (!szArglist)
		return nullptr;

	char **ppArguments = (char **)malloc(sizeof(char *) * NumArgs);
	for(int i = 0; i < NumArgs; i++)
	{
		int Size = WideCharToMultiByte(CP_UTF8, 0, szArglist[i], -1, nullptr, 0, nullptr, nullptr);
		ppArguments[i] = (char *)malloc(Size);
		WideCharToMultiByte(CP_UTF8, 0, szArglist[i], -1, ppArguments[i], Size, nullptr, nullptr);
	}

	LocalFree(szArglist);
	return ppArguments;
#else
	char **ppArguments = (char **)malloc(sizeof(const char *) * Num());
	FILE *pCmdline = fopen("/proc/self/cmdline", "rb");
	char *pArg = nullptr;
	size_t Size = 0;
	int Arg = 0;
	while(getdelim(&pArg, &Size, 0, pCmdline) != -1)
	{
		ppArguments[Arg] = (char *)malloc(str_length(pArg) + 2);
		str_copy(ppArguments[Arg], pArg, str_length(pArg) + 1);
		Arg++;
	}
	free(pArg);
	fclose(pCmdline);
	return ppArguments;
#endif
}

void CCmdlineArguments::FreeArguments(char **ppArguments, int Num)
{
	for(int i = 0; i < Num; i++)
		free(ppArguments[i]);
	free(ppArguments);
}

CCmdlineArguments::~CCmdlineArguments()
{
	if(m_ppArguments)
		FreeArguments(m_ppArguments, m_NumArgs);
}

void CCmdlineArguments::Print()
{
	log_info("antibot", "num arguments %d", Num());
	for(int i = 0; i < Num(); i++)
		log_info("antibot", " arg: %s", All()[i]);
}

int CCmdlineArguments::Num()
{
	if(m_NumArgs)
		return m_NumArgs;

	m_NumArgs = GetNumArguments();
	return m_NumArgs;
}

char **CCmdlineArguments::All()
{
	if(m_ppArguments)
		return m_ppArguments;

	m_ppArguments = AllocateArguments();
	return m_ppArguments;
}
