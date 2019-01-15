//Header file.
#include <World/ClairvoyantDebrisVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldConstants.h>
#include <World/ClairvoyantWorldUtilities.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

/*
*	Initializes the Clairvoyant debris vegetation architect.
*/
void ClairvoyantDebrisVegetationArchitect::Initialize() NOEXCEPT
{
	{
		//Add the branches 1 debris vegetation type.
		DebrisVegetationTypeProperties properties;

		properties._CutoffDistance = ClairvoyantWorldConstants::BRANCHES_CUTOFF_DISTANCE;
		properties._Density = 0.000225f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.1f, 0.2f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		VegetationSystem::Instance->AddDebrisVegetationType(	properties,
																ResourceLoader::GetPhysicalModel(HashString("DebrisVegetation_Branches_1_Model")),
																ResourceLoader::GetPhysicalMaterial(HashString("DebrisVegetation_Branches_1_Material")));
	}
}