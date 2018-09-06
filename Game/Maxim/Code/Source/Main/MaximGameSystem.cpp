//Header file.
#include <Main/MaximGameSystem.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/DirectionalLightEntity.h>
#include <Entities/DynamicPhysicalEntity.h>
#include <Entities/ParticleSystemEntity.h>
#include <Entities/StaticPhysicalEntity.h>
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/CatalystVectorMath.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(MaximGameSystem);

DynamicPhysicalEntity *RESTRICT spinner;

/*
*	Initializes the Maxim game system.
*/
void MaximGameSystem::InitializeSystem() NOEXCEPT
{
	//Create the camera.
	camera = EntitySystem::Instance->CreateEntity<CameraEntity>();

	//Move the camera into position.
	camera->Move(Vector3(0.0f, 0.0f, 10.0f));

	//Set it as the active camera.
	RenderingSystem::Instance->SetActiveCamera(camera);

	//Create some particles.
	ParticleSystemEntity *const RESTRICT particles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };
	particles->Initialize(RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle), ParticleSystemProperties(10.0f, 60.0f, 0.01f, Vector2(0.025f, 0.025f), Vector2(0.05f, 0.05f), Vector3(-25.0f, 0.0f, -25.0f), Vector3(25.0f, 25.0f, 25.0f), Vector3(-0.25f, -0.25f, -0.25f), Vector3(0.25f, 0.25f, 0.25f), Vector3(0.0f, 0.0f, 0.0f)));

	//Create, uh, sun.
	sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	sun->SetIntensity(25.0f);
	sun->Rotate(Vector3(-2.5f, 135.0f, 0.0f));

	//Disable screen space ambient occlusion.
	RenderingConfigurationManager::Instance->SetScreenSpaceAmbientOcclusionEnabled(false);

	//Create a dynamic physcal cube that will spin around and stuff. (:
	spinner = EntitySystem::Instance->CreateEntity<DynamicPhysicalEntity>();
	DynamicPhysicalInitializationData  *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

	PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube) };
	model._Material = RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Teal);

	data->_PhysicalFlags = static_cast<uint8>(PhysicalFlag::Physical) | static_cast<uint8>(PhysicalFlag::Outline);
	data->_Model = model;
	data->_Position = Vector3(0.0f, 0.0f, 0.0f);
	data->_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	data->_Scale = Vector3(1.0f, 1.0f, 1.0f);

	EntitySystem::Instance->RequestInitialization(spinner, data, false);
}

/*
*	Updates the Maxim game system synchronously.
*/
void MaximGameSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	//Eh. Rotate the "sun".
	sun->Rotate(Vector3(0.0f, 2.5f * deltaTime, 0.0f));

	//Eh. Set bloom radius.
	const KeyboardState *const RESTRICT keyboard{ InputSystem::Instance->GetKeyboardState() };

	if (keyboard->GetButtonState(KeyboardButton::UpArrow) == ButtonState::PressedHold)
	{
		RenderingConfigurationManager::Instance->SetBloomRadius(RenderingConfigurationManager::Instance->GetBloomRadius() + (deltaTime * 0.001f));
	}

	if (keyboard->GetButtonState(KeyboardButton::DownArrow) == ButtonState::PressedHold)
	{
		RenderingConfigurationManager::Instance->SetBloomRadius(RenderingConfigurationManager::Instance->GetBloomRadius() - (deltaTime * 0.001f));
	}

	//Rotate the... Thing.
	if (spinner->IsInitialized())
	{
#if defined(CATALYST_WINDOWS)
		const MouseState *const RESTRICT state{ InputSystem::Instance->GetMouseState() };

		if (state->_Left == ButtonState::Pressed || state->_Left == ButtonState::PressedHold)
		{
			const Vector3 direction{ RenderingSystem::Instance->GetWorldDirectionFromScreenCoordinate(Vector2(state->_CurrentX, state->_CurrentY)) };
			const Vector3 newPosition{ CatalystVectorMath::LinePlaneIntersection(Vector3(0.0f, 0.0f, 0.0f), camera->GetPosition(), Vector3(0.0f, 0.0f, 1.0f), direction) };

			spinner->SetPosition(newPosition);
		}

		if (state->_Right == ButtonState::Pressed || state->_Right == ButtonState::PressedHold)
		{
			spinner->Rotate(Vector3(-180.0f * state->_DeltaY, 180.0f * state->_DeltaX, 0.0f));
		}
#elif defined(CATALYST_ANDROID)
		const TouchState *const RESTRICT state{ InputSystem::Instance->GetTouchState() };

		if (state->_ButtonState == ButtonState::PressedHold)
		{
			const Vector3 direction{ RenderingSystem::Instance->GetWorldDirectionFromScreenCoordinate(Vector2(state->_CurrentX, state->_CurrentY)) };
			const Vector3 newPosition{ CatalystVectorMath::LinePlaneIntersection(Vector3(0.0f, 0.0f, 0.0f), camera->GetPosition(), Vector3(0.0f, 0.0f, 1.0f), direction) };

			spinner->SetPosition(newPosition);
		}
#endif
	}
}

/*
*	Releases the Maxim game system.
*/
void MaximGameSystem::ReleaseSystem() NOEXCEPT
{

}