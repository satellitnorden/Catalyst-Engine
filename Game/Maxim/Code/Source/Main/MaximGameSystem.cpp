//Header file.
#include <Main/MaximGameSystem.h>

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

//Singleton definition.
DEFINE_SINGLETON(MaximGameSystem);

DynamicPhysicalEntity *RESTRICT spinner;

/*
*	Initializes the Maxim game system.
*/
void MaximGameSystem::InitializeSystem() NOEXCEPT
{
	//Create the camera.
	_Camera = EntitySystem::Instance->CreateEntity<CameraEntity>();

	//Move the camera into position.
	_Camera->Move(Vector3(0.0f, 0.0f, 10.0f));

	//Set it as the active camera.
	RenderingSystem::Instance->SetActiveCamera(_Camera);

	//Create the particles.
	for (ParticleSystemEntity *RESTRICT &particles : _Particles)
	{
		particles = EntitySystem::Instance->CreateEntity<ParticleSystemEntity>();
		particles->Initialize(	RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle),
								ParticleSystemProperties(	10.0f,
															60.0f,
															0.1f,
															Vector2(0.01f, 0.01f),
															Vector2(0.02f, 0.02f),
															Vector3(-5.0f, -7.5f, -5.0f),
															Vector3(5.0f, 7.5f, 5.0f),
															Vector3(-0.2f, -0.2f, -0.2f),
															Vector3(0.2f, 0.2f, 0.2f),
															Vector3(0.0f, 0.0f, 0.0f)));
	}

	//Create the sun.
	_Sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	_Sun->SetIntensity(100.0f);
	_Sun->Rotate(Vector3(-22.5f, 170.0f, 0.0f));
}

/*
*	Updates the Maxim game system synchronously.
*/
void MaximGameSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	//Update the speed.
	_Speed += deltaTime * 0.01f;

	//Update the spawn time.
	_SpawnTime -= deltaTime * 0.01f;

	//Update the spawn timer.
	_SpawnTimer += deltaTime;

	while (_SpawnTimer >= _SpawnTime)
	{
		MaximObject *const RESTRICT newEnemy{ new MaximObject };

		newEnemy->Initialize(_Speed);

		_Enemies.EmplaceSlow(newEnemy);

		UpdateSystem::Instance->RegisterAsynchronousPreUpdateFunction([](const UpdateFunctionContext *const RESTRICT context)
		{
			static_cast<MaximObject *const RESTRICT>(context->_Arguments)->PreUpdate(context->_Context);
		}, newEnemy);

		_SpawnTimer -= _SpawnTime;
	}
}

/*
*	Releases the Maxim game system.
*/
void MaximGameSystem::ReleaseSystem() NOEXCEPT
{

}