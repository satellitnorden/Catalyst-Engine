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
	{
		//Add the rock 1 solid vegetation type.
		SolidVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantSolidVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantSolidVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._CutoffDistance = 1'000.0f;
		properties._Density = 0.0001f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.7f, 0.0f, 0.1f, 0.2f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		StaticArray<PhysicalModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetPhysicalModel(HashString("SolidVegetation_Rock_1_Model"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetPhysicalModel(HashString("SolidVegetation_Rock_1_Model"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetPhysicalModel(HashString("SolidVegetation_Rock_1_Model"));

		PhysicalMaterial material{ ResourceLoader::GetPhysicalMaterial(HashString("SolidVegetation_Rock_1_Material")) };

		VegetationSystem::Instance->AddSolidVegetationType(properties, models, material);
	}
}