#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Math.
#include <Math/General/SpringDampingSystem.h>

//Physics.
#include <Physics/Native/CharacterController.h>

class FirstPersonPlayerInputState final
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

class FirstPersonPlayerInitializationData final : public ComponentInitializationData
{

public:

};

class FirstPersonPlayerInstanceData final
{

public:

	//The character controller.
	CharacterController *RESTRICT _CharacterController{ nullptr };

	//The height spring damping system.
	SpringDampingSystem _HeightSpringDampingSystem;

	//The speed spring damping system.
	SpringDampingSystem _SpeedSpringDampingSystem;

	//The current height.
	float32 _CurrentHeight{ 0.0f };

	//The vertical velocity.
	float32 _VerticalVelocity{ 0.0f };

	//Denotes if this character wants to jump.
	bool _WantsToJump{ false };

	//Denotes if this character is crouching.
	bool _IsCrouching{ false };

	//Denotes if this character is sprinting.
	bool _IsSprinting{ false };

	//The input state.
	FirstPersonPlayerInputState _InputState;

	//The rotation.
	EulerAngles _Rotation;

};

DECLARE_COMPONENT
(
	FirstPersonPlayerComponent,
	FirstPersonPlayerInitializationData,
	FirstPersonPlayerInstanceData,
	COMPONENT_SERIAL_UPDATE(UpdatePhase::GAMEPLAY)
	
public:

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT override;

);