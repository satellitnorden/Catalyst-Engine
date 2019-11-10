//Header file.
#include <Entities/Types/LightEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/LightInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

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
			light_component._Position = light_initialization_data->_Position;

			break;
		}
	}

	light_component._Luminance = light_initialization_data->_Luminance;
	light_component._LightType = light_initialization_data->_LightType;
	light_component._Size = light_initialization_data->_Size;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<LightInitializationData>(data);
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
*	Returns the light type of this light.
*/
LightType LightEntity::GetLightType() NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._LightType;
}

/*
*	Returns the position of this light.
*/
Vector3<float> LightEntity::GetPosition() NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Position;
}

/*
*	Sets the position of this light.
*/
void LightEntity::SetPosition(const Vector3<float>& position) NOEXCEPT
{
	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Position = position;
}

/*
*	Returns the direction of this light.
*/
Vector3<float> LightEntity::GetDirection() NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Direction;
}

/*
*	Sets the direction of this light.
*/
void LightEntity::SetDirection(const Vector3<float>& direction) NOEXCEPT
{
	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Direction = direction;
}

/*
*	Returns the luminance of this light.
*/
Vector3<float> LightEntity::GetLuminance() NOEXCEPT
{
	return ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Luminance;
}

/*
*	Sets the luminance of this light.
*/
void LightEntity::SetLuminance(const Vector3<float>& luminance) NOEXCEPT
{
	ComponentManager::GetLightLightComponents()[_ComponentsIndex]._Luminance = luminance;
}