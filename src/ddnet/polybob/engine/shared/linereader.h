/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#pragma once
#include <polybob/base/types.h>

namespace polybob
{

	// buffered stream for reading lines
	class CLineReader
	{
		char *m_pBuffer;
		unsigned m_BufferPos;
		bool m_ReadLastLine;

	public:
		CLineReader();
		~CLineReader();

		bool OpenFile(IOHANDLE File);
		void OpenBuffer(char *pBuffer); // Buffer must have been allocated with malloc, will be freed by the line reader

		const char *Get(); // Returned string is valid until the line reader is destroyed
	};

} // namespace polybob
