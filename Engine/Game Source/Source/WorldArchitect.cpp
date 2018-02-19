//Header file.
#include <WorldArchitect.h>

//Asset loading.
#include <AssetLoader.h>

//Entities.
#include <DirectionalLightEntity.h>
#include <PhysicalEntity.h>
#include <PointLightEntity.h>
#include <SpotLightEntity.h>
#include <TerrainEntity.h>

//Math.
#include <GameMath.h>
#include <PerlinNoiseGenerator.h>

//Rendering.
#include <CPUTexture4.h>
#include <PhysicalModel.h>
#include <TerrainMaterial.h>
#include <TerrainUniformData.h>
#include <TextureData.h>

//Systems.
#include <EntitySystem.h>
#include <RenderingSystem.h>
#include <PhysicsSystem.h>
#include <QuestSystem.h>

namespace
{
	static constexpr uint32 HEIGHT_MAP_RESOLUTION{ 4'096 };
	static constexpr float TERRAIN_HEIGHT{ 2'000.0f };
	static constexpr float TERRAIN_SIZE{ 10'000.0f };
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
	TextureCubeMapHandle sky = RenderingSystem::Instance->CreateCubeMapTexture(GAME_RAW_TEXTURES_FOLDER "SkyFront.png", GAME_RAW_TEXTURES_FOLDER "SkyBack.png", GAME_RAW_TEXTURES_FOLDER "SkyUp.png", GAME_RAW_TEXTURES_FOLDER "SkyDown.png", GAME_RAW_TEXTURES_FOLDER "SkyRight.png", GAME_RAW_TEXTURES_FOLDER "SkyLeft.png");
	RenderingSystem::Instance->SetActiveSkyBox(sky);

	//Load the terrain material data.
	TerrainMaterial terrainMaterial;
	AssetLoader::LoadTerrainMaterial(GAME_MATERIALS_FOLDER "DefaultTerrainMaterial.ctm", terrainMaterial);

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

			terrainProperties.At(i, j).W = GameMath::LinearlyInterpolate(terrainProperties.At(i, j).W, GameMath::GetSmoothInterpolationValue(terrainProperties.At(i, j).W), 0.25f);
		}
	}

	const float heightMapPositionoffset = TERRAIN_SIZE / HEIGHT_MAP_RESOLUTION;

	for (uint32 i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			const Vector3 left{ -heightMapPositionoffset, terrainProperties.At(i > 0 ? i - 1 : i, j).W * TERRAIN_HEIGHT, 0.0f };
			const Vector3 right{ heightMapPositionoffset, terrainProperties.At(i < HEIGHT_MAP_RESOLUTION - 1 ? i + 1 : i, j).W * TERRAIN_HEIGHT, 0.0f };
			const Vector3 up{ 0.0f, terrainProperties.At(i, j > 0 ? j - 1 : j).W * TERRAIN_HEIGHT, -heightMapPositionoffset };
			const Vector3 down{ 0.0f, terrainProperties.At(i, j < HEIGHT_MAP_RESOLUTION - 1 ? j + 1 : j).W * TERRAIN_HEIGHT, heightMapPositionoffset };
			const Vector3 center{ 0.0f, terrainProperties.At(i, j).W * TERRAIN_HEIGHT, 0.0f };

			const Vector3 normal1 = Vector3::CrossProduct(up - center, left - center);
			const Vector3 normal2 = Vector3::CrossProduct(right - center, up - center);
			const Vector3 normal3 = Vector3::CrossProduct(down - center, right - center);
			const Vector3 normal4 = Vector3::CrossProduct(left - center, down - center);

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

			if (heightValue > -0.8f)
			{
				layerWeights.At(i, j).X = 1.0f;
			}

			else if (heightValue < -0.9f)
			{
				layerWeights.At(i, j).X = 0.0f;
			}

			else
			{
				layerWeights.At(i, j).X = (heightValue + 0.9f) * 10.0f;
			}

			//Determine the weight of the dirt layer.
			constexpr float dirtLayerFrequency{ 100.0f };
			layerWeights.At(i, j).Y = GameMath::GetSmoothInterpolationValue((PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * dirtLayerFrequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * dirtLayerFrequency, 0.0f, randomOffset) + 1.0f) * 0.5f) * 0.75f;

			//Determine the weight of the rock layer.
			layerWeights.At(i, j).Z = 1.0f - GameMath::GetSmootherInterpolationValue(GameMath::Clamp(Vector3::DotProduct(Vector3(terrainPropertiesValue.X, terrainPropertiesValue.Y, terrainPropertiesValue.Z), Vector3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f));

			//Determine the weight of the snow layer.
			if (heightValue < 0.7f)
			{
				layerWeights.At(i, j).W = 0.0f;
			}

			else if (heightValue > 0.8f)
			{
				layerWeights.At(i, j).W = 1.0f;
			}

			else
			{
				layerWeights.At(i, j).W = (heightValue - 0.7f) * 10.0f;
			}
		}
	}

	Texture2DHandle layerWeightsTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(layerWeights), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	//Create the terrain entity!
	TerrainEntity *RESTRICT terrain{ EntitySystem::Instance->CreateEntity<TerrainEntity>() };
	terrain->Initialize(512, terrainProperties, TerrainUniformData(0.5f, TERRAIN_HEIGHT, TERRAIN_SIZE, TERRAIN_SIZE * 0.05f, Vector3(0.0f, 0.0f, 0.0f)), layerWeightsTexture, terrainMaterial);

	/*
	//Place some stones. (:
	Vulkan2DTexture *stoneAlbedoTexture = GraphicsSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_RAW_TEXTURES_FOLDER "StoneAlbedo.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Vulkan2DTexture *stoneNormalMapTexture = GraphicsSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_RAW_TEXTURES_FOLDER "StoneNormalMap.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Vulkan2DTexture *stoneRoughnessTexture = GraphicsSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_RAW_TEXTURES_FOLDER "StoneRoughness.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the stone model.
	const PhysicalModel stoneModel = GraphicsSystem::Instance->CreatePhysicalModel(GAME_MODELS_FOLDER "Stone.obj", stoneAlbedoTexture, stoneNormalMapTexture, stoneRoughnessTexture, nullptr, nullptr);

	//Create the stones.
	for (size_t i = 0; i < 1'000; ++i)
	{
		PhysicalEntity *stone = EntitySystem::Instance->CreateEntity<PhysicalEntity>();
		stone->Initialize(stoneModel);
		Vector3 position{ Vector3(GameMath::RandomFloatInRange(-TERRAIN_SIZE * 0.5f, TERRAIN_SIZE * 0.5f), 0.0f, GameMath::RandomFloatInRange(-TERRAIN_SIZE * 0.5f, TERRAIN_SIZE * 0.5f)) };
		position.Y = PhysicsSystem::Instance->GetTerrainHeightAtPosition(position);
		stone->Move(position);
		stone->Rotate(Vector3(0.0f, GameMath::RandomFloatInRange(0.0f, 360.0f), 0.0f));
		const float stoneScale = GameMath::RandomFloatInRange(0.1f, 0.5f);
		stone->Scale(Vector3(stoneScale, stoneScale, stoneScale));
	}
	*/
}

/*
*	Updates the world architects.
*/
void WorldArchitect::Update(const float deltaTime) NOEXCEPT
{
	//Constantly rotate the sun.
	sun->Rotate(Vector3(-10.0f * deltaTime, 0.0f, 0.0f));
}