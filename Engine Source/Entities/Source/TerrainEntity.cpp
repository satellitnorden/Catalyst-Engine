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
void TerrainEntity::Initialize(const CPUTexture4 &heightMap, const uint32 terrainPlaneResolution, const TerrainUniformData &terrainUniformData, const Vulkan2DTexture *RESTRICT terrainHeightMapTexture, const Vulkan2DTexture *RESTRICT terrainNormalMapTexture, const Vulkan2DTexture *RESTRICT layer1WeightTexture, const Vulkan2DTexture *RESTRICT layer1AlbedoTexture, const Vulkan2DTexture *RESTRICT layer1NormalMapTexture, const Vulkan2DTexture *RESTRICT layer1RoughnessTexture, const Vulkan2DTexture *RESTRICT layer1MetallicTexture, const Vulkan2DTexture *RESTRICT layer1AmbientOcclusionTexture, const Vulkan2DTexture *RESTRICT layer1DisplacementTexture, const Vulkan2DTexture *RESTRICT layer2WeightTexture, const Vulkan2DTexture *RESTRICT layer2AlbedoTexture, const Vulkan2DTexture *RESTRICT layer2NormalMapTexture, const Vulkan2DTexture *RESTRICT layer2RoughnessTexture, const Vulkan2DTexture *RESTRICT layer2MetallicTexture, const Vulkan2DTexture *RESTRICT layer2AmbientOcclusionTexture, const Vulkan2DTexture *RESTRICT layer2DisplacementTexture, const Vulkan2DTexture *RESTRICT layer3WeightTexture, const Vulkan2DTexture *RESTRICT layer3AlbedoTexture, const Vulkan2DTexture *RESTRICT layer3NormalMapTexture, const Vulkan2DTexture *RESTRICT layer3RoughnessTexture, const Vulkan2DTexture *RESTRICT layer3MetallicTexture, const Vulkan2DTexture *RESTRICT layer3AmbientOcclusionTexture, const Vulkan2DTexture *RESTRICT layer3DisplacementTexture) NOEXCEPT
{
	//Initialize this terrain entity.
	GraphicsSystem::Instance->InitializeTerrainEntity(*this, heightMap, terrainPlaneResolution, terrainUniformData, terrainHeightMapTexture, terrainNormalMapTexture, layer1WeightTexture, layer1AlbedoTexture, layer1NormalMapTexture, layer1RoughnessTexture, layer1MetallicTexture, layer1AmbientOcclusionTexture, layer1DisplacementTexture, layer2WeightTexture, layer2AlbedoTexture, layer2NormalMapTexture, layer2RoughnessTexture, layer2MetallicTexture, layer2AmbientOcclusionTexture, layer2DisplacementTexture, layer3WeightTexture, layer3AlbedoTexture, layer3NormalMapTexture, layer3RoughnessTexture, layer3MetallicTexture, layer3AmbientOcclusionTexture, layer3DisplacementTexture);
}