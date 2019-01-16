//Header file.
#include <World/ClairvoyantDebrisVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantVegetationPlacement.h>
#include <World/ClairvoyantWorldConstants.h>

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
		properties._Density = 0.0003f; //0.000025f step
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			ClairvoyantVegetationPlacement::TransformationGenerationProperties properties;

			properties._Rotation = Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f));
			properties._MinimumScale = 0.1f;
			properties._MaximumScale = 0.2f;
			properties._Material = ClairvoyantTerrainMaterial::Stone_1;

			return ClairvoyantVegetationPlacement::GenerateTransformation(properties, box, transformation);
		};

		VegetationSystem::Instance->AddDebrisVegetationType(	properties,
																ResourceLoader::GetPhysicalModel(HashString("DebrisVegetation_Branches_1_Model")),
																ResourceLoader::GetPhysicalMaterial(HashString("DebrisVegetation_Branches_1_Material")));
	}
}