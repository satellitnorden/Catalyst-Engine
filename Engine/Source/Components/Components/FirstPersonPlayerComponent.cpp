//Header file.
#include <Components/Components/FirstPersonPlayerComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>

//First person player component constants.
namespace FirstPersonPlayerComponentConstants
{
	constexpr float32 STANDING_HEIGHT{ 2.0f };
	constexpr float32 STANDING_WALKING_SPEED{ 5.0f };
	constexpr float32 STANDING_RUNNING_SPEED{ 10.0f };
	constexpr float32 CROUCHING_HEIGHT{ 1.0f };
	constexpr float32 CROUCHING_WALKING_SPEED{ 2.5f };
	constexpr float32 CROUCHING_RUNNING_SPEED{ 5.0f };
	constexpr float32 JUMP_FORCE{ 8.0f };
}

/*
*	Updates the input state.
*/
void UpdateInputState(FirstPersonPlayerInputState *const RESTRICT input_state, const float32 delta_time) NOEXCEPT
{
	//Define constants.
	constexpr float32 GAMEPAD_ROTATION_SPEED{ 2.5f };
	constexpr float32 MOUSE_ROTATION_SPEED{ 100.0f };

	//Retrieve the input states.
	const GamepadState *const RESTRICT gamepad_state{ InputSystem::Instance->GetGamepadState(InputLayer::GAME) };
	const KeyboardState *const RESTRICT keyboard_state{ InputSystem::Instance->GetKeyboardState(InputLayer::GAME) };
	const MouseState *const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState(InputLayer::GAME) };

	//Update the rotation.
	input_state->_Rotation._X = 0.0f;
	input_state->_Rotation._Y = 0.0f;

	input_state->_Rotation._X += -gamepad_state->_RightThumbstickY * GAMEPAD_ROTATION_SPEED;
	input_state->_Rotation._Y += gamepad_state->_RightThumbstickX * GAMEPAD_ROTATION_SPEED;

	input_state->_Rotation._X += -mouse_state->_DeltaY * MOUSE_ROTATION_SPEED;
	input_state->_Rotation._Y += mouse_state->_DeltaX * MOUSE_ROTATION_SPEED;

	//Update the movement.
	input_state->_Movement._X = 0.0f;
	input_state->_Movement._Y = 0.0f;

	input_state->_Movement._X += gamepad_state->_LeftThumbstickY;
	input_state->_Movement._Y += gamepad_state->_LeftThumbstickX;

	input_state->_Movement._X += static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED_HELD);
	input_state->_Movement._X -= static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED_HELD);

	input_state->_Movement._Y += static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED_HELD);
	input_state->_Movement._Y -= static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED_HELD);

	//Update if the player is crouching.
	switch (InputSystem::Instance->GetLastUpdatedInputDeviceType())
	{
		case InputDeviceType::GAMEPAD:
		{
			if (gamepad_state->_B == ButtonState::PRESSED)
			{
				input_state->_GamepadCrouchToggle = !input_state->_GamepadCrouchToggle;
				input_state->_IsCrouching = input_state->_GamepadCrouchToggle;
			}

			break;
		}

		case InputDeviceType::KEYBOARD:
		case InputDeviceType::MOUSE:
		{
			input_state->_IsCrouching = keyboard_state->GetButtonState(KeyboardButton::C) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::C) == ButtonState::PRESSED_HELD;

			break;
		}
	}

	//Update if the player is sprinting.
	switch (InputSystem::Instance->GetLastUpdatedInputDeviceType())
	{
		case InputDeviceType::GAMEPAD:
		{
			if (gamepad_state->_LeftThumb == ButtonState::PRESSED)
			{
				input_state->_GamepadSprintToggle = !input_state->_GamepadSprintToggle;
				input_state->_IsSprinting = input_state->_GamepadSprintToggle;
			}

			break;
		}

		case InputDeviceType::KEYBOARD:
		case InputDeviceType::MOUSE:
		{
			input_state->_IsSprinting = keyboard_state->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED_HELD;

			break;
		}
	}

	//Update if the jump button was pressed.
	input_state->_JumpButtonPressed = gamepad_state->_A == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::Spacebar) == ButtonState::PRESSED;
}

