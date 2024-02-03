//Header file.
#include <Components/Components/LightComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

DEFINE_COMPONENT(LightComponent, LightSharedData, LightInitializationData, LightInstanceData);

/*
*	Initializes this component.
*/
void LightComponent::Initialize() NOEXCEPT
{

}

/*
*	Post-initializes this component.
*/
void LightComponent::PostInitialize() NOEXCEPT
{

}

/*
*	Terminates this component.
*/
void LightComponent::Terminate() NOEXCEPT
{

}

NO_DISCARD bool LightComponent::NeedsPreProcessing() const NOEXCEPT
{
	return false;
}

/*
*	Preprocessed initialization data an instance.
*/
void LightComponent::PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{

}

/*
*	Creates an instance.
*/
void LightComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	LightInitializationData *const RESTRICT _initialization_data{ static_cast<LightInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	LightInstanceData &instance_data{ _InstanceData.Back() };

	//Copy the data.
	switch (_initialization_data->_LightType)
	{
		case LightType::DIRECTIONAL:
		{
			Memory::Copy(&instance_data._DirectionalLightData, &_initialization_data->_DirectionalLightData, sizeof(instance_data._DirectionalLightData));

			break;
		}

		case LightType::POINT:
		{
			Memory::Copy(&instance_data._PointLightData, &_initialization_data->_PointLightData, sizeof(instance_data._PointLightData));

			break;
		}
	}

	instance_data._Color = _initialization_data->_Color;
	instance_data._LightType = _initialization_data->_LightType;
	instance_data._LightProperties = _initialization_data->_LightProperties;
	instance_data._Intensity = _initialization_data->_Intensity;

	//Free the initialization data.
	FreeInitializationData(_initialization_data);
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void LightComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{

}

/*
*	Destroys an instance.
*/
void LightComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 LightComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return 1;
}

void LightComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = static_cast<UpdatePhase>(0);
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::BATCH;
	update_configuration->_BatchSize = 0;
}

/*
*	Runs before the given update phase.
*/
void LightComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

/*
*	Updates this component.
*/
void LightComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	PROFILING_SCOPE("LightComponent::Update");

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

/*
*	Runs after the given update phase.
*/
void LightComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}