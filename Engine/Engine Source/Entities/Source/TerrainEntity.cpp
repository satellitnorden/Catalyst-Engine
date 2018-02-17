//Header file.
#include <TerrainEntity.h>

//Components.
#include <ComponentManager.h>

//Systems.
#include <RenderingSystem.h>

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
const Vector3& TerrainEntity::GetPosition() NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetTerrainComponents()[componentsIndex].terrainUniformData.terrainPosition;
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
void TerrainEntity::Initialize(const CPUTexture4 &heightMap, const uint32 terrainPlaneResolution, const TerrainUniformData &terrainUniformData, const Texture2DHandle terrainHeightMapTexture, const Texture2DHandle terrainNormalMapTexture, const Texture2DHandle layer1WeightTexture, const Texture2DHandle layer1AlbedoTexture, const Texture2DHandle layer1NormalMapTexture, const Texture2DHandle layer1RoughnessTexture, const Texture2DHandle layer1MetallicTexture, const Texture2DHandle layer1AmbientOcclusionTexture, const Texture2DHandle layer1DisplacementTexture, const Texture2DHandle layer2WeightTexture, const Texture2DHandle layer2AlbedoTexture, const Texture2DHandle layer2NormalMapTexture, const Texture2DHandle layer2RoughnessTexture, const Texture2DHandle layer2MetallicTexture, const Texture2DHandle layer2AmbientOcclusionTexture, const Texture2DHandle layer2DisplacementTexture, const Texture2DHandle layer3WeightTexture, const Texture2DHandle layer3AlbedoTexture, const Texture2DHandle layer3NormalMapTexture, const Texture2DHandle layer3RoughnessTexture, const Texture2DHandle layer3MetallicTexture, const Texture2DHandle layer3AmbientOcclusionTexture, const Texture2DHandle layer3DisplacementTexture) NOEXCEPT
{
	//Initialize this terrain entity.
	RenderingSystem::Instance->InitializeTerrainEntity(*this, heightMap, terrainPlaneResolution, terrainUniformData, terrainHeightMapTexture, terrainNormalMapTexture, layer1WeightTexture, layer1AlbedoTexture, layer1NormalMapTexture, layer1RoughnessTexture, layer1MetallicTexture, layer1AmbientOcclusionTexture, layer1DisplacementTexture, layer2WeightTexture, layer2AlbedoTexture, layer2NormalMapTexture, layer2RoughnessTexture, layer2MetallicTexture, layer2AmbientOcclusionTexture, layer2DisplacementTexture, layer3WeightTexture, layer3AlbedoTexture, layer3NormalMapTexture, layer3RoughnessTexture, layer3MetallicTexture, layer3AmbientOcclusionTexture, layer3DisplacementTexture);
}