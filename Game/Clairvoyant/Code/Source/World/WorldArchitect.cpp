//Header file.
#include <World/WorldArchitect.h>

//Clairvoyant.
#include <Resources/ClairvoyantSettings.h>
#include <Sound/ClairvoyantSoundGUIDs.h>

//Core.
#include <Core/HashString.h>
#include <Core/Algorithms/SortingAlgorithms.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/DirectionalLightEntity.h>
#include <Entities/InstancedPhysicalEntity.h>
#include <Entities/ParticleSystemEntity.h>
#include <Entities/PointLightEntity.h>
#include <Entities/Sound3DEntity.h>
#include <Entities/SpotLightEntity.h>
#include <Entities/StaticPhysicalEntity.h>
#include <Entities/TerrainEntity.h>
#include <Entities/VegetationEntity.h>
#include <Entities/Initialization Data/TerrainInitializationData.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Matrix4.h>
#include <Math/PerlinNoiseGenerator.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture2D.h>
#include <Rendering/Engine Layer/EnvironmentMaterial.h>
#include <Rendering/Engine Layer/OceanMaterial.h>
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Engine Layer/RenderingUtilities.h>
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/TerrainUniformData.h>
#include <Rendering/Engine Layer/TextureData.h>
#include <Rendering/Engine Layer/WaterUniformData.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WorldArchitect);

