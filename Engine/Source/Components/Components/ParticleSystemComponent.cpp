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
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

DEFINE_COMPONENT(ParticleSystemComponent, ParticleSystemSharedData, ParticleSystemInitializationData, ParticleSystemInstanceData);

/*
*	Initializes this component.
*/
void ParticleSystemComponent::Initialize() NOEXCEPT
{
	
}

/*
*	Post-initializes this component.
*/
void ParticleSystemComponent::PostInitialize() NOEXCEPT
{
	//Register the storage buffer.
	RenderingSystem::Instance->GetBufferManager()->RegisterStorageBuffer
	(
		HashString("Particles"),
		sizeof(ParticlePackedInstance) * 4'096,
		[](DynamicArray<byte> *const RESTRICT data, void *const RESTRICT arguments)
		{
			ParticleSystemComponent *const RESTRICT particle_system_component{ static_cast<ParticleSystemComponent *const RESTRICT>(arguments) };

			if (particle_system_component->_SharedData._PackedInstances.Empty())
			{
				return;
			}

			data->Resize<false>(sizeof(ParticlePackedInstance) * particle_system_component->_SharedData._PackedInstances.Size());
			Memory::Copy(data->Data(), particle_system_component->_SharedData._PackedInstances.Data(), sizeof(ParticlePackedInstance) * particle_system_component->_SharedData._PackedInstances.Size());
		},
		this
	);
}

/*
*	Terminates this component.
*/
void ParticleSystemComponent::Terminate() NOEXCEPT
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
void ParticleSystemComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Define constants.
	constexpr uint32 MAXIMUM_SPAWN_RATE_PER_SUB_EMITTER{ 512 };

	//Set up the instance data.
	ParticleSystemInitializationData *const RESTRICT _initialization_data{ static_cast<ParticleSystemInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	ParticleSystemInstanceData &instance_data{ _InstanceData.Back() };

	//Add the sub emitters.
	const uint32 number_of_sub_emitters{ CatalystBaseMath::Maximum<uint32>(_initialization_data->_Emitter._SpawnRate / MAXIMUM_SPAWN_RATE_PER_SUB_EMITTER, 1) };
	
	for (uint32 sub_emitter_index{ 0 }; sub_emitter_index < number_of_sub_emitters; ++sub_emitter_index)
	{
		//Add the new sub emitter.
		instance_data._SubEmitters.Emplace();
		ParticleSubEmitter &new_sub_emitter{ instance_data._SubEmitters.Back() };

		//Copy data.
		Memory::Copy(&new_sub_emitter._Emitter, &_initialization_data->_Emitter, sizeof(ParticleEmitter));

		//Modify the spawn rate.
		new_sub_emitter._Emitter._SpawnRate = CatalystBaseMath::Maximum<uint32>(_initialization_data->_Emitter._SpawnRate / number_of_sub_emitters, 1);

		//Reset the time since the last particle spawn.
		new_sub_emitter._TimeSinceLastParticleSpawn = 0.0f;
	}

	//Free the initialization data.
	FreeInitializationData(_initialization_data);
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void ParticleSystemComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(WorldTransformComponent::Instance->Has(entity), "Particle system component needs a world transform component!");
}

/*
*	Destroys an instance.
*/
void ParticleSystemComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 ParticleSystemComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return _InstanceData[instance_index]._SubEmitters.Size();
}

void ParticleSystemComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = UpdatePhase::PRE_RENDER;
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::SUB_INSTANCE;
}

/*
*	Runs before the given update phase.
*/
void ParticleSystemComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

