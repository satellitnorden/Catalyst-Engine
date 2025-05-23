//Header file.
#include <Components/Components/ParticleSystemComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#if defined(CATALYST_EDITOR)
	#include <Systems/CatalystEditorSystem.h>
#endif
#include <Systems/CatalystEngineSystem.h>
#include <Systems/ContentSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Particles push constant data.
*/
class ParticlesPushConstantData final
{

public:

	//The material index.
	uint32 _MaterialIndex;

	//The start index.
	uint32 _StartIndex;

};

/*
*	Gathers a particles input stream.
*/
void GatherParticlesInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Gather particles.
	{
		for (ParticleSystemInstanceData &instance_data : ParticleSystemComponent::Instance->InstanceData())
		{
			if (instance_data._NumberOfInstances == 0)
			{
				continue;
			}

			//Add a new entry.
			input_stream->_Entries.Emplace();
			RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

			new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
			new_entry._VertexBuffer = EMPTY_HANDLE;
			new_entry._IndexBuffer = EMPTY_HANDLE;
			new_entry._IndexBufferOffset = 0;
			new_entry._InstanceBuffer = EMPTY_HANDLE;
			new_entry._VertexCount = 4;
			new_entry._IndexCount = 0;
			new_entry._InstanceCount = instance_data._NumberOfInstances;

			//Set up the push constant data.
			ParticlesPushConstantData push_constant_data;

			push_constant_data._MaterialIndex = instance_data._Material->_Index;
			push_constant_data._StartIndex = instance_data._StartInstanceIndex;

			for (uint64 i{ 0 }; i < sizeof(ParticlesPushConstantData); ++i)
			{
				input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
			}
		}
	}
}

/*
*	Splits one emitter into X sub emitters.
*/
FORCE_INLINE void SplitIntoSubEmitters(const ParticleEmitter &master_emitter, DynamicArray<ParticleSubEmitter> *const RESTRICT sub_emitters) NOEXCEPT
{
	//Define constants.
	constexpr uint32 MAXIMUM_BURST_PER_SUB_EMITTER{ 64 };
	constexpr uint32 MAXIMUM_SPAWN_RATE_PER_SUB_EMITTER{ 64 };

	//Clear the sub emitters.
	sub_emitters->Clear();

	//Calculate the number of sub emitters.
	const uint32 number_of_burst_sub_emitters{ master_emitter._InitialBurst > 0 ? BaseMath::Maximum<uint32>(master_emitter._InitialBurst / MAXIMUM_BURST_PER_SUB_EMITTER, 1) : 0 };
	const uint32 number_of_spawn_rate_sub_emitters{ master_emitter._SpawnRate > 0 ? BaseMath::Maximum<uint32>(master_emitter._SpawnRate / MAXIMUM_SPAWN_RATE_PER_SUB_EMITTER, 1) : 0 };

	//Add the burst sub emitters.
	for (uint32 sub_emitter_index{ 0 }; sub_emitter_index < number_of_burst_sub_emitters; ++sub_emitter_index)
	{
		//Add the new sub emitter.
		sub_emitters->Emplace();
		ParticleSubEmitter &new_sub_emitter{ sub_emitters->Back() };

		//Copy data.
		Memory::Copy(&new_sub_emitter._Emitter, &master_emitter, sizeof(ParticleEmitter));

		//Modify the initial burst/spawn rate.
		new_sub_emitter._Emitter._InitialBurst = BaseMath::Maximum<uint32>(master_emitter._InitialBurst / number_of_burst_sub_emitters, 1);
		new_sub_emitter._Emitter._SpawnRate = 0;

		//Reset the time since the last particle spawn.
		new_sub_emitter._TimeSinceLastParticleSpawn = 0.0f;

		//Reset if the initial burst has been done.
		new_sub_emitter._HasDoneInitialBurst = false;
	}

	//Add the spawn rate sub emitters.
	for (uint32 sub_emitter_index{ 0 }; sub_emitter_index < number_of_spawn_rate_sub_emitters; ++sub_emitter_index)
	{
		//Add the new sub emitter.
		sub_emitters->Emplace();
		ParticleSubEmitter &new_sub_emitter{ sub_emitters->Back() };

		//Copy data.
		Memory::Copy(&new_sub_emitter._Emitter, &master_emitter, sizeof(ParticleEmitter));

		//Modify the initial burst/spawn rate.
		new_sub_emitter._Emitter._InitialBurst = 0;
		new_sub_emitter._Emitter._SpawnRate = BaseMath::Maximum<uint32>(master_emitter._SpawnRate / number_of_spawn_rate_sub_emitters, 1);

		//Reset the time since the last particle spawn.
		new_sub_emitter._TimeSinceLastParticleSpawn = 0.0f;

		//Reset if the initial burst has been done.
		new_sub_emitter._HasDoneInitialBurst = false;
	}
}

