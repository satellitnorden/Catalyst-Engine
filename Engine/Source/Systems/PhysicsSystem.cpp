//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Core/CatalystPhysicsMath.h>
#include <Math/General/Vector.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

//Define the physics system.
#if defined(CATALYST_PHYSICS_PHYSX)
	#include <Physics/Abstraction/PhysX/PhysXPhysicsSystem.h>
	#define PHYSICS_SYSTEM PhysXPhysicsSystem
#endif

/*
*	Casts a ray.
*/
void PhysicsSystem::CastRay(const PhysicsChannel channels, const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT
{
	//Check that the ray is valid.
	ASSERT(Ray::IsValid(ray), "Invalid ray!");

	//Cast rays against the different channels.
	if ((channels & PhysicsChannel::Ocean) == PhysicsChannel::Ocean)
	{
		CastRayOcean(ray, result);
	}

	if ((channels & PhysicsChannel::Terrain) == PhysicsChannel::Terrain)
	{
		CastRayTerrain(ray, result);
	}
}

/*
*	Adds an impulse at a position.
*/
void PhysicsSystem::AddImpulse(const Vector3<float> &position, const float radius, const float power) NOEXCEPT
{
	
}

/*
*	Casts a ray against the ocean.
*/
void PhysicsSystem::CastRayOcean(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT
{
	//Just do a simple line-plane intersection.
	 result->_HasHit = CatalystGeometryMath::RayPlaneIntersection(	ray,
																	Plane(Vector3<float>(0.0f, GetOceanHeight(), 0.0f), VectorConstants::UP),
																	&result->_HitPosition);
	result->_HitEntity = nullptr;
}

/*
*	Casts a ray against the terrain.
*/
void PhysicsSystem::CastRayTerrain(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT
{

}