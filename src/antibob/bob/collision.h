#pragma once

#include <polybob/antibot/antibot_data.h>
#include <polybob/base/vmath.h>

class CBobCollision
{
	CAntibotRoundData *m_pRoundData = nullptr;

public:
	void OnInit(CAntibotRoundData *pRoundData);

	int GetWidth() const;
	int GetHeight() const;
	int GetTile(int x, int y) const;
	int GetCollisionAt(float x, float y) const { return GetTile(round_to_int(x), round_to_int(y)); }
	int IsSolid(int x, int y) const;
	bool CheckPoint(float x, float y) const { return IsSolid(round_to_int(x), round_to_int(y)); }
	bool CheckPoint(vec2 Pos) const { return CheckPoint(Pos.x, Pos.y); }
	int IntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
};
