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

	// WARNING: these are just the default** ddnet tunings from 2025
	//          this does not support tune zones in the map!
	//          this does not support custom tunes set by config or rcon!
	//          **this might not even be correct ddnet default tunings!
	//          gun_curvature, gun_speed, shotgun_curvature, shotgun_speed, shotgun_speeddif
	//          are not the default ddnet values! they are the default teeworlds/ddnet-insta
	//          values instead!!!
	polybob::CTuningParams m_Tuning;
};
