#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/SpringDampingSystem.h>

//Gameplay.
#include <Gameplay/Character.h>
#include <Gameplay/PlayerConfiguration.h>

/*
*	Class representing a player, including common player functionality.
*/
class Player : public Character
{

public:

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~Player() NOEXCEPT
	{

	}

protected:

	/*
	*	Initialize this player.
	*/
	void InitializePlayer() NOEXCEPT;

	/*
	*	Updates this player.
	*/
	void UpdatePlayer(const float32 delta_time) NOEXCEPT;

	//The player configuration.
	PlayerConfiguration _PlayerConfiguration;

private:

	/*
	*	Player input state definition.
	*/
	class PlayerInputState final
	{

	public:

		//The rotation.
		Vector2<float32> _Rotation{ 0.0f, 0.0f };

		//The movement spring damping systems.
		StaticArray<SpringDampingSystem, 2> _MovementSpringDampingSystems;

		//The movement.
		Vector2<float32> _Movement{ 0.0f, 0.0f };

		//Denotes if the player is crouching.
		bool _IsCrouching{ false };

		//Denotes if the player is running.
		bool _IsRunning{ false };

		//Denotes if the jump button was pressed.
		bool _JumpButtonPressed{ false };

		//Denotes if gamepad crouch is toggled.
		bool _GamepadCrouchToggle{ false };

		//Denotes if gamepad run is toggled.
		bool _GamepadRunToggle{ false };

	};

	//The current input state.
	PlayerInputState _CurrentInputState;

	//The rotation.
	Vector3<float32> _Rotation{ VectorConstants::ZERO };

	//The crouch spring damping system.
	SpringDampingSystem _CrouchSpringDampingSystem;

	//The speed spring damping system.
	SpringDampingSystem _SpeedSpringDampingSystem;

	/*
	*	Updates the current input state.
	*/
	void UpdateCurrentInputState(const float32 delta_time) NOEXCEPT;

};