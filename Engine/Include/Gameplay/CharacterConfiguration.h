#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Class for the character configuration.
*	Used to set game-specific properties and enable/disable features.
*/
class CharacterConfiguration final
{

public:

	//The standing height.
	float32 _StandingHeight{ 2.0f };

	//The standing walking speed.
	float32 _StandingWalkingSpeed{ 5.0f };

	//The standing running speed.
	float32 _StandingRunningSpeed{ 10.0f };

	//The crouching height.
	float32 _CrouchingHeight{ 1.0f };

	//The crouching walking speed.
	float32 _CrouchingWalkingSpeed{ 2.5f };

	//The crouching running speed.
	float32 _CrouchingRunningSpeed{ 5.0f };

	//The jump force.
	float32 _JumpForce{ 8.0f };

};