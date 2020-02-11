//Header file.
#include <Systems/WorldSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Singleton definition.
DEFINE_SINGLETON(WorldSystem);

/*
*	Post-initializes the world system.
*/
void WorldSystem::PostInitialize() NOEXCEPT
{
	//Post-initialize the sky system.
	_SkySystem.PostInitialize();
}

/*
*	Updates the world system during the pre update phase.
*/
void WorldSystem::PreUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//Update all particle systems.
	UpdateParticleSystems(context);
}

/*
*	Updates the world system during the logic update phase.
*/
void WorldSystem::LogicUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//Update all distance triggers.
	UpdateDistanceTriggers();
}

/*
*	Updates all particle systems.
*/
void WorldSystem::UpdateParticleSystems(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//Update all particle systems.
	const uint64 number_of_particle_system_components{ ComponentManager::GetNumberOfParticleSystemComponents() };
	ParticleSystemComponent *RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemComponents() };
	ParticleSystemRenderComponent *RESTRICT render_component{ ComponentManager::GetParticleSystemParticleSystemRenderComponents() };

	for (uint64 i{ 0 }; i < number_of_particle_system_components; ++i, ++component, ++render_component)
	{
		//Calculate how many particle to spawn per second.
		const float particles_to_spawn_per_second{ component->_Lifetime / static_cast<float>(component->_NumberOfInstances) };

		//Update the time since the last particle spawned.
		component->_TimeSinceLastSpawn += context->_DeltaTime * component->_SpawnFrequency;

		//Update the first particle index to spawn.
		component->_FirstParticleIndexToSpawn = component->_FirstParticleIndexToSpawn + component->_NumberOfParticlesToSpawn;

		//Wrap around.
		while (component->_FirstParticleIndexToSpawn >= component->_NumberOfInstances)
		{
			component->_FirstParticleIndexToSpawn -= component->_NumberOfInstances;
		}

		//Calculate the number of particles to spawn this update.
		component->_NumberOfParticlesToSpawn = 0;

		while (component->_TimeSinceLastSpawn >= particles_to_spawn_per_second)
		{
			component->_TimeSinceLastSpawn -= particles_to_spawn_per_second;

			++component->_NumberOfParticlesToSpawn;
		}
	}
}

/*
*	Updates all distance triggers.
*/
void WorldSystem::UpdateDistanceTriggers() NOEXCEPT
{
	//Cache the perceiver position.
	const Vector3<float> perceiver_position{ Perceiver::Instance->GetPosition() };

	//Update all distance triggers.
	const uint64 number_of_distance_trigger_components{ ComponentManager::GetNumberOfDistanceTriggerComponents() };
	DistanceTriggerComponent* RESTRICT distance_trigger_component{ ComponentManager::GetDistanceTriggerDistanceTriggerComponents() };

	for (uint64 i{ 0 }; i < number_of_distance_trigger_components; ++i, ++distance_trigger_component)
	{
		//Calculate the distance, squared, to the perceiver.
		const float distance_squared{ Vector3<float>::LengthSquared(perceiver_position - distance_trigger_component->_Position) };

		//Modify the state as needed.
		switch (distance_trigger_component->_CurrentState)
		{
			case DistanceTriggerEntity::State::UNENTERED:
			{
				if (distance_squared <= distance_trigger_component->_TriggerDistanceSquared)
				{
					distance_trigger_component->_EnterFunction();

					distance_trigger_component->_CurrentState = DistanceTriggerEntity::State::ENTERED;
				}

				break;
			}

			case DistanceTriggerEntity::State::ENTERED:
			{
				if (distance_squared > distance_trigger_component->_TriggerDistanceSquared)
				{
					distance_trigger_component->_ExitFunction();

					distance_trigger_component->_CurrentState = DistanceTriggerEntity::State::UNENTERED;
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}
}