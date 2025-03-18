#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>

//Forward declarations.
class Entity;

class RaycastConfiguration final
{

public:

	//The maximum hit distance.
	float32 _MaximumHitDistance;

	//The ignored entities.
	ArrayProxy<Entity *RESTRICT> _IgnoredEntities;

};