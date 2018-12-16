#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Physics.
#include <Physics/PhysicsCore.h>
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
	void CastRay(const PhysicsChannel channels, const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Adds an impulse at a position.
	*/
	void AddImpulse(const Vector3<float> &position, const float radius, const float power) NOEXCEPT;

	/*
	*	Returns the wind direction.
	*/
	const Vector3<float>& GetWindDirection() const NOEXCEPT { return _WindDirection; }

	/*
	*	Returns the wind speed.
	*/
	float GetWindSpeed() const NOEXCEPT { return _WindSpeed; }

	/*
	*	Sets the wind speed.
	*/
	void SetWindSpeed(const float newWindSpeed) NOEXCEPT { _WindSpeed = newWindSpeed; }

#if defined(CATALYST_ENABLE_OCEAN)
	/*
	*	Returns the ocean height.
	*/
	constexpr float GetOceanHeight() const NOEXCEPT { return 0.0f; }
#endif

private:

	//The wind direction.
	Vector3<float> _WindDirection{ Vector3<float>::Normalize(Vector3<float>::RIGHT + Vector3<float>::FORWARD) };

	//The wind speed.
	float _WindSpeed{ 4.0f };

	/*
	*	Casts a ray against dynamic physical entities.
	*/
	void CastRayDynamicPhysical(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Casts a ray against the ocean.
	*/
	void CastRayOcean(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Casts a ray against the terrain.
	*/
	void CastRayTerrain(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Simulates physics on dynamic physical entities.
	*/
	void SimulatePhysicsDynamicPhysical(const UpdateContext *const RESTRICT context) NOEXCEPT;

};