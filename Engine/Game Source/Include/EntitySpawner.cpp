//Header file.
#include <EntitySpawner.h>

//Entities.
#include <PhysicalEntity.h>

//Math.
#include <GameMath.h>

//Rendering.
#include <PhysicalModel.h>

//Systems.
#include <EntitySystem.h>
#include <RenderingSystem.h>

/*
*	Default constructor.
*/
EntitySpawner::EntitySpawner() NOEXCEPT
{

}

/*
*	Default destructor.
*/
EntitySpawner::~EntitySpawner() NOEXCEPT
{

}

/*
*	Updates this entity spawner.
*/
void EntitySpawner::Update(const float deltaTime) NOEXCEPT
{
	/*
	static Vulkan2DTexture *stoneAlbedoTexture = GraphicsSystem::Instance->Create2DTexture(GAME_RAW_TEXTURES_FOLDER "StoneAlbedo.png", TextureCreationParameters());
	static Vulkan2DTexture *stoneNormalMapTexture = GraphicsSystem::Instance->Create2DTexture(GAME_RAW_TEXTURES_FOLDER "StoneNormalMap.png", TextureCreationParameters());
	static Vulkan2DTexture *stoneRoughnessTexture = GraphicsSystem::Instance->Create2DTexture(GAME_RAW_TEXTURES_FOLDER "StoneRoughness.png", TextureCreationParameters());

	//Create the stone model.
	static const PhysicalModel stoneModel = GraphicsSystem::Instance->CreatePhysicalModel(GAME_MODELS_FOLDER "Stone.obj", stoneAlbedoTexture, stoneNormalMapTexture, stoneRoughnessTexture, nullptr, nullptr);

	spawnTimer += deltaTime;

	while (false)
	{
		spawnTimer -= 0.1f;

		PhysicalEntity *RESTRICT stone = EntitySystem::Instance->CreateEntity<PhysicalEntity>();
		stone->Initialize(stoneModel);
		stone->Move(Vector3(GameMath::RandomFloatInRange(-250.0f, 250.0f), GameMath::RandomFloatInRange(100.0f, 1'000.0f), GameMath::RandomFloatInRange(-250.0f, 250.0f)));
		stone->Rotate(Vector3(0.0f, GameMath::RandomFloatInRange(0.0f, 360.0f), 0.0f));
		const float stoneScale = GameMath::RandomFloatInRange(0.1f, 0.5f);
		stone->Scale(Vector3(stoneScale, stoneScale, stoneScale));
	}
	*/
}