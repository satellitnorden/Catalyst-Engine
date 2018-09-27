//Header file.
#include <Main/WorldArchitect.h>

//Core.
#include <Core/General/HashString.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoiseGenerator.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WorldArchitect);

//World architect constants.
namespace WorldArchitectConstants
{
	constexpr uint64 MAP_RESOLUTION{ 128 };
}

/*
*	Initializes the world architect.
*/
void WorldArchitect::Initialize() NOEXCEPT
{
	//Create some terrain.
	CPUTexture2D properties{ WorldArchitectConstants::MAP_RESOLUTION };

	const float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

	for (uint64 i = 0; i < WorldArchitectConstants::MAP_RESOLUTION; ++i)
	{
		for (uint64 j = 0; j < WorldArchitectConstants::MAP_RESOLUTION; ++j)
		{
			const float xCoordinate{ static_cast<float>(i) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };
			const float yCoordinate{ static_cast<float>(j) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };

			properties.At(i, j)._W = PerlinNoiseGenerator::GenerateNoise(xCoordinate * 7.5f, yCoordinate * 7.5f, 0.0f, randomOffset);
		}
	}

	CPUTexture2D weights{ WorldArchitectConstants::MAP_RESOLUTION };

	for (uint64 i = 0; i < WorldArchitectConstants::MAP_RESOLUTION; ++i)
	{
		for (uint64 j = 0; j < WorldArchitectConstants::MAP_RESOLUTION; ++j)
		{
			weights.At(i, j)._X = 1.0f;
			weights.At(i, j)._Y = 0.0f;
			weights.At(i, j)._Z = 0.0f;
			weights.At(i, j)._W = 0.0f;
		}
	}

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