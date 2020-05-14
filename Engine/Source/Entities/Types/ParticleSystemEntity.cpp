//Header file.
#include <Entities/Types/ParticleSystemEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/ParticleSystemInitializationData.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/ParticleInstanceData.h>

//Systems.
#include <Systems/EntitySystem.h>
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
*	Initializes this entity.
*/
void ParticleSystemEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewParticleSystemComponentsIndex(this);

	//Copy the data.
	const ParticleSystemInitializationData*const RESTRICT particle_system_initialization_data{ static_cast<const ParticleSystemInitializationData*const RESTRICT>(data) };
	ParticleSystemComponent& component{ ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex] };
	ParticleSystemRenderComponent& render_component{ ComponentManager::GetParticleSystemParticleSystemRenderComponents()[_ComponentsIndex] };

	//Initialize the component.
	component._ParticleSystemProperties = particle_system_initialization_data->_ParticleSystemProperties;
	component._OriginalWorldPosition = particle_system_initialization_data->_InitialWorldPosition;
	component._CurrentWorldPosition = particle_system_initialization_data->_InitialWorldPosition;
	component._MinimumPosition = particle_system_initialization_data->_MinimumPosition;
	component._MaximumPosition = particle_system_initialization_data->_MaximumPosition;
	component._MinimumVelocity = particle_system_initialization_data->_MinimumVelocity;
	component._MaximumVelocity = particle_system_initialization_data->_MaximumVelocity;
	component._MinimumScale = particle_system_initialization_data->_MinimumScale;
	component._MaximumScale = particle_system_initialization_data->_MaximumScale;
	component._NumberOfInstances = CatalystBaseMath::Ceiling<uint32>(particle_system_initialization_data->_Lifetime * particle_system_initialization_data->_InitialSpawnFrequency);
	component._SpawnFrequency = particle_system_initialization_data->_InitialSpawnFrequency;
	component._Lifetime = particle_system_initialization_data->_Lifetime;
	component._FadeTime = particle_system_initialization_data->_FadeTime;

	//Set up the initial instance data.
	DynamicArray<ParticleInstanceData> instance_data;
	instance_data.Upsize<false>(component._NumberOfInstances);

	for (ParticleInstanceData& instance : instance_data)
	{
		instance._Position = Vector3<float32>(0.0f, 0.0f, 0.0f);
		instance._Velocity = Vector3<float32>(0.0f, 0.0f, 0.0f);
		instance._Size = Vector2<float32>(0.0f, 0.0f);
		instance._Time = component._Lifetime;
	}

	component._TimeSinceLastSpawn = 0.0f;
	component._FirstParticleIndexToSpawn = 0;
	component._NumberOfParticlesToSpawn = 0;

	//Initialize the render component.
	render_component._MaterialIndex = particle_system_initialization_data->_MaterialIndex;
	RenderingSystem::Instance->CreateBuffer(sizeof(ParticleInstanceData) * component._NumberOfInstances,
											BufferUsage::StorageBuffer | BufferUsage::VertexBuffer,
											MemoryProperty::DeviceLocal,
											&render_component._TransformationsBuffer);
	const void* RESTRICT data_chunks[]{ instance_data.Data() };
	const uint64 data_sizes[]{ sizeof(ParticleInstanceData) * instance_data.Size() };
	RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &render_component._TransformationsBuffer);

	render_component._NumberOfInstances = component._NumberOfInstances;

	RenderingSystem::Instance->CreateRenderDataTable(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::ParticleSystem), &component._RenderDataTable);
	RenderingSystem::Instance->BindStorageBufferToRenderDataTable(0, 0, &component._RenderDataTable, render_component._TransformationsBuffer);

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<ParticleSystemInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void ParticleSystemEntity::Terminate() NOEXCEPT
{
	//Cache the components.
	ParticleSystemComponent &component{ ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex] };
	ParticleSystemRenderComponent &render_component{ ComponentManager::GetParticleSystemParticleSystemRenderComponents()[_ComponentsIndex] };

	//Destroy the render data table.
	RenderingSystem::Instance->DestroyRenderDataTable(&component._RenderDataTable);

	//Destroy the transformations buffer.
	RenderingSystem::Instance->DestroyBuffer(&render_component._TransformationsBuffer);

	//Return this entitiy's components index.
	ComponentManager::ReturnParticleSystemComponentsIndex(_ComponentsIndex);
}

/*
*	Returns whether or not this entity should automatically terminate.
*/
bool ParticleSystemEntity::ShouldAutomaticallyTerminate() const NOEXCEPT
{
	ASSERT(!IsLooping(), "Looping particle system entities will never automatically terminate!");

	return ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._FirstParticleIndexToSpawn > (ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._NumberOfInstances * 2);
}

/*
*	Returns whether or not this particle system entity is looping.
*/
NO_DISCARD bool ParticleSystemEntity::IsLooping() const NOEXCEPT
{
	return TEST_BIT(ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._ParticleSystemProperties, Property::LOOPING);
}

/*
*	Returns the world position.
*/
NO_DISCARD const WorldPosition &ParticleSystemEntity::GetWorldPosition() const NOEXCEPT
{
	return ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._CurrentWorldPosition;
}

/*
*	Sets the world position.
*/
void ParticleSystemEntity::SetWorldPosition(const WorldPosition &value) NOEXCEPT
{
	ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._CurrentWorldPosition = value;
}

/*
*	Sets the minimum velocity.
*/
void ParticleSystemEntity::SetMinimumVelocity(const Vector3<float32> &velocity) NOEXCEPT
{
	ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._MinimumVelocity = velocity;
}

/*
*	Sets the maximum velocity.
*/
void ParticleSystemEntity::SetMaximumVelocity(const Vector3<float32> &velocity) NOEXCEPT
{
	ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._MaximumVelocity = velocity;
}

/*
*	Sets the spawn frequency.
*/
void ParticleSystemEntity::SetSpawnFrequency(const float spawn_frequency) NOEXCEPT
{
	ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex]._SpawnFrequency = spawn_frequency;
}