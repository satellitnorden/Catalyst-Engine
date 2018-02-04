//Header file.
#include <WorldArchitect.h>

//Entities.
#include <DirectionalLightEntity.h>
#include <PhysicalEntity.h>
#include <PointLightEntity.h>
#include <SpotLightEntity.h>
#include <TerrainEntity.h>

//Graphics.
#include <HeightMap.h>
#include <PhysicalModel.h>

//Math.
#include <GameMath.h>
#include <PerlinNoiseGenerator.h>

//Systems.
#include <EntitySystem.h>
#include <GraphicsSystem.h>
#include <QuestSystem.h>

//Preprocessor defines.
#define HEIGHT_MAP_RESOLUTION 2'048

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
	sun->SetIntensity(25.0f);

	//Create the sky!
	VulkanCubeMapTexture *RESTRICT sky = GraphicsSystem::Instance->CreateCubeMapTexture(GAME_TEXTURES_FOLDER "SkyFront.png", GAME_TEXTURES_FOLDER "SkyBack.png", GAME_TEXTURES_FOLDER "SkyUp.png", GAME_TEXTURES_FOLDER "SkyDown.png", GAME_TEXTURES_FOLDER "SkyRight.png", GAME_TEXTURES_FOLDER "SkyLeft.png");
	GraphicsSystem::Instance->SetActiveSkyBox(sky);

	/*
	//Create the height map!
	HeightMap heightMap{ HEIGHT_MAP_RESOLUTION };

	CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE("Height Map Generation",
	for (size_t i = 0; i < HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (size_t j = 0; j < HEIGHT_MAP_RESOLUTION; ++j)
		{
			heightMap.At(i, j) = PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(HEIGHT_MAP_RESOLUTION), static_cast<float>(j) / static_cast<float>(HEIGHT_MAP_RESOLUTION), 0.0f);
		}
	}
	);

	//Create the terrain entity!
	TerrainEntity *RESTRICT terrain{ EntitySystem::Instance->CreateEntity<TerrainEntity>() };
	terrain->Initialize(128, heightMap);
	*/

	Vulkan2DTexture *floorAlbedoTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "FloorAlbedo.png");
	Vulkan2DTexture *floorNormalMapTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "FloorNormalMap.png");
	Vulkan2DTexture *floorRoughnessTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "FloorRoughness.png");
	Vulkan2DTexture *floorAmbientOcclusionTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "FloorAmbientOcclusion.png");

	const PhysicalModel floorModel = GraphicsSystem::Instance->CreatePhysicalModel(GAME_MODELS_FOLDER "Floor.fbx", floorAlbedoTexture, floorNormalMapTexture, floorRoughnessTexture, nullptr, floorAmbientOcclusionTexture);

	PhysicalEntity *floor = EntitySystem::Instance->CreateEntity<PhysicalEntity>();
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
	*/

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
}

/*
*	Updates the world architects.
*/
void WorldArchitect::Update(const float deltaTime) NOEXCEPT
{
	//Constantly rotate the sun.
	sun->Rotate(Vector3(-1.0f * deltaTime, 0.0f, 0.0f));
}