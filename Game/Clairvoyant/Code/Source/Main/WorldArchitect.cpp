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
	constexpr uint64 MAP_RESOLUTION{ 512 };
}

/*
*	Initializes the world architect.
*/
void WorldArchitect::Initialize() NOEXCEPT
{
	//Create some terrain.
	CPUTexture2D properties{ WorldArchitectConstants::MAP_RESOLUTION };

	const float randomOffset1{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
	const float randomOffset2{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
	const float randomOffset3{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
	const float randomOffset4{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

	for (uint64 i = 0; i < WorldArchitectConstants::MAP_RESOLUTION; ++i)
	{
		for (uint64 j = 0; j < WorldArchitectConstants::MAP_RESOLUTION; ++j)
		{
			const float xCoordinate{ static_cast<float>(i) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };
			const float yCoordinate{ static_cast<float>(j) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };

			properties.At(i, j)._W = PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, 0.0f, randomOffset1);
			properties.At(i, j)._W += PerlinNoiseGenerator::GenerateNoise(xCoordinate * 7.5f, yCoordinate * 7.5f, 0.0f, randomOffset2) * 0.25f;
			properties.At(i, j)._W += PerlinNoiseGenerator::GenerateNoise(xCoordinate * 15.0f, yCoordinate * 15.0f, 0.0f, randomOffset3) * 0.125f;
			properties.At(i, j)._W += 0.25f;
		}
	}

	CPUTexture2D weights{ WorldArchitectConstants::MAP_RESOLUTION };

	for (uint64 i = 0; i < WorldArchitectConstants::MAP_RESOLUTION; ++i)
	{
		for (uint64 j = 0; j < WorldArchitectConstants::MAP_RESOLUTION; ++j)
		{
			//Determine the weight of the sand layer.
			float sandLayerWeight;

			const float worldHeight{ properties.At(i, j)._W * 250.0f };

			if (worldHeight < 0.0f)
			{
				sandLayerWeight = 1.0f;
			}

			else if (worldHeight > 10.0f)
			{
				sandLayerWeight = 0.0f;
			}

			else
			{
				sandLayerWeight = 1.0f - (worldHeight * 0.1f);
			}

			//Determine the weight of the first grass layer.
			weights.At(i, j)._X = 1.0f - sandLayerWeight;

			//Determine the weight of the second grass layer.
			const float xCoordinate{ static_cast<float>(i) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };
			const float yCoordinate{ static_cast<float>(j) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };

			weights.At(i, j)._Y = ((PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, 0.0f, randomOffset4) + 1.0f) * 0.5f);

			//Modify the weight of the first grass layer.
			weights.At(i, j)._X = CatalystBaseMath::Clamp<float>(weights.At(i, j)._X - weights.At(i, j)._Y, 0.0f, 1.0f);

			weights.At(i, j)._Y = CatalystBaseMath::Clamp<float>(weights.At(i, j)._Y - sandLayerWeight, 0.0f, 1.0f);
			weights.At(i, j)._Y = CatalystBaseMath::SmoothStep<5>(weights.At(i, j)._Y);

			weights.At(i, j)._Z = 0.0f;

			//Determine the weight of the snow layer.
			if (worldHeight < 950.0f)
			{
				weights.At(i, j)._W = 0.0f;
			}

			else if (worldHeight > 1'050.0f)
			{
				weights.At(i, j)._W = 1.0f;
			}

			else
			{
				weights.At(i, j)._W = (worldHeight - 950.0f) * 0.01f;
			}
		}
	}

	TerrainPatchInformation information;

	information._X = 0;
	information._Y = 0;
	information._NormalHeightMap = properties;
	information._LayerWeightsMap = weights;
	information._Material = ResourceLoader::GetTerrainMaterial(HashString("DefaultTerrainMaterial"));
	information._DisplacementInformation._FirstLayerDisplacement = 1.25f;
	information._DisplacementInformation._SecondLayerDisplacement = 0.1f;
	information._DisplacementInformation._ThirdLayerDisplacement = 1.0f;
	information._DisplacementInformation._FourthLayerDisplacement = 1.0f;
	information._DisplacementInformation._FifthLayerDisplacement = 1.0f;

	TerrainSystem::Instance->AddTerrainPatch(std::move(information));
}