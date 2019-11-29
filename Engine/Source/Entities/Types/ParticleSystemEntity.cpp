//Header file.
#include <Entities/Types/ParticleSystemEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/ParticleSystemInitializationData.h>

//Rendering.
#include <Rendering/Native/ParticleInstanceData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
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
	component._MinimumPosition = particle_system_initialization_data->_MinimumPosition;
	component._MaximumPosition = particle_system_initialization_data->_MaximumPosition;
	component._MinimumVelocity = particle_system_initialization_data->_MinimumVelocity;
	component._MaximumVelocity = particle_system_initialization_data->_MaximumVelocity;
	component._MinimumScale = particle_system_initialization_data->_MinimumScale;
	component._MaximumScale = particle_system_initialization_data->_MaximumScale;
	component._SpawnFrequency = particle_system_initialization_data->_SpawnFrequency;
	component._Lifetime = particle_system_initialization_data->_Lifetime;
	component._FadeTime = particle_system_initialization_data->_FadeTime;

	//Calculate the maximum number of instances that will be active at one time.
	const uint64 maximum_instances{ static_cast<uint64>(component._Lifetime / component._SpawnFrequency) };

	component._InstanceData.UpsizeSlow(maximum_instances);

	for (ParticleInstanceData& instance_data : component._InstanceData)
	{
		instance_data._Time = component._Lifetime;
	}

	component._TimeSinceLastSpawn = 0.0f;

	//Initialize the render component.
	render_component._MaterialIndex = particle_system_initialization_data->_MaterialIndex;
	RenderingSystem::Instance->CreateBuffer(sizeof(ParticleInstanceData) * maximum_instances,
											BufferUsage::VertexBuffer,
											MemoryProperty::HostCoherent | MemoryProperty::HostVisible,
											//MemoryProperty::DeviceLocal,
											&render_component._TransformationsBuffer);
	render_component._NumberOfTransformations = 0;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<ParticleSystemInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void ParticleSystemEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnParticleSystemComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD Matrix4 *const RESTRICT ParticleSystemEntity::GetWorldTransform() NOEXCEPT
{
	//return &ComponentManager::GetAnimatedModelAnimatedModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
	return nullptr;
}