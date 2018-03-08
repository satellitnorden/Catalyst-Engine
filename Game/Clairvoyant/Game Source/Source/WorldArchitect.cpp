//Header file.
#include <WorldArchitect.h>

//Clairvoyant.
#include <ClairvoyantSettings.h>

//Entities.
#include <DirectionalLightEntity.h>
#include <InstancedPhysicalEntity.h>
#include <PointLightEntity.h>
#include <SpotLightEntity.h>
#include <TerrainEntity.h>
#include <WaterEntity.h>

//Math.
#include <GameMath.h>
#include <Matrix4.h>
#include <PerlinNoiseGenerator.h>

//Rendering.
#include <CPUTexture4.h>
#include <PhysicalModel.h>
#include <TerrainMaterial.h>
#include <TerrainUniformData.h>
#include <TextureData.h>
#include <WaterMaterial.h>
#include <WaterUniformData.h>

//Resources.
#include <ResourceLoader.h>

//Systems.
#include <EntitySystem.h>
#include <RenderingSystem.h>
#include <PhysicsSystem.h>
#include <QuestSystem.h>

namespace
{
	static constexpr uint32 HEIGHT_MAP_RESOLUTION{ 2'048 };
	static constexpr float TERRAIN_HEIGHT{ 1'000.0f };
	static constexpr float TERRAIN_SIZE{ 10'000.0f };
	static constexpr float WATER_HEIGHT{ -1.0f };
}

/*
*	Default constructor.
*/
WorldArchitect::WorldArchitect() NOEXCEPT
{

}

/*
*	Default destructor.
*/
WorldArchitect::~WorldArchitect() NOEXCEPT
{

}

/*
*	Initializes the world architect.
*/
void WorldArchitect::Initialize() NOEXCEPT
{
	//Create the sun!
	sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	sun->SetIntensity(10.0f);

	//Create the sky!
	TextureCubeMapHandle sky = RenderingSystem::Instance->CreateCubeMapTexture(CLAIRVOYANT_RAW_TEXTURES_FOLDER "SkyFront.png", CLAIRVOYANT_RAW_TEXTURES_FOLDER "SkyBack.png", CLAIRVOYANT_RAW_TEXTURES_FOLDER "SkyUp.png", CLAIRVOYANT_RAW_TEXTURES_FOLDER "SkyDown.png", CLAIRVOYANT_RAW_TEXTURES_FOLDER "SkyRight.png", CLAIRVOYANT_RAW_TEXTURES_FOLDER "SkyLeft.png");
	RenderingSystem::Instance->SetActiveSkyBox(sky);

	//Load the resource collection file.
	ResourceLoader::LoadResourceCollection(CLAIRVOYANT_RESOURCES_FOLDER "ClairvoyantResourceCollection.crc");

	//Load the terrain material data.
	TerrainMaterial terrainMaterial{ ResourceLoader::GetTerrainMaterial(4) };

	//Create the terrain properties!
	CPUTexture4 terrainProperties{ HEIGHT_MAP_RESOLUTION };

	const float randomOffset{ GameMath::RandomFloatInRange(0.0f, 1.0f) };

	for (uint32 i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			float frequency = 5.0f;
			float multiplier = 1.0f;
			terrainProperties.At(i, j).W = PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			frequency *= 2.0f;
			multiplier *= 0.5f;
			terrainProperties.At(i, j).W += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			frequency *= 2.0f;
			multiplier *= 0.5f;
			terrainProperties.At(i, j).W += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;
			
			frequency *= 2.0f;
			multiplier *= 0.5f;
			terrainProperties.At(i, j).W += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			frequency *= 2.0f;
			multiplier *= 0.5f;
			terrainProperties.At(i, j).W += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			terrainProperties.At(i, j).W = GameMath::LinearlyInterpolate(terrainProperties.At(i, j).W, GameMath::SmoothStep<1>(terrainProperties.At(i, j).W), 0.25f);
		}
	}

	static constexpr float heightMapPositionoffset{ TERRAIN_SIZE / HEIGHT_MAP_RESOLUTION };

	for (uint32 i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			const Vector3 left{ -heightMapPositionoffset, terrainProperties.At(i > 0 ? i - 1 : i, j).W * TERRAIN_HEIGHT, 0.0f };
			const Vector3 right{ heightMapPositionoffset, terrainProperties.At(i < HEIGHT_MAP_RESOLUTION - 1 ? i + 1 : i, j).W * TERRAIN_HEIGHT, 0.0f };
			const Vector3 up{ 0.0f, terrainProperties.At(i, j > 0 ? j - 1 : j).W * TERRAIN_HEIGHT, -heightMapPositionoffset };
			const Vector3 down{ 0.0f, terrainProperties.At(i, j < HEIGHT_MAP_RESOLUTION - 1 ? j + 1 : j).W * TERRAIN_HEIGHT, heightMapPositionoffset };
			const Vector3 center{ 0.0f, terrainProperties.At(i, j).W * TERRAIN_HEIGHT, 0.0f };

			const Vector3 normal1{ Vector3::CrossProduct(up - center, left - center) };
			const Vector3 normal2{ Vector3::CrossProduct(right - center, up - center) };
			const Vector3 normal3{ Vector3::CrossProduct(down - center, right - center) };
			const Vector3 normal4{ Vector3::CrossProduct(left - center, down - center) };

			Vector3 finalNormal{ (Vector3::Normalize(normal1 + normal2 + normal3 + normal4) + 1.0f) * 0.5f };

			terrainProperties.At(i, j).X = finalNormal.X;
			terrainProperties.At(i, j).Y = finalNormal.Y;
			terrainProperties.At(i, j).Z = finalNormal.Z;
		}
	}

	//Calculate the layer weights.
	CPUTexture4 layerWeights{ HEIGHT_MAP_RESOLUTION };

	for (uint32 i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			//Determine the weight of the grass layer.
			Vector4 terrainPropertiesValue{ terrainProperties.At(i, j) * 2.0f - 1.0f };
			
			const float heightValue{ terrainPropertiesValue.W };

			if (heightValue > (WATER_HEIGHT + 0.1f))
			{
				layerWeights.At(i, j).X = 1.0f;
			}

			else if (heightValue < WATER_HEIGHT)
			{
				layerWeights.At(i, j).X = 0.0f;
			}

			else
			{
				layerWeights.At(i, j).X = (heightValue + -WATER_HEIGHT) * 10.0f;
			}

			layerWeights.At(i, j).X = GameMath::SmoothStep<1>(layerWeights.At(i, j).X);

			//Determine the weight of the dirt layer.
			constexpr float dirtLayerFrequency{ 1.0f };
			layerWeights.At(i, j).Y = (PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * dirtLayerFrequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * dirtLayerFrequency, 0.0f, randomOffset) + 1.0f) * 0.5f;
			layerWeights.At(i, j).Y = GameMath::SmoothStep<5>(layerWeights.At(i, j).Y);

			//Determine the weight of the rock layer.
			layerWeights.At(i, j).Z = 1.0f - GameMath::SmoothStep<3>(GameMath::Clamp(Vector3::DotProduct(Vector3(terrainPropertiesValue.X, terrainPropertiesValue.Y, terrainPropertiesValue.Z), Vector3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f));

			//Determine the weight of the snow layer.
			if (heightValue < 0.45f)
			{
				layerWeights.At(i, j).W = 0.0f;
			}

			else if (heightValue > 0.55f)
			{
				layerWeights.At(i, j).W = 1.0f;
			}

			else
			{
				layerWeights.At(i, j).W = (heightValue - 0.45f) * 10.0f;
			}
		}
	}

