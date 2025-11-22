#pragma once

namespace polybob
{

	extern const int gs_Msg_SixToSeven[50];
	inline int Msg_SixToSeven(int a)
	{
		if(a < 0 || a >= 50)
			return -1;
		return gs_Msg_SixToSeven[a];
	}
	extern const int gs_Msg_SevenToSix[40];
	inline int Msg_SevenToSix(int a)
	{
		if(a < 0 || a >= 40)
			return -1;
		return gs_Msg_SevenToSix[a];
	}
	extern const int gs_Obj_SixToSeven[21];
	inline int Obj_SixToSeven(int a)
	{
		if(a < 0 || a >= 21)
			return -1;
		return gs_Obj_SixToSeven[a];
	}
	extern const int gs_Obj_SevenToSix[25];
	inline int Obj_SevenToSix(int a)
	{
		if(a < 0 || a >= 25)
			return -1;
		return gs_Obj_SevenToSix[a];
	}

} // namespace polybob
