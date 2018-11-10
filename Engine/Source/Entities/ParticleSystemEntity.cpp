//Header file.
#include <Entities/ParticleSystemEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/EngineSystem.h>
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
*	Returns whether or not this entity should automatically terminate.
*/
bool ParticleSystemEntity::ShouldAutomaticallyTerminate() const NOEXCEPT
{
	//Return whether or not this entity should automatically terminate.
	const ParticleSystemProperties &properties{ ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._Properties };
	const bool isLooping{ TEST_BIT(properties._Properties, ParticleSystemProperty::Looping) };

	if (isLooping)
	{
		return false;
	}

	else
	{
		const float startingTime{ ComponentManager::GetParticleSystemParticleSystemRenderComponents()[_ComponentsIndex]._ParticleSystemStartingTime };
		const float totalTime{ EngineSystem::Instance->GetTotalTime() };
		const float elapsedTime{ totalTime - startingTime };

		return elapsedTime >= properties._Lifetime - properties._SpawnFrequency + properties._Lifetime;
	}
}

/*
*	Returns the position of this entity.
*/
RESTRICTED NO_DISCARD Vector3 *const RESTRICT ParticleSystemEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetParticleSystemParticleSystemRenderComponents()[_ComponentsIndex]._WorldPosition;
}