#include "collision.h"

#include "polybob/game/mapitems.h"

#include <polybob/antibot/antibot_data.h>

void CBobCollision::OnInit(CAntibotRoundData *pRoundData)
{
	m_pRoundData = pRoundData;
}

int CBobCollision::GetWidth() const
{
	if(!m_pRoundData)
		return 0;
	return m_pRoundData->m_Map.m_Width;
}

int CBobCollision::GetHeight() const
{
	if(!m_pRoundData)
		return 0;
	return m_pRoundData->m_Map.m_Height;
}

int CBobCollision::GetTile(int x, int y) const
{
	if(!m_pRoundData)
		return 0;
	if(!m_pRoundData->m_Map.m_pTiles)
		return 0;

	int Nx = std::clamp(x / 32, 0, GetWidth() - 1);
	int Ny = std::clamp(y / 32, 0, GetHeight() - 1);
	int Pos = (Ny * GetWidth()) + Nx;

	return m_pRoundData->m_Map.m_pTiles[Pos];
}

int CBobCollision::IsSolid(int x, int y) const
{
	int Index = GetTile(x, y);
	return Index == polybob::TILE_SOLID || Index == polybob::TILE_NOHOOK;
}

int CBobCollision::IntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const
{
	float Distance = distance(Pos0, Pos1);
	int End(Distance + 1);
	vec2 Last = Pos0;
	for(int i = 0; i <= End; i++)
	{
		float a = i / (float)End;
		vec2 Pos = mix(Pos0, Pos1, a);
		// Temporary position for checking collision
		int Ix = round_to_int(Pos.x);
		int Iy = round_to_int(Pos.y);

		if(CheckPoint(Ix, Iy))
		{
			if(pOutCollision)
				*pOutCollision = Pos;
			if(pOutBeforeCollision)
				*pOutBeforeCollision = Last;
			return GetCollisionAt(Ix, Iy);
		}

		Last = Pos;
	}
	if(pOutCollision)
		*pOutCollision = Pos1;
	if(pOutBeforeCollision)
		*pOutBeforeCollision = Pos1;
	return 0;
}
