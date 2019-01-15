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
			properties._AxisAlignedBoundingBox = &box;
			properties._MinimumAngle = 0.5f;
			properties._MinimumScale = 0.03f;
			properties._MaximumScale = 0.06f;
			properties._Transformation = transformation;

			return ClairvoyantVegetationPlacement::GenerateTransformation(properties);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Fern_1_Material")) };

		properties._CutoffDistance = ClairvoyantWorldConstants::PLANTS_CUTOFF_DISTANCE;
		properties._Density = 0.0035f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetation_Fern_1_Model")), material);
	}

	{
		//Add the grass 1 grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 0.5f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			ClairvoyantVegetationPlacement::TransformationGenerationProperties properties;

			properties._Rotation = Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f));
			properties._AxisAlignedBoundingBox = &box;
			properties._MinimumAngle = 0.5f;
			properties._MinimumScale = 0.5f;
			properties._MaximumScale = 1.0f;
			properties._Transformation = transformation;

			return ClairvoyantVegetationPlacement::GenerateTransformation(properties);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("GrassVegetation_Grass_1_Material")) };

		properties._CutoffDistance = ClairvoyantWorldConstants::GRASS_CUTOFF_DISTANCE;
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