/*
*	Initializes this component.
*/
void ParticleSystemComponent::Initialize() NOEXCEPT
{
	//Register the "Particles" input stream.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("Particles"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(ParticlesPushConstantData),
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			GatherParticlesInputStream(input_stream);
		},
		RenderInputStream::Mode::DRAW_INSTANCED,
		nullptr
	);

	//Add the editable fields.
	AddEditableMaterialAssetField
	(
		"Material",
		offsetof(ParticleSystemInitializationData, _Material),
		offsetof(ParticleSystemInstanceData, _Material)
	);

	AddEditableEnumerationField
	(
		"Emitter Mode",
		offsetof(ParticleSystemInitializationData, _Emitter._ParticleEmitterMode),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._ParticleEmitterMode)
	);

	AddEditableUint32Field
	(
		"Initial Burst",
		offsetof(ParticleSystemInitializationData, _Emitter._InitialBurst),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._InitialBurst)
	);

	AddEditableFloatField
	(
		"Sphere Radius",
		offsetof(ParticleSystemInitializationData, _Emitter._SphereMode._Radius),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._SphereMode._Radius)
	);

	AddEditableVector3Field
	(
		"Minimum Velocity",
		offsetof(ParticleSystemInitializationData, _Emitter._MinimumVelocity),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._MinimumVelocity)
	);

	AddEditableVector3Field
	(
		"Maximum Velocity",
		offsetof(ParticleSystemInitializationData, _Emitter._MaximumVelocity),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._MaximumVelocity)
	);

	AddEditableVector2Field
	(
		"Minimum Size",
		offsetof(ParticleSystemInitializationData, _Emitter._MinimumSize),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._MinimumSize)
	);

	AddEditableVector2Field
	(
		"Maximum Size",
		offsetof(ParticleSystemInitializationData, _Emitter._MaximumSize),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._MaximumSize)
	);

	AddEditableFloatField
	(
		"Minimum Lifetime",
		offsetof(ParticleSystemInitializationData, _Emitter._MinimumLifetime),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._MinimumLifetime)
	);

	AddEditableFloatField
	(
		"Maximum Lifetime",
		offsetof(ParticleSystemInitializationData, _Emitter._MaximumLifetime),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._MaximumLifetime)
	);

	AddEditableUint32Field
	(
		"Spawn Rate",
		offsetof(ParticleSystemInitializationData, _Emitter._SpawnRate),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._SpawnRate)
	);

	AddEditableVector3Field
	(
		"Position Offset",
		offsetof(ParticleSystemInitializationData, _Emitter._PositionOffset),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._PositionOffset)
	);

	AddEditableFloatField
	(
		"Drag",
		offsetof(ParticleSystemInitializationData, _Emitter._Drag),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._Drag)
	);

	AddEditableFloatField
	(
		"Gravity Affection",
		offsetof(ParticleSystemInitializationData, _Emitter._GravityAffection),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._GravityAffection)
	);

	AddEditableFloatField
	(
		"Wind Affection",
		offsetof(ParticleSystemInitializationData, _Emitter._WindAffection),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._WindAffection)
	);

	AddEditableBoolField
	(
		"Shrink Near Camera",
		offsetof(ParticleSystemInitializationData, _Emitter._ShrinkNearCamera),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._ShrinkNearCamera)
	);

	AddEditableFloatField
	(
		"Fade In Time",
		offsetof(ParticleSystemInitializationData, _Emitter._FadeInTime),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._FadeInTime)
	);

	AddEditableFloatField
	(
		"Fade Out Time",
		offsetof(ParticleSystemInitializationData, _Emitter._FadeOutTime),
		offsetof(ParticleSystemInstanceData, _MasterEmitter._FadeOutTime)
	);
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
			ParticleSystemComponent *const RESTRICT particle_system_component{ static_cast<ParticleSystemComponent* const RESTRICT>(arguments) };

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
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 ParticleSystemComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return _InstanceData[instance_index]._SubEmitters.Size();
}

