#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Physics.
#include <Physics/Ray.h>
#include <Physics/RayCastResult.h>

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
	*	Updates the physics system synchronously during the physics update phase.
	*/
	void PhysicsUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Casts a ray.
	*/
	void CastRay(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Returns the wind direction.
	*/
	const Vector3& GetWindDirection() const NOEXCEPT { return _WindDirection; }

	/*
	*	Returns the wind strength.
	*/
	float GetWindStrength() const NOEXCEPT { return _WindStrength; }

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

	//The wind direction.
	Vector3 _WindDirection{ -1.0f, 0.0f, -1.0f };

	//The wind strength.
	float _WindStrength{ 5.0f };

};