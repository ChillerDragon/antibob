#pragma once

class CCmdlineArguments
{
	int GetNumArguments();
	char **AllocateArguments();
	void FreeArguments(char **ppArguments, int Num);

	char **m_ppArguments = nullptr;
	int m_NumArgs = 0;

public:
	~CCmdlineArguments();
	void Print();
	int Num();
	char **All();
};
