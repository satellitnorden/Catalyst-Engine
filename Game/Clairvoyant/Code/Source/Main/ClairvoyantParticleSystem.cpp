//Header file.
#include <Main/ClairvoyantParticleSystem.h>

//Core.
#include <Core/HashString.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/ParticleSystemEntity.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantParticleSystem);

/*
*	Initializes the particle system.
*/
void ClairvoyantParticleSystem::InitializeSystem() NOEXCEPT
{
	CameraEntity *const RESTRICT camera{ RenderingSystem::Instance->GetActiveCamera() };

	ParticleSystemEntity *const RESTRICT dustParticles{ EntitySystem::Instance->CreateChildEntity<ParticleSystemEntity>(camera) };
	dustParticles->Initialize(RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle), ParticleSystemProperties(5.0f, 60.0f, 0.0f, Vector2(0.05f, 0.05f), Vector2(0.2f, 0.2f), Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f), Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)));
}