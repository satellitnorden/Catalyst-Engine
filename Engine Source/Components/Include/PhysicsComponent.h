#pragma once

//Engine core.
#include <EngineCore.h>

//Components.
#include <Component.h>

//Math.
#include <Vector3.h>

class PhysicsComponent final : public Component
{

public:

	/*
	*	Default constructor.
	*/
	PhysicsComponent() CATALYST_NOEXCEPT;

	/*
	*	Constructor taking all values as arguments.
	*/
	PhysicsComponent(Entity *CATALYST_RESTRICT initialOwner, const float initialMass, const Vector3 &initialVelocity) CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~PhysicsComponent() CATALYST_NOEXCEPT;

	/*
	*	Returns the mass, denoted in KG.
	*/
	float GetMass() const CATALYST_NOEXCEPT { return mass; }

	/*
	*	Sets the mass, denoted in KG.
	*/
	void SetMass(const float newMass) CATALYST_NOEXCEPT { mass = newMass; }

	/*
	*	Returns the velocity, denoted in meters per second squared, const.
	*/
	const Vector3& GetVelocity() const CATALYST_NOEXCEPT { return velocity; }

	/*
	*	Returns the velocity, denoted in meters per second squared, non-const.
	*/
	Vector3& GetVelocity() CATALYST_NOEXCEPT { return velocity; }

	/*
	*	Sets the velocity, denoted in meters per second squared.
	*/
	void SetVelocity(const Vector3 &newVelocity) CATALYST_NOEXCEPT { velocity = newVelocity; }

private:

	//The mass, denoted in KG.
	float mass;

	//The velocity, denoted in meters per second squared.
	Vector3 velocity;

};