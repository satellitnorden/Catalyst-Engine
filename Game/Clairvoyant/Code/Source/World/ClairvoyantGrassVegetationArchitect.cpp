//Header file.
#include <World/ClairvoyantGrassVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldUtilities.h>

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
	{
		//Add the default grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._Thickness = 1.0f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldUtilities::GenerateTransformation(0.5f, 0.0f, 0.5f, 1.0f, Vector3<float>(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("Grass1GrassVegetationMaterial")) };

		properties._CutoffDistance = 250.0f;
		properties._Density = 0.05f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetationModel")), material);

		properties._CutoffDistance *= 0.5f;
		properties._Density *= 2.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetationModel")), material);

		properties._CutoffDistance *= 0.5f;
		properties._Density *= 2.0f;

		VegetationSystem::Instance->AddGrassVegetationType(properties, ResourceLoader::GetGrassVegetationModel(HashString("GrassVegetationModel")), material);
	}
}