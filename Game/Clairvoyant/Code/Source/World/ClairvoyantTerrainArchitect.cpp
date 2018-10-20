//Header file.
#include <World/ClairvoyantTerrainArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldConstants.h>

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
	constexpr float TERRAIN_MINIMUM_HEIGHT{ 2'000.0f };
	constexpr float TERRAIN_MAXIMUM_HEIGHT{ 32'000.0f };

	constexpr float GRASS_RANGE{ 10'000.0f };
}

/*
*	Returns the random offset.
*/
float GetRandomOffset(const uint8 index) NOEXCEPT
{
	constexpr float MAXIMUM_OFFSET{ 10.0f };

	switch (index)
	{
		case 0:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 1:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 2:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 3:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 4:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 5:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 6:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 7:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

			return randomOffset;
		}

		case 8:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 9:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 10:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 11:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 12:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}
	}

	ASSERT(false, "You should add a case here. ):");

	return 0.0f;
}

/*
*	Generates the height.
*/
void ClairvoyantTerrainArchitect::GenerateHeight(const TerrainProperties &properties, const Vector3 &worldPosition, float *const RESTRICT height) NOEXCEPT
{
	//Calculate the coordinates.
	const float coordinateX{ worldPosition._X / 16'000.0f };
	const float coordinateY{ worldPosition._Z / 16'000.0f };

	//Generate the noise.
	*height = PerlinNoiseGenerator::GenerateNoise(coordinateX, coordinateY, GetRandomOffset(0));

	float frequency{ 1.0f };
	float influence{ 1.0f };

	for (uint8 i = 0; i < 9; ++i)
	{
		frequency *= 2.0f;
		influence *= 0.5f;

		*height += CatalystBaseMath::SmoothStep<2>((PerlinNoiseGenerator::GenerateNoise(coordinateX * frequency, coordinateY * frequency, GetRandomOffset(i + 1)) + 1.0f) * 0.5f) * influence;
	}

	//Calculate the height influence.
	const float landscapeCoordinateX{ worldPosition._X / 64'000.0f };
	const float landscapeCoordinateY{ worldPosition._Z / 64'000.0f };

	float heightInfluence{ ((PerlinNoiseGenerator::GenerateNoise(landscapeCoordinateX, landscapeCoordinateY, GetRandomOffset(10)) + 1.0f) * 0.5f) };
	float flatness{ ((PerlinNoiseGenerator::GenerateNoise(landscapeCoordinateX, landscapeCoordinateY, GetRandomOffset(11)) + 1.0f) * 0.5f) };
	flatness *= flatness;

	//Apply the height.
	*height *= CatalystBaseMath::LinearlyInterpolate(ClairvoyantTerrainGenerationConstants::TERRAIN_MINIMUM_HEIGHT, ClairvoyantTerrainGenerationConstants::TERRAIN_MAXIMUM_HEIGHT, flatness) * heightInfluence;
}

/*
*	Generates the layer weights.
*/
void ClairvoyantTerrainArchitect::GenerateLayerWeights(const TerrainProperties &properties, const Vector3 &worldPosition, const Vector3 &normal, Vector4 *const RESTRICT layerWeights) NOEXCEPT
{
	{
		//Determine the weight of the first grass layer.
		layerWeights->_X = 0.0f;
	}

	{
		//Determine the weight of the second grass layer.
		if (worldPosition._Y < 0.0f)
		{
			layerWeights->_Y = 0.0f;
		}

		else if (worldPosition._Y > 1'000.0f)
		{
			layerWeights->_Y = 1.0f;
		}

		else
		{
			layerWeights->_Y = worldPosition._Y * 0.001f;
		}
	}

	//Determine the weight of the rock layer.
	layerWeights->_Z = CatalystBaseMath::SmoothStep<3>(1.0f - CatalystBaseMath::Clamp<float>(Vector3::DotProduct(normal, Vector3::UP), 0.0f, 1.0f));

	//Determine the weight of the snow layer.
	if (worldPosition._Y < ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)
	{
		layerWeights->_W = 0.0f;
	}

	else if (worldPosition._Y > ClairvoyantWorldConstants::SNOW_BLEND_END)
	{
		layerWeights->_W = CatalystBaseMath::Maximum<float>(1.0f - layerWeights->_Z, 0.0f);
	}

	else
	{
		layerWeights->_W = CatalystBaseMath::Maximum<float>(((worldPosition._Y - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN) * 0.001f) - layerWeights->_Z, 0.0f);
	}
}

/*
*	Generates the patch properties.
*/
void ClairvoyantTerrainArchitect::GeneratePatchProperties(const TerrainProperties &properties, const Vector3 &worldPosition, TerrainMaterial *const RESTRICT material, TerrainDisplacementInformation *const RESTRICT displacementInformation) NOEXCEPT
{
	*material = ResourceLoader::GetTerrainMaterial(HashString("DefaultTerrainMaterial"));

	displacementInformation->_FirstLayerDisplacement = 0.5f;
	displacementInformation->_SecondLayerDisplacement = 0.1f;
	displacementInformation->_ThirdLayerDisplacement = 0.5f;
	displacementInformation->_FourthLayerDisplacement = 1.0f;
	displacementInformation->_FifthLayerDisplacement = 0.5f;
}