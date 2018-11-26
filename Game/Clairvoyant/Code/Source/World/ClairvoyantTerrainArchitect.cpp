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
	constexpr float TERRAIN_HEIGHT{ 10'000.0f };
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

		case 13:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 14:
		{
			static float randomOffset{ CatalystBaseMath::RandomFloatInRange(0.0f, MAXIMUM_OFFSET) };

			return randomOffset;
		}

		case 15:
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
	//Define constants.
	constexpr float FREQUENCY_MULTIPLIER{ 2.5f };

	constexpr float LARGE_MOUNTAINS_BASE_INFLUENCE{ 15.0f };
	constexpr float MEDIUM_MOUNTAINS_BASE_INFLUENCE{ LARGE_MOUNTAINS_BASE_INFLUENCE * 0.5f };
	constexpr float SMALL_MOUNTAINS_BASE_INFLUENCE{ MEDIUM_MOUNTAINS_BASE_INFLUENCE * 0.5f };

	//Calculate the coordinates.
	const float coordinateX{ worldPosition._X / 100'000.0f };
	const float coordinateY{ worldPosition._Z / 100'000.0f };

	//Calculate properties.
	const float flatness{ PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX, coordinateY, GetRandomOffset(0)) };
	const float influenceMultiplier{ CatalystBaseMath::LinearlyInterpolate(0.425f, 0.525f, PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX, coordinateY, GetRandomOffset(2))) };
	const float spikinessMultiplier{ CatalystBaseMath::LinearlyInterpolate(0.825f, 1.0f, PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX, coordinateY, GetRandomOffset(3))) };
	const float turbulence{ PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX, coordinateY, GetRandomOffset(4)) };

	//General landscape shape.
	*height = PerlinNoiseGenerator::GenerateNoise(coordinateX, coordinateY, GetRandomOffset(5));

	//Large mountains.
	float largeMountainsNoise{ PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX, coordinateY, GetRandomOffset(6)) };

	for (uint8 i{ 0 }; i < 2; ++i)
	{
		largeMountainsNoise *= largeMountainsNoise;
	}

	largeMountainsNoise = CatalystBaseMath::LinearlyInterpolate(largeMountainsNoise, largeMountainsNoise * largeMountainsNoise, spikinessMultiplier);

	*height += largeMountainsNoise * LARGE_MOUNTAINS_BASE_INFLUENCE;

	//Medium mountains.
	float mediumMountainsNoise{ PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 2.0f, coordinateY * 2.0f, GetRandomOffset(7)) };

	for (uint8 i{ 0 }; i < 2; ++i)
	{
		mediumMountainsNoise *= mediumMountainsNoise;
	}

	mediumMountainsNoise = CatalystBaseMath::LinearlyInterpolate(mediumMountainsNoise, mediumMountainsNoise * mediumMountainsNoise, spikinessMultiplier);

	*height += mediumMountainsNoise * MEDIUM_MOUNTAINS_BASE_INFLUENCE;

	//Small mountains.
	float smallMountainsNoise{ PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * 4.0f, coordinateY * 4.0f, GetRandomOffset(8)) };

	for (uint8 i{ 0 }; i < 2; ++i)
	{
		smallMountainsNoise *= smallMountainsNoise;
	}

	smallMountainsNoise = CatalystBaseMath::LinearlyInterpolate(smallMountainsNoise, smallMountainsNoise * smallMountainsNoise, spikinessMultiplier);

	*height += smallMountainsNoise * SMALL_MOUNTAINS_BASE_INFLUENCE;

	//Large plateaus.
	float frequency{ 1.0f * FREQUENCY_MULTIPLIER * FREQUENCY_MULTIPLIER };
	float influence{ 1.0f * influenceMultiplier * influenceMultiplier };

	*height += CatalystBaseMath::SmoothStep<8>(PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * frequency, coordinateY * frequency, GetRandomOffset(9))) * influence;

	//Medium plateaus.
	frequency *= FREQUENCY_MULTIPLIER;
	influence *= influenceMultiplier;

	*height += CatalystBaseMath::SmoothStep<7>(PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * frequency, coordinateY * frequency, GetRandomOffset(10))) * influence;

	//Small plateaus.
	frequency *= FREQUENCY_MULTIPLIER;
	influence *= influenceMultiplier;

	*height += CatalystBaseMath::SmoothStep<6>(PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * frequency, coordinateY * frequency, GetRandomOffset(11))) * influence;

	//Large hills.
	frequency *= FREQUENCY_MULTIPLIER;
	influence *= influenceMultiplier;

	*height += CatalystBaseMath::SmoothStep<5>(PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * frequency, coordinateY * frequency, GetRandomOffset(12))) * influence;

	//Medium hills.
	frequency *= FREQUENCY_MULTIPLIER;
	influence *= influenceMultiplier;

	*height += CatalystBaseMath::SmoothStep<4>(PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * frequency, coordinateY * frequency, GetRandomOffset(13))) * influence;

	//Small hills.
	frequency *= FREQUENCY_MULTIPLIER;
	influence *= influenceMultiplier;

	*height += CatalystBaseMath::SmoothStep<3>(PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * frequency, coordinateY * frequency, GetRandomOffset(14))) * influence;

	//Turbulence.
	frequency *= FREQUENCY_MULTIPLIER;
	influence *= influenceMultiplier;

	*height += CatalystBaseMath::SmoothStep<2>(PerlinNoiseGenerator::GenerateNormalizedNoise(coordinateX * frequency, coordinateY * frequency, GetRandomOffset(15))) * influence * turbulence;

	//Apply the height.
	*height *= ClairvoyantTerrainGenerationConstants::TERRAIN_HEIGHT * flatness;
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