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
#include <Systems/UpdateSystem.h>

//Maxim.
#include <Main/MaximObject.h>

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
	camera->Move(Vector3(0.0f, 0.0f, 5.0f));

	//Set it as the active camera.
	RenderingSystem::Instance->SetActiveCamera(camera);

	//Create some particles.
	ParticleSystemEntity *const RESTRICT particles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };
	particles->Initialize(RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle), ParticleSystemProperties(10.0f, 60.0f, 0.01f, Vector2(0.025f, 0.025f), Vector2(0.05f, 0.05f), Vector3(-25.0f, -25.0f, -25.0f), Vector3(25.0f, 25.0f, 25.0f), Vector3(-0.25f, -0.25f, -0.25f), Vector3(0.25f, 0.25f, 0.25f), Vector3(0.0f, 0.0f, 0.0f)));

	//Create, uh, sun.
	sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	sun->SetIntensity(25.0f);
	sun->Rotate(Vector3(-10.0f, 135.0f, 0.0f));

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

	//Spawn some objects.
	static DynamicArray<MaximObject> objects;
	static float timer{ 0.0f };

	timer += deltaTime;

	constexpr float SPAWN{ 5.0f };

	while (timer >= SPAWN)
	{
		objects.EmplaceSlow();

		UpdateSystem::Instance->RegisterAsynchronousPreUpdateFunction([](const UpdateFunctionContext *const RESTRICT context)
		{
			static_cast<MaximObject *const RESTRICT>(context->_Arguments)->PreUpdate(context->_Context);
		}, &objects.Back());

		timer -= SPAWN;
	}
}

/*
*	Releases the Maxim game system.
*/
void MaximGameSystem::ReleaseSystem() NOEXCEPT
{

}