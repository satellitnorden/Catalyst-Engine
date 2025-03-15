#if defined(CATALYST_PHYSICS_JOLT)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/Character.h>

class JoltCharacterControllerAbstractionData final
{

public:

	//The character.
	JPH::Character *RESTRICT _Character;

};

#endif