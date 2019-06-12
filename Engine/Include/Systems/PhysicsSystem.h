#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/PhysicsCore.h>
#include <Physics/RayCastResult.h>

class PhysicsSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PhysicsSystem);

	/*
	*	Default constructor.
	*/
	PhysicsSystem() NOEXCEPT
	{
	
	}

};