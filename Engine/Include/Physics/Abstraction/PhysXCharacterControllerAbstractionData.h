#if defined(CATALYST_PHYSICS_PHYSX)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <ThirdParty/PhysX/PxPhysicsAPI.h>

class PhysXCharacterControllerAbstractionData final
{

public:

	//The controller.
	physx::PxController* RESTRICT _Controller;

	//Denotes if the character is on the ground.
	bool _IsOnGround{ true };

};

#endif