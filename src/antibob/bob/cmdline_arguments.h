#pragma once

class CCmdlineArguments
{
	static int GetNumArguments();
	char **AllocateArguments();
	static void FreeArguments(char **ppArguments, int Num);

	char **m_ppArguments = nullptr;
	int m_NumArgs = 0;

public:
	~CCmdlineArguments();
	void Print();
	int Num();
	char **All();
};
