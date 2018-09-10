//Header file.
#include <Main/MaximGameSystem.h>

//Entities.
#include <Entities/StaticPhysicalEntity.h>

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
	StaticArray<Vector3, 2> positions
	{
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f, 25.0f, 0.0f)
	};

	for (uint64 i = 0, size = _Particles.Size(); i < size; ++i)
	{
		_Particles[i] = EntitySystem::Instance->CreateEntity<ParticleSystemEntity>();
		_Particles[i]->Initialize(	RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle),
								ParticleSystemProperties(	10.0f,
															60.0f,
															0.1f,
															Vector2(0.01f, 0.01f),
															Vector2(0.02f, 0.02f),
															Vector3(-5.0f, -12.5f, -5.0f),
															Vector3(5.0f, 12.5f, 5.0f),
															Vector3(-0.1f, -0.1f, -0.1f),
															Vector3(0.1f, 0.1f, 0.1f)),
															positions[i]);
	}

	//Create the sun.
	_Sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	_Sun->SetIntensity(1.0f);
	_Sun->Rotate(Vector3(-22.5f, 170.0f, 0.0f));
	_Sun->SetColor(Vector3(0.0f, 1.0f, 1.0f));
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

	//Spawn new objects, if necessary.
	while (_SpawnTimer >= _SpawnTime)
	{
		_Enemies.EmplaceSlow();

		_Enemies.Back().Initialize(_Speed);

		UpdateSystem::Instance->RegisterAsynchronousPreUpdate(&_Enemies.Back());

		_SpawnTimer -= _SpawnTime;
	}

	//Update the positions of the particles.
	for (ParticleSystemEntity *const RESTRICT particles : _Particles)
	{
		particles->Move(Vector3(0.0f, -_Speed * deltaTime, 0.0f));

		while (particles->GetPosition()._Y <= -25.0f)
		{
			particles->Move(Vector3(0.0f, 50.0f, 0.0f));
		}
	}
}

/*
*	Destroys a Maxim object.
*/
void MaximGameSystem::DestroyMaximObject(MaximObject *const RESTRICT object) NOEXCEPT
{
	//Terminate the entity.
	EntitySystem::Instance->RequesTermination(object->GetEntity(), false);

	//Destroy the entity.
	EntitySystem::Instance->RequestDestruction(object->GetEntity(), false);

	//Remove this Maxim object from the internal list.
	_Enemies.Erase(*object);
}