#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Math.
#include <Math/Vector3.h>

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
	void UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the wind strength.
	*/
	float GetWindStrength() const NOEXCEPT { return _WindStrength; }

	/*
	*	Returns the wind direction.
	*/
	const Vector3& GetWindDirection() const NOEXCEPT { return _WindDirection; }

#if defined(CATALYST_ENABLE_OCEAN)
	/*
	*	Returns the ocean height.
	*/
	constexpr float GetOceanHeight() const NOEXCEPT { return 0.0f; }
#endif

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
	float _WindStrength{ 5.0f };

	//The wind direction.
	Vector3 _WindDirection{ -1.0f, 0.0f, -1.0f };

};