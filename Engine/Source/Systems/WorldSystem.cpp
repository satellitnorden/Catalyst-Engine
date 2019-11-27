//Header file.
#include <Systems/WorldSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/Core/ComponentManager.h>

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
		//Update the transformations buffer.
		const void* RESTRICT data[]{ component->_InstanceData.Data() };
		const uint64 data_sizes[]{ sizeof(ParticleInstanceData) * component->_InstanceData.Size() };
		RenderingSystem::Instance->UploadDataToBuffer(data, data_sizes, 1, &render_component->_TransformationsBuffer);

		//Update the number of transformations.
		render_component->_NumberOfTransformations = 0;
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