//Header file.
#include <Components/Components/LightComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

void LightComponent::DefaultInitializationData(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	LightInitializationData *const RESTRICT _initialization_data{ static_cast<LightInitializationData* const RESTRICT>(initialization_data) };

	_initialization_data->_DirectionalLightData._Rotation = EulerAngles(1.0f, 0.0f, 0.0f);
	_initialization_data->_Color = Vector3<float32>(1.0f, 1.0f, 1.0f);
	_initialization_data->_Intensity = 1.0f;
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
}

/*
*	Destroys an instance.
*/
void LightComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}