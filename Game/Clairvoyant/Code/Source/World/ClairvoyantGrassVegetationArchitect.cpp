//Header file.
#include <World/ClairvoyantGrassVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldUtilities.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

//Clairvoyant grass vegetation architect constants.
namespace ClairvoyantGrassVegetationArchitectConstants
{
	constexpr float PLANTS_CUTOFF_DISTANCE{ 1'000.0f };
	constexpr float GRASS_CUTOFF_DISTANCE{ 250.0f };
}

/*
*	Initializes the Clairvoyant grass vegetation architect.
*/
void ClairvoyantGrassVegetationArchitect::Initialize() NOEXCEPT
{
	//Define constants.
	constexpr float MINIMUM_WIND_MODULATOR_FACTOR{ 0.1f };
	constexpr float MAXIMUM_WIND_MODULATOR_FACTOR{ 0.2f };

	{
		//Add the fern 1 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 1.0f;
		properties._WindModulatorFactor = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_MODULATOR_FACTOR, MAXIMUM_WIND_MODULATOR_FACTOR);
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.03f, 0.06f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Fern_1_Material")) };

		properties._CutoffDistance = ClairvoyantGrassVegetationArchitectConstants::PLANTS_CUTOFF_DISTANCE;
		properties._Density = 0.0035f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Fern_1_Model")), material);
	}

	{
		//Add the grass 1 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.5f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.5f, 1.0f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Grass_1_Material")) };

		properties._CutoffDistance = ClairvoyantGrassVegetationArchitectConstants::GRASS_CUTOFF_DISTANCE;
		properties._Density = 0.175f;
		properties._WindModulatorFactor = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_MODULATOR_FACTOR, MAXIMUM_WIND_MODULATOR_FACTOR);

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);

		properties._CutoffDistance *= 0.5f;
		properties._Density *= 2.0f;
		properties._WindModulatorFactor = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_MODULATOR_FACTOR, MAXIMUM_WIND_MODULATOR_FACTOR);

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);

		properties._CutoffDistance *= 0.5f;
		properties._Density *= 2.0f;
		properties._WindModulatorFactor = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_MODULATOR_FACTOR, MAXIMUM_WIND_MODULATOR_FACTOR);

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);
	}
}