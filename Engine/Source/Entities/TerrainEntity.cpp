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
const Vector3& TerrainEntity::GetPosition() const NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetTerrainTerrainComponents()[componentsIndex]._TerrainUniformData.terrainPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& TerrainEntity::GetRotation() const NOEXCEPT
{
	//Terrain entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& TerrainEntity::GetScale() const NOEXCEPT
{
	//Terrain entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void TerrainEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetTerrainTerrainComponents()[componentsIndex]._TerrainUniformData.terrainPosition += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void TerrainEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
{
	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void TerrainEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}