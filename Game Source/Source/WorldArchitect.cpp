//Header file.
#include <WorldArchitect.h>

//Entities.
#include <DirectionalLightEntity.h>
#include <PhysicalEntity.h>
#include <PointLightEntity.h>
#include <SpotLightEntity.h>
#include <TerrainEntity.h>

//Graphics.
#include <CPUTexture4.h>
#include <PhysicalModel.h>
#include <TerrainUniformData.h>

//Math.
#include <GameMath.h>
#include <PerlinNoiseGenerator.h>

//Systems.
#include <EntitySystem.h>
#include <GraphicsSystem.h>
#include <QuestSystem.h>

//Preprocessor defines.
#define HEIGHT_MAP_RESOLUTION 1'024
#define TERRAIN_HEIGHT 1'024.0f
#define TERRAIN_SIZE 4'096.0f

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
	//sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	//sun->SetIntensity(25.0f);

	//Create the sky!
	VulkanCubeMapTexture *RESTRICT sky = GraphicsSystem::Instance->CreateCubeMapTexture(GAME_TEXTURES_FOLDER "SkyFront.png", GAME_TEXTURES_FOLDER "SkyBack.png", GAME_TEXTURES_FOLDER "SkyUp.png", GAME_TEXTURES_FOLDER "SkyDown.png", GAME_TEXTURES_FOLDER "SkyRight.png", GAME_TEXTURES_FOLDER "SkyLeft.png");
	GraphicsSystem::Instance->SetActiveSkyBox(sky);

	//Create the height map!
	CPUTexture4 heightMap{ HEIGHT_MAP_RESOLUTION };

	for (uint32 i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			heightMap.At(i, j) = PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION), static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION), 0.0f);
		}
	}

	Vulkan2DTexture *RESTRICT terrainHeightMapTexture = GraphicsSystem::Instance->Create2DTexture(heightMap);

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

	Vulkan2DTexture *RESTRICT terrainNormalMapTexture = GraphicsSystem::Instance->Create2DTexture(normalMap);

	//Load the remaining terrain textures.
	Vulkan2DTexture *RESTRICT terrainAlbedoTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "Terrain/Sand1Albedo.png");
	Vulkan2DTexture *RESTRICT terrainNormalTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "Terrain/Sand1Normal.png");
	Vulkan2DTexture *RESTRICT terrainRoughnessTexture = nullptr;
	Vulkan2DTexture *RESTRICT terrainAmbientOcclusionTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "Terrain/Sand1AmbientOcclusion.png");
	Vulkan2DTexture *RESTRICT terrainDisplacementTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "Terrain/Sand1Displacement.png");

	//Create the terrain entity!
	TerrainEntity *RESTRICT terrain{ EntitySystem::Instance->CreateEntity<TerrainEntity>() };
	terrain->Initialize(64, TerrainUniformData(2.5f, TERRAIN_HEIGHT, TERRAIN_SIZE, Vector3(0.0f, 0.0f, 0.0f)), terrainHeightMapTexture, terrainNormalMapTexture, terrainAlbedoTexture, terrainNormalTexture, terrainRoughnessTexture, nullptr, terrainAmbientOcclusionTexture, terrainDisplacementTexture);

	Vulkan2DTexture *RESTRICT floorAlbedoTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "FloorAlbedo.png");
	Vulkan2DTexture *RESTRICT floorNormalMapTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "FloorNormalMap.png");
	Vulkan2DTexture *RESTRICT floorRoughnessTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "FloorRoughness.png");
	Vulkan2DTexture *RESTRICT floorAmbientOcclusionTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "FloorAmbientOcclusion.png");

	const PhysicalModel floorModel = GraphicsSystem::Instance->CreatePhysicalModel(GAME_MODELS_FOLDER "Floor.fbx", floorAlbedoTexture, floorNormalMapTexture, floorRoughnessTexture, nullptr, floorAmbientOcclusionTexture);

	PhysicalEntity *RESTRICT floor = EntitySystem::Instance->CreateEntity<PhysicalEntity>();
	floor->Initialize(floorModel);
	floor->Rotate(Vector3(-90.0f, 0.0f, 0.0f));
	floor->Scale(Vector3(1'000.0f, 1'000.0f, 1'000.0f));

	/*
	Vector3 pointLightPositions[5]{ Vector3(0.0f, 20.0f, 0.0f), Vector3(50.0f, 10.0f, 50.0f), Vector3(50.0f, 10.0f, -50.0f), Vector3(-50.0f, 10.0f, 50.0f), Vector3(-50.0f, 10.0f, -50.0f) };

	for (size_t i = 0; i < 5; ++i)
	{
		PointLightEntity *light = EntitySystem::Instance->CreateEntity<PointLightEntity>();
		light->SetAttenuationDistance(1'000.0f);
		light->SetColor(Vector3(GameMath::RandomFloatInRange(0.0f, 1.0f), GameMath::RandomFloatInRange(0.0f, 1.0f), GameMath::RandomFloatInRange(0.0f, 1.0f)));
		light->Move(pointLightPositions[i]);
		light->SetIntensity(10.0f);
	}

	Vulkan2DTexture *gunAlbedoTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "GunAlbedo.png");
	Vulkan2DTexture *gunNormalMapTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "GunNormal.png");
	Vulkan2DTexture *gunRoughnessTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "GunRoughness.png");
	Vulkan2DTexture *gunMetallicTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "GunMetallic.png");
	Vulkan2DTexture *gunAmbientOcclusionTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "GunAmbientOcclusion.png");

	const PhysicalModel gunModel = GraphicsSystem::Instance->CreatePhysicalModel(GAME_MODELS_FOLDER "Gun.fbx", gunAlbedoTexture, gunNormalMapTexture, gunRoughnessTexture, gunMetallicTexture, gunAmbientOcclusionTexture);

	PhysicalEntity *gun = EntitySystem::Instance->CreateEntity<PhysicalEntity>();
	gun->Initialize(gunModel);
	gun->Move(Vector3(0.0f, 10.0f, 0.0f));
	gun->Rotate(Vector3(-90.0f, 0.0f, 0.0f));
	gun->Scale(Vector3(0.2f, 0.2f, 0.2f));
	
	Vulkan2DTexture *stoneAlbedoTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "StoneAlbedo.png");
	Vulkan2DTexture *stoneNormalMapTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "StoneNormalMap.png");
	Vulkan2DTexture *stoneRoughnessTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "StoneRoughness.png");

	//Create the stone model.
	const PhysicalModel stoneModel = GraphicsSystem::Instance->CreatePhysicalModel(GAME_MODELS_FOLDER "Stone.obj", stoneAlbedoTexture, stoneNormalMapTexture, stoneRoughnessTexture, nullptr, nullptr);

	//Create the stones.
	for (size_t i = 0; i < 3'000; ++i)
	{
		PhysicalEntity *stone = EntitySystem::Instance->CreateEntity<PhysicalEntity>();
		stone->Initialize(stoneModel);
		stone->Move(Vector3(GameMath::RandomFloatInRange(-250.0f, 250.0f), 0.0f, GameMath::RandomFloatInRange(-250.0f, 250.0f)));
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
	//sun->Rotate(Vector3(-1.0f * deltaTime, 0.0f, 0.0f));
}