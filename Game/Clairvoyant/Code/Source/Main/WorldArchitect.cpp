//Header file.
#include <Main/WorldArchitect.h>

//Core.
#include <Core/General/HashString.h>

//Entities.
#include <Entities/TerrainEntity.h>
#include <Entities/InitializationData/TerrainInitializationData.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoiseGenerator.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/TextureData.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WorldArchitect);

//World architect constants.
namespace WorldArchitectsConstant
{
	constexpr float TERRAIN_HEIGHT{ 256.0f };
	constexpr uint64 TERRAIN_RESOLUTION{ 1'024 };
}

/*
*	Initializes the world architect.
*/
void WorldArchitect::Initialize() NOEXCEPT
{
	//Create some terrain.
	TerrainEntity *const RESTRICT entity{ EntitySystem::Instance->CreateEntity<TerrainEntity>() };

	TerrainInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<TerrainInitializationData>() };

	data->_Properties = EntityInitializationData::EntityProperty::None;
	data->_AxisAlignedBoundingBox._Minimum = Vector3(0.0f, 0.0f, 0.0f);
	data->_AxisAlignedBoundingBox._Maximum = Vector3(0.0f, 0.0f, 0.0f);

	CPUTexture2D properties{ WorldArchitectsConstant::TERRAIN_RESOLUTION };

	const float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

	for (uint64 i = 0; i < WorldArchitectsConstant::TERRAIN_RESOLUTION; ++i)
	{
		for (uint64 j = 0; j < WorldArchitectsConstant::TERRAIN_RESOLUTION; ++j)
		{
			const float xCoordinate{ static_cast<float>(i) / static_cast<float>(WorldArchitectsConstant::TERRAIN_RESOLUTION) };
			const float yCoordinate{ static_cast<float>(j) / static_cast<float>(WorldArchitectsConstant::TERRAIN_RESOLUTION) };

			properties.At(i, j)._X = 0.0f;
			properties.At(i, j)._Y = 1.0f;
			properties.At(i, j)._Z = 0.0f;
			properties.At(i, j)._W = PerlinNoiseGenerator::GenerateNoise(xCoordinate * 5.0f, yCoordinate * 5.0f, 0.0f, randomOffset);
		}
	}

	data->_TerrainProperties = properties;
	data->_TerrainUniformData._TerrainFirstLayerDisplacementHeight = 10.0f;
	data->_TerrainUniformData._TerrainSecondLayerDisplacementHeight = 10.0f;
	data->_TerrainUniformData._TerrainThirdLayerDisplacementHeight = 10.0f;
	data->_TerrainUniformData._TerrainFourthLayerDisplacementHeight = 10.0f;
	data->_TerrainUniformData._TerrainFifthLayerDisplacementHeight = 10.0f;
	data->_TerrainUniformData._TerrainHeight = WorldArchitectsConstant::TERRAIN_HEIGHT;
	data->_TerrainUniformData._TerrainSize = 1'000.0f;
	data->_TerrainUniformData._TerrainTextureTilingFactor = 25.0f;
	data->_TerrainUniformData._TerrainPosition = Vector3(0.0f, WorldArchitectsConstant::TERRAIN_HEIGHT * 0.25f, 0.0f);

	CPUTexture2D weights{ WorldArchitectsConstant::TERRAIN_RESOLUTION };

	for (uint64 i = 0; i < WorldArchitectsConstant::TERRAIN_RESOLUTION; ++i)
	{
		for (uint64 j = 0; j < WorldArchitectsConstant::TERRAIN_RESOLUTION; ++j)
		{
			weights.At(i, j)._X = 0.0f;
			weights.At(i, j)._Y = 0.0f;
			weights.At(i, j)._Z = 0.0f;
			weights.At(i, j)._W = 0.0f;
		}
	}

	data->_LayerWeightsTexture = RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(weights), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Nearest, TextureFormat::R32G32B32A32_Float));
	data->_TerrainMaterial = ResourceLoader::GetTerrainMaterial(HashString("DefaultTerrainMaterial"));

	EntitySystem::Instance->InitializeEntity(entity, data);
}