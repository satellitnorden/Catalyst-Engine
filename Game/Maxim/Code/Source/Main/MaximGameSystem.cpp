//Header file.
#include <Main/MaximGameSystem.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/DirectionalLightEntity.h>
#include <Entities/ParticleSystemEntity.h>
#include <Entities/StaticPhysicalEntity.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Math.
#include <Math/CatalystMath.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(MaximGameSystem);

/*
*	Initializes the Maxim game system.
*/
void MaximGameSystem::InitializeSystem() NOEXCEPT
{
	//Create the camera.
	camera = EntitySystem::Instance->CreateEntity<CameraEntity>();

	//Move the camera into position.
	camera->Move(Vector3(0.0f, 1.0f + 8.0f, 32.0f));
	camera->Rotate(Vector3(-20.0f, 0.0f, 0.0f));

	//Set it as the active camera.
	RenderingSystem::Instance->SetActiveCamera(camera);

	//Create something to look at.
	{
		PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Plane) };
		model.SetMaterial(RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red));

		StaticPhysicalEntity *const RESTRICT object{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
		object->Initialize(model, Vector3(0.0f, -2.5f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 100.0f));
	}

	{
		PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Octahedron) };
		model.SetMaterial(RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red));

		StaticArray<Vector3, 5> positions
		{
			Vector3(0.0f, 1.0f, 0.0f),
			Vector3(10.0f, 1.0f, 0.0f),
			Vector3(-10.0f, 1.0f, 0.0f),
			Vector3(0.0f, 1.0f, 10.0f),
			Vector3(0.0f, 1.0f, -10.0f)
		};

		StaticArray<Vector3, 5> scales
		{
			Vector3(2.0f, 7.5f, 2.0f),
			Vector3(3.0f, 10.0f, 3.0f),
			Vector3(3.0f, 10.0f, 3.0f),
			Vector3(1.0f, 5.0f, 1.0f),
			Vector3(4.0f, 12.5f, 4.0f)
		};

		for (uint8 i = 0; i < 5; ++i)
		{
			StaticPhysicalEntity *const RESTRICT object{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
			object->Initialize(model, positions[i], Vector3(0.0f, 0.0f, 0.0f), scales[i]);
		}
	}

	{
		PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube) };
		model.SetMaterial(RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red));

		StaticArray<Vector3, 6> positions
		{
			Vector3(10.0f, 1.0f, 10.0f),
			Vector3(10.0f, 1.0f, -10.0f),
			Vector3(-10.0f, 1.0f, -10.0f),
			Vector3(-10.0f, 1.0f, 10.0f),
			Vector3(-32.5f, 1.0f, -25.0f),
			Vector3(32.5f, 1.0f, -25.0f)
		};

		StaticArray<Vector3, 6> scales
		{
			Vector3(1.0f, 10.0f, 1.0f),
			Vector3(1.0f, 20.0f, 1.0f),
			Vector3(1.0f, 20.0f, 1.0f),
			Vector3(1.0f, 10.0f, 1.0f),
			Vector3(1.0f, 250.0f, 1.0f),
			Vector3(1.0f, 250.0f, 1.0f)
		};

		for (uint8 i = 0; i < 6; ++i)
		{
			StaticPhysicalEntity *const RESTRICT object{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
			object->Initialize(model, positions[i], Vector3(0.0f, 0.0f, 0.0f), scales[i]);
		}
	}

	{
		PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube) };
		model.SetMaterial(RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red));

		{
			StaticPhysicalEntity *const RESTRICT object{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
			object->Initialize(model, Vector3(25.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 100.0f, 10.0f));
		}

		{
			StaticPhysicalEntity *const RESTRICT object{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
			object->Initialize(model, Vector3(-25.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 100.0f, 10.0f));
		}
	}

	//Create some particles.
	ParticleSystemEntity *const RESTRICT particles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };
	particles->Initialize(RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle), ParticleSystemProperties(10.0f, 60.0f, 0.01f, Vector2(0.025f, 0.025f), Vector2(0.05f, 0.05f), Vector3(-25.0f, 0.0f, -25.0f), Vector3(25.0f, 25.0f, 25.0f), Vector3(-0.25f, -0.25f, -0.25f), Vector3(0.25f, 0.25f, 0.25f), Vector3(0.0f, 0.0f, 0.0f)));

	//Create, uh, sun.
	sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	sun->SetIntensity(25.0f);
	sun->Rotate(Vector3(-2.5f, 135.0f, 0.0f));

	//Disable screen space ambient occlusion.
	RenderingConfigurationManager::Instance->SetScreenSpaceAmbientOcclusionEnabled(false);
}

/*
*	Updates the Maxim game system synchronously.
*/
void MaximGameSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	//Eh. Rotate the "sun".
	sun->Rotate(Vector3(0.0f, 2.5f * deltaTime, 0.0f));

	//Eh. Set bloom radius.
	const KeyboardState *const RESTRICT keyboard{ InputSystem::Instance->GetCurrentKeyboardState() };

	if (keyboard->GetKeyboardButtonState(KeyboardButton::UpArrow) == KeyboardButtonState::PressedHold)
	{
		RenderingConfigurationManager::Instance->SetBloomRadius(RenderingConfigurationManager::Instance->GetBloomRadius() + (deltaTime * 0.001f));
	}

	if (keyboard->GetKeyboardButtonState(KeyboardButton::DownArrow) == KeyboardButtonState::PressedHold)
	{
		RenderingConfigurationManager::Instance->SetBloomRadius(RenderingConfigurationManager::Instance->GetBloomRadius() - (deltaTime * 0.001f));
	}
}

/*
*	Releases the Maxim game system.
*/
void MaximGameSystem::ReleaseSystem() NOEXCEPT
{

}