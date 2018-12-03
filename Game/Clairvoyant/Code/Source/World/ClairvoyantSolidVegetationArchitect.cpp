//Header file.
#include <World/ClairvoyantSolidVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldUtilities.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

//Clairvoyant grass vegetation architect constants.
namespace ClairvoyantSolidVegetationArchitectConstants
{
	constexpr float MEDIUM_DETAIL_DISTANCE{ 250.5f };
	constexpr float LOW_DETAIL_DISTANCE{ 500.0f };
}

/*
*	Initializes the Clairvoyant solid vegetation architect.
*/
void ClairvoyantSolidVegetationArchitect::Initialize() NOEXCEPT
{
	/*
	{
		//Add the test solid vegetation type.
		SolidVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantSolidVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantSolidVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._CutoffDistance = 1'000.0f;
		properties._Density = 1;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(true, true, false, 0.5f, 2.0f, 0.25f, 0.25f, Vector3(0.0f, 0.0f, 0.0f), box, transformation);
		};

		StaticArray<PhysicalModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetPhysicalModel(HashString("TestModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetPhysicalModel(HashString("TestModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetPhysicalModel(HashString("TestModel"));

		PhysicalMaterial material{ ResourceLoader::GetPhysicalMaterial(HashString("TestMaterial")) };

		VegetationSystem::Instance->AddSolidVegetationType(properties, models, material);
	}
	*/

	{
		//Add the rock volcanic solid vegetation type.
		SolidVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantSolidVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantSolidVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._CutoffDistance = 1'000.0f;
		properties._Density = 50;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(true, true, true, true, 0.8f, 0.0f, 0.15f, 0.3f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		StaticArray<PhysicalModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetPhysicalModel(HashString("LowDetailRockVolcanicModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetPhysicalModel(HashString("MediumDetailRockVolcanicModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetPhysicalModel(HashString("HighDetailRockVolcanicModel"));

		PhysicalMaterial material{ ResourceLoader::GetPhysicalMaterial(HashString("RockVolcanicMaterial")) };

		VegetationSystem::Instance->AddSolidVegetationType(properties, models, material);
	}

	{
		//Add the tree stump solid vegetation type.
		SolidVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantSolidVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantSolidVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._CutoffDistance = 1'000.0f;
		properties._Density = 250;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(true, true, true, false, 0.85f, 0.0f, 0.035f, 0.07f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		StaticArray<PhysicalModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetPhysicalModel(HashString("LowDetailTreeStumpModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetPhysicalModel(HashString("MediumDetailTreeStumpModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetPhysicalModel(HashString("HighDetailTreeStumpModel"));

		PhysicalMaterial material{ ResourceLoader::GetPhysicalMaterial(HashString("TreeStumpMaterial")) };

		VegetationSystem::Instance->AddSolidVegetationType(properties, models, material);
	}
}