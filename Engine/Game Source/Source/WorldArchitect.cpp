//Header file.
#include <WorldArchitect.h>

//Asset loading.
#include <AssetLoader.h>
#include <TerrainMaterialData.h>

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
	TextureCubeMapHandle sky = RenderingSystem::Instance->CreateCubeMapTexture(GAME_TEXTURES_FOLDER "SkyFront.png", GAME_TEXTURES_FOLDER "SkyBack.png", GAME_TEXTURES_FOLDER "SkyUp.png", GAME_TEXTURES_FOLDER "SkyDown.png", GAME_TEXTURES_FOLDER "SkyRight.png", GAME_TEXTURES_FOLDER "SkyLeft.png");
	RenderingSystem::Instance->SetActiveSkyBox(sky);

	//Load the terrain material data.
	TerrainMaterialData terrainMaterialData;
	AssetLoader::LoadTerrainMaterialData(GAME_TEXTURES_FOLDER "DefaultTerrainMaterial.ctm", terrainMaterialData);

	//Create the height map!
	CPUTexture4 heightMap{ HEIGHT_MAP_RESOLUTION };

	const float randomOffset{ GameMath::RandomFloatInRange(0.0f, 1.0f) };

	for (uint32 i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			float frequency = 5.0f;
			float multiplier = 1.0f;
			heightMap.At(i, j) = PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			frequency *= 2.0f;
			multiplier *= 0.5f;
			heightMap.At(i, j) += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			frequency *= 2.0f;
			multiplier *= 0.5f;
			heightMap.At(i, j) += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;
			
			frequency *= 2.0f;
			multiplier *= 0.5f;
			heightMap.At(i, j) += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			frequency *= 2.0f;
			multiplier *= 0.5f;
			heightMap.At(i, j) += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			heightMap.At(i, j) = GameMath::LinearlyInterpolate(heightMap.At(i, j).X, GameMath::GetSmoothInterpolationValue(heightMap.At(i, j).X), 0.25f);
		}
	}

	Texture2DHandle terrainHeightMapTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(heightMap), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	//Create the normal map!
	CPUTexture4 normalMap{ HEIGHT_MAP_RESOLUTION };
	const float heightMapPositionoffset = TERRAIN_SIZE / HEIGHT_MAP_RESOLUTION;

	for (uint32 i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			const Vector3 left{ -heightMapPositionoffset, heightMap.At(i > 0 ? i - 1 : i, j).X * TERRAIN_HEIGHT, 0.0f };
			const Vector3 right{ heightMapPositionoffset, heightMap.At(i < HEIGHT_MAP_RESOLUTION - 1 ? i + 1 : i, j).X * TERRAIN_HEIGHT, 0.0f };
			const Vector3 up{ 0.0f, heightMap.At(i, j > 0 ? j - 1 : j).X * TERRAIN_HEIGHT, -heightMapPositionoffset };
			const Vector3 down{ 0.0f, heightMap.At(i, j < HEIGHT_MAP_RESOLUTION - 1 ? j + 1 : j).X * TERRAIN_HEIGHT, heightMapPositionoffset };
			const Vector3 center{ 0.0f, heightMap.At(i, j).X * TERRAIN_HEIGHT, 0.0f };

			const Vector3 normal1 = Vector3::CrossProduct(up - center, left - center);
			const Vector3 normal2 = Vector3::CrossProduct(right - center, up - center);
			const Vector3 normal3 = Vector3::CrossProduct(down - center, right - center);
			const Vector3 normal4 = Vector3::CrossProduct(left - center, down - center);

			normalMap.At(i, j) = (Vector3::Normalize(normal1 + normal2 + normal3 + normal4) + 1.0f) * 0.5f;
		}
	}

	Texture2DHandle terrainNormalMapTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(normalMap), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	//Calculate the layer 1 weight.
	CPUTexture4 layer1Weight{ HEIGHT_MAP_RESOLUTION };

	for (uint32 i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			Vector4 normalMapValue{ normalMap.At(i, j) * 2.0f - 1.0f };
			
			layer1Weight.At(i, j) = 1.0f - GameMath::GetSmootherInterpolationValue(GameMath::Clamp(Vector3::DotProduct(Vector3(normalMapValue.X, normalMapValue.Y, normalMapValue.Z), Vector3(0.0f, 1.0f, 0.0f)) - 0.25f, 0.0f, 1.0f));
		}
	}

	//Calculate the layer 1 weight.
	CPUTexture4 layer2Weight{ HEIGHT_MAP_RESOLUTION };

	for (uint32 i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			const float heightMapValue{ heightMap.At(i, j).X };

			if (heightMapValue < 0.7f)
			{
				layer2Weight.At(i, j) = 1.0f;
			}

			else if (heightMapValue > 0.8f)
			{
				layer2Weight.At(i, j) = 0.0f;
			}

			else
			{
				layer2Weight.At(i, j) = 1.0f - ((heightMapValue - 0.7f) * 10.0f);
			}
		}
	}

	Texture2DHandle layer1WeightTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(layer1Weight), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));
	Texture2DHandle layer2WeightTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(layer2Weight), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	//Load the remaining terrain textures.
	Texture2DHandle layer1AlbedoTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "Terrain/Stone3Albedo.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer1NormalMapTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "Terrain/Stone3NormalMap.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer1RoughnessTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "Terrain/Stone3Roughness.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer1AmbientOcclusionTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "Terrain/Stone3AmbientOcclusion.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer1DisplacementTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "Terrain/Stone3Displacement.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	Texture2DHandle layer2AlbedoTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer({ GAME_TEXTURES_FOLDER "Terrain/Grass1AlbedoMip0.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AlbedoMip1.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AlbedoMip2.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AlbedoMip3.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AlbedoMip4.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AlbedoMip5.png" }), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer2NormalMapTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer({ GAME_TEXTURES_FOLDER "Terrain/Grass1NormalMapMip0.png", GAME_TEXTURES_FOLDER "Terrain/Grass1NormalMapMip1.png", GAME_TEXTURES_FOLDER "Terrain/Grass1NormalMapMip2.png", GAME_TEXTURES_FOLDER "Terrain/Grass1NormalMapMip3.png", GAME_TEXTURES_FOLDER "Terrain/Grass1NormalMapMip4.png", GAME_TEXTURES_FOLDER "Terrain/Grass1NormalMapMip5.png" }), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer2RoughnessTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer({ GAME_TEXTURES_FOLDER "Terrain/Grass1RoughnessMip0.png", GAME_TEXTURES_FOLDER "Terrain/Grass1RoughnessMip1.png", GAME_TEXTURES_FOLDER "Terrain/Grass1RoughnessMip2.png", GAME_TEXTURES_FOLDER "Terrain/Grass1RoughnessMip3.png", GAME_TEXTURES_FOLDER "Terrain/Grass1RoughnessMip4.png", GAME_TEXTURES_FOLDER "Terrain/Grass1RoughnessMip5.png" }), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer2AmbientOcclusionTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer({ GAME_TEXTURES_FOLDER "Terrain/Grass1AmbientOcclusionMip0.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AmbientOcclusionMip1.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AmbientOcclusionMip2.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AmbientOcclusionMip3.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AmbientOcclusionMip4.png", GAME_TEXTURES_FOLDER "Terrain/Grass1AmbientOcclusionMip5.png" }), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer2DisplacementTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer({ GAME_TEXTURES_FOLDER "Terrain/Grass1DisplacementMip0.png", GAME_TEXTURES_FOLDER "Terrain/Grass1DisplacementMip1.png", GAME_TEXTURES_FOLDER "Terrain/Grass1DisplacementMip2.png", GAME_TEXTURES_FOLDER "Terrain/Grass1DisplacementMip3.png", GAME_TEXTURES_FOLDER "Terrain/Grass1DisplacementMip4.png", GAME_TEXTURES_FOLDER "Terrain/Grass1DisplacementMip5.png" }), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	Texture2DHandle layer3AlbedoTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "Terrain/Snow1Albedo.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer3NormalMapTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "Terrain/Snow1NormalMap.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer3RoughnessTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "Terrain/Snow1Roughness.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Texture2DHandle layer3DisplacementTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "Terrain/Snow1Displacement.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the terrain entity!
	TerrainEntity *RESTRICT terrain{ EntitySystem::Instance->CreateEntity<TerrainEntity>() };
	terrain->Initialize(	heightMap, 512, TerrainUniformData(2.0f, TERRAIN_HEIGHT, TERRAIN_SIZE, TERRAIN_SIZE * 0.075f, Vector3(0.0f, 0.0f, 0.0f)),
							terrainHeightMapTexture, terrainNormalMapTexture,
							layer1WeightTexture, layer1AlbedoTexture, layer1NormalMapTexture, layer1RoughnessTexture, nullptr, layer1AmbientOcclusionTexture, layer1DisplacementTexture,
							layer2WeightTexture, layer2AlbedoTexture, layer2NormalMapTexture, layer2RoughnessTexture, nullptr, layer2AmbientOcclusionTexture, layer2DisplacementTexture,
							nullptr, layer3AlbedoTexture, layer3NormalMapTexture, layer3RoughnessTexture, nullptr, nullptr, layer3DisplacementTexture);

	/*
	//Place some stones. (:
	Vulkan2DTexture *stoneAlbedoTexture = GraphicsSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "StoneAlbedo.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Vulkan2DTexture *stoneNormalMapTexture = GraphicsSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "StoneNormalMap.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
	Vulkan2DTexture *stoneRoughnessTexture = GraphicsSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(GAME_TEXTURES_FOLDER "StoneRoughness.png"), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

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