	Texture2DHandle layerWeightsTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(layerWeights), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	//Create the terrain entity!
	TerrainEntity *RESTRICT terrain{ EntitySystem::Instance->CreateEntity<TerrainEntity>() };
	terrain->Initialize(512, terrainProperties, TerrainUniformData(3.0f, 0.5f, 1.0f, 10.0f, 2.0f, TERRAIN_HEIGHT, TERRAIN_SIZE, TERRAIN_SIZE * 0.05f, Vector3(0.0f, 0.0f, 0.0f)), layerWeightsTexture, terrainMaterial);

	//Load the water material.
	WaterMaterial waterMaterial{ ResourceLoader::GetWaterMaterial(5) };

	//Create the water.
	WaterEntity *RESTRICT water = EntitySystem::Instance->CreateEntity<WaterEntity>();
	water->Initialize(waterMaterial, WaterUniformData(TERRAIN_SIZE, 250.0f, Vector3(0.0f, 0.0f, 0.0f)));

	//Create the stone model.
	PhysicalModel stoneModel{ ResourceLoader::GetPhysicalModel(2) };

	//Create the stone material.
	PhysicalMaterial stoneMaterial{ ResourceLoader::GetPhysicalMaterial(0) };

	stoneModel.SetMaterial(stoneMaterial);

