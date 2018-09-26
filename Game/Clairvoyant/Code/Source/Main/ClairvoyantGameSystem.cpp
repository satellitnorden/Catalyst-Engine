//Header file.
#include <Main/ClairvoyantGameSystem.h>

//Clairvoyant.
#include <Main/WorldArchitect.h>
#include <Player/ClairvoyantPlayer.h>

//Entities.
#include <Entities/ParticleSystemEntity.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantGameSystem);

/*
*	Initializes the Clairvoyant game system.
*/
void ClairvoyantGameSystem::InitializeSystem() NOEXCEPT
{
	//Load the resource collection.
	ResourceLoader::LoadResourceCollection("..\\..\\..\\Resources\\Final\\ClairvoyantResourceCollection.crc");

	//Set the environment materials/blend.
	EnvironmentManager::Instance->SetNightEnvironmentMaterial(RenderingSystem::Instance->GetCommonEnvironmentMaterial(RenderingSystem::CommonEnvironmentMaterial::Night));
	EnvironmentManager::Instance->SetDayEnvironmentMaterial(RenderingSystem::Instance->GetCommonEnvironmentMaterial(RenderingSystem::CommonEnvironmentMaterial::Day));
	EnvironmentManager::Instance->SetEnvironmentBlend(1.0f);
#if defined(CATALYST_ENABLE_OCEAN)
	EnvironmentManager::Instance->SetOceanMaterial(RenderingSystem::Instance->GetCommonOceanMaterial(RenderingSystem::CommonOceanMaterial::Ocean));
#endif

	//Create the sun.
	_Sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	_Sun->Rotate(Vector3(-25.0f, 0.0f, 0.0f));
	_Sun->SetIntensity(100.0f);

	//Create some particles.
	ParticleSystemInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<ParticleSystemInitializationData>() };

	data->_Properties = EntityInitializationData::EntityProperty::None;
	data->_Material = RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle);
	data->_ParticleSystemProperties._Properties = static_cast<uint32>(ParticleSystemProperties::ParticleSystemProperty::AffectedByWind) | static_cast<uint32>(ParticleSystemProperties::ParticleSystemProperty::Looping);
	data->_ParticleSystemProperties._FadeTime = 10.0f;
	data->_ParticleSystemProperties._Lifetime = 60.0f;
	data->_ParticleSystemProperties._SpawnFrequency = 0.01f;
	data->_ParticleSystemProperties._MinimumScale = Vector2(0.01f, 0.01f);
	data->_ParticleSystemProperties._MaximumScale = Vector2(0.02f, 0.02f);
	data->_ParticleSystemProperties._MinimumPosition = Vector3(-1'000.0f, 0.0f, -1'000.0f);
	data->_ParticleSystemProperties._MaximumPosition = Vector3(1'000.0f, 1'000.0f, 1'000.0f);
	data->_ParticleSystemProperties._MinimumVelocity = Vector3(-0.1f, -0.1f, -0.1f);
	data->_ParticleSystemProperties._MaximumVelocity = Vector3(0.1f, 0.1f, 0.1f);
	data->_Position = 0.0f;

	ParticleSystemEntity *const RESTRICT particles = EntitySystem::Instance->CreateEntity<ParticleSystemEntity>();

	EntitySystem::Instance->InitializeEntity(particles, data);

	//Initialize the world architects.
	WorldArchitect::Instance->Initialize();

	//Initialize the player.
	ClairvoyantPlayer::Instance->Initialize();
}