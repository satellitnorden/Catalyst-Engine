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
		//Update the time since the last particle spawned.
		component->_TimeSinceLastSpawn += context->_DeltaTime;

		//Calculate the number of particles to spawn this update.
		uint64 particles_to_spawn{ 0 };

		while (component->_TimeSinceLastSpawn >= component->_SpawnFrequency)
		{
			component->_TimeSinceLastSpawn -= component->_SpawnFrequency;

			++particles_to_spawn;
		}

		//Update the number of transformations. Will be recalculated when updating the instances.
		render_component->_NumberOfTransformations = 0;

		//Update all instances.
		for (ParticleInstanceData& instance_data : component->_InstanceData)
		{
			//Update the time of the instance.
			instance_data._Time += context->_DeltaTime;

			//Has the time exceeded the lifetime of particles?
			if (instance_data._Time >= component->_Lifetime)
			{
				//Should a new particle be spawned here?
				if (particles_to_spawn > 0)
				{
					--particles_to_spawn;

					instance_data._Position = Vector3<float>(	CatalystRandomMath::RandomFloatInRange(component->_MinimumPosition._X, component->_MaximumPosition._X),
																CatalystRandomMath::RandomFloatInRange(component->_MinimumPosition._Y, component->_MaximumPosition._Y),
																CatalystRandomMath::RandomFloatInRange(component->_MinimumPosition._Z, component->_MaximumPosition._Z));

					instance_data._Velocity = Vector3<float>(	CatalystRandomMath::RandomFloatInRange(component->_MinimumVelocity._X, component->_MaximumVelocity._X),
																CatalystRandomMath::RandomFloatInRange(component->_MinimumVelocity._Y, component->_MaximumVelocity._Y),
																CatalystRandomMath::RandomFloatInRange(component->_MinimumVelocity._Z, component->_MaximumVelocity._Z));

					instance_data._Scale = Vector2<float>(	CatalystRandomMath::RandomFloatInRange(component->_MinimumScale._X, component->_MaximumScale._X),
															CatalystRandomMath::RandomFloatInRange(component->_MinimumScale._Y, component->_MaximumScale._Y));

					instance_data._Time = 0.0f;

					++render_component->_NumberOfTransformations;
				}
			}

			//Otherwise, just update the particle.
			else
			{
				instance_data._Position += instance_data._Velocity * context->_DeltaTime;

				++render_component->_NumberOfTransformations;
			}
		}

		//Update the transformations buffer.
		const void* RESTRICT data[]{ component->_InstanceData.Data() };
		const uint64 data_sizes[]{ sizeof(ParticleInstanceData) * component->_InstanceData.Size() };
		RenderingSystem::Instance->UploadDataToBuffer(data, data_sizes, 1, &render_component->_TransformationsBuffer);
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