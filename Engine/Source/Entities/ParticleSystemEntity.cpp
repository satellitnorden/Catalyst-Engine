//Header file.
#include <Entities/ParticleSystemEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
ParticleSystemEntity::ParticleSystemEntity() NOEXCEPT
{
	//Get a new components index.
	_ComponentsIndex = ComponentManager::GetNewParticleSystemComponentsIndex(this);
}

/*
*	Initializes this particle system entity.
*/
void ParticleSystemEntity::Initialize(const ParticleMaterial &material, const ParticleSystemProperties &properties) NOEXCEPT
{
	//Initialize this particle system entity via the rendering system.
	RenderingSystem::Instance->InitializeParticleSystemEntity(*this, material, properties);
}

/*
*	Returns the position of this entity.
*/
NO_DISCARD RESTRICTED Vector3 *const RESTRICT ParticleSystemEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._Properties.worldPosition;
}