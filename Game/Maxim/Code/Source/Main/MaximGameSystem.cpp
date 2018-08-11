//Header file.
#include <Main/MaximGameSystem.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/DirectionalLightEntity.h>
#include <Entities/StaticPhysicalEntity.h>

//Math.
#include <Math/CatalystMath.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/SoundSystem.h>

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
	camera->Move(Vector3(0.0f, 1.0f + 4.0f, 16.0f));
	camera->Rotate(Vector3(-15.0f, 0.0f, 0.0f));

	//Set it as the active camera.
	RenderingSystem::Instance->SetActiveCamera(camera);

	//Set it as the active listener.
	SoundSystem::Instance->SetActiveListener(camera);

	//Create something to look at.
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

		for (uint8 i = 0; i < 5; ++i)
		{
			StaticPhysicalEntity *const RESTRICT object{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
			object->Initialize(model, positions[i], Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 5.0f, 1.0f));
		}
	}

	{
		PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube) };
		model.SetMaterial(RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red));

		StaticArray<Vector3, 4> positions
		{
			Vector3(10.0f, 1.0f, 10.0f),
			Vector3(10.0f, 1.0f, -10.0f),
			Vector3(-10.0f, 1.0f, -10.0f),
			Vector3(-10.0f, 1.0f, 10.0f)
		};

		for (uint8 i = 0; i < 4; ++i)
		{
			StaticPhysicalEntity *const RESTRICT object{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
			object->Initialize(model, positions[i], Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 10.0f, 1.0f));
		}
	}

	//Create, uh, sun.
	sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	sun->SetIntensity(100.0f);
}

/*
*	Updates the Maxim game system synchronously.
*/
void MaximGameSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	//Eh. Rotate the "sun".
	sun->Rotate(Vector3(0.0f, 2.5f * deltaTime, 0.0f));
}

/*
*	Releases the Maxim game system.
*/
void MaximGameSystem::ReleaseSystem() NOEXCEPT
{

}