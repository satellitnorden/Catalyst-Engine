#if defined(CATALYST_PHYSICS_CATALYST)
//Header file.
#include <Physics/Abstraction/Catalyst/CatalystPhysicsSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Math.
#include <Math/CatalystPhysicsMath.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CatalystPhysicsSystem);

/*
*	Initializes the Catalyst physics system.
*/
void CatalystPhysicsSystem::InitializeSystem() NOEXCEPT
{
	
}

/*
*	Updates the Catalyst physics system synchronously.
*/
void CatalystPhysicsSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
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

/*
*	Releases the Catalyst physics system.
*/
void CatalystPhysicsSystem::ReleaseSystem() NOEXCEPT
{
	
}
#endif