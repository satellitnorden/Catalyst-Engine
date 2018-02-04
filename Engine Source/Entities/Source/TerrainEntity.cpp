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
Vector3& TerrainEntity::GetPosition() CATALYST_NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetTerrainEntityTerrainComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
Vector3& TerrainEntity::GetRotation() CATALYST_NOEXCEPT
{
	//Terrain entities has no rotation.
	static Vector3 defaultRotation{ 0.0f, 0.0f, 0.0f };

	return defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
Vector3& TerrainEntity::GetScale() CATALYST_NOEXCEPT
{
	//Terrain entities has no scale.
	static Vector3 defaultScale{ 1.0f, 1.0f, 1.0f };

	return defaultScale;
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