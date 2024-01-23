//Header file.
#include <Components/Components/ParticleSystemComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

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

	/*
	switch (update_phase)
	{
		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
	*/
}