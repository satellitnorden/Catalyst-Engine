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
	constexpr float FLOWER_CUTOFF_DISTANCE{ 500.0f };
	constexpr float GRASS_CUTOFF_DISTANCE{ 250.0f };
}

/*
*	Initializes the Clairvoyant grass vegetation architect.
*/
void ClairvoyantGrassVegetationArchitect::Initialize() NOEXCEPT
{
	{
		//Add the flower 1 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.75f;
		properties._WindModulatorFactor = 0.1f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.5f, 1.0f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Flower_1_Material")) };

		properties._CutoffDistance = ClairvoyantGrassVegetationArchitectConstants::FLOWER_CUTOFF_DISTANCE;
		properties._Density = 0.005f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);
	}

	{
		//Add the flower 2 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.75f;
		properties._WindModulatorFactor = 0.1f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.5f, 1.0f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Flower_2_Material")) };

		properties._CutoffDistance = ClairvoyantGrassVegetationArchitectConstants::FLOWER_CUTOFF_DISTANCE;
		properties._Density = 0.005f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);
	}

	{
		//Add the flower 3 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.75f;
		properties._WindModulatorFactor = 0.1f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.5f, 1.0f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Flower_3_Material")) };

		properties._CutoffDistance = ClairvoyantGrassVegetationArchitectConstants::FLOWER_CUTOFF_DISTANCE;
		properties._Density = 0.005f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);
	}

	{
		//Add the flower 4 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.75f;
		properties._WindModulatorFactor = 0.1f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.5f, 1.0f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Flower_4_Material")) };

		properties._CutoffDistance = ClairvoyantGrassVegetationArchitectConstants::FLOWER_CUTOFF_DISTANCE;
		properties._Density = 0.005f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);
	}

	{
		//Add the flower 5 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.75f;
		properties._WindModulatorFactor = 0.1f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.5f, 1.0f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Flower_5_Material")) };

		properties._CutoffDistance = ClairvoyantGrassVegetationArchitectConstants::FLOWER_CUTOFF_DISTANCE;
		properties._Density = 0.005f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);
	}

	{
		//Add the grass 1 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.5f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.5f, 1.0f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Grass_1_Material")) };

		properties._CutoffDistance = ClairvoyantGrassVegetationArchitectConstants::GRASS_CUTOFF_DISTANCE;
		properties._Density = 0.1f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);

		properties._CutoffDistance *= 0.5f;
		properties._Density *= 2.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);

		properties._CutoffDistance *= 0.5f;
		properties._Density *= 2.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);
	}
}