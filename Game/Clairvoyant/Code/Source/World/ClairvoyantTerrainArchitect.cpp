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

//Systems.
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainProperties.h>

//Clairvoyant terrain generation constants.
namespace ClairvoyantTerrainGenerationConstants
{
	constexpr float TERRAIN_MINIMUM_HEIGHT{ 2'000.0f };
	constexpr float TERRAIN_MAXIMUM_HEIGHT{ 32'000.0f };
}

/*
*	Returns the random offset.
*/
float GetRandomOffset(const uint8 index) NOEXCEPT
{
	constexpr float MAXIMUM_OFFSET{ 1'000.0f };

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
	const float coordinateX{ worldPosition._X / 32'000.0f };
	const float coordinateY{ worldPosition._Z / 32'000.0f };

	//Generate the noise.
	*height = PerlinNoiseGenerator::GenerateNoise(coordinateX, coordinateY, GetRandomOffset(0));

	float frequency{ 1.0f };
	float influence{ 1.0f };

	for (uint8 i = 0; i < 10; ++i)
	{
		frequency *= 2.0f;
		influence *= 0.5f;

		*height += CatalystBaseMath::SmoothStep<2>((PerlinNoiseGenerator::GenerateNoise(coordinateX * frequency, coordinateY * frequency, GetRandomOffset(i + 1)) + 1.0f) * 0.5f) * influence;
	}

	//Calculate the height influence.
	const float landscapeCoordinateX{ worldPosition._X / 32'000.0f };
	const float landscapeCoordinateY{ worldPosition._Z / 32'000.0f };

	float heightInfluence{ ((PerlinNoiseGenerator::GenerateNoise(landscapeCoordinateX, landscapeCoordinateY, GetRandomOffset(11)) + 1.0f) * 0.5f) };
	float flatness{ ((PerlinNoiseGenerator::GenerateNoise(landscapeCoordinateX, landscapeCoordinateY, GetRandomOffset(12)) + 1.0f) * 0.5f) };

	//Apply the height.
	*height *= CatalystBaseMath::LinearlyInterpolate(ClairvoyantTerrainGenerationConstants::TERRAIN_MINIMUM_HEIGHT, ClairvoyantTerrainGenerationConstants::TERRAIN_MAXIMUM_HEIGHT, flatness) * heightInfluence;
}

/*
*	Generates the layer weights.
*/
void ClairvoyantTerrainArchitect::GenerateLayerWeights(const TerrainProperties &properties, const Vector3 &worldPosition, Vector4 *const RESTRICT layerWeights) NOEXCEPT
{
	float height;
	Vector3 normal;
	
	TerrainSystem::Instance->GetTerrainHeightAtPosition(worldPosition, &height);
	TerrainSystem::Instance->GetTerrainNormalAtPosition(worldPosition, &normal);

	{
		layerWeights->_X = 1.0f;
	}

	{
		//Determine the weight of the sand layer.
		if (height < ClairvoyantWorldConstants::SAND_BLEND_BEGIN)
		{
			layerWeights->_Y = 1.0f;
		}

		else if (height > ClairvoyantWorldConstants::SAND_BLEND_END)
		{
			layerWeights->_Y = 0.0f;
		}

		else
		{
			layerWeights->_Y = 1.0f - (height - ClairvoyantWorldConstants::SAND_BLEND_BEGIN) / (ClairvoyantWorldConstants::SAND_BLEND_END - ClairvoyantWorldConstants::SAND_BLEND_BEGIN);
		}
	}

	//Determine the weight of the snow layer.
	if (height < ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)
	{
		layerWeights->_Z = 0.0f;
	}

	else if (height > ClairvoyantWorldConstants::SNOW_BLEND_END)
	{
		layerWeights->_Z = 1.0f;
	}

	else
	{
		layerWeights->_Z = (height - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN) / (ClairvoyantWorldConstants::SNOW_BLEND_END - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN);
	}

	//Determine the weight of the rock layer.
	layerWeights->_W = 1.0f - CatalystBaseMath::Clamp<float>(Vector3::DotProduct(normal, Vector3::UP), 0.0f, 1.0f);
}

/*
*	Generates the patch properties.
*/
void ClairvoyantTerrainArchitect::GeneratePatchProperties(const TerrainProperties &properties, const Vector3 &worldPosition, TerrainMaterial *const RESTRICT material) NOEXCEPT
{
	*material = ResourceLoader::GetTerrainMaterial(HashString("DefaultTerrainMaterial"));
}