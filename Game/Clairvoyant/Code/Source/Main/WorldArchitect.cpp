//Header file.
#include <Main/WorldArchitect.h>

//Core.
#include <Core/General/HashString.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoiseGenerator.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

//Terrain.
#include <Terrain/TerrainUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(WorldArchitect);

//World architect constants.
namespace WorldArchitectConstants
{
	constexpr uint64 MAP_RESOLUTION{ 512 };
}

/*
*	Initializes the world architect.
*/
void WorldArchitect::Initialize() NOEXCEPT
{
	{
		//Create some terrain.
		const TerrainProperties *const RESTRICT terrainProperties{ TerrainSystem::Instance->GetTerrainProperties() };

		CPUTexture2D properties{ WorldArchitectConstants::MAP_RESOLUTION };

		const float randomOffset1{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
		const float randomOffset2{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
		const float randomOffset3{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
		const float randomOffset4{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

		for (uint64 i = 0; i < WorldArchitectConstants::MAP_RESOLUTION; ++i)
		{
			for (uint64 j = 0; j < WorldArchitectConstants::MAP_RESOLUTION; ++j)
			{
				const float xCoordinate{ static_cast<float>(i) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };
				const float yCoordinate{ static_cast<float>(j) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };

				properties.At(i, j)._W = PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, 0.0f, randomOffset1);
				properties.At(i, j)._W += PerlinNoiseGenerator::GenerateNoise(xCoordinate * 7.5f, yCoordinate * 7.5f, 0.0f, randomOffset2) * 0.25f;
				properties.At(i, j)._W += PerlinNoiseGenerator::GenerateNoise(xCoordinate * 15.0f, yCoordinate * 15.0f, 0.0f, randomOffset3) * 0.125f;
				properties.At(i, j)._W += 0.25f;
			}
		}

		TerrainUtilities::GeneratePatchNormals(terrainProperties->_PatchSize, terrainProperties->_Height, &properties);

		CPUTexture2D weights{ WorldArchitectConstants::MAP_RESOLUTION };

		for (uint64 i = 0; i < WorldArchitectConstants::MAP_RESOLUTION; ++i)
		{
			for (uint64 j = 0; j < WorldArchitectConstants::MAP_RESOLUTION; ++j)
			{
				//Determine the weight of the sand layer.
				float sandLayerWeight;

				const float worldHeight{ properties.At(i, j)._W * terrainProperties->_Height };

				if (worldHeight < 0.0f)
				{
					sandLayerWeight = 1.0f;
				}

				else if (worldHeight > 10.0f)
				{
					sandLayerWeight = 0.0f;
				}

				else
				{
					sandLayerWeight = 1.0f - (worldHeight * 0.1f);
				}

				//Determine the weight of the first grass layer.
				weights.At(i, j)._X = 1.0f - sandLayerWeight;

				//Determine the weight of the second grass layer.
				const float xCoordinate{ static_cast<float>(i) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };
				const float yCoordinate{ static_cast<float>(j) / static_cast<float>(WorldArchitectConstants::MAP_RESOLUTION) };

				weights.At(i, j)._Y = ((PerlinNoiseGenerator::GenerateNoise(xCoordinate, yCoordinate, 0.0f, randomOffset4) + 1.0f) * 0.5f);

				//Modify the weight of the first grass layer.
				weights.At(i, j)._X = CatalystBaseMath::Clamp<float>(weights.At(i, j)._X - weights.At(i, j)._Y, 0.0f, 1.0f);

				weights.At(i, j)._Y = CatalystBaseMath::Clamp<float>(weights.At(i, j)._Y - sandLayerWeight, 0.0f, 1.0f);
				weights.At(i, j)._Y = CatalystBaseMath::SmoothStep<5>(weights.At(i, j)._Y);

				//Determine the weight of the rock layer.
				const Vector3 normal{ properties.At(i, j)._X, properties.At(i, j)._Y, properties.At(i, j)._Z };

				weights.At(i, j)._Z = CatalystBaseMath::SmoothStep<5>(1.0f - CatalystBaseMath::Clamp<float>(Vector3::DotProduct(normal, Vector3::UP) + 0.1f, 0.0f, 1.0f));

				//Determine the weight of the snow layer.
				if (worldHeight < 950.0f)
				{
					weights.At(i, j)._W = 0.0f;
				}

				else if (worldHeight > 1'050.0f)
				{
					weights.At(i, j)._W = 1.0f;
				}

				else
				{
					weights.At(i, j)._W = (worldHeight - 950.0f) * 0.01f;
				}
			}
		}

		TerrainPatchInformation information;

		information._X = 0;
		information._Y = 0;
		information._NormalHeightMap = properties;
		information._LayerWeightsMap = weights;
		information._Material = ResourceLoader::GetTerrainMaterial(HashString("DefaultTerrainMaterial"));
		information._DisplacementInformation._FirstLayerDisplacement = 1.25f;
		information._DisplacementInformation._SecondLayerDisplacement = 0.1f;
		information._DisplacementInformation._ThirdLayerDisplacement = 1.0f;
		information._DisplacementInformation._FourthLayerDisplacement = 2.0f;
		information._DisplacementInformation._FifthLayerDisplacement = 1.0f;

		TerrainSystem::Instance->AddTerrainPatch(std::move(information));
	}

	//Initialize the vegetation.
	InitializeVegetation();
}

/*
*	Initializes the vegetation.
*/
void WorldArchitect::InitializeVegetation()
{
	{
		//Add the debris vegetation type.
		VegetationTypeProperties properties;

		properties._CutoffDistance = 200.0f;
		properties._Density = 100;
		properties._WindModulatorFactor = 0.0f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return WorldArchitect::Instance->GenerateTransformation(true, 0.0f, 0.05f, 0.1f, Vector3(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};
		VegetationModel model{ ResourceLoader::GetVegetationModel(HashString("DebrisVegetationModel")) };
		VegetationMaterial material{ ResourceLoader::GetVegetationMaterial(HashString("DebrisVegetationMaterial")) };

		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 400.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 600.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);
	}

	{
		//Add the fern vegetation type.
		VegetationTypeProperties properties;

		properties._CutoffDistance = 100.0f;
		properties._Density = 100;
		properties._WindModulatorFactor = 0.05f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return WorldArchitect::Instance->GenerateTransformation(false, 0.0f, 0.03f, 0.06f, Vector3(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};
		VegetationModel model{ ResourceLoader::GetVegetationModel(HashString("FernVegetationModel")) };
		VegetationMaterial material{ ResourceLoader::GetVegetationMaterial(HashString("FernVegetationMaterial")) };

		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 200.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 300.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);
	}

	{
		//Add the grass vegetation type.
		VegetationTypeProperties properties;

		properties._CutoffDistance = 20.0f;
		properties._Density = 2'000;
		properties._WindModulatorFactor = 0.1f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return WorldArchitect::Instance->GenerateTransformation(true, 0.45f, 0.15f, 0.3f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};
		VegetationModel model{ ResourceLoader::GetVegetationModel(HashString("GrassVegetationModel")) };
		VegetationMaterial material{ ResourceLoader::GetVegetationMaterial(HashString("GrassVegetationMaterial")) };

		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 40.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 60.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 80.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 100.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);
	}

	{
		//Add the grass 2 vegetation type.
		VegetationTypeProperties properties;

		properties._CutoffDistance = 25.0f;
		properties._Density = 1'000;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return WorldArchitect::Instance->GenerateTransformation(true, 0.45f, 0.25f, 0.5f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};
		VegetationModel model{ ResourceLoader::GetVegetationModel(HashString("GrassVegetationModel")) };
		VegetationMaterial material{ ResourceLoader::GetVegetationMaterial(HashString("Grass2VegetationMaterial")) };

		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 50.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);

		properties._CutoffDistance = 75.0f;
		VegetationSystem::Instance->AddVegetationType(properties, model, material);
	}
}

/*
*	Generates a transformation.
*/
bool WorldArchitect::GenerateTransformation(const bool underwater, const float height, const float minimumScale, const float maximumScale, const Vector3 &randomRotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT
{
	Vector3 position(CatalystBaseMath::RandomFloatInRange(box._Minimum._X, box._Maximum._X), 0.0f, CatalystBaseMath::RandomFloatInRange(box._Minimum._Z, box._Maximum._Z));
	
	const float terrainheight{ TerrainSystem::Instance->GetTerrainHeightAtPosition(position) };

	if (!underwater && terrainheight < 0.0f)
	{
		return false;
	}

	position._Y = terrainheight + height;

	const Vector3 terrainNormal{ TerrainSystem::Instance->GetTerrainNormalAtPosition(position) };

	if (!CatalystBaseMath::RandomChance(CatalystBaseMath::Maximum<float>(Vector3::DotProduct(terrainNormal, Vector3::UP), 0.0f) - 0.25f))
	{
		return false;
	}

	const float scale{ CatalystBaseMath::RandomFloatInRange(minimumScale, maximumScale) };

	transformation->SetTranslation(position);
	transformation->SetScale(Vector3(scale, scale, scale));

	Matrix4 rotationMatrix{ Matrix4::Orientation(terrainNormal, Vector3::UP) };
	rotationMatrix.Rotate(randomRotation);

	*transformation = *transformation * rotationMatrix;

	return true;
}