/*
*	Updates this component.
*/
void ParticleSystemComponent::ParallelSubInstanceUpdate(const UpdatePhase update_phase, const uint64 instance_index, const uint64 sub_instance_index) NOEXCEPT
{
	switch (update_phase)
	{
		case UpdatePhase::PRE_RENDER:
		{
			//Cache data that will be used.
			const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };
			const Vector3<float32> camera_forward_vector{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetForwardVector() };

			//Cache the delta time.
			const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

			//Cache the instance data.
			Entity *const RESTRICT entity{ InstanceToEntity(instance_index) };
			ParticleSystemInstanceData &particle_system_instance_data{ _InstanceData[instance_index] };
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

			//Cache the sub emitter.
			ParticleSubEmitter &sub_emitter{ particle_system_instance_data._SubEmitters[sub_instance_index] };

			//Cache the current world position.
			WorldPosition current_world_position{ world_transform_instance_data._CurrentWorldTransform.GetWorldPosition() };

			//Rotate the offset.
			Vector3<float32> rotated_position_offset{ sub_emitter._Emitter._PositionOffset };
			rotated_position_offset.Rotate(world_transform_instance_data._CurrentWorldTransform.GetRotation().ToEulerAngles());

			//Apply the position offset.
			current_world_position.SetLocalPosition(current_world_position.GetLocalPosition() + rotated_position_offset);

			//Check spawning of new particles.
			{
				PROFILING_SCOPE("Spawn New Particles");

				//Calculate the spawn rate reciprocal.
				const float32 spawn_rate_reciprocal{ sub_emitter._Emitter._SpawnRate > 0.0f ? 1.0f / static_cast<float32>(sub_emitter._Emitter._SpawnRate) : FLOAT32_MAXIMUM };

				//Calculate the number of particles to spawn.
				uint32 number_of_particles_to_spawn{ 0 };

				if (sub_emitter._Emitter._InitialBurst > 0 && !sub_emitter._HasDoneInitialBurst)
				{
					number_of_particles_to_spawn += sub_emitter._Emitter._InitialBurst;
					sub_emitter._HasDoneInitialBurst = true;
				}

				sub_emitter._TimeSinceLastParticleSpawn += delta_time;

				while (sub_emitter._TimeSinceLastParticleSpawn >= spawn_rate_reciprocal)
				{
					++number_of_particles_to_spawn;
					sub_emitter._TimeSinceLastParticleSpawn -= spawn_rate_reciprocal;
				}

				for (uint32 particle_to_spawn_index{ 0 }; particle_to_spawn_index < number_of_particles_to_spawn; ++particle_to_spawn_index)
				{
					//Create the new particle instance.
					sub_emitter._Instances.Emplace();
					ParticleInstance &new_particle_instance{ sub_emitter._Instances.Back() };

					//Randomize the world position.
					switch (sub_emitter._Emitter._ParticleEmitterMode)
					{
						case ParticleEmitterMode::SPHERE:
						{
							new_particle_instance._WorldPosition = WorldPosition
							(
								current_world_position.GetCell(),
								current_world_position.GetLocalPosition() + CatalystRandomMath::RandomPointInSphere(sub_emitter._Emitter._SphereMode._Radius)
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
					new_particle_instance._Size = BaseMath::LinearlyInterpolate(sub_emitter._Emitter._MinimumSize, sub_emitter._Emitter._MaximumSize, CatalystRandomMath::RandomFloat());

					//Set the age.
					new_particle_instance._Age = 0.0f;

					//Randomize the lifetime.
					new_particle_instance._Lifetime = CatalystRandomMath::RandomFloatInRange(sub_emitter._Emitter._MinimumLifetime, sub_emitter._Emitter._MaximumLifetime);
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
						//Apply drag.
						if (sub_emitter._Emitter._Drag > 0.0f)
						{
							instance._Velocity -= instance._Velocity * sub_emitter._Emitter._Drag * delta_time;
						}

						//Apply gravity.
						if (sub_emitter._Emitter._GravityAffection != 0.0f)
						{
							instance._Velocity._Y -= PhysicsConstants::GRAVITY * delta_time * sub_emitter._Emitter._GravityAffection;
						}

						//Update the position based on the velocity.
						{
							Vector3<float32> local_position{ instance._WorldPosition.GetLocalPosition() };

							local_position += instance._Velocity * delta_time;

							instance._WorldPosition.SetLocalPosition(local_position);
						}

						//Apply wind.
						if (sub_emitter._Emitter._WindAffection > 0.0f)
						{
							Vector3<float32> local_position{ instance._WorldPosition.GetLocalPosition() };

							local_position += WorldSystem::Instance->GetWindSystem()->GetWindDirection() * WorldSystem::Instance->GetWindSystem()->GetWindSpeed() * delta_time * sub_emitter._Emitter._WindAffection;

							instance._WorldPosition.SetLocalPosition(local_position);
						}

						//Calculate the camera relative position.
						const Vector3<float32> camera_relative_position{ instance._WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()) };

						//Calculate modifiers.
						float32 size_modifier{ 1.0f };

						//Shrink near camera.
						if (sub_emitter._Emitter._ShrinkNearCamera)
						{
							const float32 distance_to_camera{ Vector3<float32>::Length(camera_relative_position - camera_local_position) };

							size_modifier = BaseMath::Minimum<float32>(distance_to_camera / 16.0f, 1.0f);
						}

						//Do _very_ simple culling.
						if (Vector3<float32>::DotProduct(camera_forward_vector, camera_relative_position - camera_local_position) > 0.0f)
						{
							sub_emitter._PackedInstances.Emplace();
							ParticlePackedInstance &packed_instance{ sub_emitter._PackedInstances.Back() };

							packed_instance._WorldPosition = camera_relative_position;
							packed_instance._Size = instance._Size * size_modifier;
							packed_instance._NormalizedAge = instance._Age / instance._Lifetime;
							packed_instance._FadeInTime = sub_emitter._Emitter._FadeInTime;
							packed_instance._FadeOutTime = sub_emitter._Emitter._FadeOutTime;
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
	//Check if any instance should be deleted.
#if defined(CATALYST_EDITOR)
	if (CatalystEditorSystem::Instance->IsInGame())
#endif
	{
		for (uint64 instance_index{ 0 }; instance_index < NumberOfInstances(); ++instance_index)
		{
			const ParticleSystemInstanceData &instance_data{ _InstanceData[instance_index] };
			Entity *const RESTRICT entity{ InstanceToEntity(instance_index) };

			if (TEST_BIT(entity->_Flags, Entity::Flags::QUEUED_FOR_DESTRUCTION))
			{
				continue;
			}

			bool all_sub_emitters_dead{ true };

			for (const ParticleSubEmitter& sub_emitter : instance_data._SubEmitters)
			{
				all_sub_emitters_dead &= (sub_emitter._Emitter._InitialBurst == 0 || sub_emitter._HasDoneInitialBurst) && sub_emitter._Emitter._SpawnRate == 0 && sub_emitter._PackedInstances.Empty();
			}

			if (all_sub_emitters_dead)
			{
				EntitySystem::Instance->DestroyEntity(entity);
			}
		}
	}

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
		for (ParticleSystemInstanceData &instance_data : InstanceData())
		{
			instance_data._NumberOfInstances = 0;
		}

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

/*
*	Sets default values for initialization data.
*/
void ParticleSystemComponent::DefaultInitializationData(ParticleSystemInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	initialization_data->_Material = ContentSystem::Instance->GetAsset<MaterialAsset>(HashString("Default"));
	initialization_data->_Emitter._MinimumSize = Vector2<float32>(1.0f);
	initialization_data->_Emitter._MaximumSize = Vector2<float32>(1.0f);
	initialization_data->_Emitter._MinimumLifetime = 1.0f;
	initialization_data->_Emitter._MaximumLifetime = 1.0f;
	initialization_data->_Emitter._SpawnRate = 1;
	initialization_data->_Emitter._FadeInTime = 0.1f;
	initialization_data->_Emitter._FadeOutTime = 0.9f;
}

/*
*	Creates an instance.
*/
void ParticleSystemComponent::CreateInstance(Entity *const RESTRICT entity, ParticleSystemInitializationData *const RESTRICT initialization_data, ParticleSystemInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy data.
	instance_data->_Material = initialization_data->_Material;
	instance_data->_MasterEmitter = initialization_data->_Emitter;

	//Split into sub emitters.
	SplitIntoSubEmitters(instance_data->_MasterEmitter, &instance_data->_SubEmitters);
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
*	Callback for after an editable field change happens.
*/
void ParticleSystemComponent::PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
{
	//Cache the instance data.
	ParticleSystemInstanceData &instance_data{ InstanceData(entity) };

	//Set up the sub emitters again.
	SplitIntoSubEmitters(instance_data._MasterEmitter, &instance_data._SubEmitters);
}