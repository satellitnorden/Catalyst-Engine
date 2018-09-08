//Header file.
#include <Entities/TerrainEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
TerrainEntity::TerrainEntity() NOEXCEPT
{
	//Set the entity type.
	entityType = EntityType::Terrain;
}

/*
*	Returns the position of this entity.
*/
NO_DISCARD RESTRICTED Vector3 *const RESTRICT TerrainEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetTerrainTerrainComponents()[_ComponentsIndex]._TerrainUniformData._TerrainPosition;
}