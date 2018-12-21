//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/CatalystPhysicsMath.h>
#include <Math/CatalystVectorMath.h>
#include <Math/Vector3.h>

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
*	Initializes the physics system.
*/
void PhysicsSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the physics system.
	PHYSICS_SYSTEM::Instance->InitializeSystem();
}

/*
*	Updates the physics system synchronously.
*/
void PhysicsSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the physics system synchronously.
	PHYSICS_SYSTEM::Instance->UpdateSystemSynchronous(context);
}

/*
*	Releases the physics system.
*/
void PhysicsSystem::ReleaseSystem() NOEXCEPT
{
	//Release the physics system.
	PHYSICS_SYSTEM::Instance->ReleaseSystem();
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
*	Adds an impulse at a position.
*/
void PhysicsSystem::AddImpulse(const Vector3<float> &position, const float radius, const float power) NOEXCEPT
{
	//Iterate over all dynamic physical components and add the impulse.
	const uint64 numberOfDynamicPhysicalComponents{ ComponentManager::GetNumberOfDynamicPhysicalComponents() };
	PhysicsComponent *RESTRICT physicsComponent{ ComponentManager::GetDynamicPhysicalPhysicsComponents() };
	TransformComponent *RESTRICT transformComponent{ ComponentManager::GetDynamicPhysicalTransformComponents() };

	for (uint64 i{ 0 }; i < numberOfDynamicPhysicalComponents; ++i, ++physicsComponent, ++transformComponent)
	{
		//Don't add the impulse on dynamic physical entities that doesn't simulate physics.
		if (!physicsComponent->_SimulatePhysics)
		{
			continue;
		}

		//Calculate the impulse weight.
		const float impulseWeight{ 1.0f - CatalystBaseMath::Minimum<float>(Vector3<float>::Length(transformComponent->_Position - position) / radius, 1.0f) };

		//Calculate the impulse direction.
		const Vector3<float> impulseDirection{ Vector3<float>::Normalize(transformComponent->_Position - position) };

		//Apply the impulse to the velocity.
		physicsComponent->_Velocity += CatalystPhysicsMath::CalculateAcceleration(impulseDirection * power * impulseWeight, physicsComponent->_Mass);
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

		//Calculate the net force acting upon this object, starting with the gravitational force.
		Vector3<float> force{ CatalystPhysicsMath::CalculateGravitationalForce(physicsComponent->_Mass) };

		//Calculate the acceleration.
		Vector3<float> acceleration{ CatalystPhysicsMath::CalculateAcceleration(force, physicsComponent->_Mass) };

		//Apply the acceleration to the velocity.
		physicsComponent->_Velocity += acceleration * context->_DeltaTime;

		//Apply the velocity to the position.
		transformComponent->_Position += physicsComponent->_Velocity * context->_DeltaTime;

		//For now, don't let dynamic physical entities fall beneath the terrain.
		float terrainHeight;
		TerrainSystem::Instance->GetTerrainHeightAtPosition(transformComponent->_Position, &terrainHeight);

		if (terrainHeight > transformComponent->_Position._Y)
		{
			transformComponent->_Position._Y = terrainHeight;

			Vector3<float> terrainNormal;
			TerrainSystem::Instance->GetTerrainNormalAtPosition(transformComponent->_Position, &terrainNormal);

			physicsComponent->_Velocity = CatalystPhysicsMath::CalculateReflectedDirection(Vector3<float>::Normalize(physicsComponent->_Velocity), terrainNormal) * Vector3<float>::Length(physicsComponent->_Velocity) / physicsComponent->_Mass;
		}
	}
}