//World architects constants.
namespace WorldAchitectConstants
{
	constexpr uint32 HEIGHT_MAP_RESOLUTION{ 256 };
	constexpr float TERRAIN_EXTENT{ 1'000.0f };
	constexpr float TERRAIN_HEIGHT{ 750.0f };
	constexpr uint64 VEGETATION_DENSITY{ 50'000 };

	//Resource ID's.
	constexpr HashString DAY_ENVIRONMENT_MATERIAL{ "DayEnvironmentMaterial" };
	constexpr HashString NIGHT_ENVIRONMENT_MATERIAL{ "NightEnvironmentMaterial" };

	constexpr HashString FOG_1_MATERIAL{ "Fog1Material" };

	constexpr HashString GRASS_1_MATERIAL{ "Grass1Material" };
	constexpr HashString STONE_1_MATERIAL{ "Stone1Material" };
	constexpr HashString STONE_2_MATERIAL{ "Stone2Material" };
	constexpr HashString TREE_1_MATERIAL{ "Tree1Material" };

	constexpr HashString STONE_1_MODEL{ "Stone1Model" };
	constexpr HashString STONE_2_MODEL{ "Stone2Model" };
	constexpr HashString TREE_1_MODEL{ "Tree1Model" };

	constexpr HashString GRASS_TERRAIN_MATERIAL{ "GrassTerrainMaterial" };

	constexpr HashString DEFAULT_VEGETATION_MATERIAL{ "DefaultVegetationMaterial" };

	constexpr HashString DEFAULT_WATER_MATERIAL{ "DefaultWaterMaterial" };
}

/*
*	Initializes the world architect.
*/
void WorldArchitect::Initialize() NOEXCEPT
{
	//Load the resource collection files.
	ResourceLoader::LoadResourceCollection(CLAIRVOYANT_RESOURCES_FOLDER "ClairvoyantResourceCollection.crc");

	//Set the environment material.
	EnvironmentManager::Instance->SetNightEnvironmentMaterial(ResourceLoader::GetEnvironmentMaterial(WorldAchitectConstants::NIGHT_ENVIRONMENT_MATERIAL));
	EnvironmentManager::Instance->SetDayEnvironmentMaterial(ResourceLoader::GetEnvironmentMaterial(WorldAchitectConstants::DAY_ENVIRONMENT_MATERIAL));

	//Set the ocean material.
	EnvironmentManager::Instance->SetOceanMaterial(ResourceLoader::GetOceanMaterial(WorldAchitectConstants::DEFAULT_WATER_MATERIAL));

	//Initialize the task.
	InitializeTask();

	//Create the test scene.
	CreateTestScene();
}

/*
*	Updates the world architect.
*/
void WorldArchitect::Update(const float deltaTime) NOEXCEPT
{
	/*
	//Update depending on the current state.
	switch (currentState)
	{
		case WorldArchitectState::Idling:
		{
			UpdateIdling();

			break;
		}

		case WorldArchitectState::Scanning:
		{
			UpdateScanning();

			break;
		}

		case WorldArchitectState::Generating:
		{
			UpdateGenerating();

			break;
		}
	}
	*/
}

/*
*	Initializes the task.
*/
void WorldArchitect::InitializeTask() NOEXCEPT
{
	task.arguments = nullptr;
	task.semaphore = &semaphore;
}

/*
*	Updates the idling state.
*/
void WorldArchitect::UpdateIdling() NOEXCEPT
{
	//Gather the world scanning data.
	scanningData.cameraPosition = RenderingSystem::Instance->GetActiveCamera()->GetPosition();

	//Update the task.
	task.function = [](void *const RESTRICT)
	{
		WorldArchitect::Instance->Scan();
	};

	//Execute the task.
	TaskSystem::Instance->ExecuteTask(&task);

	//Set the new state.
	currentState = WorldArchitectState::Scanning;
}

/*
*	Updates the generating state.
*/
void WorldArchitect::UpdateGenerating() NOEXCEPT
{
	//When the task is done, go back to idling.
	if (semaphore.IsSignalled())
	{
		currentState = WorldArchitectState::Idling;
	}
}

/*
*	Updates the scanning state.
*/
void WorldArchitect::UpdateScanning() NOEXCEPT
{
	//Check if the scanning task has been finished.
	if (semaphore.IsSignalled())
	{
		//If the suggested world chunk is valid, proceed with generating, otherwise go back to idling.
		if (suggestedWorldChunk.IsValid())
		{
			//Update the task.
			task.function = [](void *const RESTRICT)
			{
				WorldArchitect::Instance->Generate();
			};

			//Execute the task.
			TaskSystem::Instance->ExecuteTask(&task);

			currentState = WorldArchitectState::Generating;
		}

		else
		{
			currentState = WorldArchitectState::Idling;
		}
	}
}

/*
*	Scans the world, generating chunks to generate and chunks to destroy.
*/
void WorldArchitect::Scan() NOEXCEPT
{
	//Invalidate the suggested world chunk.
	suggestedWorldChunk.Invalidate();

	//Calculate the grid position that the camera is currently in.
	const int32 currentGridPositionX{ static_cast<int32>(scanningData.cameraPosition.X / WorldAchitectConstants::TERRAIN_EXTENT) };
	const int32 currentGridPositionY{ static_cast<int32>(scanningData.cameraPosition.Z / WorldAchitectConstants::TERRAIN_EXTENT) };

	//Generate a list of suggested world chunks, adding those closest to the camera first so that they take priority.
	StaticArray<SuggestedWorldChunk, WORLD_CHUNK_GRID_SIZE * WORLD_CHUNK_GRID_SIZE> suggestedWorldChunks;

	for (uint8 i = 0; i < WORLD_CHUNK_GRID_SIZE; ++i)
	{
		for (uint8 j = 0; j < WORLD_CHUNK_GRID_SIZE; ++j)
		{
			const int32 suggestedGridPositionX{ currentGridPositionX + (i - ((WORLD_CHUNK_GRID_SIZE - 1) / 2)) };
			const int32 suggestedGridPositionY{ currentGridPositionY + (j - ((WORLD_CHUNK_GRID_SIZE - 1) / 2)) };

			suggestedWorldChunks[(i * WORLD_CHUNK_GRID_SIZE) + j] = SuggestedWorldChunk(suggestedGridPositionX, suggestedGridPositionY);
		}
	}

	SortingAlgorithms::InsertionSort<SuggestedWorldChunk>(suggestedWorldChunks.begin(), suggestedWorldChunks.end(), [](const SuggestedWorldChunk *const RESTRICT first, const SuggestedWorldChunk *const RESTRICT second)
	{
		const Vector3 firstWorldPosition{ first->gridPositionX * WorldAchitectConstants::TERRAIN_EXTENT, 0.0f, first->gridPositionY * WorldAchitectConstants::TERRAIN_EXTENT };
		const Vector3 secondWorldPosition{ second->gridPositionX * WorldAchitectConstants::TERRAIN_EXTENT, 0.0f, second->gridPositionY * WorldAchitectConstants::TERRAIN_EXTENT };

		const float distanceToFirst{ Vector3::LengthSquaredXZ(WorldArchitect::Instance->scanningData.cameraPosition - firstWorldPosition) };
		const float distanceToSecond{ Vector3::LengthSquaredXZ(WorldArchitect::Instance->scanningData.cameraPosition - secondWorldPosition) };

		return distanceToFirst < distanceToSecond;
	});

	//Go through the list of world chunks and compare it with the list of suggested world chunks and destroy any world chunk that shouldn't be active.
	for (WorldChunk &worldChunk : worldChunks)
	{
		if (!worldChunk.IsValid())
		{
			continue;
		}

		bool foundCorrespondingSuggestedWorldChunk{ false };

		for (const SuggestedWorldChunk &newSuggestedWorldChunk : suggestedWorldChunks)
		{
			if (worldChunk.gridPositionX == newSuggestedWorldChunk.gridPositionX && worldChunk.gridPositionY == newSuggestedWorldChunk.gridPositionY)
			{
				foundCorrespondingSuggestedWorldChunk = true;

				break;
			}
		}

		if (!foundCorrespondingSuggestedWorldChunk)
		{
			EntitySystem::Instance->RequesTermination(worldChunk.terrainEntity, false);
			//EntitySystem::Instance->RequesDestruction(worldChunk.terrainEntity, false);

			worldChunk.Invalidate();
		}
	}

	//Go through the list of suggested world chunks and compare it with the current list of world chunks and set the suggested new chunk.
	for (const SuggestedWorldChunk &newSuggestedWorldChunk : suggestedWorldChunks)
	{
		bool foundCorrespondingWorldChunk{ false };

		for (const WorldChunk &worldChunk : worldChunks)
		{
			if (worldChunk.gridPositionX == newSuggestedWorldChunk.gridPositionX && worldChunk.gridPositionY == newSuggestedWorldChunk.gridPositionY)
			{
				foundCorrespondingWorldChunk = true;

				break;
			}
		}

		if (!foundCorrespondingWorldChunk)
		{
			suggestedWorldChunk.gridPositionX = newSuggestedWorldChunk.gridPositionX;
			suggestedWorldChunk.gridPositionY = newSuggestedWorldChunk.gridPositionY;

			break;
		}
	}
}

/*
*	Generates a new chunk.
*/
void WorldArchitect::Generate() NOEXCEPT
{
	//Add the new world chunk to the list of world chunks.
	WorldChunk *RESTRICT newWorldChunk{ nullptr };

	for (WorldChunk &worldChunk : worldChunks)
	{
		if (!worldChunk.IsValid())
		{
			newWorldChunk = &worldChunk;

			break;
		}
	}

	if (!newWorldChunk)
	{
		return;
	}

	//Set the grid position of this world chunk.
	newWorldChunk->gridPositionX = suggestedWorldChunk.gridPositionX;
	newWorldChunk->gridPositionY = suggestedWorldChunk.gridPositionY;

	//Calculate the world position from the suggested world chunk.
	const Vector3 worldPosition{ static_cast<float>(suggestedWorldChunk.gridPositionX) * WorldAchitectConstants::TERRAIN_EXTENT, 0.0f, static_cast<float>(suggestedWorldChunk.gridPositionY) * WorldAchitectConstants::TERRAIN_EXTENT };

	//Generate the terrain.
	newWorldChunk->terrainEntity = GenerateTerrain(worldPosition);
}

/*
*	Creates the test scene.
*/
void WorldArchitect::CreateTestScene() NOEXCEPT
{
	//Defines.
	constexpr float randomFactor{ 10.0f };

	//Create the floor.
	PhysicalModel planeModel{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Plane) };
	planeModel.SetMaterial(ResourceLoader::GetPhysicalMaterial(WorldAchitectConstants::GRASS_1_MATERIAL));

	StaticPhysicalEntity *const RESTRICT plane{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	plane->Initialize(planeModel, Vector3(0.0f, 100.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 100.0f));

	//Create the first tree.
	PhysicalModel tree1Model{ ResourceLoader::GetPhysicalModel(WorldAchitectConstants::TREE_1_MODEL) };
	tree1Model.SetMaterial(ResourceLoader::GetPhysicalMaterial(WorldAchitectConstants::TREE_1_MATERIAL));

	StaticPhysicalEntity *const RESTRICT tree1{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	tree1->Initialize(tree1Model, Vector3(0.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor), 100.0f, 0.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor)), Vector3(-90.0f, 0.0f, 0.0f), Vector3(0.075f, 0.075f, 0.075f));

	//Create the second tree.
	StaticPhysicalEntity *const RESTRICT tree2{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	tree2->Initialize(tree1Model, Vector3(0.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor), 100.0f, 25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor)), Vector3(-90.0f, 0.0f, 0.0f), Vector3(0.075f, 0.075f, 0.075f));

