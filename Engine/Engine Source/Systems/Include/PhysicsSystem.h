#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
class PhysicsComponent;
class Vector3;

class PhysicsSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PhysicsSystem);

	/*
	*	Default constructor.
	*/
	PhysicsSystem() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~PhysicsSystem() NOEXCEPT;

	/*
	*	Updates the physics system synchronously.
	*/
	void UpdateSystemSynchronous(const float deltaTime) NOEXCEPT;

	/*
	*	Given a world position, returns the height of the terrain at that point.
	*/
	float GetTerrainHeightAtPosition(const Vector3 &position) const NOEXCEPT;

};