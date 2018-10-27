//Header file.
#include <Player/ClairvoyantPlayer.h>

//Entities.
#include <Entities/DynamicPhysicalEntity.h>
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/UpdateSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantPlayer);

/*
*	Initializes the Clairvoyant player.
*/
void ClairvoyantPlayer::Initialize() NOEXCEPT
{
	//Register the player for updates.
	UpdateSystem::Instance->RegisterAsynchronousLogicUpdate(this);
}

/*
*	Updates the Clairvoyant player asynchronously during the logic update phase.
*/
bool ClairvoyantPlayer::LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Apply gamepad controls.
	ApplyGamepadControls(context);

	//Apply keyboard controls.
	//ApplyKeyboardControls(context);

	return true;
}

/*
*	Applies gamepad controls.
*/
void ClairvoyantPlayer::ApplyGamepadControls(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Define constants.
	constexpr float ROTATION_SPEED{ 100.0f };
	constexpr float FLYING_NORMAL_SPEED{ 100.0f };
	constexpr float FLYING_FAST_SPEED{ 1'000.0f };
	constexpr float WALKING_NORMAL_SPEED{ 2.0f };
	constexpr float WALKING_FASTL_SPEED{ 10.0f };

	//Get the gamepad state.
	const GamepadState *const RESTRICT state{ InputSystem::Instance->GetGamepadState() };

	//Switch whether or not to constraint the viewer to the ground.
	static bool constrainToTerrain{ false };

	if (state->_A == ButtonState::Pressed)
	{
		constrainToTerrain = !constrainToTerrain;
	}

	//Determine the speed.
	const float speed{ constrainToTerrain ? CatalystBaseMath::LinearlyInterpolate(WALKING_NORMAL_SPEED, WALKING_FASTL_SPEED, state->_RightTrigger) : CatalystBaseMath::LinearlyInterpolate(FLYING_NORMAL_SPEED, FLYING_FAST_SPEED, state->_RightTrigger) };

	//Move the viewer.
	Viewer::Instance->Move(Viewer::Instance->GetRightVector() * state->_LeftThumbstickX * speed * context->_DeltaTime);
	Viewer::Instance->Move(Viewer::Instance->GetForwardVector() * state->_LeftThumbstickY * speed * context->_DeltaTime);

	if (state->_LeftShoulder == ButtonState::Pressed
		|| state->_LeftShoulder == ButtonState::PressedHold)
	{
		Viewer::Instance->Move(Vector3::DOWN * context->_DeltaTime * speed);
	}

	if (state->_RightShoulder == ButtonState::Pressed
		|| state->_RightShoulder == ButtonState::PressedHold)
	{
		Viewer::Instance->Move(Vector3::UP * context->_DeltaTime * speed);
	}

	//Constrain the viewer to the terrain.
	if (constrainToTerrain)
	{
		Vector3 position{ Viewer::Instance->GetPosition() };

		float terrainHeight;

		if (TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &terrainHeight))
		{
			position._Y = terrainHeight + 2.0f;

			Viewer::Instance->SetPosition(position);
		}
	}

	//Rotate the viewer.
	Viewer::Instance->Rotate(Vector3(state->_RightThumbstickY * ROTATION_SPEED * context->_DeltaTime, -state->_RightThumbstickX * ROTATION_SPEED * context->_DeltaTime, 0.0f));

	Vector3 rotation{ Viewer::Instance->GetRotation() };
	rotation._X = CatalystBaseMath::Clamp<float>(rotation._X, -89.0f, 89.0f);
	Viewer::Instance->SetRotation(rotation);

	//Spawn... Boxes. (:
	if (state->_X == ButtonState::Pressed)
	{
		const Ray ray{ Viewer::Instance->GetPosition(), Viewer::Instance->GetForwardVector(), FLOAT_MAXIMUM };
		RayCastResult result;

		PhysicsSystem::Instance->CastRay(PhysicsChannel::Ocean, ray, &result);

		DynamicPhysicalEntity *const RESTRICT box{ EntityCreationSystem::Instance->CreateEntity<DynamicPhysicalEntity>() };

		DynamicPhysicalInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

		data->_Properties = EntityInitializationData::EntityProperty::None;
		data->_PhysicalFlags = PhysicalFlag::Outline | PhysicalFlag::Physical;
		data->_Model = RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube);
		data->_Material = RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red);
		data->_Position = result._HitPosition;
		data->_Rotation = Vector3(0.0f, 0.0f, 0.0f);
		data->_Scale = Vector3(1.0f, 1.0f, 1.0f);
		data->_OutlineColor = Vector3(CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f), CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f), CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f));

		EntityCreationSystem::Instance->RequestInitialization(box, data, false);
	}
}

