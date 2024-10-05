//Header file.
#include <Components/Components/LightComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

/*
*	Initializes this component.
*/
void LightComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableColorField
	(
		"Color",
		offsetof(LightInitializationData, _Color),
		offsetof(LightInstanceData, _Color)
	);
	AddEditableFloatField
	(
		"Intensity",
		offsetof(LightInitializationData, _Intensity),
		offsetof(LightInstanceData, _Intensity)
	);
	AddEditableEnumerationField
	(
		"Light Type",
		offsetof(LightInitializationData, _LightType),
		offsetof(LightInstanceData, _LightType)
	);
	AddEditableFloatField
	(
		"Radius",
		offsetof(LightInitializationData, _PointLightData._Radius),
		offsetof(LightInstanceData, _PointLightData._Radius)
	);
}

void LightComponent::DefaultInitializationData(LightInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	initialization_data->_LightType = LightType::DIRECTIONAL;
	initialization_data->_DirectionalLightData._Rotation = EulerAngles(1.0f, 0.0f, 0.0f);
	initialization_data->_Color = Vector3<float32>(1.0f, 1.0f, 1.0f);
	initialization_data->_Intensity = 1.0f;
}

/*
*	Creates an instance.
*/
void LightComponent::CreateInstance(Entity *const RESTRICT entity, LightInitializationData *const RESTRICT initialization_data, LightInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy the data.
	instance_data->_LightType = initialization_data->_LightType;

	switch (instance_data->_LightType)
	{
		case LightType::DIRECTIONAL:
		{
			Memory::Copy(&instance_data->_DirectionalLightData, &initialization_data->_DirectionalLightData, sizeof(instance_data->_DirectionalLightData));

			break;
		}

		case LightType::POINT:
		{
			Memory::Copy(&instance_data->_PointLightData, &initialization_data->_PointLightData, sizeof(instance_data->_PointLightData));

			break;
		}
	}

	instance_data->_Color = initialization_data->_Color;
	instance_data->_LightProperties = initialization_data->_LightProperties;
	instance_data->_Intensity = initialization_data->_Intensity;
}

/*
*	Destroys an instance.
*/
void LightComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}