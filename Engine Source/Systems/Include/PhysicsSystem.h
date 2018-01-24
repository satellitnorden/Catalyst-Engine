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

	/*
	*	Registers a physics component.
	*/
	void RegisterPhysicsComponent(PhysicsComponent *CATALYST_RESTRICT newPhysicsComponent) CATALYST_NOEXCEPT;

	/*
	*	Unregisters a physics component.
	*/
	void UnregisterPhysicsComponent(PhysicsComponent *CATALYST_RESTRICT physicsComponent) CATALYST_NOEXCEPT;

private:

	//Container for all physics component.
	DynamicArray<PhysicsComponent *CATALYST_RESTRICT> physicsComponents;

};