	DynamicArray<Matrix4> stoneTransformations;
	stoneTransformations.Reserve(1'000);

	//Create the stones.
	for (uint64 i = 0; i < 1'000; ++i)
	{
		Vector3 position{ Vector3(GameMath::RandomFloatInRange(-TERRAIN_SIZE * 0.5f, TERRAIN_SIZE * 0.5f), 0.0f, GameMath::RandomFloatInRange(-TERRAIN_SIZE * 0.5f, TERRAIN_SIZE * 0.5f)) };

		const Vector3 terrainNormal{ PhysicsSystem::Instance->GetTerrainNormalAtPosition(position) };

		if (Vector3::DotProduct(terrainNormal, Vector3(0.0f, 1.0f, 0.0f)) < 0.9f)
		{
			continue;
		}

		position.Y = PhysicsSystem::Instance->GetTerrainHeightAtPosition(position);

		if (position.Y <= PhysicsSystem::Instance->GetWaterHeight())
		{
			continue;
		}

		const Vector3 rotation{ 0.0f, GameMath::RandomFloatInRange(0.0f, 360.0f), 0.0f };
		const float stoneScale = GameMath::RandomFloatInRange(0.1f, 0.5f);
		const Vector3 scale{ stoneScale, stoneScale, stoneScale };

		stoneTransformations.EmplaceFast(position, rotation, scale);
	}

	InstancedPhysicalEntity *RESTRICT stones = EntitySystem::Instance->CreateEntity<InstancedPhysicalEntity>();
	stones->Initialize(stoneModel, stoneTransformations);

	//Create the tree stomp model.
	PhysicalModel treeStompModel{ ResourceLoader::GetPhysicalModel(3) };

	//Create the tree stomp material.
	PhysicalMaterial treeStompMaterial{ ResourceLoader::GetPhysicalMaterial(1) };

	treeStompModel.SetMaterial(treeStompMaterial);

	DynamicArray<Matrix4> treeStompTransformations;
	treeStompTransformations.Reserve(1'000);

	//Create the tree stomps.
	for (uint64 i = 0; i < 1'000; ++i)
	{
		Vector3 position{ Vector3(GameMath::RandomFloatInRange(-TERRAIN_SIZE * 0.5f, TERRAIN_SIZE * 0.5f), 0.0f, GameMath::RandomFloatInRange(-TERRAIN_SIZE * 0.5f, TERRAIN_SIZE * 0.5f)) };

		const Vector3 terrainNormal{ PhysicsSystem::Instance->GetTerrainNormalAtPosition(position) };

		if (Vector3::DotProduct(terrainNormal, Vector3(0.0f, 1.0f, 0.0f)) < 0.9f)
		{
			continue;
		}

		position.Y = PhysicsSystem::Instance->GetTerrainHeightAtPosition(position);

		if (position.Y <= PhysicsSystem::Instance->GetWaterHeight())
		{
			continue;
		}

		const Vector3 rotation{ -90.0f, 0.0f, GameMath::RandomFloatInRange(0.0f, 360.0f) };
		const float uniformScale = GameMath::RandomFloatInRange(0.75f, 1.0f);
		const Vector3 scale{ uniformScale, uniformScale, uniformScale };

		treeStompTransformations.EmplaceFast(position, rotation, scale);
	}

	InstancedPhysicalEntity *RESTRICT treeStomps = EntitySystem::Instance->CreateEntity<InstancedPhysicalEntity>();
	treeStomps->Initialize(treeStompModel, treeStompTransformations);
}

/*
*	Updates the world architects.
*/
void WorldArchitect::Update(const float deltaTime) NOEXCEPT
{
	//Constantly rotate the sun.
	sun->Rotate(Vector3(-10.0f * deltaTime, 0.0f, 0.0f));
}