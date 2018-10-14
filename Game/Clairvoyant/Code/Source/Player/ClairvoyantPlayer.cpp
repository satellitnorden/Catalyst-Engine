//Header file.
#include <Player/ClairvoyantPlayer.h>

//Entities.
#include <Entities/DynamicPhysicalEntity.h>
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Systems.
#include <Systems/EntitySystem.h>
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
	//Create the camera and set it as the active one.
	_Camera = EntitySystem::Instance->CreateEntity<CameraEntity>();
	RenderingSystem::Instance->SetActiveCamera(_Camera);

	//Register the player for updates.
	UpdateSystem::Instance->RegisterAsynchronousLogicUpdate(this);
}

/*
*	Updates the Clairvoyant player asynchronously during the logic update phase.
*/
bool ClairvoyantPlayer::LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Define constants.
	constexpr float ROTATION_SPEED{ 100.0f };
	constexpr float FLYING_NORMAL_SPEED{ 100.0f };
	constexpr float FLYING_FAST_SPEED{ 1'000.0f };
	constexpr float WALKING_SPEED{ 2.0f };

	//Get the gamepad state.
	const GamepadState *const RESTRICT state{ InputSystem::Instance->GetGamepadState() };

	//Switch whether or not to constraint the camera to the ground.
	static bool constrainToTerrain{ false };

	if (state->_A == ButtonState::Pressed)
	{
		constrainToTerrain = !constrainToTerrain;
	}

	//Determine the speed.
	const float speed{ constrainToTerrain ? WALKING_SPEED : CatalystBaseMath::LinearlyInterpolate(FLYING_NORMAL_SPEED, FLYING_FAST_SPEED, state->_RightTrigger) };

	//Move the camera.
	_Camera->Move(_Camera->GetRightVector() * state->_LeftThumbstickX * speed * context->_DeltaTime);
	_Camera->Move(_Camera->GetForwardVector() * state->_LeftThumbstickY * speed * context->_DeltaTime);

	if (state->_LeftShoulder == ButtonState::Pressed
		|| state->_LeftShoulder == ButtonState::PressedHold)
	{
		_Camera->Move(Vector3::DOWN * context->_DeltaTime * speed);
	}

	if (state->_RightShoulder == ButtonState::Pressed
		|| state->_RightShoulder == ButtonState::PressedHold)
	{
		_Camera->Move(Vector3::UP * context->_DeltaTime * speed);
	}

	//Constrain the camera to the terrain.
	if (constrainToTerrain)
	{
		Vector3 position{ _Camera->GetPosition() };

		float terrainHeight;

		if (TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &terrainHeight))
		{
			position._Y = terrainHeight + 2.0f;

			_Camera->SetPosition(position);
		}
	}

	//Rotate the camera.
	_Camera->Rotate(Vector3(state->_RightThumbstickY * ROTATION_SPEED * context->_DeltaTime, -state->_RightThumbstickX * ROTATION_SPEED * context->_DeltaTime, 0.0f));

	Vector3 rotation{ _Camera->GetRotation() };
	rotation._X = CatalystBaseMath::Clamp<float>(rotation._X, -89.0f, 89.0f);
	_Camera->SetRotation(rotation);

	//Spawn... Boxes. (:
	if (state->_X == ButtonState::Pressed)
	{
		const Ray ray{ _Camera->GetPosition(), _Camera->GetForwardVector(), FLOAT_MAXIMUM };
		RayCastResult result;

		PhysicsSystem::Instance->CastRay(PhysicsChannel::Ocean, ray, &result);

		DynamicPhysicalEntity *const RESTRICT box{ EntitySystem::Instance->CreateEntity<DynamicPhysicalEntity>() };

		DynamicPhysicalInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

		data->_Properties = EntityInitializationData::EntityProperty::None;
		data->_PhysicalFlags = PhysicalFlag::Outline | PhysicalFlag::Physical;
		data->_Model = RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube);
		data->_Material = RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red);
		data->_Position = result._HitPosition;
		data->_Rotation = Vector3(0.0f, 0.0f, 0.0f);
		data->_Scale = Vector3(1.0f, 1.0f, 1.0f);
		data->_OutlineColor = Vector3(CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f), CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f), CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f));

		EntitySystem::Instance->RequestInitialization(box, data, false);
	}

	return true;
}