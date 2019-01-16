//Header file.
#include <World/ClairvoyantGrassVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantVegetationPlacement.h>
#include <World/ClairvoyantWorldConstants.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

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
			ClairvoyantVegetationPlacement::TransformationGenerationProperties properties;

			properties._Rotation = Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f));
			properties._MinimumScale = 0.03f;
			properties._MaximumScale = 0.06f;
			properties._Material = ClairvoyantTerrainMaterial::Grass_2;

			return ClairvoyantVegetationPlacement::GenerateTransformation(properties, box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Fern_1_Material")) };

		properties._CutoffDistance = ClairvoyantWorldConstants::PLANTS_CUTOFF_DISTANCE;
		properties._Density = 0.0045f; //0.00025f step

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Fern_1_Model")), material);
	}

	{
		//Add the fern 2 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.5f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			ClairvoyantVegetationPlacement::TransformationGenerationProperties properties;

			properties._Rotation = Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f));
			properties._MinimumScale = 0.5f;
			properties._MaximumScale = 1.0f;
			properties._Material = ClairvoyantTerrainMaterial::Grass_2;

			return ClairvoyantVegetationPlacement::GenerateTransformation(properties, box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Fern_2_Material")) };

		properties._CutoffDistance = ClairvoyantWorldConstants::GRASS_CUTOFF_DISTANCE;
		properties._Density = 0.05f; //0.025f step
		properties._WindModulatorFactor = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_MODULATOR_FACTOR, MAXIMUM_WIND_MODULATOR_FACTOR);

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);
	}

	{
		//Add the grass 1 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.5f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			ClairvoyantVegetationPlacement::TransformationGenerationProperties properties;

			properties._Rotation = Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f));
			properties._MinimumScale = 0.5f;
			properties._MaximumScale = 1.0f;
			properties._Material = ClairvoyantTerrainMaterial::Grass_1;

			return ClairvoyantVegetationPlacement::GenerateTransformation(properties, box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Grass_1_Material")) };

		properties._CutoffDistance = ClairvoyantWorldConstants::GRASS_CUTOFF_DISTANCE;
		properties._Density = 0.325f; //0.025f step
		properties._WindModulatorFactor = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_MODULATOR_FACTOR, MAXIMUM_WIND_MODULATOR_FACTOR);

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Default_Model")), material);
	}
}