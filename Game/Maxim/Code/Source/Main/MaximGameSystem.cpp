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
	camera->Move(Vector3(0.0f, 1.0f + 2.0f, 1.0f));
	camera->Rotate(Vector3(-60.0f, 0.0f, 0.0f));

	//Set it as the active camera.
	RenderingSystem::Instance->SetActiveCamera(camera);

	//Set it as the active listener.
	SoundSystem::Instance->SetActiveListener(camera);

	//Create something to look at.
	PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube) };
	model.SetMaterial(RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red));

	StaticPhysicalEntity *const RESTRICT plane{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	plane->Initialize(model, Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));

	//Create, uh, sun.
	DirectionalLightEntity *const RESTRICT sun{ EntitySystem::Instance->CreateEntity<DirectionalLightEntity>() };
}

/*
*	Updates the Maxim game system synchronously.
*/
void MaximGameSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	//Eh. Move the camera a bit.
	camera->Move(Vector3(CatalystMath::SineRadians(EngineSystem::Instance->GetTotalGameTime()) * 0.01f, 0.0f, 0.0f));
}

/*
*	Releases the Maxim game system.
*/
void MaximGameSystem::ReleaseSystem() NOEXCEPT
{

}