	//Create the third tree.
	StaticPhysicalEntity *const RESTRICT tree3{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	tree3->Initialize(tree1Model, Vector3(0.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor), 100.0f, -25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor)), Vector3(-90.0f, 0.0f, 0.0f), Vector3(0.075f, 0.075f, 0.075f));

	//Create the fourth tree.
	StaticPhysicalEntity *const RESTRICT tree4{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	tree4->Initialize(tree1Model, Vector3(25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor), 100.0f, 0.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor)), Vector3(-90.0f, 0.0f, 0.0f), Vector3(0.075f, 0.075f, 0.075f));

	//Create the fifth tree.
	StaticPhysicalEntity *const RESTRICT tree5{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	tree5->Initialize(tree1Model, Vector3(-25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor), 100.0f, 0.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor)), Vector3(-90.0f, 0.0f, 0.0f), Vector3(0.075f, 0.075f, 0.075f));

	//Create the first stone.
	PhysicalModel stone1Model{ ResourceLoader::GetPhysicalModel(WorldAchitectConstants::STONE_1_MODEL) };
	stone1Model.SetMaterial(ResourceLoader::GetPhysicalMaterial(WorldAchitectConstants::STONE_1_MATERIAL));

	StaticPhysicalEntity *const RESTRICT stone1{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	stone1->Initialize(stone1Model, Vector3(25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor), 100.0f, 25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor)), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.2f, 0.2f, 0.2f));

	//Create the second stone.
	StaticPhysicalEntity *const RESTRICT stone2{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	stone2->Initialize(stone1Model, Vector3(-25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor), 100.0f, -25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor)), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.2f, 0.2f, 0.2f));

	//Create the third stone.
	PhysicalModel stone2Model{ ResourceLoader::GetPhysicalModel(WorldAchitectConstants::STONE_2_MODEL) };
	stone2Model.SetMaterial(ResourceLoader::GetPhysicalMaterial(WorldAchitectConstants::STONE_2_MATERIAL));

	StaticPhysicalEntity *const RESTRICT stone3{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	stone3->Initialize(stone2Model, Vector3(25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor), 100.0f, -25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor)), Vector3(-90.0f, 0.0f, 0.0f), Vector3(0.175f, 0.175f, 0.175f));

	//Create the fourth stone.
	StaticPhysicalEntity *const RESTRICT stone4{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	stone4->Initialize(stone2Model, Vector3(-25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor), 100.0f, 25.0f + CatalystMath::RandomFloatInRange(-randomFactor, randomFactor)), Vector3(-90.0f, 0.0f, 0.0f), Vector3(0.175f, 0.175f, 0.175f));

	/*
	//Generate some vegetation.
	DynamicArray<VegetationTransformation> vegetationTransformations;
	vegetationTransformations.Reserve(WorldAchitectConstants::VEGETATION_DENSITY);

	for (uint64 i = 0; i < WorldAchitectConstants::VEGETATION_DENSITY; ++i)
	{
		//Generate a position.
		Vector3 position{ Vector3(CatalystMath::RandomFloatInRange(-50.0f, 50.0f), 100.0f, CatalystMath::RandomFloatInRange(-50.0f, 50.0f)) };

		vegetationTransformations.EmplaceFast(position, Vector2(CatalystMath::RandomFloatInRange(2.0f, 4.0f), CatalystMath::RandomFloatInRange(0.75f, 1.25f)), CatalystMath::RandomFloatInRange(0.0f, 360.0f));
	}

	VegetationEntity *const RESTRICT vegetation{ EntitySystem::Instance->CreateEntity<VegetationEntity>() };
	vegetation->Initialize(ResourceLoader::GetVegetationMaterial(WorldAchitectConstants::DEFAULT_VEGETATION_MATERIAL), vegetationTransformations, VegetationProperties(100.0f));
	*/
}

