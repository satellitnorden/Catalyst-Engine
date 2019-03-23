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

			//Transform the local transform of the entity.
			transformComponent->_LocalTransform = newTransform * transformComponent->_LocalTransform;

			//Transform all children.
			for (Entity *const RESTRICT child : (*physicsEntity)->_Children)
			{
				child->Transform(transformComponent->_LocalTransform);
			}
		}
	}

	/*
	*	Handles collisions on physics entities.
	*/
	void HandleCollisions() NOEXCEPT
	{
		//Iterate over all physics components and handle collisions on them.
		const uint64 numberOfPhysicsComponents{ ComponentManager::GetNumberOfPhysicsComponents() };

		//Return early if there's none to update.
		if (numberOfPhysicsComponents == 0)
		{
			return;
		}

		Entity **RESTRICT physicsEntity{ ComponentManager::GetPhysicsEntities()->Begin() };
		PhysicsComponent *RESTRICT physicsComponent{ ComponentManager::GetPhysicsPhysicsComponents() };
		TransformComponent *RESTRICT physicsTransformComponent{ ComponentManager::GetPhysicsTransformComponents() };

		for (uint64 i{ 0 }; i < numberOfPhysicsComponents; ++i, ++physicsEntity, ++physicsComponent, ++physicsTransformComponent)
		{
			//Remember if a collision has happened.
			bool hasCollided{ false };

			//Check if this physics entity is colliding with any collision or physics entities and calculate a new velocity if so.
			Entity **RESTRICT collisionEntity{ ComponentManager::GetCollisionEntities()->Begin() };
			CollisionComponent *RESTRICT collisionComponent{ ComponentManager::GetCollisionCollisionComponents() };
			TransformComponent *RESTRICT collisionTransformComponent{ ComponentManager::GetCollisionTransformComponents() };

			for (uint64 j{ 0 }; j < numberOfPhysicsComponents; ++j, ++collisionEntity, ++collisionComponent, ++collisionTransformComponent)
			{
				if (CatalystPhysicsMath::IsColliding(physicsComponent->_PhysicsShape, physicsTransformComponent->_WorldTransform, collisionComponent->_PhysicsShape, collisionTransformComponent->_WorldTransform))
				{
					hasCollided = true;
				}
			}
		}
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