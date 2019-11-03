//Header file.
#include <Entities/Types/ParticleSystemEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/ParticleSystemInitializationData.h>
//Systems.
#include <Systems/EntityCreationSystem.h>

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
	ParticleSystemComponent& particle_system_component{ ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex] };

	particle_system_component._GlobalMaterialIndex = particle_system_initialization_data->_GlobalMaterialIndex;
	particle_system_component._PositionExtent = particle_system_initialization_data->_PositionExtent;
	particle_system_component._VelocityExtent = particle_system_initialization_data->_VelocityExtent;
	particle_system_component._SizeExtent = particle_system_initialization_data->_SizeExtent;
	particle_system_component._FadeTime = particle_system_initialization_data->_FadeTime;
	particle_system_component._Lifetime = particle_system_initialization_data->_Lifetime;

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