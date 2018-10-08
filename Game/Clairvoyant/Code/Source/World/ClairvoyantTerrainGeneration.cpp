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

	constexpr float LANDSCAPE_RANGE{ 100'000.0f };
	constexpr float LANDSCAPE_INFLUENCE{ 0.5f };

	constexpr float LARGE_MOUNTAIN_RANGE{ 10'000.0f };
	constexpr float LARGE_MOUNTAIN_INFLUENCE{ 2.0f };

	constexpr float SMALL_MOUNTAIN_RANGE{ 1'000.0f };
	constexpr float SMALL_MOUNTAIN_INFLUENCE{ 0.1f };

	constexpr float LARGE_HILL_RANGE{ 100.0f };
	constexpr float LARGE_HILL_INFLUENCE{ 0.0025f };

	constexpr float SMALL_HILL_RANGE{ 10.0f };
	constexpr float SMALL_HILL_INFLUENCE{ 0.00035f };

	constexpr float GRASS_RANGE{ 1'000.0f };
}

namespace ClairvoyantTerrainGeneration
{

	/*
	*	Returns the random offset.
	*/
	float GetRandomOffset(const uint8 index) NOEXCEPT
	{
		switch (index)
		{
			case 0:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}

			case 1:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}

			case 2:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}

			case 3:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}

			case 4:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}

			case 5:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}

			case 6:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}

			case 7:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}

			case 8:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}

			case 9:
			{
				static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

				return randomOffset;
			}
		}

		ASSERT(false, "You should add a case here. ):");

		return 0.0f;
	}

	/*
	*	Generates the height.
	*/
	void GenerateHeight(const TerrainProperties &properties, const Vector3 &worldPosition, float *const RESTRICT height) NOEXCEPT
	{
		//Calculate the landscape coordinate.
		float landscapeCoordinateX{ worldPosition._X / ClairvoyantTerrainGenerationConstants::LANDSCAPE_RANGE };
		float landscapeCoordinateY{ worldPosition._Z / ClairvoyantTerrainGenerationConstants::LANDSCAPE_RANGE };

		{
			//Apply the lancscape range.
			*height = PerlinNoiseGenerator::GenerateNoise(landscapeCoordinateX, landscapeCoordinateY, GetRandomOffset(0)) * ClairvoyantTerrainGenerationConstants::LANDSCAPE_INFLUENCE;
		}

		{
			//Apply the large mountain range.
			float rangeCoordinateX{ worldPosition._X / ClairvoyantTerrainGenerationConstants::LARGE_MOUNTAIN_RANGE };
			float rangeCoordinateY{ worldPosition._Z / ClairvoyantTerrainGenerationConstants::LARGE_MOUNTAIN_RANGE };

			const float noise{ (PerlinNoiseGenerator::GenerateNoise(rangeCoordinateX, rangeCoordinateY, GetRandomOffset(1)) + 1.0f) * 0.5f };
			const float influence{ (PerlinNoiseGenerator::GenerateNoise(landscapeCoordinateX, landscapeCoordinateY, GetRandomOffset(2)) + 1.0f) * 0.5f };

			*height += CatalystBaseMath::PowerOf(noise, 4) * influence * ClairvoyantTerrainGenerationConstants::LARGE_MOUNTAIN_INFLUENCE;
		}
		
		{
			//Apply the small mountain range.
			float rangeCoordinateX{ worldPosition._X / ClairvoyantTerrainGenerationConstants::SMALL_MOUNTAIN_RANGE };
			float rangeCoordinateY{ worldPosition._Z / ClairvoyantTerrainGenerationConstants::SMALL_MOUNTAIN_RANGE };

			const float noise{ (PerlinNoiseGenerator::GenerateNoise(rangeCoordinateX, rangeCoordinateY, GetRandomOffset(3)) + 1.0f) * 0.5f };
			const float influence{ (PerlinNoiseGenerator::GenerateNoise(landscapeCoordinateX, landscapeCoordinateY, GetRandomOffset(4)) + 1.0f) * 0.5f };

			*height += noise * influence * ClairvoyantTerrainGenerationConstants::SMALL_MOUNTAIN_INFLUENCE;
		}
		
		{
			//Apply the large hill range.
			float rangeCoordinateX{ worldPosition._X / ClairvoyantTerrainGenerationConstants::LARGE_HILL_RANGE };
			float rangeCoordinateY{ worldPosition._Z / ClairvoyantTerrainGenerationConstants::LARGE_HILL_RANGE };

			const float noise{ (PerlinNoiseGenerator::GenerateNoise(rangeCoordinateX, rangeCoordinateY, GetRandomOffset(5)) + 1.0f) * 0.5f };
			const float influence{ (PerlinNoiseGenerator::GenerateNoise(landscapeCoordinateX, landscapeCoordinateY, GetRandomOffset(6)) + 1.0f) * 0.5f };

			*height += noise * influence * ClairvoyantTerrainGenerationConstants::LARGE_HILL_INFLUENCE;
		}

		{
			//Apply the large hill range.
			float rangeCoordinateX{ worldPosition._X / ClairvoyantTerrainGenerationConstants::SMALL_HILL_RANGE };
			float rangeCoordinateY{ worldPosition._Z / ClairvoyantTerrainGenerationConstants::SMALL_HILL_RANGE };

			const float noise{ (PerlinNoiseGenerator::GenerateNoise(rangeCoordinateX, rangeCoordinateY, GetRandomOffset(7)) + 1.0f) * 0.5f };
			const float influence{ (PerlinNoiseGenerator::GenerateNoise(landscapeCoordinateX, landscapeCoordinateY, GetRandomOffset(8)) + 1.0f) * 0.5f };

			*height += noise * influence * ClairvoyantTerrainGenerationConstants::SMALL_HILL_INFLUENCE;
		}

		//Apply the height.
		*height *= ClairvoyantTerrainGenerationConstants::TERRAIN_HEIGHT;
	}

	/*
	*	Generates the layer weights.
	*/
	void GenerateLayerWeights(const TerrainProperties &properties, const Vector3 &worldPosition, const Vector3 &normal, Vector4 *const RESTRICT layerWeights) NOEXCEPT
	{
		{
			//Determine the weight of the first grass layer.
			if (worldPosition._Y < 0.0f)
			{
				layerWeights->_X = 0.0f;
			}

			else if (worldPosition._Y > 1'000.0f)
			{
				layerWeights->_X = 1.0f;
			}

			else
			{
				layerWeights->_X = worldPosition._Y * 0.001f;
			}
		}

		{
			//Determine the weight of the second grass layer.
			float xCoordinate{ worldPosition._X / ClairvoyantTerrainGenerationConstants::GRASS_RANGE };
			float yCoordinate{ worldPosition._Z / ClairvoyantTerrainGenerationConstants::GRASS_RANGE };

			const float noise{ PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, GetRandomOffset(9)) };

			layerWeights->_Y = (noise + 1.0f) * 0.5f;
		}

		//Determine the weight of the rock layer.
		layerWeights->_Z = CatalystBaseMath::SmoothStep<1>(1.0f - CatalystBaseMath::Clamp<float>(Vector3::DotProduct(normal, Vector3::UP) - 0.1f, 0.0f, 1.0f));

		//Determine the weight of the snow layer.
		if (worldPosition._Y < 7'000.0f)
		{
			layerWeights->_W = 0.0f;
		}

		else if (worldPosition._Y > 8'000.0f)
		{
			layerWeights->_W = 1.0f;
		}

		else
		{
			layerWeights->_W = (worldPosition._Y - 7'000.0f) * 0.001f;
		}
	}

	/*
	*	Generates the patch properties.
	*/
	void GeneratePatchProperties(const TerrainProperties &properties, const Vector3 &worldPosition, TerrainMaterial *const RESTRICT material, TerrainDisplacementInformation *const RESTRICT displacementInformation) NOEXCEPT
	{
		*material = ResourceLoader::GetTerrainMaterial(HashString("DefaultTerrainMaterial"));

		displacementInformation->_FirstLayerDisplacement = 0.5f;
		displacementInformation->_SecondLayerDisplacement = 0.1f;
		displacementInformation->_ThirdLayerDisplacement = 0.5f;
		displacementInformation->_FourthLayerDisplacement = 1.0f;
		displacementInformation->_FifthLayerDisplacement = 0.5f;
	}

}