#pragma once

#include <polybob/antibot/antibot_data.h>
#include <polybob/base/vmath.h>
#include <polybob/game/gamecore.h>

class CBobGameWorld
{
	CAntibotRoundData *m_pRoundData = nullptr;
public:
	void OnInit(CAntibotRoundData *pRoundData);

	const CAntibotCharacterData *IntersectCharacter(
		vec2 Pos0,
		vec2 Pos1,
		float Radius,
		vec2 &NewPos,
		const CAntibotCharacterData *pNotThis,
		int CollideWith,
		const CAntibotCharacterData *pThisOnly);

	polybob::CTuningParams m_Tuning;
};
