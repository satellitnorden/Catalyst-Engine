#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>

class RaycastConfiguration final
{

public:

	//The physics channels.
	PhysicsChannel _PhysicsChannels;

	//The maximum hit distance.
	float _MaximumHitDistance;

	//The terrain ray march step.
	float _TerrainRayMarchStep;

};