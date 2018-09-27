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
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(WorldArchitect);

//World architect constants.
namespace WorldArchitectConstants
{
	constexpr float TERRAIN_HEIGHT{ 100.0f };
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

	CPUTexture2D properties{ WorldArchitectConstants::TERRAIN_RESOLUTION };

	const float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

	for (uint64 i = 0; i < WorldArchitectConstants::TERRAIN_RESOLUTION; ++i)
	{
		for (uint64 j = 0; j < WorldArchitectConstants::TERRAIN_RESOLUTION; ++j)
		{
			const float xCoordinate{ static_cast<float>(i) / static_cast<float>(WorldArchitectConstants::TERRAIN_RESOLUTION) };
			const float yCoordinate{ static_cast<float>(j) / static_cast<float>(WorldArchitectConstants::TERRAIN_RESOLUTION) };

			properties.At(i, j)._W = PerlinNoiseGenerator::GenerateNoise(xCoordinate * 7.5f, yCoordinate * 7.5f, 0.0f, randomOffset);
		}
	}

	TerrainUtilities::GeneratePatchNormals(1'000.0f, WorldArchitectConstants::TERRAIN_RESOLUTION, WorldArchitectConstants::TERRAIN_HEIGHT, &properties);

	data->_TerrainProperties = properties;
	data->_TerrainUniformData._TerrainFirstLayerDisplacementHeight = 10.0f;
	data->_TerrainUniformData._TerrainSecondLayerDisplacementHeight = 1.0f;
	data->_TerrainUniformData._TerrainThirdLayerDisplacementHeight = 5.0f;
	data->_TerrainUniformData._TerrainFourthLayerDisplacementHeight = 10.0f;
	data->_TerrainUniformData._TerrainFifthLayerDisplacementHeight = 10.0f;
	data->_TerrainUniformData._TerrainHeight = WorldArchitectConstants::TERRAIN_HEIGHT;
	data->_TerrainUniformData._TerrainSize = 1'000.0f;
	data->_TerrainUniformData._TerrainTextureTilingFactor = 50.0f;
	data->_TerrainUniformData._TerrainPosition = Vector3(0.0f, WorldArchitectConstants::TERRAIN_HEIGHT * 0.25f, 0.0f);

	CPUTexture2D weights{ WorldArchitectConstants::TERRAIN_RESOLUTION };

	for (uint64 i = 0; i < WorldArchitectConstants::TERRAIN_RESOLUTION; ++i)
	{
		for (uint64 j = 0; j < WorldArchitectConstants::TERRAIN_RESOLUTION; ++j)
		{
			weights.At(i, j)._X = 1.0f;
			weights.At(i, j)._Y = 0.0f;
			weights.At(i, j)._Z = 0.0f;
			weights.At(i, j)._W = 0.0f;
		}
	}

	data->_LayerWeightsTexture = RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(weights), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Nearest, TextureFormat::R32G32B32A32_Float));
	data->_TerrainMaterial = ResourceLoader::GetTerrainMaterial(HashString("DefaultTerrainMaterial"));

	EntitySystem::Instance->InitializeEntity(entity, data);

	TerrainPatchInformation information;

	information._X = 0;
	information._Y = 0;
	information._NormalHeightMap = properties;
	information._LayerWeightsMap = weights;
	information._Material = ResourceLoader::GetTerrainMaterial(HashString("DefaultTerrainMaterial"));
	information._DisplacementInformation._FirstLayerDisplacement = 10.0f;
	information._DisplacementInformation._SecondLayerDisplacement = 1.0f;
	information._DisplacementInformation._ThirdLayerDisplacement = 5.0f;
	information._DisplacementInformation._FourthLayerDisplacement = 10.0f;
	information._DisplacementInformation._FifthLayerDisplacement = 10.0f;

	TerrainSystem::Instance->AddTerrainPatch(std::move(information));
}