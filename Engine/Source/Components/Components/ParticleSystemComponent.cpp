//Header file.
#include <Components/Components/ParticleSystemComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/WorldSystem.h>

DEFINE_COMPONENT(ParticleSystemComponent, ParticleSystemSharedData, ParticleSystemInitializationData, ParticleSystemInstanceData);

/*
*	Initializes this component.
*/
void ParticleSystemComponent::Initialize() NOEXCEPT
{

}

NO_DISCARD bool ParticleSystemComponent::NeedsPreProcessing() const NOEXCEPT
{
	return false;
}

/*
*	Preprocessed initialization data an instance.
*/
void ParticleSystemComponent::PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{

}

/*
*	Creates an instance.
*/
void ParticleSystemComponent::CreateInstance(const EntityIdentifier entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	ParticleSystemInitializationData *const RESTRICT _initialization_data{ static_cast<ParticleSystemInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	ParticleSystemInstanceData &instance_data{ _InstanceData.Back() };

	//Copy data.
	instance_data._MinimumSize = _initialization_data->_MinimumSize;
	instance_data._MaximumSize = _initialization_data->_MaximumSize;
	instance_data._MinimumLifetime = _initialization_data->_MinimumLifetime;
	instance_data._MaximumLifetime = _initialization_data->_MaximumLifetime;
	instance_data._SpawnRate = _initialization_data->_SpawnRate;

	//Reset the time since the last particle spawn.
	instance_data._TimeSinceLastParticleSpawn = 0.0f;

	//Free the initialization data.
	FreeInitializationData(_initialization_data);
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void ParticleSystemComponent::PostCreateInstance(const EntityIdentifier entity) NOEXCEPT
{
	ASSERT(WorldTransformComponent::Instance->Has(entity), "Particle system component needs a world transform component!");
}

/*
*	Destroys an instance.
*/
void ParticleSystemComponent::DestroyInstance(const EntityIdentifier entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

void ParticleSystemComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = UpdatePhase::PRE_RENDER;
	update_configuration->_BatchSize = 1;
}

void ParticleSystemComponent::Update(const UpdatePhase update_phase, const uint64 start_index, const uint64 end_index) NOEXCEPT
{
	PROFILING_SCOPE(ParticleSystemComponent::Update);

	switch (update_phase)
	{
		case UpdatePhase::PRE_RENDER:
		{
			//Cache the delta time.
			const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

			//Iterate over the instances.
			for (uint64 instance_index{ start_index }; instance_index < end_index; ++instance_index)
			{
				//Cache the instance data.
				const EntityIdentifier entity{ InstanceToEntity(instance_index) };
				ParticleSystemInstanceData &particle_system_instance_data{ _InstanceData[instance_index] };
				const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

				//Check spawning of new particles.
				{
					//Update the time since the last particle spawn.
					particle_system_instance_data._TimeSinceLastParticleSpawn += delta_time;

					//Spawn new particles.
					while (particle_system_instance_data._TimeSinceLastParticleSpawn >= particle_system_instance_data._SpawnRate)
					{
						//Create the new particle instance.
						particle_system_instance_data._Instances.Emplace();
						ParticleInstance &new_particle_instance{ particle_system_instance_data._Instances.Back() };

						//Randomize the world position.
						new_particle_instance._WorldPosition = WorldPosition
						(
							world_transform_instance_data._CurrentWorldTransform.GetCell(),
							world_transform_instance_data._CurrentWorldTransform.GetLocalPosition() + CatalystRandomMath::RandomPointInSphere(4.0f)
						);

						//Randomize the size.
						new_particle_instance._Size = Vector2<float32>
						(
							CatalystRandomMath::RandomFloatInRange(particle_system_instance_data._MinimumSize._X, particle_system_instance_data._MaximumSize._X),
							CatalystRandomMath::RandomFloatInRange(particle_system_instance_data._MinimumSize._Y, particle_system_instance_data._MaximumSize._Y)
						);

						//Set the age.
						new_particle_instance._Age = 0.0f;

						//Randomize the lifetime.
						new_particle_instance._Lifetime = CatalystRandomMath::RandomFloatInRange(particle_system_instance_data._MinimumLifetime, particle_system_instance_data._MaximumLifetime);
					}
				}

				//Update all instances.
				particle_system_instance_data._PackedInstances.Clear();

				for (uint64 _instance_index{ 0 }; _instance_index < particle_system_instance_data._Instances.Size();)
				{
					ParticleInstance &instance{ particle_system_instance_data._Instances[_instance_index] };

					//Update the age.
					instance._Age += delta_time;

					if (instance._Age < instance._Lifetime)
					{
						particle_system_instance_data._PackedInstances.Emplace();
						ParticlePackedInstance &packed_instance{ particle_system_instance_data._PackedInstances.Back() };

						packed_instance._WorldPosition = instance._WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell());
						packed_instance._Size = instance._Size;
						packed_instance._NormalizedAge = instance._Age / instance._Lifetime;

						++_instance_index;
					}

					else
					{
						particle_system_instance_data._Instances.EraseAt<false>(_instance_index);
					}
				}
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