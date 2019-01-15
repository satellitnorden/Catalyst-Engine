//Header file.
#include <World/ClairvoyantSolidVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantVegetationPlacement.h>
#include <World/ClairvoyantWorldConstants.h>

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
			ClairvoyantVegetationPlacement::TransformationGenerationProperties properties;

			properties._Rotation = Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f));
			properties._AxisAlignedBoundingBox = &box;
			properties._MinimumAngle = 0.5f;
			properties._MinimumScale = 0.1f;
			properties._MaximumScale = 0.2f;
			properties._Transformation = transformation;

			return ClairvoyantVegetationPlacement::GenerateTransformation(properties);
		};

		VegetationSystem::Instance->AddSolidVegetationType(	properties,
															ResourceLoader::GetPhysicalModel(HashString("SolidVegetation_Rock_1_Model")),
															ResourceLoader::GetPhysicalMaterial(HashString("SolidVegetation_Rock_1_Material")));
	}
}