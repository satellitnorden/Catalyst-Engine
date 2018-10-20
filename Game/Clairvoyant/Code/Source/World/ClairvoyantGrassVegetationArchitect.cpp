//Header file.
#include <World/ClairvoyantGrassVegetationArchitect.h>

//Clairvoyant.
#include <World/ClairvoyantWorldConstants.h>

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
		//Add the common fern grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantGrassVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantGrassVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._CutoffDistance = 500.0f;
		properties._Density = 50;
		properties._Thickness = 1.0f;
		properties._WindModulatorFactor = 0.05f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantGrassVegetationArchitect::GenerateTransformation(true, false, false, 0.5f, 0.0f, 0.035f, 0.07f, Vector3(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		StaticArray<GrassModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("LowDetailCommonFernModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("MediumDetailCommonFernModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("HighDetailCommonFernModel"));

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("CommonFernGrassMaterial")) };

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);
	}

	{
		//Add the default grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantGrassVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantGrassVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._Density = 2'000;
		properties._Thickness = 0.25f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantGrassVegetationArchitect::GenerateTransformation(true, false, true, 0.5f, 0.5f, 0.25f, 0.5f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};

		StaticArray<GrassModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;
		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("DefaultGrassMaterial")) };

		properties._CutoffDistance = 250.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);

		properties._CutoffDistance = 125.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);

		properties._CutoffDistance = 62.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);
	}

	{
		//Add the snow grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantGrassVegetationArchitectConstants::MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantGrassVegetationArchitectConstants::LOW_DETAIL_DISTANCE;
		properties._Density = 500;
		properties._Thickness = 0.25f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantGrassVegetationArchitect::GenerateTransformation(false, true, false, 0.5f, 0.5f, 0.25f, 0.5f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};

		StaticArray<GrassModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;
		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("SnowGrassMaterial")) };

		properties._CutoffDistance = 250.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);

		properties._CutoffDistance = 125.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);

		properties._CutoffDistance = 62.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);
	}
}

/*
*	Generates a transformation.
*/
bool ClairvoyantGrassVegetationArchitect::GenerateTransformation(const bool grass, const bool snow, const bool underwater, const float minimumAngle, const float height, const float minimumScale, const float maximumScale, const Vector3 &rotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT
{
	//Generate a random position.
	Vector3 position(CatalystBaseMath::RandomFloatInRange(box._Minimum._X, box._Maximum._X), 0.0f, CatalystBaseMath::RandomFloatInRange(box._Minimum._Z, box._Maximum._Z));

	//Get the terrain height.
	if (!TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &position._Y))
	{
		return false;
	}

	//Handle spawning on grass.
	if (!grass)
	{
		if (position._Y < ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)
		{
			return false;
		}

		else if (position._Y < ClairvoyantWorldConstants::SNOW_BLEND_END)
		{
			if (1.0f - CatalystBaseMath::RandomChance((position._Y - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN) / (ClairvoyantWorldConstants::SNOW_BLEND_END - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)))
			{
				return false;
			}
		}
	}

	//Handle spawning on snow.
	if (!snow)
	{
		if (position._Y > ClairvoyantWorldConstants::SNOW_BLEND_END)
		{
			return false;
		}

		else if (position._Y > ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)
		{
			if (CatalystBaseMath::RandomChance((position._Y - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN) / (ClairvoyantWorldConstants::SNOW_BLEND_END - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)))
			{
				return false;
			}
		}
	}

	//Handle spawning underwater.
	if (!underwater)
	{
		if (position._Y < 0.0f)
		{
			return false;
		}
	}

	//Get the terrain normal.
	Vector3 terrainNormal;

	if (!TerrainSystem::Instance->GetTerrainNormalAtPosition(position, &terrainNormal))
	{
		return false;
	}

	//Handle spawning based on angle.
	const float angle{ CatalystBaseMath::Maximum<float>(Vector3::DotProduct(terrainNormal, Vector3::UP), 0.0f) };

	if (angle < minimumAngle)
	{
		return false;
	}

	const float scaledAngle{ CatalystBaseMath::Scale(angle, minimumAngle, 1.0f, 0.0f, 1.0f) };

	if (!CatalystBaseMath::RandomChance(scaledAngle))
	{
		return false;
	}

	//Adjust the height.
	position += terrainNormal * height;

	//Generate the transformation!
	const float scale{ CatalystBaseMath::RandomFloatInRange(minimumScale, maximumScale) };

	transformation->SetTranslation(position);
	transformation->SetScale(Vector3(scale, scale, scale));

	Matrix4 rotationMatrix{ Matrix4::Orientation(terrainNormal, Vector3::UP) };
	rotationMatrix.Rotate(rotation);

	*transformation = *transformation * rotationMatrix;

	return true;
}