/*
*	Updates this component.
*/
void ParticleSystemComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	PROFILING_SCOPE("ParticleSystemComponent::Update");

	switch (update_phase)
	{
		case UpdatePhase::PRE_RENDER:
		{
			//Cache data that will be used.
			const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };
			const Vector3<float32> camera_forward_vector{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetForwardVector() };

			//Cache the delta time.
			const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

			//Iterate over the instances.
			for (uint64 instance_index{ start_instance_index }; instance_index < end_instance_index; ++instance_index)
			{
				//Cache the instance data.
				Entity *const RESTRICT entity{ InstanceToEntity(instance_index) };
				ParticleSystemInstanceData &particle_system_instance_data{ _InstanceData[instance_index] };
				const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };
				
				//Cache the sub emitter.
				ParticleSubEmitter &sub_emitter{ particle_system_instance_data._SubEmitters[sub_instance_index] };

				//Check spawning of new particles.
				{
					PROFILING_SCOPE("Spawn New Particles");

					//Update the time since the last particle spawn.
					sub_emitter._TimeSinceLastParticleSpawn += delta_time;

					//Spawn new particles.
					const float32 spawn_rate_reciprocal{ 1.0f / static_cast<float32>(sub_emitter._Emitter._SpawnRate) };

					while (sub_emitter._TimeSinceLastParticleSpawn >= spawn_rate_reciprocal)
					{
						//Create the new particle instance.
						sub_emitter._Instances.Emplace();
						ParticleInstance &new_particle_instance{ sub_emitter._Instances.Back() };

						//Randomize the world position.
						switch (sub_emitter._Emitter._EmitMode)
						{
							case ParticleEmitter::EmitMode::SPHERE:
							{
								new_particle_instance._WorldPosition = WorldPosition
								(
									world_transform_instance_data._CurrentWorldTransform.GetCell(),
									world_transform_instance_data._CurrentWorldTransform.GetLocalPosition() + CatalystRandomMath::RandomPointInSphere(sub_emitter._Emitter._SphereMode._Radius)
								);

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}

						//Randomize the velocity.
						new_particle_instance._Velocity = Vector3<float32>
						(
							CatalystRandomMath::RandomFloatInRange(sub_emitter._Emitter._MinimumVelocity._X, sub_emitter._Emitter._MaximumVelocity._X),
							CatalystRandomMath::RandomFloatInRange(sub_emitter._Emitter._MinimumVelocity._Y, sub_emitter._Emitter._MaximumVelocity._Y),
							CatalystRandomMath::RandomFloatInRange(sub_emitter._Emitter._MinimumVelocity._Z, sub_emitter._Emitter._MaximumVelocity._Z)
						);

						//Randomize the size.
						new_particle_instance._Size = CatalystBaseMath::LinearlyInterpolate(sub_emitter._Emitter._MinimumSize, sub_emitter._Emitter._MaximumSize, CatalystRandomMath::RandomFloat());

						//Set the age.
						new_particle_instance._Age = 0.0f;

						//Randomize the lifetime.
						new_particle_instance._Lifetime = CatalystRandomMath::RandomFloatInRange(sub_emitter._Emitter._MinimumLifetime, sub_emitter._Emitter._MaximumLifetime);
					
						//Update the time since the last particle spawn.
						sub_emitter._TimeSinceLastParticleSpawn -= spawn_rate_reciprocal;
					}
				}

				//Update all instances.
				sub_emitter._PackedInstances.Clear();

				{
					PROFILING_SCOPE("Simulate Particles");

					for (uint64 _instance_index{ 0 }; _instance_index < sub_emitter._Instances.Size();)
					{
						ParticleInstance &instance{ sub_emitter._Instances[_instance_index] };

						//Update the age.
						instance._Age += delta_time;

						if (instance._Age < instance._Lifetime)
						{
							//Update the position based on the velocity.
							{
								Vector3<float32> local_position{ instance._WorldPosition.GetLocalPosition() };

								local_position += instance._Velocity * delta_time;

								instance._WorldPosition.SetLocalPosition(local_position);
							}

							//Calculate the camera relative position.
							const Vector3<float32> camera_relative_position{ instance._WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()) };

							//Calculate modifiers.
							float32 size_modifier{ 1.0f };

#if 1
							//Apply wind.
							{
								Vector3<float32> local_position{ instance._WorldPosition.GetLocalPosition() };

								local_position += WorldSystem::Instance->GetWindSystem()->GetWindDirection() * WorldSystem::Instance->GetWindSystem()->GetWindSpeed() * 0.5f * delta_time;

								instance._WorldPosition.SetLocalPosition(local_position);
							}
#endif

							//Modify the size based on the distance to the camera.
							{
								const float32 distance_to_camera{ Vector3<float32>::Length(camera_relative_position - camera_local_position) };

								size_modifier = CatalystBaseMath::Minimum<float32>(distance_to_camera / 16.0f, 1.0f);
							}

							//Do _very_ simple culling.
							if (Vector3<float32>::DotProduct(camera_forward_vector, camera_relative_position - camera_local_position) > 0.0f)
							{
								sub_emitter._PackedInstances.Emplace();
								ParticlePackedInstance& packed_instance{ sub_emitter._PackedInstances.Back() };

								packed_instance._WorldPosition = camera_relative_position;
								packed_instance._Size = instance._Size * size_modifier;
								packed_instance._NormalizedAge = instance._Age / instance._Lifetime;
							}

							++_instance_index;
						}

						else
						{
							PROFILING_SCOPE("Remove Particle");

							sub_emitter._Instances.EraseAt<false>(_instance_index);
						}
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

/*
*	Runs after the given update phase.
*/
void ParticleSystemComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{
	//Gather all packed instances.
	_SharedData._PackedInstances.Clear();

	//Calculate the number of shared instances.
	uint64 number_of_packed_instances{ 0 };

	for (const ParticleSystemInstanceData &instance_data : InstanceData())
	{
		for (const ParticleSubEmitter &sub_emitter : instance_data._SubEmitters)
		{
			number_of_packed_instances += sub_emitter._PackedInstances.Size();
		}
	}

	//Don't do anything if there's nothing to do. (:
	if (number_of_packed_instances == 0)
	{
		return;
	}

	//Resize to fit the number of packed instances.
	_SharedData._PackedInstances.Resize<false>(number_of_packed_instances);

	//Copy over the packed instances.
	uint32 instance_counter{ 0 };

	for (ParticleSystemInstanceData &instance_data : InstanceData())
	{
		instance_data._StartInstanceIndex = static_cast<uint32>(instance_counter);

		for (const ParticleSubEmitter &sub_emitter : instance_data._SubEmitters)
		{
			if (!sub_emitter._PackedInstances.Empty())
			{
				Memory::Copy(&_SharedData._PackedInstances[instance_counter], sub_emitter._PackedInstances.Data(), sizeof(ParticlePackedInstance) * sub_emitter._PackedInstances.Size());
			
				instance_counter += static_cast<uint32>(sub_emitter._PackedInstances.Size());
			}
		}

		instance_data._NumberOfInstances = instance_counter - instance_data._StartInstanceIndex;
	}
}