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
		//Add the default grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantGrassVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantGrassVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._Density = 1'000;
		properties._Thickness = 0.5f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.25f, 0.2f, 0.4f, Vector3<float>(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("DefaultGrassVegetationMaterial")) };

		properties._CutoffDistance = 250.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("LowDetailDefaultGrassVegetationModel")), material);

		properties._CutoffDistance = 125.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("MediumDetailDefaultGrassVegetationModel")), material);

		properties._CutoffDistance = 62.5f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("HighDetailDefaultGrassVegetationModel")), material);
	}
}