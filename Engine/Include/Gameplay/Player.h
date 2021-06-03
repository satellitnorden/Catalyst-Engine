#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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

	/*
	*	Returns whether or not this player is enabled.
	*/
	FORCE_INLINE NO_DISCARD bool GetEnabled() const NOEXCEPT
	{
		return _Enabled;
	}

	/*
	*	Sets whether or not this player is enabled.
	*/
	FORCE_INLINE void SetEnabled(const bool value) NOEXCEPT
	{
		_Enabled = value;
	}

protected:

	//The player configuration.
	PlayerConfiguration _PlayerConfiguration;

	/*
	*	Initialize this player.
	*/
	void InitializePlayer() NOEXCEPT;

	/*
	*	Updates this player.
	*/
	void UpdatePlayer(const float32 delta_time) NOEXCEPT;

private:

	/*
	*	Player input state definition.
	*/
	class PlayerInputState final
	{

	public:

		//The rotation.
		Vector2<float32> _Rotation{ 0.0f, 0.0f };

		//The movement.
		Vector2<float32> _Movement{ 0.0f, 0.0f };

		//Denotes if the player is crouching.
		bool _IsCrouching{ false };

		//Denotes if the player is sprinting.
		bool _IsSprinting{ false };

		//Denotes if the jump button was pressed.
		bool _JumpButtonPressed{ false };

		//Denotes if gamepad crouch is toggled.
		bool _GamepadCrouchToggle{ false };

		//Denotes if gamepad sprint is toggled.
		bool _GamepadSprintToggle{ false };

	};

	//The current input state.
	PlayerInputState _CurrentInputState;

	//The rotation.
	EulerAngles _Rotation;

	//Denotes whether or not this player is enabled.
	bool _Enabled{ true };

	/*
	*	Updates the current input state.
	*/
	void UpdateCurrentInputState(const float32 delta_time) NOEXCEPT;

};