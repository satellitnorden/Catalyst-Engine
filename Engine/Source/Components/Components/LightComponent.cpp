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
void LightComponent::CreateInstance(const EntityIdentifier entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
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
void LightComponent::PostCreateInstance(const EntityIdentifier entity) NOEXCEPT
{

}

/*
*	Destroys an instance.
*/
void LightComponent::DestroyInstance(const EntityIdentifier entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

void LightComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = static_cast<UpdatePhase>(0);
	update_configuration->_BatchSize = 0;
}

void LightComponent::Update(const UpdatePhase update_phase, const uint64 start_index, const uint64 end_index) NOEXCEPT
{
	PROFILING_SCOPE(LightComponent::Update);

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