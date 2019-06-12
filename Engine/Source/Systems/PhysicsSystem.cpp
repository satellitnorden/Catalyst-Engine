//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

/*
*	Updates the physics system during the physics update phase.
*/
void PhysicsSystem::PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update all character movements.
	for (CharacterMovement *const RESTRICT movement : ComponentManager::ReadSingletonComponent<PhysicsSingletonComponent>()->_CharacterMovements)
	{
		/*
		//Calculate the expected new position.
		const Vector3<float> expectedPosition{ movement->_Position + movement->_Velocity * context->_DeltaTime };

		//Consider collision against statid model entities.
		const uint64 numberOfStaticModelComponents{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT staticModelComponent{ ComponentManager::GetStaticModelStaticModelComponents() };

		for (uint64 i{ 0 }; i < numberOfStaticModelComponents; ++i, ++staticModelComponent)
		{
			const Vector3<float> closestPoint{ AxisAlignedBoundingBox::GetClosestPoint(staticModelComponent->_WorldSpaceAxisAlignedBoundingBox, expectedPosition + Vector3<float>(0.0f, 1.0f, 0.0)) };

			if (closestPoint._Y > 0.0f && Vector3<float>::LengthSquaredXZ(closestPoint - expectedPosition) < (movement->_Radius * movement->_Radius))
			{
				movement->_Velocity = VectorConstants::ZERO;

				break;
			}
		}
		*/

		//Apply the movement.
		movement->_Position += movement->_Velocity * context->_DeltaTime;
		movement->_Velocity = VectorConstants::ZERO;
	}
}

/*
*	Registers a character movement.
*/
void PhysicsSystem::RegisterCharacterMovement(CharacterMovement *const RESTRICT movement) NOEXCEPT
{
	//Add it to the container.
	ComponentManager::WriteSingletonComponent<PhysicsSingletonComponent>()->_CharacterMovements.EmplaceSlow(movement);
}