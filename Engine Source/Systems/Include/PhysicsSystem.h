#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
class PhysicsComponent;

class PhysicsSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PhysicsSystem);

	/*
	*	Default constructor.
	*/
	PhysicsSystem() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~PhysicsSystem() CATALYST_NOEXCEPT;

	/*
	*	Updates the physics system synchronously.
	*/
	void UpdateSystemSynchronous(const float deltaTime) CATALYST_NOEXCEPT;

};