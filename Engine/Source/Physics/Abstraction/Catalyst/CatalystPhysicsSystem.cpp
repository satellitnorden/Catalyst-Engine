#if defined(CATALYST_PHYSICS_CATALYST)
//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystPhysicsMath.h>

//Systems.
#include <Systems/TerrainSystem.h>

namespace CatalystPhysicsSystemLogic
{

	/*
	*	Simulates physics on physics entities.
	*/
	void SimulatePhysics(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		//Iterate over all physics components and simulate physics on them.
		const uint64 numberOfPhysicsComponents{ ComponentManager::GetNumberOfPhysicsComponents() };

		//Return early if there's none to update.
		if (numberOfPhysicsComponents == 0)
		{
			return;
		}

		Entity **RESTRICT physicsEntity{ ComponentManager::GetPhysicsEntities()->Begin() };
		PhysicsComponent *RESTRICT physicsComponent{ ComponentManager::GetPhysicsPhysicsComponents() };
		TransformComponent *RESTRICT transformComponent{ ComponentManager::GetPhysicsTransformComponents() };

		for (uint64 i{ 0 }; i < numberOfPhysicsComponents; ++i, ++physicsEntity, ++physicsComponent, ++transformComponent)
		{
			//Calculate the new transform.
			Matrix4 newTransform;

			//Calculate the net force acting upon this object, starting with the gravitational force.
			Vector3<float> force{ CatalystPhysicsMath::CalculateGravitationalForce(physicsComponent->_Mass) };

			//Calculate the acceleration.
			Vector3<float> acceleration{ CatalystPhysicsMath::CalculateAcceleration(force, physicsComponent->_Mass) };

			//Apply the acceleration to the velocity.
			physicsComponent->_Velocity += acceleration * context->_DeltaTime;

			//Apply the velocity to the position.
			newTransform.SetTranslation(physicsComponent->_Velocity * context->_DeltaTime);

			//Transform the world transform of the entity.
			transformComponent->_WorldTransform = newTransform * transformComponent->_WorldTransform;
		}
	}

	/*
	*	Handles collisions on physics entities.
	*/
	void HandleCollisions() NOEXCEPT
	{

	}
}

/*
*	Initializes the physics system.
*/
void PhysicsSystem::Initialize() NOEXCEPT
{

}

/*
*	Updates the physics system.
*/
void PhysicsSystem::Update(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Simulate physics.
	CatalystPhysicsSystemLogic::SimulatePhysics(context);

	//Handle collisions.
	CatalystPhysicsSystemLogic::HandleCollisions();
}

/*
*	Terminates the physics system.
*/
void PhysicsSystem::Terminate() NOEXCEPT
{

}
#endif