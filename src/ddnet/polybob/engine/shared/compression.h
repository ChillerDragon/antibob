#pragma once

namespace polybob {

// variable int packing
class CVariableInt
{
public:
	enum
	{
		MAX_BYTES_PACKED = 5, // maximum number of bytes in a packed int
	};

	static unsigned char *Pack(unsigned char *pDst, int i, int DstSize);
	static const unsigned char *Unpack(const unsigned char *pSrc, int *pInOut, int SrcSize);

	static long Compress(const void *pSrc, int SrcSize, void *pDst, int DstSize);
	static long Decompress(const void *pSrc, int SrcSize, void *pDst, int DstSize);
};

} // namespace polybob
