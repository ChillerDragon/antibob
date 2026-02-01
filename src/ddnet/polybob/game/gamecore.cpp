#include <polybob/base/system.h>
#include <polybob/game/gamecore.h>

#include <cstddef>
#include <limits>

namespace polybob
{
	static_assert(std::numeric_limits<char>::is_signed, "char must be signed for StrToInts to work correctly");

	void StrToInts(int *pInts, size_t NumInts, const char *pStr)
	{
		dbg_assert(NumInts > 0, "StrToInts: NumInts invalid");
		const size_t StrSize = str_length(pStr) + 1;
		dbg_assert(StrSize <= NumInts * sizeof(int), "StrToInts: string truncated");

		for(size_t i = 0; i < NumInts; i++)
		{
			// Copy to temporary buffer to ensure we don't read past the end of the input string
			char aBuf[sizeof(int)] = {0, 0, 0, 0};
			for(size_t c = 0; c < sizeof(int) && i * sizeof(int) + c < StrSize; c++)
			{
				aBuf[c] = pStr[i * sizeof(int) + c];
			}
			pInts[i] = ((aBuf[0] + 128) << 24) | ((aBuf[1] + 128) << 16) | ((aBuf[2] + 128) << 8) | (aBuf[3] + 128);
		}
		// Last byte is always zero and unused in this format
		pInts[NumInts - 1] &= 0xFFFFFF00;
	}

	bool IntsToStr(const int *pInts, size_t NumInts, char *pStr, size_t StrSize)
	{
		dbg_assert(NumInts > 0, "IntsToStr: NumInts invalid");
		dbg_assert(StrSize >= NumInts * sizeof(int), "IntsToStr: StrSize invalid");

		// Unpack string without validation
		size_t StrIndex = 0;
		for(size_t IntIndex = 0; IntIndex < NumInts; IntIndex++)
		{
			const int CurrentInt = pInts[IntIndex];
			pStr[StrIndex] = ((CurrentInt >> 24) & 0xff) - 128;
			StrIndex++;
			pStr[StrIndex] = ((CurrentInt >> 16) & 0xff) - 128;
			StrIndex++;
			pStr[StrIndex] = ((CurrentInt >> 8) & 0xff) - 128;
			StrIndex++;
			pStr[StrIndex] = (CurrentInt & 0xff) - 128;
			StrIndex++;
		}
		// Ensure null-termination
		pStr[StrIndex - 1] = '\0';

		// Ensure valid UTF-8
		if(str_utf8_check(pStr))
		{
			return true;
		}
		pStr[0] = '\0';
		return false;
	}

}
