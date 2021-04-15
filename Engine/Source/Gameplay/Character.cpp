//Header file.
#include <Gameplay/Character.h>

//Systems.
#include <Systems/PhysicsSystem.h>

/*
*	Initialize this character.
*/
void Character::InitializeCharacter() NOEXCEPT
{
	//Create the character controller.
	{
		CharacterControllerConfiguration configuration;

		configuration._CapsuleRadius = 0.2'75f;
		configuration._CapsuleHeight = 2.0f;

		_CharacterController = PhysicsSystem::Instance->CreateCharacterController(configuration);
	}

	//Set the character controllers world position.
	_CharacterController->SetWorldPosition(WorldPosition(Vector3<float32>(0.0f, 1.0f, 0.0f)));
}

/*
*	Update this character.
*/
void Character::UpdateCharacter(const float32 delta_time) NOEXCEPT
{

}