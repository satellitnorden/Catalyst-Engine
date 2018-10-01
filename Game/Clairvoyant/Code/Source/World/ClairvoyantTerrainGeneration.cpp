//Header file.
#include <World/ClairvoyantTerrainGeneration.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoiseGenerator.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Terrain.
#include <Terrain/TerrainDisplacementInformation.h>
#include <Terrain/TerrainProperties.h>

//Clairvoyant terrain generation constants.
namespace ClairvoyantTerrainGenerationConstants
{
	constexpr float TERRAIN_HEIGHT{ 10'000.0f };

	constexpr float FIRST_RANGE{ 10'000'000'000.0f };
	constexpr float FIRST_INCLUENCE{ 1.0f };

	constexpr float SECOND_RANGE{ 1'000'000'000.0f };
	constexpr float SECOND_INCLUENCE{ 0.5f };

	constexpr float THIRD_RANGE{ 100'000'000.0f };
	constexpr float THIRD_INCLUENCE{ 0.25f };

	constexpr float FOURTH_RANGE{ 10'000'000.0f };
	constexpr float FOURTH_INCLUENCE{ 0.125f };

	constexpr float FIFTH_RANGE{ 1'000'000.0f };
	constexpr float FIFTH_INCLUENCE{ 0.0625f };

	constexpr float SIXTH_RANGE{ 100'000.0f };
	constexpr float SIXTH_INCLUENCE{ 0.03125f };

	constexpr float SEVENTH_RANGE{ 10'000.0f };
	constexpr float SEVENTH_INCLUENCE{ 0.015625f };

	constexpr float EIGHTH_RANGE{ 1'000.0f };
	constexpr float EIGHTH_INCLUENCE{ 0.0078125f };

	constexpr float NINTH_RANGE{ 100.0f };
	constexpr float NINTH_INCLUENCE{ 0.00390625f };

	constexpr float TENTH_RANGE{ 10.0f };
	constexpr float TENTH_INCLUENCE{ 0.001953125f };
}

namespace ClairvoyantTerrainGeneration
{

	/*
	*	Returns the random offset.
	*/
	float GetRandomOffset() NOEXCEPT
	{
		return 0.0f;
	}

	/*
	*	Generates the height.
	*/
	void GenerateHeight(const TerrainProperties &properties, const Vector3 &worldPosition, float *const RESTRICT height) NOEXCEPT
	{
		//Apply the first range.
		float xCoordinate{ worldPosition._X / ClairvoyantTerrainGenerationConstants::FIRST_RANGE };
		float yCoordinate{ worldPosition._Z / ClairvoyantTerrainGenerationConstants::FIRST_RANGE };

		*height = PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset()) * ClairvoyantTerrainGenerationConstants::FIRST_INCLUENCE;

		//Apply the second range.
		xCoordinate = worldPosition._X / ClairvoyantTerrainGenerationConstants::SECOND_RANGE;
		yCoordinate = worldPosition._Z / ClairvoyantTerrainGenerationConstants::SECOND_RANGE;

		*height += PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset()) * ClairvoyantTerrainGenerationConstants::SECOND_INCLUENCE;

		//Apply the third range.
		xCoordinate = worldPosition._X / ClairvoyantTerrainGenerationConstants::THIRD_RANGE;
		yCoordinate = worldPosition._Z / ClairvoyantTerrainGenerationConstants::THIRD_RANGE;

		*height += PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset()) * ClairvoyantTerrainGenerationConstants::THIRD_INCLUENCE;

		//Apply the fourth range.
		xCoordinate = worldPosition._X / ClairvoyantTerrainGenerationConstants::FOURTH_RANGE;
		yCoordinate = worldPosition._Z / ClairvoyantTerrainGenerationConstants::FOURTH_RANGE;

		*height += PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset()) * ClairvoyantTerrainGenerationConstants::FOURTH_INCLUENCE;

		//Apply the fifth range.
		xCoordinate = worldPosition._X / ClairvoyantTerrainGenerationConstants::FIFTH_RANGE;
		yCoordinate = worldPosition._Z / ClairvoyantTerrainGenerationConstants::FIFTH_RANGE;

		*height += PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset()) * ClairvoyantTerrainGenerationConstants::FIFTH_INCLUENCE;

		//Apply the sixth range.
		xCoordinate = worldPosition._X / ClairvoyantTerrainGenerationConstants::SIXTH_RANGE;
		yCoordinate = worldPosition._Z / ClairvoyantTerrainGenerationConstants::SIXTH_RANGE;

		*height += PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset()) * ClairvoyantTerrainGenerationConstants::SIXTH_INCLUENCE;

		//Apply the seventh range.
		xCoordinate = worldPosition._X / ClairvoyantTerrainGenerationConstants::SEVENTH_RANGE;
		yCoordinate = worldPosition._Z / ClairvoyantTerrainGenerationConstants::SEVENTH_RANGE;

		*height += PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset()) * ClairvoyantTerrainGenerationConstants::SEVENTH_INCLUENCE;

		//Apply the eighth range.
		xCoordinate = worldPosition._X / ClairvoyantTerrainGenerationConstants::EIGHTH_RANGE;
		yCoordinate = worldPosition._Z / ClairvoyantTerrainGenerationConstants::EIGHTH_RANGE;

		*height += PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset()) * ClairvoyantTerrainGenerationConstants::EIGHTH_INCLUENCE;

		//Apply the ninth range.
		xCoordinate = worldPosition._X / ClairvoyantTerrainGenerationConstants::NINTH_RANGE;
		yCoordinate = worldPosition._Z / ClairvoyantTerrainGenerationConstants::NINTH_RANGE;

		*height += PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset()) * ClairvoyantTerrainGenerationConstants::NINTH_INCLUENCE;

		//Apply the height.
		*height *= ClairvoyantTerrainGenerationConstants::TERRAIN_HEIGHT;
	}

	/*
	*	Generates the layer weights.
	*/
	void GenerateLayerWeights(const TerrainProperties &properties, const Vector3 &worldPosition, const Vector3 &normal, Vector4 *const RESTRICT layerWeights) NOEXCEPT
	{
		//Determine the weight of the first grass layer.
		if (worldPosition._Y < 0.0f)
		{
			layerWeights->_X = 0.0f;
		}

		else if (worldPosition._Y > 20.0f)
		{
			layerWeights->_X = 1.0f;
		}

		else
		{
			layerWeights->_X = worldPosition._Y * 0.05f;
		}

		layerWeights->_Y = 0.0f;
		layerWeights->_Z = 0.0f;
		layerWeights->_W = 0.0f;
	}

	/*
	*	Generates the patch properties.
	*/
	void GeneratePatchProperties(const TerrainProperties &properties, const Vector3 &worldPosition, TerrainMaterial *const RESTRICT material, TerrainDisplacementInformation *const RESTRICT displacementInformation) NOEXCEPT
	{
		*material = ResourceLoader::GetTerrainMaterial(HashString("DefaultTerrainMaterial"));

		displacementInformation->_FirstLayerDisplacement = 1.25f;
		displacementInformation->_SecondLayerDisplacement = 0.1f;
		displacementInformation->_ThirdLayerDisplacement = 1.0f;
		displacementInformation->_FourthLayerDisplacement = 2.0f;
		displacementInformation->_FifthLayerDisplacement = 1.0f;
	}

}