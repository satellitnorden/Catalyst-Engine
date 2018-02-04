//Header file.
#include <TerrainEntity.h>

//Components.
#include <ComponentManager.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(TerrainEntity);

/*
*	Default constructor.
*/
TerrainEntity::TerrainEntity() CATALYST_NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewTerrainEntityComponentsIndex();

	//Add this terrain entity to the universal container.
	Instances.Emplace(this);
}

/*
*	Default destructor.
*/
TerrainEntity::~TerrainEntity() CATALYST_NOEXCEPT
{
	//Remove this terrain entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& TerrainEntity::GetPosition() CATALYST_NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetTerrainEntityTerrainComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& TerrainEntity::GetRotation() CATALYST_NOEXCEPT
{
	//Terrain entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& TerrainEntity::GetScale() CATALYST_NOEXCEPT
{
	//Terrain entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void TerrainEntity::Move(const Vector3 &moveVector) CATALYST_NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetTerrainEntityTerrainComponents()[componentsIndex].position += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void TerrainEntity::Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT
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
void TerrainEntity::Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}