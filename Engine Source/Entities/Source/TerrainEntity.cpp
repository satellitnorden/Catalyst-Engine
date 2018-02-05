//Header file.
#include <TerrainEntity.h>

//Components.
#include <ComponentManager.h>

//Systems.
#include <GraphicsSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(TerrainEntity);

/*
*	Default constructor.
*/
TerrainEntity::TerrainEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewTerrainEntityComponentsIndex();

	//Add this terrain entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
TerrainEntity::~TerrainEntity() NOEXCEPT
{
	//Remove this terrain entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& TerrainEntity::GetPosition() NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetTerrainEntityTerrainComponents()[componentsIndex].terrainUniformData.terrainPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& TerrainEntity::GetRotation() NOEXCEPT
{
	//Terrain entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& TerrainEntity::GetScale() NOEXCEPT
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
	ComponentManager::GetTerrainEntityTerrainComponents()[componentsIndex].terrainUniformData.terrainPosition += moveVector;

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

/*
*	Initializes this terrain entity.
*/
void TerrainEntity::Initialize(const uint32 terrainResolution, const HeightMap &initialHeightMap) NOEXCEPT
{
	//Initialize this terrain entity.
	GraphicsSystem::Instance->InitializeTerrainEntity(*this, terrainResolution);
}