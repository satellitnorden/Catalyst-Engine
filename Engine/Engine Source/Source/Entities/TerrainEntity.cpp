//Header file.
#include <Entities/TerrainEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(TerrainEntity);

/*
*	Default constructor.
*/
TerrainEntity::TerrainEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewTerrainComponentsIndex();

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
const Vector3& TerrainEntity::GetPosition() const NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetTerrainComponents()[componentsIndex].terrainUniformData.terrainPosition;
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
	ComponentManager::GetTerrainComponents()[componentsIndex].terrainUniformData.terrainPosition += moveVector;

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
void TerrainEntity::Initialize(const uint32 terrainPlaneResolution, const CPUTexture4 &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Initialize this terrain entity.
	RenderingSystem::Instance->InitializeTerrainEntity(*this, terrainPlaneResolution, terrainProperties, terrainUniformData, layerWeightsTexture, terrainMaterial);
}