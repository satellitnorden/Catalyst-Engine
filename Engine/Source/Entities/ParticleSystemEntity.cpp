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
	//Set the entity type.
	_Type = EntityType::ParticleSystem;
}

/*
*	Returns the position of this entity.
*/
RESTRICTED NO_DISCARD Vector3 *const RESTRICT ParticleSystemEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetParticleSystemParticleSystemRenderComponents()[_ComponentsIndex]._WorldPosition;
}