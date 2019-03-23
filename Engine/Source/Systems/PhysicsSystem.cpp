//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/Core/CatalystPhysicsMath.h>
#include <Math/Core/CatalystVectorMath.h>
#include <Math/General/Vector3.h>

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
	if ((channels & PhysicsChannel::DynamicPhysical) == PhysicsChannel::DynamicPhysical)
	{
		CastRayDynamicPhysical(ray, result);
	}

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
	/*
	//Iterate over all dynamic physical components and add the impulse.
	const uint64 numberOfDynamicPhysicalComponents{ ComponentManager::GetNumberOfDynamicPhysicalComponents() };
	PhysicsComponent *RESTRICT physicsComponent{ ComponentManager::GetDynamicPhysicalPhysicsComponents() };
	TransformComponent *RESTRICT transformComponent{ ComponentManager::GetDynamicPhysicalTransformComponents() };

	for (uint64 i{ 0 }; i < numberOfDynamicPhysicalComponents; ++i, ++physicsComponent, ++transformComponent)
	{
		//Calculate the impulse weight.
		const float impulseWeight{ 1.0f - CatalystBaseMath::Minimum<float>(Vector3<float>::Length(transformComponent->_Position - position) / radius, 1.0f) };

		//Calculate the impulse direction.
		const Vector3<float> impulseDirection{ Vector3<float>::Normalize(transformComponent->_Position - position) };

		//Apply the impulse to the velocity.
		physicsComponent->_Velocity += CatalystPhysicsMath::CalculateAcceleration(impulseDirection * power * impulseWeight, physicsComponent->_Mass);
	}
	*/
}

/*
*	Casts a ray against dynamic physical entities.
*/
void PhysicsSystem::CastRayDynamicPhysical(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT
{
	//Do a simple ray-box intersection test to determine which dynamic physical entity was hit.
	const uint64 numberOfDynamicPhysicalComponents{ ComponentManager::GetNumberOfDynamicPhysicalComponents() };
	const FrustumCullingComponent *RESTRICT component{ ComponentManager::GetDynamicPhysicalFrustumCullingComponents() };

	for (uint64 i = 0; i < numberOfDynamicPhysicalComponents; ++i, ++component)
	{
		const AxisAlignedBoundingBox &box{ component->_WorldSpaceAxisAlignedBoundingBox };

		if (CatalystVectorMath::LineBoxIntersection(box, ray))
		{
			result->_HasHit = true;
			result->_HitPosition = AxisAlignedBoundingBox::CalculateCenter(box);
			result->_HitEntity = ComponentManager::GetDynamicPhysicalEntities()->At(i);

			return;
		}
	}

	//If there was not hit, update the result.
	result->_HasHit = false;
	result->_HitPosition = Vector3<float>::ZERO;
	result->_HitEntity = nullptr;
}

/*
*	Casts a ray against the ocean.
*/
void PhysicsSystem::CastRayOcean(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT
{
	//Just do a simple line-plane intersection.
	result->_HasHit = true;
	result->_HitPosition = CatalystVectorMath::LinePlaneIntersection(Vector3<float>(0.0f, GetOceanHeight(), 0.0f), ray._Origin, Vector3<float>::UP, ray._Direction);
	result->_HitEntity = nullptr;
}

/*
*	Casts a ray against the terrain.
*/
void PhysicsSystem::CastRayTerrain(const Ray &ray, RayCastResult *const RESTRICT result) NOEXCEPT
{

}