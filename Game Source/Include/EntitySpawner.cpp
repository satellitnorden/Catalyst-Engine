//Header file.
#include <EntitySpawner.h>

//Entities.
#include <PhysicalEntity.h>

//Graphics.
#include <PhysicalModel.h>

//Math.
#include <GameMath.h>

//Systems.
#include <EntitySystem.h>
#include <GraphicsSystem.h>

/*
*	Default constructor.
*/
EntitySpawner::EntitySpawner() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
EntitySpawner::~EntitySpawner() CATALYST_NOEXCEPT
{

}

/*
*	Updates this entity spawner.
*/
void EntitySpawner::Update(const float deltaTime) CATALYST_NOEXCEPT
{
	static Vulkan2DTexture *stoneAlbedoTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "StoneAlbedo.png");
	static Vulkan2DTexture *stoneNormalMapTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "StoneNormalMap.png");
	static Vulkan2DTexture *stoneRoughnessTexture = GraphicsSystem::Instance->Create2DTexture(GAME_TEXTURES_FOLDER "StoneRoughness.png");

	//Create the stone model.
	static const PhysicalModel stoneModel = GraphicsSystem::Instance->CreatePhysicalModel(GAME_MODELS_FOLDER "Stone.obj", stoneAlbedoTexture, stoneNormalMapTexture, stoneRoughnessTexture, nullptr, nullptr);

	spawnTimer += deltaTime;

	while (false)
	{
		spawnTimer -= 0.1f;

		PhysicalEntity *CATALYST_RESTRICT stone = EntitySystem::Instance->CreateEntity<PhysicalEntity>();
		stone->Initialize(stoneModel);
		stone->Move(Vector3(GameMath::RandomFloatInRange(-250.0f, 250.0f), GameMath::RandomFloatInRange(100.0f, 1'000.0f), GameMath::RandomFloatInRange(-250.0f, 250.0f)));
		stone->Rotate(Vector3(0.0f, GameMath::RandomFloatInRange(0.0f, 360.0f), 0.0f));
		const float stoneScale = GameMath::RandomFloatInRange(0.1f, 0.5f);
		stone->Scale(Vector3(stoneScale, stoneScale, stoneScale));
	}
}