/*
*	Applies keyboard controls.
*/
void ClairvoyantPlayer::ApplyKeyboardControls(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Define constants.
	constexpr float ROTATION_SPEED{ 360.0f };
	constexpr float FLYING_NORMAL_SPEED{ 100.0f };
	constexpr float FLYING_FAST_SPEED{ 1'000.0f };
	constexpr float WALKING_SPEED{ 2.0f };

	//Get the gamepad state.
	const KeyboardState *const RESTRICT keyboardState{ InputSystem::Instance->GetKeyboardState() };
	const MouseState *const RESTRICT mouseState{ InputSystem::Instance->GetMouseState() };

	//Switch whether or not to constraint the viewer to the ground.
	static bool constrainToTerrain{ false };

	if (keyboardState->GetButtonState(KeyboardButton::Spacebar) == ButtonState::Pressed)
	{
		constrainToTerrain = !constrainToTerrain;
	}

	//Determine the speed.
	const float speed{ constrainToTerrain ? WALKING_SPEED : keyboardState->GetButtonState(KeyboardButton::LeftShift) == ButtonState::Pressed || keyboardState->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PressedHold ? FLYING_FAST_SPEED : FLYING_NORMAL_SPEED };

	//Move the viewer.
	if (keyboardState->GetButtonState(KeyboardButton::D) == ButtonState::Pressed
		|| keyboardState->GetButtonState(KeyboardButton::D) == ButtonState::PressedHold)
	{
		Viewer::Instance->Move(Viewer::Instance->GetRightVector() * speed * context->_DeltaTime);
	}

	if (keyboardState->GetButtonState(KeyboardButton::A) == ButtonState::Pressed
		|| keyboardState->GetButtonState(KeyboardButton::A) == ButtonState::PressedHold)
	{
		Viewer::Instance->Move(Viewer::Instance->GetRightVector() * -speed * context->_DeltaTime);
	}

	if (keyboardState->GetButtonState(KeyboardButton::W) == ButtonState::Pressed
		|| keyboardState->GetButtonState(KeyboardButton::W) == ButtonState::PressedHold)
	{
		Viewer::Instance->Move(Viewer::Instance->GetForwardVector() * speed * context->_DeltaTime);
	}

	if (keyboardState->GetButtonState(KeyboardButton::S) == ButtonState::Pressed
		|| keyboardState->GetButtonState(KeyboardButton::S) == ButtonState::PressedHold)
	{
		Viewer::Instance->Move(Viewer::Instance->GetForwardVector() * -speed * context->_DeltaTime);
	}

	if (keyboardState->GetButtonState(KeyboardButton::F) == ButtonState::Pressed
		|| keyboardState->GetButtonState(KeyboardButton::F) == ButtonState::PressedHold)
	{
		Viewer::Instance->Move(Vector3::DOWN * context->_DeltaTime * speed);
	}

	if (keyboardState->GetButtonState(KeyboardButton::R) == ButtonState::Pressed
		|| keyboardState->GetButtonState(KeyboardButton::R) == ButtonState::PressedHold)
	{
		Viewer::Instance->Move(Vector3::UP * context->_DeltaTime * speed);
	}

	//Constrain the viewer to the terrain.
	if (constrainToTerrain)
	{
		Vector3 position{ Viewer::Instance->GetPosition() };

		float terrainHeight;

		if (TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &terrainHeight))
		{
			position._Y = terrainHeight + 2.0f;

			Viewer::Instance->SetPosition(position);
		}
	}

	//Rotate the viewer.
	Viewer::Instance->Rotate(Vector3(mouseState->_DeltaY * ROTATION_SPEED, -mouseState->_DeltaX * ROTATION_SPEED, 0.0f));

	Vector3 rotation{ Viewer::Instance->GetRotation() };
	rotation._X = CatalystBaseMath::Clamp<float>(rotation._X, -89.0f, 89.0f);
	Viewer::Instance->SetRotation(rotation);

	//Spawn... Boxes. (:
	if (keyboardState->GetButtonState(KeyboardButton::E) == ButtonState::Pressed)
	{
		const Ray ray{ Viewer::Instance->GetPosition(), Viewer::Instance->GetForwardVector(), FLOAT_MAXIMUM };
		RayCastResult result;

		PhysicsSystem::Instance->CastRay(PhysicsChannel::Ocean, ray, &result);

		DynamicPhysicalEntity *const RESTRICT box{ EntityCreationSystem::Instance->CreateEntity<DynamicPhysicalEntity>() };

		DynamicPhysicalInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

		data->_Properties = EntityInitializationData::EntityProperty::None;
		data->_PhysicalFlags = PhysicalFlag::Outline | PhysicalFlag::Physical;
		data->_Model = RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube);
		data->_Material = RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red);
		data->_Position = result._HitPosition;
		data->_Rotation = Vector3(0.0f, 0.0f, 0.0f);
		data->_Scale = Vector3(1.0f, 1.0f, 1.0f);
		data->_OutlineColor = Vector3(CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f), CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f), CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f));

		EntityCreationSystem::Instance->RequestInitialization(box, data, false);
	}
}