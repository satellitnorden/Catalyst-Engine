//Header file.
#include <World/ClairvoyantGrassVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldUtilities.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

//Clairvoyant grass vegetation architect constants.
namespace ClairvoyantGrassVegetationArchitectConstants
{
	constexpr float MEDIUM_DETAIL_DISTANCE{ 62.5f };
	constexpr float LOW_DETAIL_DISTANCE{ 125.0f };
}

/*
*	Initializes the Clairvoyant grass vegetation architect.
*/
void ClairvoyantGrassVegetationArchitect::Initialize() NOEXCEPT
{
	{
		//Add the common fern grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantGrassVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantGrassVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._CutoffDistance = 500.0f;
		properties._Density = 50;
		properties._Thickness = 1.0f;
		properties._WindModulatorFactor = 0.05f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(true, false, false, false, 0.5f, 0.0f, 0.035f, 0.07f, Vector3(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("CommonFernGrassMaterial")) };

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("HighDetailCommonFernModel")), material);
	}

	{
		//Add the default grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantGrassVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantGrassVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._Density = 1'500;
		properties._Thickness = 0.25f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(true, false, false, true, 0.5f, 0.5f, 0.2f, 0.4f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("DefaultGrassMaterial")) };

		properties._CutoffDistance = 250.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("LowDetailDefaultGrassModel")), material);

		properties._CutoffDistance = 125.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("MediumDetailDefaultGrassModel")), material);

		properties._CutoffDistance = 62.5f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("HighDetailDefaultGrassModel")), material);
	}

	{
		//Add the sand grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantGrassVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantGrassVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._Density = 225;
		properties._Thickness = 0.25f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(false, true, false, false, 0.5f, 0.5f, 0.2f, 0.4f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("SandGrassMaterial")) };

		properties._CutoffDistance = 250.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("LowDetailDefaultGrassModel")), material);

		properties._CutoffDistance = 125.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("MediumDetailDefaultGrassModel")), material);

		properties._CutoffDistance = 62.5f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("HighDetailDefaultGrassModel")), material);
	}

	{
		//Add the snow grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantGrassVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantGrassVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._Density = 250;
		properties._Thickness = 0.25f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(false, false, true, false, 0.5f, 0.5f, 0.2f, 0.4f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("SnowGrassMaterial")) };

		properties._CutoffDistance = 250.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("LowDetailDefaultGrassModel")), material);

		properties._CutoffDistance = 125.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("MediumDetailDefaultGrassModel")), material);

		properties._CutoffDistance = 62.5f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("HighDetailDefaultGrassModel")), material);
	}
}