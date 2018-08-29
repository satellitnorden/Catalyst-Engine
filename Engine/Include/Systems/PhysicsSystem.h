#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Math.
#include <Math/Vector3.h>

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
	PhysicsSystem() NOEXCEPT { }

	/*
	*	Updates the physics system synchronously.
	*/
	void UpdateSystemSynchronous(const float deltaTime) NOEXCEPT;

	/*
	*	Returns the wind strength.
	*/
	float GetWindStrength() const NOEXCEPT { return windStrength; }

	/*
	*	Returns the wind direction.
	*/
	const Vector3& GetWindDirection() const NOEXCEPT { return windDirection; }

	/*
	*	Returns the ocean height.
	*/
	constexpr float GetOceanHeight() const NOEXCEPT { return 0.0f; }

	/*
	*	Given a world position, returns the height of the terrain at that point.
	*/
	float GetTerrainHeightAtPosition(const Vector3 &position) const NOEXCEPT;

	/*
	*	Given a world position, returns the normal of the terrain at that point.
	*/
	Vector3 GetTerrainNormalAtPosition(const Vector3 &position) const NOEXCEPT;

private:

	//The wind strength.
	float windStrength{ 5.0f };

	//The wind direction.
	Vector3 windDirection{ -1.0f, 0.0f, -1.0f };

};