/*
*	Generates the terrain.
*/
RESTRICTED TerrainEntity *const RESTRICT WorldArchitect::GenerateTerrain(const Vector3 &worldPosition) NOEXCEPT
{
	//Create the terrain properties!
	CPUTexture2D terrainProperties{ WorldAchitectConstants::HEIGHT_MAP_RESOLUTION };

	static const float randomOffset{ CatalystMath::RandomFloatInRange(0.0f, 1.0f) };
	
	constexpr float startingFrequency{ 1.0f };
	constexpr float frequenyMultiplier{ 2.0f };
	constexpr float contributionMultiplier{ 0.25f };

	const float xOffset{ worldPosition.X / WorldAchitectConstants::TERRAIN_EXTENT };
	const float yOffset{ worldPosition.Z / WorldAchitectConstants::TERRAIN_EXTENT };

	for (uint32 i = 0; i < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++j)
		{
			//Calculate the X and Y coordinate.
			const float xCoordinate{ static_cast<float>(i) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION - 1) + xOffset };
			const float yCoordinate{ static_cast<float>(j) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION - 1) + yOffset };

			//Set the height.
			Vector4 &terrainPropery{ terrainProperties.At(i, j) };

			float frequency{ startingFrequency };
			float contribution{ 1.0f };
			terrainPropery.W = PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) * contribution;

			frequency *= frequenyMultiplier;
			contribution *= contributionMultiplier;
			terrainPropery.W += PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) * contribution;

			frequency *= frequenyMultiplier;
			contribution *= contributionMultiplier;
			terrainPropery.W += PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) * contribution;

			frequency *= frequenyMultiplier;
			contribution *= contributionMultiplier;
			terrainPropery.W += PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) * contribution;

			frequency *= frequenyMultiplier;
			contribution *= contributionMultiplier;
			terrainPropery.W += PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) * contribution;

			terrainPropery.W += 0.5f;
		}
	}

	constexpr float heightMapPositionoffset{ WorldAchitectConstants::TERRAIN_EXTENT / WorldAchitectConstants::HEIGHT_MAP_RESOLUTION };

	for (uint32 i = 0; i < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++j)
		{
			const Vector3 left{ -heightMapPositionoffset, terrainProperties.At(i > 0 ? i - 1 : i, j).W * WorldAchitectConstants::TERRAIN_HEIGHT, 0.0f };
			const Vector3 right{ heightMapPositionoffset, terrainProperties.At(i < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION - 1 ? i + 1 : i, j).W * WorldAchitectConstants::TERRAIN_HEIGHT, 0.0f };
			const Vector3 up{ 0.0f, terrainProperties.At(i, j > 0 ? j - 1 : j).W * WorldAchitectConstants::TERRAIN_HEIGHT, -heightMapPositionoffset };
			const Vector3 down{ 0.0f, terrainProperties.At(i, j < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION - 1 ? j + 1 : j).W * WorldAchitectConstants::TERRAIN_HEIGHT, heightMapPositionoffset };
			const Vector3 center{ 0.0f, terrainProperties.At(i, j).W * WorldAchitectConstants::TERRAIN_HEIGHT, 0.0f };

			const Vector3 normal1{ Vector3::CrossProduct(up - center, left - center) };
			const Vector3 normal2{ Vector3::CrossProduct(right - center, up - center) };
			const Vector3 normal3{ Vector3::CrossProduct(down - center, right - center) };
			const Vector3 normal4{ Vector3::CrossProduct(left - center, down - center) };

			Vector3 finalNormal{ (Vector3::Normalize(normal1 + normal2 + normal3 + normal4) + 1.0f) * 0.5f };

			Vector4 &terrainPropery{ terrainProperties.At(i, j) };

			terrainPropery.X = finalNormal.X;
			terrainPropery.Y = finalNormal.Y;
			terrainPropery.Z = finalNormal.Z;
		}
	}

	//Calculate the layer weights.
	CPUTexture2D layerWeights{ WorldAchitectConstants::HEIGHT_MAP_RESOLUTION };

	for (uint32 i = 0; i < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++j)
		{
			//Get the height.
			const float terrainHeight{ terrainProperties.At(i, j).W };
			const float worldHeight{ terrainHeight * WorldAchitectConstants::TERRAIN_HEIGHT };

			//Set the weight of the first grass layer.
			Vector4 &layerWeight{ layerWeights.At(i, j) };

			layerWeight.X = 0.0f;

			//Set the weight of the second grass layer.
			if (worldHeight < PhysicsSystem::Instance->GetOceanHeight())
			{
				layerWeight.Y = 0.0f;
			}

			else if (worldHeight > PhysicsSystem::Instance->GetOceanHeight() + 100.0f)
			{
				layerWeight.Y = 1.0f;
			}

			else
			{
				layerWeight.Y = (worldHeight - PhysicsSystem::Instance->GetOceanHeight()) * 0.01f;
			}

			//Set the weight of the rock layer.
			const Vector4 &terrainPropertiesValue{ terrainProperties.At(i, j) };
			layerWeight.Z = 1.0f - CatalystMath::SmoothStep<5>(CatalystMath::Clamp<float>(Vector3::DotProduct(Vector3(terrainPropertiesValue.X, terrainPropertiesValue.Y, terrainPropertiesValue.Z), Vector3(0.0f, 1.0f, 0.0f)) - 0.25f, 0.0f, 1.0f));

			//Set the weight of the snow layer.
			if (worldHeight < 950.0f)
			{
				layerWeight.W = 0.0f;
			}

			else if (worldHeight > 1'050.0f)
			{
				layerWeight.W = CatalystMath::Clamp<float>(1.0f - layerWeights.At(i, j).Z, 0.0f, 1.0f);
			}

			else
			{
				layerWeight.W = CatalystMath::Clamp<float>(((worldHeight - 950.0f) * 0.01f) - layerWeights.At(i, j).Z, 0.0f, 1.0f);
			}
		}
	}

	//Get the terrain material data.
	TerrainMaterial terrainMaterial{ ResourceLoader::GetTerrainMaterial(WorldAchitectConstants::GRASS_TERRAIN_MATERIAL) };

	Texture2DHandle layerWeightsTexture = RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(layerWeights), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	//Create the terrain entity!
	TerrainInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<TerrainInitializationData>() };

	RenderingUtilities::CalculateTerrainAxisAlignedBoundingBox(&terrainProperties, WorldAchitectConstants::TERRAIN_EXTENT, &worldPosition, WorldAchitectConstants::TERRAIN_HEIGHT, &data->axisAlignedBoundingBox);
	data->terrainProperties = terrainProperties;
	data->terrainUniformData = TerrainUniformData(3.0f, 0.5f, 1.0f, 10.0f, 2.0f, WorldAchitectConstants::TERRAIN_HEIGHT, WorldAchitectConstants::TERRAIN_EXTENT, WorldAchitectConstants::TERRAIN_EXTENT * 0.1f, worldPosition);
	data->layerWeightsTexture = layerWeightsTexture;
	data->terrainMaterial = terrainMaterial;

	TerrainEntity *RESTRICT terrain{ EntitySystem::Instance->CreateEntity<TerrainEntity>() };

	EntitySystem::Instance->RequestInitialization(terrain, data, false);

	return terrain;
}

