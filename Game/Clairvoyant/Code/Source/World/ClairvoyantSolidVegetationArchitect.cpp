//Header file.
#include <World/ClairvoyantSolidVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldConstants.h>
#include <World/ClairvoyantWorldUtilities.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

/*
*	Initializes the Clairvoyant solid vegetation architect.
*/
void ClairvoyantSolidVegetationArchitect::Initialize() NOEXCEPT
{
	{
		//Add the rock 1 solid vegetation type.
		SolidVegetationTypeProperties properties;

		properties._CutoffDistance = ClairvoyantWorldConstants::ROCKS_CUTOFF_DISTANCE;
		properties._Density = 0.00015f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.7f, 0.0f, 0.1f, 0.2f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		VegetationSystem::Instance->AddSolidVegetationType(	properties,
															ResourceLoader::GetPhysicalModel(HashString("SolidVegetation_Rock_1_Model")),
															ResourceLoader::GetPhysicalMaterial(HashString("SolidVegetation_Rock_1_Material")));
	}

	/*
	{
		//Add the rock 1 solid vegetation type.
		SolidVegetationTypeProperties properties;

		properties._CutoffDistance = ClairvoyantWorldConstants::ROCKS_CUTOFF_DISTANCE;
		properties._Density = 0.001f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 1.0f, 2.0f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		VegetationSystem::Instance->AddSolidVegetationType(properties,
			ResourceLoader::GetPhysicalModel(HashString("DebrisVegetation_Branches_1_Model")),
			ResourceLoader::GetPhysicalMaterial(HashString("DebrisVegetation_Branches_1_Material")));
	}
	*/
}