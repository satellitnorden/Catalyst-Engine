//Header file.
#include <Systems/WorldSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/DynamicModelInitializationData.h>
#include <Entities/Types/DynamicModelEntity.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Resources.
#include <Resources/Core/LevelResource.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/ResourceSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WorldSystem);

/*
*	Initializes the world system.
*/
void WorldSystem::Initialize(const CatalystProjectWorldConfiguration &configuration) NOEXCEPT
{
	//Set the world grid size.
	_WorldGridSize = configuration._WorldGridSize;

	//Register the updates.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<WorldSystem *const RESTRICT>(arguments)->InputUpdate();
	},
	this,
	UpdatePhase::INPUT,
	UpdatePhase::RENDER,
	false);

	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<WorldSystem *const RESTRICT>(arguments)->LogicUpdate();
	},
	this,
	UpdatePhase::LOGIC,
	UpdatePhase::PHYSICS,
	false);
}

/*
*	Post-initializes the world system.
*/
void WorldSystem::PostInitialize() NOEXCEPT
{
	//Post-initialize the sky system.
	_SkySystem.PostInitialize();
}

/*
*	Returns the current world grid cell.
*/
NO_DISCARD const Vector3<int32> &WorldSystem::GetCurrentWorldGridCell() const NOEXCEPT
{
	//This should probably be cached somehow, but let's just ask the Perceiver for now. (:
	return Perceiver::Instance->GetWorldTransform().GetCell();
}

/*
*	Updates the world system during the input update phase.
*/
void WorldSystem::InputUpdate() NOEXCEPT
{
	//Update all particle systems.
	UpdateParticleSystems();
}

/*
*	Updates the world system during the logic update phase.
*/
void WorldSystem::LogicUpdate() NOEXCEPT
{
	//Update all distance triggers.
	UpdateDistanceTriggers();
}

/*
*	Updates all particle systems.
*/
void WorldSystem::UpdateParticleSystems() NOEXCEPT
{
	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Update all particle systems.
	const uint64 number_of_particle_system_components{ ComponentManager::GetNumberOfParticleSystemComponents() };
	ParticleSystemComponent *RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemComponents() };
	ParticleSystemRenderComponent *RESTRICT render_component{ ComponentManager::GetParticleSystemParticleSystemRenderComponents() };

	for (uint64 i{ 0 }; i < number_of_particle_system_components; ++i, ++component, ++render_component)
	{
		//Update the time since the last particle spawned.
		component->_TimeSinceLastSpawn += delta_time;

		//Update the first particle index to spawn.
		component->_FirstParticleIndexToSpawn = component->_FirstParticleIndexToSpawn + component->_NumberOfParticlesToSpawn;

		//Wrap around.
		if (TEST_BIT(component->_ParticleSystemProperties, ParticleSystemEntity::Property::LOOPING))
		{
			while (component->_FirstParticleIndexToSpawn >= component->_NumberOfInstances)
			{
				component->_FirstParticleIndexToSpawn -= component->_NumberOfInstances;
			}
		}

		//Calculate the number of particles to spawn this update.
		component->_NumberOfParticlesToSpawn = 0;

		const float32 spawn_frequency_reciprocal{ 1.0f / component->_SpawnFrequency };

		while (component->_TimeSinceLastSpawn >= spawn_frequency_reciprocal)
		{
			component->_TimeSinceLastSpawn -= spawn_frequency_reciprocal;

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
	const Vector3<float> perceiver_position{ Perceiver::Instance->GetWorldTransform().GetAbsolutePosition() };

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