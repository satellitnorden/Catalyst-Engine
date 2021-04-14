#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Gameplay.
#include <Gameplay/CharacterConfiguration.h>

//Physics.
#include <Physics/Native/CharacterController.h>

/*
*	Class representing a character, including common character functionality.
*/
class Character
{

public:

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~Character() NOEXCEPT
	{

	}

protected:

	/*
	*	Initialize this character.
	*/
	void InitializeCharacter() NOEXCEPT;

	/*
	*	Update this character.
	*/
	void UpdateCharacter(const float32 delta_time) NOEXCEPT;

	//The character configuration.
	CharacterConfiguration _CharacterConfiguration;

	//The character controller.
	CharacterController *RESTRICT _CharacterController{ nullptr };

	//The vertical velocity.
	float32 _VerticalVelocity{ 0.0f };

};