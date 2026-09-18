#include "cmdline_arguments.h"

#include <polybob/base/log.h>
#include <polybob/base/system.h>

#include <cstdio>
#include <string>
#include <vector>

using polybob::str_copy;
using polybob::str_length;

int CCmdlineArguments::GetNumArguments()
{
	FILE *pCmdline = fopen("/proc/self/cmdline", "rb");
	if(!pCmdline)
		return 0;
	char aBuf[4096];
	size_t Read = fread(aBuf, 1, sizeof(aBuf), pCmdline);
	fclose(pCmdline);
	int NumArgs = 0;
	for(size_t i = 0; i < Read; i++)
		if(aBuf[i] == '\0')
			NumArgs++;
	return NumArgs;
}

char **CCmdlineArguments::AllocateArguments()
{
	FILE *pCmdline = fopen("/proc/self/cmdline", "rb");
	if(!pCmdline)
		return nullptr;

	std::string Cmdline;
	char aBuf[4096];
	size_t Read;
	while(!feof(pCmdline) && !ferror(pCmdline))
	{
		Read = fread(aBuf, 1, sizeof(aBuf), pCmdline);
		if(Read == 0)
			break;
		Cmdline.append(aBuf, Read);
	}
	fclose(pCmdline);

	if(Cmdline.empty())
		return nullptr;

	std::vector<const char *> vArgs;
	size_t Start = 0;
	for(size_t i = 0; i < Cmdline.size(); i++)
	{
		if(Cmdline[i] == '\0')
		{
			vArgs.push_back(Cmdline.c_str() + Start);
			Start = i + 1;
		}
	}

	char **ppArguments = (char **)malloc(sizeof(const char *) * vArgs.size());
	for(size_t i = 0; i < vArgs.size(); i++)
	{
		ppArguments[i] = (char *)calloc(str_length(vArgs[i]) + 2, 1);
		str_copy(ppArguments[i], vArgs[i], str_length(vArgs[i]) + 1);
	}
	m_NumArgs = vArgs.size();
	return ppArguments;
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
	const char *const *ppArguments = All();
	if(!ppArguments)
		return;
	for(int i = 0; i < Num(); i++)
		// NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage) the analyzer loses track of the malloc'ed argument array between the separate /proc/self/cmdline reads
		log_info("antibot", " arg: %s", ppArguments[i]);
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