/*
*	Returns whether or not the player can stand up.
*/
NO_DISCARD bool CanStandUp(const WorldPosition &world_position) NOEXCEPT
{
	//Do a raycast up. If it hits something, this character can't stand up.
	Ray ray;

	ray.SetOrigin(world_position.GetAbsolutePosition() + Vector3<float32>(0.0f, 1.0f, 0.0f));
	ray.SetDirection(Vector3<float32>(0.0f, 1.0f, 0.0f));

	RaycastConfiguration raycast_configuration;

	raycast_configuration._PhysicsChannels = PhysicsChannel::DYNAMIC_MODELS | PhysicsChannel::STATIC_MODELS | PhysicsChannel::TERRAIN;
	raycast_configuration._MaximumHitDistance = 1.1f;
	raycast_configuration._TerrainRayMarchStep = 1.0f;

	RaycastResult raycast_result;

	PhysicsSystem::Instance->CastRay(ray, raycast_configuration, &raycast_result);

	return !raycast_result._HasHit;
}

/*
*	Updates this component.
*/
void FirstPersonPlayerComponent::SerialUpdate(const UpdatePhase update_phase) NOEXCEPT
{
	PROFILING_SCOPE("FirstPersonPlayerComponent::SerialUpdate");

	switch (update_phase)
	{
		case UpdatePhase::GAMEPLAY:
		{
			//Cache the number of instances.
			const uint64 number_of_instances{ NumberOfInstances() };

			//Cache the delta time.
			const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

			for (uint64 instance_index{ 0 }; instance_index < number_of_instances; ++instance_index)
			{
				//Cache the instance data.
				Entity *const RESTRICT entity{ InstanceToEntity(instance_index) };
				FirstPersonPlayerInstanceData &instance_data{ _InstanceData[instance_index] };
				WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

				//Accumulate the movement.
				Vector3<float32> movement;

				//Update the input state.
				UpdateInputState(&instance_data._InputState, delta_time);

				//Apply rotation.
				instance_data._Rotation._Roll += instance_data._InputState._Rotation._X * delta_time;
				instance_data._Rotation._Yaw += instance_data._InputState._Rotation._Y * delta_time;
				instance_data._Rotation._Roll = BaseMath::Clamp<float32>(instance_data._Rotation._Roll, BaseMath::DegreesToRadians(-89.0f), BaseMath::DegreesToRadians(89.0f));

				//Calculate the forward and right vector.
				Vector3<float32> forward{ CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(instance_data._Rotation) };
				Vector3<float32> right{ CatalystCoordinateSpacesUtilities::RotatedWorldRightVector(instance_data._Rotation) };

				//Ignore the Y axis on the forward and right vectors.
				forward.NormalizeXZ();
				right.NormalizeXZ();

				//Accumulate the movement.
				movement += forward * instance_data._InputState._Movement._X;
				movement += right * instance_data._InputState._Movement._Y;

				//Add jump.
				if (instance_data._InputState._JumpButtonPressed)
				{
					instance_data._WantsToJump = true;
				}

				//Update whether or not this character is crouching.
				if (instance_data._InputState._IsCrouching)
				{
					if (instance_data._IsCrouching)
					{
						//Nothing to do here. (:
					}

					else
					{
						instance_data._IsCrouching = true;
					}
				}

				else
				{
					{
						if (instance_data._IsCrouching && CanStandUp(instance_data._CharacterController->GetWorldPosition()))
						{
							instance_data._IsCrouching = false;
						}

						else
						{
							//Nothing to do here. (:
						}
					}
				}

				//Update whether or not this character is sprinting.
				if (instance_data._InputState._IsSprinting)
				{
					if (instance_data._IsSprinting)
					{
						//Nothing to do here. (:
					}

					else
					{
						instance_data._IsSprinting = true;
					}
				}

				else
				{
					if (instance_data._IsSprinting)
					{
						instance_data._IsSprinting = false;
					}

					else
					{
						//Nothing to do here. (:
					}
				}

				//Update the speed spring damping system.
				float32 desired_speed;

				if (instance_data._IsCrouching)
				{
					desired_speed = instance_data._IsSprinting ? FirstPersonPlayerComponentConstants::CROUCHING_RUNNING_SPEED : FirstPersonPlayerComponentConstants::CROUCHING_WALKING_SPEED;
				}

				else
				{
					desired_speed = instance_data._IsSprinting ? FirstPersonPlayerComponentConstants::STANDING_RUNNING_SPEED : FirstPersonPlayerComponentConstants::STANDING_WALKING_SPEED;
				}

				instance_data._SpeedSpringDampingSystem.SetDesired(desired_speed);

				const float32 current_speed{ instance_data._SpeedSpringDampingSystem.Update(delta_time) };

				//Update the vertical velocity.
				if (instance_data._CharacterController->IsOnGround())
				{
					if (instance_data._WantsToJump)
					{
						instance_data._VerticalVelocity = FirstPersonPlayerComponentConstants::JUMP_FORCE;
					}

					else
					{
						instance_data._VerticalVelocity = -PhysicsConstants::GRAVITY * delta_time;
					}
				}

				else
				{
					instance_data._VerticalVelocity += -PhysicsConstants::GRAVITY * delta_time;
				}

				//Calculate the total displacement.
				Vector3<float32> total_displacement{ 0.0f, 0.0f, 0.0f };

				total_displacement += movement * current_speed * delta_time;
				total_displacement += Vector3<float32>(0.0f, instance_data._VerticalVelocity, 0.0f) * delta_time;

				//Move the character controller.
				instance_data._CharacterController->Move(total_displacement);

				//Update the current height.
				instance_data._HeightSpringDampingSystem.SetDesired(instance_data._IsCrouching ? FirstPersonPlayerComponentConstants::CROUCHING_HEIGHT : FirstPersonPlayerComponentConstants::STANDING_HEIGHT);
				instance_data._CurrentHeight = instance_data._HeightSpringDampingSystem.Update(delta_time);
				instance_data._CharacterController->ResizeCapsuleHeight(instance_data._CurrentHeight);

				//Reset whether or not this character wants to jump.
				instance_data._WantsToJump = false;

				//Update the world transform.
				{
					const Vector3<float32> character_controller_position{ instance_data._CharacterController->GetWorldPosition().GetAbsolutePosition() };

					const Quaternion roll_rotation{ CatalystCoordinateSpacesUtilities::RotatedWorldRightVector(instance_data._Rotation), instance_data._Rotation._Roll };
					const Quaternion yaw_rotation{ CatalystWorldCoordinateSpace::UP, instance_data._Rotation._Yaw };

					world_transform_instance_data._CurrentWorldTransform = WorldTransform(character_controller_position + Vector3<float32>(0.0f, instance_data._CurrentHeight, 0.0f), roll_rotation * yaw_rotation, 1.0f);
				}


				//Update the camera.
				RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->SetWorldTransform(world_transform_instance_data._CurrentWorldTransform);

				//Hide the cursor.
				InputSystem::Instance->HideCursor(InputLayer::GAME);
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Creates an instance.
*/
void FirstPersonPlayerComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	FirstPersonPlayerInitializationData *const RESTRICT _initialization_data{ static_cast<FirstPersonPlayerInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	FirstPersonPlayerInstanceData &instance_data{ _InstanceData.Back() };

	//Create the character controller.
	{
		CharacterControllerConfiguration configuration;

		configuration._CapsuleRadius = 0.2'75f;
		configuration._CapsuleHeight = 2.0f;

		instance_data._CharacterController = PhysicsSystem::Instance->CreateCharacterController(configuration);
	}

	//Set up the height spring damping system.
	instance_data._HeightSpringDampingSystem.SetCurrent(FirstPersonPlayerComponentConstants::STANDING_HEIGHT);
	instance_data._HeightSpringDampingSystem.SetDesired(FirstPersonPlayerComponentConstants::STANDING_HEIGHT);
	instance_data._HeightSpringDampingSystem.SetDampingCoefficient(32.0f);
	instance_data._HeightSpringDampingSystem.SetSpringConstant(256.0f);

	//Set up the speed spring damping system.
	instance_data._SpeedSpringDampingSystem.SetCurrent(FirstPersonPlayerComponentConstants::STANDING_WALKING_SPEED);
	instance_data._SpeedSpringDampingSystem.SetDesired(FirstPersonPlayerComponentConstants::STANDING_WALKING_SPEED);
	instance_data._SpeedSpringDampingSystem.SetDampingCoefficient(16.0f);
	instance_data._SpeedSpringDampingSystem.SetSpringConstant(128.0f);
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void FirstPersonPlayerComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(WorldTransformComponent::Instance->Has(entity), "Particle system component needs a world transform component!");

	//Cache the instance data.
	FirstPersonPlayerInstanceData &player_instance_data{ InstanceData(entity) };
	const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

	//Set the world position.
	player_instance_data._CharacterController->SetWorldPosition(world_transform_instance_data._CurrentWorldTransform.GetWorldPosition());
}

/*
*	Destroys an instance.
*/
void FirstPersonPlayerComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}