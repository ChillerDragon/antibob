#include <polybob/antibot/antibot_data.h>

#include "gameworld.h"

void CBobGameWorld::OnInit(CAntibotRoundData *pRoundData)
{
	m_pRoundData = pRoundData;
}

CAntibotCharacterData *CBobGameWorld::IntersectCharacter(
	vec2 Pos0,
	vec2 Pos1,
	float Radius,
	vec2 &NewPos,
	const CAntibotCharacterData *pNotThis,
	int CollideWith,
	const CAntibotCharacterData *pThisOnly)
{
	return nullptr;
}
