#include "gameworld.h"

#include <polybob/antibot/antibot_data.h>

const float CHARACTER_PHYS_SIZE = 28.0f;

void CBobGameWorld::OnInit(CAntibotRoundData *pRoundData)
{
	m_pRoundData = pRoundData;
}

const CAntibotCharacterData *CBobGameWorld::IntersectCharacter(
	vec2 Pos0,
	vec2 Pos1,
	float Radius,
	vec2 &NewPos,
	const CAntibotCharacterData *pNotThis,
	int CollideWith,
	const CAntibotCharacterData *pThisOnly)
{
	if(!m_pRoundData)
		return nullptr;

	float ClosestLen = distance(Pos0, Pos1) * 100.0f;
	const CAntibotCharacterData *pClosest = nullptr;

	for(const CAntibotCharacterData &Entity : m_pRoundData->m_aCharacters)
	{
		if(&Entity == pNotThis)
			continue;

		if(pThisOnly && &Entity != pThisOnly)
			continue;

		// TODO: collide check
		// if(CollideWith != -1 && !pEntity->CanCollide(CollideWith))
		// 	continue;

		vec2 IntersectPos;
		if(closest_point_on_line(Pos0, Pos1, Entity.m_Pos, IntersectPos))
		{
			float Len = distance(Entity.m_Pos, IntersectPos);
			if(Len < CHARACTER_PHYS_SIZE + Radius)
			{
				Len = distance(Pos0, IntersectPos);
				if(Len < ClosestLen)
				{
					NewPos = IntersectPos;
					ClosestLen = Len;
					pClosest = &Entity;
				}
			}
		}
	}

	return pClosest;
}
