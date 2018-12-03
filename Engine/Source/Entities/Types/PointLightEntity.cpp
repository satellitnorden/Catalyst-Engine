//Header file.
#include <Entities/Types/PointLightEntity.h>

//Components.
#include <Components/ComponentManager.h>

/*
*	Default constructor.
*/
PointLightEntity::PointLightEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::PointLight;
}

/*
*	Returns the position of this entity.
*/
RESTRICTED NO_DISCARD Vector3<float> *const RESTRICT PointLightEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetPointLightPointLightComponents()[_ComponentsIndex]._Position;
}