//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/CatalystVectorMath.h>
#include <Math/Vector3.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

/*
*	Updates the physics system synchronously during the physics update phase.
*/
void PhysicsSystem::PhysicsUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Simulate physics on dynamic physical entities.
	SimulatePhysicsDynamicPhysical(context);
}

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
	result->_HitPosition = Vector3<float>(0.0f, 0.0f, 0.0f);
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

/*
*	Simulates physics on dynamic physical entities.
*/
void PhysicsSystem::SimulatePhysicsDynamicPhysical(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Iterate over all dynamic physical components and simulate physics on them.
	const uint64 numberOfDynamicPhysicalComponents{ ComponentManager::GetNumberOfDynamicPhysicalComponents() };
	PhysicsComponent *RESTRICT physicsComponent{ ComponentManager::GetDynamicPhysicalPhysicsComponents() };
	TransformComponent *RESTRICT transformComponent{ ComponentManager::GetDynamicPhysicalTransformComponents() };

	for (uint64 i{ 0 }; i < numberOfDynamicPhysicalComponents; ++i, ++physicsComponent, ++transformComponent)
	{
		//Don't simulate physics on dynamic physical entities that doesn't want it.
		if (!physicsComponent->_SimulatePhysics)
		{
			continue;
		}

		//Calculate the gravitational acceleration.
		const float gravitationalAcceleration{ _GravitationalSpeed };

		//Apply the gravitational acceleration to the velocity.
		physicsComponent->_Velocity += _GravitationalDirection * gravitationalAcceleration * context->_DeltaTime;

		//Apply the velocity to the position.
		transformComponent->_Position += physicsComponent->_Velocity * context->_DeltaTime;

		//For now, don't let dynamic physical entities fall beneath the terrain.
		float terrainHeight;
		TerrainSystem::Instance->GetTerrainHeightAtPosition(transformComponent->_Position, &terrainHeight);

		if (terrainHeight > transformComponent->_Position._Y)
		{
			transformComponent->_Position._Y = terrainHeight;
			physicsComponent->_Velocity = Vector3<float>(0.0f, 0.0f, 0.0f);
		}
	}
}