#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Gameplay.
#include <Gameplay/CharacterConfiguration.h>

//Math.
#include <Math/General/SpringDampingSystem.h>

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

	//The character configuration.
	CharacterConfiguration _CharacterConfiguration;

	//The height spring damping system.
	SpringDampingSystem _HeightSpringDampingSystem;

	//The speed spring damping system.
	SpringDampingSystem _SpeedSpringDampingSystem;

	//The cached movement.
	Vector3<float32> _CachedMovement{ 0.0f, 0.0f, 0.0f };

	//The character controller.
	CharacterController *RESTRICT _CharacterController{ nullptr };

	//The current height.
	float32 _CurrentHeight{ 0.0f };

	//The vertical velocity.
	float32 _VerticalVelocity{ 0.0f };

	//Denotes if this character wants to jump.
	bool _WantsToJump{ false };

	//Denotes if this character wants to crouch.
	bool _WantsToCrouch{ false };

	//Denotes if this character is crouching.
	bool _IsCrouching{ false };

	//Denotes if this character wants to sprint.
	bool _WantsToSprint{ false };

	//Denotes if this character is sprinting.
	bool _IsSprinting{ false };

	/*
	*	Initialize this character.
	*/
	void InitializeCharacter() NOEXCEPT;

	/*
	*	Updates this character.
	*/
	void UpdateCharacter(const float32 delta_time) NOEXCEPT;

	/*
	*	Post updates this character.
	*/
	void PostUpdateCharacter(const float32 delta_time) NOEXCEPT;

	/*
	*	Adds movement.
	*/
	FORCE_INLINE void AddMovement(const Vector3<float32> &amount) NOEXCEPT
	{
		_CachedMovement += amount;
	}

	/*
	*	Sets whether or not this character wants to jump.
	*/
	FORCE_INLINE void SetWantsToJump(const bool value) NOEXCEPT
	{
		_WantsToJump = value;
	}

	/*
	*	Sets whether or not this character wants to crouch.
	*/
	FORCE_INLINE void SetWantsToCrouch(const bool value) NOEXCEPT
	{
		_WantsToCrouch = value;
	}

	/*
	*	Sets whether or not this character wants to sprint.
	*/
	FORCE_INLINE void SetWantsToSprint(const bool value) NOEXCEPT
	{
		_WantsToSprint = value;
	}

private:

	/*
	*	Returns whether or not this character can stand up.
	*/
	NO_DISCARD bool CanStandUp() const NOEXCEPT;

};