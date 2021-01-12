//Header file.
#include <Entities/Types/LightEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/LightInitializationData.h>

//Systems.
#include <Systems/EntitySystem.h>

/*
*	Default constructor.
*/
LightEntity::LightEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Light;
}

/*
*	Initializes this entity.
*/
void LightEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewLightComponentsIndex(this);

	//Copy the data over to the components.
	LightComponent &light_component{ ComponentManager::GetLightLightComponents()[_ComponentsIndex] };
	const LightInitializationData *const RESTRICT light_initialization_data{ static_cast<const LightInitializationData *const RESTRICT>(data) };

	switch (light_initialization_data->_LightType)
	{
		case LightType::DIRECTIONAL:
		{
			light_component._Direction = light_initialization_data->_Direction;

			break;
		}

		case LightType::POINT:
		{
			light_component._WorldPosition = light_initialization_data->_WorldPosition;

			break;
		}
	}

	light_component._Color = light_initialization_data->_Color;
	light_component._LightType = light_initialization_data->_LightType;
	light_component._LightProperties = light_initialization_data->_LightProperties;
	light_component._Intensity = light_initialization_data->_Intensity;
	light_component._Radius = light_initialization_data->_Radius;
	light_component._Size = light_initialization_data->_Size;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<LightInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void LightEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnLightComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the direction of this light.
*/
Vector3<float32> LightEntity::GetDirection() const NOEXCEPT
{
	ASSERT(GetLightType() == LightType::DIRECTIONAL, "Direction is only used for directional lights!");

	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Direction;
}

/*
*	Sets the direction of this light.
*/
void LightEntity::SetDirection(const Vector3<float32>& direction) NOEXCEPT
{
	ASSERT(GetLightType() == LightType::DIRECTIONAL, "Direction is only used for directional lights!");

	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Direction = direction;
}

/*
*	Returns the world position of this light.
*/
NO_DISCARD const WorldPosition &LightEntity::GetWorldPosition() const NOEXCEPT
{
	ASSERT(GetLightType() == LightType::POINT, "Position is only used for point lights!");

	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._WorldPosition;
}

/*
*	Sets the world position of this light.
*/
void LightEntity::SetWorldPosition(const WorldPosition &world_position) NOEXCEPT
{
	ASSERT(GetLightType() == LightType::POINT, "Position is only used for point lights!");

	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._WorldPosition = world_position;
}

/*
*	Returns the color of this light.
*/
Vector3<float32> LightEntity::GetColor() const NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Color;
}

/*
*	Sets the color of this light.
*/
void LightEntity::SetColor(const Vector3<float32> &color) NOEXCEPT
{
	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Color = color;
}

/*
*	Returns the light type of this light.
*/
LightType LightEntity::GetLightType() const NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._LightType;
}

/*
*	Sets the light type of this light.
*/
void LightEntity::SetLightType(const LightType type) NOEXCEPT
{
	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._LightType = type;
}

/*
*	Returns the light properties of this light.
*/
uint32 LightEntity::GetLightProperties() const NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._LightProperties;
}

/*
*	Sets the light properties of this light.
*/
void LightEntity::SetLightProperties(const uint32 properties) NOEXCEPT
{
	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._LightProperties = properties;
}

/*
*	Returns the intensity of this light.
*/
float32 LightEntity::GetIntensity() const NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Intensity;
}

/*
*	Sets the intensity of this light.
*/
void LightEntity::SetIntensity(const float32 intensity) NOEXCEPT
{
	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Intensity = intensity;
}

/*
*	Returns the radius of this light.
*/
float32 LightEntity::GetRadius() const NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Radius;
}

/*
*	Sets the radius of this light.
*/
void LightEntity::SetRadius(const float32 radius) NOEXCEPT
{
	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Radius = radius;
}

/*
*	Returns the size of this light.
*/
float32 LightEntity::GetSize() const NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Size;
}

/*
*	Sets the size of this light.
*/
void LightEntity::SetSize(const float32 size) NOEXCEPT
{
	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Size = size;
}

/*
*	Returns if this light is surface shadow casting.
*/
NO_DISCARD bool LightEntity::IsSurfaceShadowCasting() const NOEXCEPT
{
	return TEST_BIT(GetLightProperties(), CatalystShaderConstants::LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT);
}

/*
*	Sets is this light is surface shadow casting.
*/
void LightEntity::SetIsSurfaceShadowCasting(const bool is_surface_shadow_casting) NOEXCEPT
{
	if (is_surface_shadow_casting)
	{
		SetLightProperties(GetLightProperties() | CatalystShaderConstants::LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT);
	}

	else
	{
		uint32 current_light_properties{ GetLightProperties() };
		CLEAR_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT);
		SetLightProperties(current_light_properties);
	}
}

/*
*	Returns if this light is volumetric.
*/
NO_DISCARD bool LightEntity::IsVolumetric() const NOEXCEPT
{
	return TEST_BIT(GetLightProperties(), CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_BIT);
}

/*
*	Sets is this light is volumetric.
*/
void LightEntity::SetIsVolumetric(const bool is_volumetric) NOEXCEPT
{
	if (is_volumetric)
	{
		SetLightProperties(GetLightProperties() | CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_BIT);
	}
	
	else
	{
		uint32 current_light_properties{ GetLightProperties() };
		CLEAR_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_BIT);
		SetLightProperties(current_light_properties);
	}
}

/*
*	Returns if this light is volumetric shadow casting.
*/
NO_DISCARD bool LightEntity::IsVolumetricShadowCasting() const NOEXCEPT
{
	return TEST_BIT(GetLightProperties(), CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT);
}

/*
*	Sets is this light is volumetric shadow casting.
*/
void LightEntity::SetIsVolumetricShadowCasting(const bool is_volumetric_shadow_casting) NOEXCEPT
{
	if (is_volumetric_shadow_casting)
	{
		SetLightProperties(GetLightProperties() | CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT);
	}

	else
	{
		uint32 current_light_properties{ GetLightProperties() };
		CLEAR_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT);
		SetLightProperties(current_light_properties);
	}
}