/*
*	Generates the vegetation.
*/
void WorldArchitect::GenerateVegetation(const Vector3 &worldPosition, const float extent) NOEXCEPT
{
	/*
	constexpr uint8 numberOfVegetationLayers{ 8 };

	StaticArray<float, numberOfVegetationLayers> cutoffDistances
	{
		1024.0f,
		512.0f,
		256.0f,
		128.0f,
		64.0f,
		64.0f,
		32.0f,
		32.0f
	};

	StaticArray<Semaphore, numberOfVegetationLayers> vegetationSemaphores;

	for (uint64 i = 0; i < numberOfVegetationLayers; ++i)
	{
		TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
		{
			DynamicArray<VegetationTransformation> vegetationTransformations;
			vegetationTransformations.Reserve(WorldAchitectConstants::VEGETATION_DENSITY);

			for (uint64 i = 0; i < WorldAchitectConstants::VEGETATION_DENSITY; ++i)
			{
				//Generate a position.
				Vector3 position{ Vector3(CatalystMath::RandomFloatInRange(-5'000.0f * 0.5f, 5'000.0f * 0.5f), 0.0f, CatalystMath::RandomFloatInRange(-5'000.0f * 0.5f, 5'000.0f * 0.5f)) };

				//Align the height to the terrain.
				position.Y = PhysicsSystem::Instance->GetTerrainHeightAtPosition(position);

				//Decrease the height a bit to make the vegetation "sink into" the ground.
				position.Y -= 0.1f;

				//Test the height against the water height.
				if (position.Y < PhysicsSystem::Instance->GetWaterHeight() + 50.0f)
				{
					continue;
				}

				else if (position.Y < PhysicsSystem::Instance->GetWaterHeight() + 150.0f)
				{
					if (CatalystMath::RandomChance(1.0f - ((position.Y - (PhysicsSystem::Instance->GetWaterHeight() + 50.0f)) * 0.01f)))
					{
						continue;
					}
				}

				//Test the height against the snow height.
				if (position.Y > 1'000.0f)
				{
					continue;
				}

				else if (position.Y > 900.0f)
				{
					if (CatalystMath::RandomChance((position.Y - 900.0f) * 0.01f))
					{
						continue;
					}
				}

				//Test the position against the terrain normal.
				const Vector3 terrainNormal{ PhysicsSystem::Instance->GetTerrainNormalAtPosition(position) };

				const float dotProduct{ Vector3::DotProduct(terrainNormal, Vector3(0.0f, 1.0f, 0.0f)) };

				if (dotProduct < 0.75f)
				{
					continue;
				}

				if (!CatalystMath::RandomChance((dotProduct - 0.75f) * 4.0f))
				{
					continue;
				}

				vegetationTransformations.EmplaceFast(position, Vector2(CatalystMath::RandomFloatInRange(1.0f, 2.0f), CatalystMath::RandomFloatInRange(1.0f, 1.75f)), CatalystMath::RandomFloatInRange(0.0f, 360.0f));
			}

			VegetationEntity *const RESTRICT vegetation{ EntitySystem::Instance->CreateEntity<VegetationEntity>() };
			vegetation->Initialize(ResourceLoader::GetVegetationMaterial(WorldAchitectConstants::DEFAULT_VEGETATION_MATERIAL), vegetationTransformations, VegetationProperties(*reinterpret_cast<float *const RESTRICT>(arguments)));

		}, &cutoffDistances[i], &vegetationSemaphores[i]));
	}

	for (Semaphore &vegetationSemaphore : vegetationSemaphores)
	{
		vegetationSemaphore.WaitFor();
	}
	*/
}

/*
*	Generates the particle systems.
*/
void WorldArchitect::GenerateParticleSystems(const Vector3 &worldPosition, const float extent) NOEXCEPT
{
	//Spawn some fog. (:
	ParticleSystemEntity *const RESTRICT fogParticles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };
	fogParticles->Initialize(ResourceLoader::GetParticleMaterial(WorldAchitectConstants::FOG_1_MATERIAL), ParticleSystemProperties(2.5f, 30.0f, 0.0f, Vector2(100.0f, 100.0f), Vector2(500.0f, 500.0f), Vector3(-extent * 0.5f, 0.0f, -extent * 0.5f), Vector3(extent * 0.5f, 250.0f, extent * 0.5f), PhysicsSystem::Instance->GetWindDirection() * 1.0f, PhysicsSystem::Instance->GetWindDirection() * 2.5f, worldPosition));
}