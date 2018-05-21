//Header file.
#include <WorldArchitect.h>

//Clairvoyant.
#include <ClairvoyantSettings.h>
#include <ClairvoyantSoundGUIDs.h>

//Engine core.
#include <Engine Core/HashString.h>

//Entities.
#include <Entities/DirectionalLightEntity.h>
#include <Entities/InstancedPhysicalEntity.h>
#include <Entities/ParticleSystemEntity.h>
#include <Entities/PointLightEntity.h>
#include <Entities/Sound3DEntity.h>
#include <Entities/SpotLightEntity.h>
#include <Entities/StaticPhysicalEntity.h>
#include <Entities/TerrainEntity.h>
#include <Entities/VegetationEntity.h>

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
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/TerrainUniformData.h>
#include <Rendering/Engine Layer/TextureData.h>
#include <Rendering/Engine Layer/WaterUniformData.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/EnvironmentSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WorldArchitect);

//World architects constants.
namespace WorldAchitectConstants
{
#if defined(CATALYST_DEBUG)
	constexpr uint32 HEIGHT_MAP_RESOLUTION{ 1'024 };
#else
	constexpr uint32 HEIGHT_MAP_RESOLUTION{ 4'096 };
#endif
	constexpr float TERRAIN_SIZE{ 10'000.0f };
	constexpr uint64 VEGETATION_DENSITY{ 25'000'000 };

	//Resource ID's.
	constexpr HashString DAY_ENVIRONMENT_MATERIAL{ "DayEnvironmentMaterial" };
	constexpr HashString NIGHT_ENVIRONMENT_MATERIAL{ "NightEnvironmentMaterial" };
	constexpr HashString STONE_MATERIAL{ "StoneMaterial" };
	constexpr HashString TREE_STOMP_MATERIAL{ "TreeStompMaterial" };
	constexpr HashString STONE_MODEL{ "StoneModel" };
	constexpr HashString TREE_STOMP_MODEL{ "TreeStompModel" };
	constexpr HashString DEFAULT_TERRAIN_MATERIAL{ "DefaultTerrainMaterial" };
	constexpr HashString DEFAULT_VEGETATION_MATERIAL{ "DefaultVegetationMaterial" };
	constexpr HashString DEFAULT_WATER_MATERIAL{ "DefaultWaterMaterial" };
	constexpr HashString FOG_1_MATERIAL{ "Fog1Material" };
	constexpr HashString CLOUD_1_MATERIAL{ "Cloud1Material" };
	constexpr HashString CLOUD_2_MATERIAL{ "Cloud2Material" };
	constexpr HashString CLOUD_3_MATERIAL{ "Cloud3Material" };
	constexpr HashString CLOUD_4_MATERIAL{ "Cloud4Material" };
}

/*
*	Initializes the world architect.
*/
void WorldArchitect::Initialize() NOEXCEPT
{
	//Load the resource collection file.
	ResourceLoader::LoadResourceCollection(CLAIRVOYANT_RESOURCES_FOLDER "ClairvoyantResourceCollection.crc");

	//Set the environment material.
	EnvironmentSystem::Instance->SetNightEnvironmentMaterial(ResourceLoader::GetEnvironmentMaterial(WorldAchitectConstants::NIGHT_ENVIRONMENT_MATERIAL));
	EnvironmentSystem::Instance->SetDayEnvironmentMaterial(ResourceLoader::GetEnvironmentMaterial(WorldAchitectConstants::DAY_ENVIRONMENT_MATERIAL));

	//Set the ocean material.
	EnvironmentSystem::Instance->SetOceanMaterial(ResourceLoader::GetOceanMaterial(WorldAchitectConstants::DEFAULT_WATER_MATERIAL));

	//Generate the island.
	GenerateIsland(Vector3(0.0f, 0.0f, 0.0f));
	GenerateIsland(Vector3(25'000.0f, 0.0f, 0.0f));
	GenerateIsland(Vector3(-25'000.0f, 0.0f, 0.0f));
	GenerateIsland(Vector3(0.0f, 0.0f, 25'000.0f));
	GenerateIsland(Vector3(0.0f, 0.0f, -25'000.0f));

	/*
	//Create the stone model.
	PhysicalModel stoneModel{ ResourceLoader::GetPhysicalModel(WorldAchitectConstants::STONE_MODEL) };

	//Create the stone material.
	PhysicalMaterial stoneMaterial{ ResourceLoader::GetPhysicalMaterial(WorldAchitectConstants::STONE_MATERIAL) };

	stoneModel.SetMaterial(stoneMaterial);

	DynamicArray<Matrix4> stoneTransformations;
	stoneTransformations.Reserve(1'000);

	//Create the stones.
	const FMOD::Studio::EventDescription *const RESTRICT windEventDescription{ SoundSystem::Instance->GetEventDescription(&ClairvoyantSoundGUIDs::WIND) };

	for (uint64 i = 0; i < 1'000; ++i)
	{
		Vector3 position{ Vector3(CatalystMath::RandomFloatInRange(-WorldAchitectConstants::TERRAIN_SIZE * 0.5f, WorldAchitectConstants::TERRAIN_SIZE * 0.5f), 0.0f, CatalystMath::RandomFloatInRange(-WorldAchitectConstants::TERRAIN_SIZE * 0.5f, WorldAchitectConstants::TERRAIN_SIZE * 0.5f)) };

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

		const Vector3 rotation{ 0.0f, CatalystMath::RandomFloatInRange(0.0f, 360.0f), 0.0f };
		const float stoneScale = CatalystMath::RandomFloatInRange(0.1f, 0.5f);
		const Vector3 scale{ stoneScale, stoneScale, stoneScale };

		stoneTransformations.EmplaceFast(position, rotation, scale);

		Sound3DEntity *const RESTRICT sound{ EntitySystem::Instance->CreateEntity<Sound3DEntity>() };
		sound->Initialize(windEventDescription);
		sound->Move(position);
	}

	InstancedPhysicalEntity *RESTRICT stones = EntitySystem::Instance->CreateEntity<InstancedPhysicalEntity>();
	stones->Initialize(stoneModel, stoneTransformations);

	//Aaand create a really big stone.
	StaticPhysicalEntity *const RESTRICT bigStone{ EntitySystem::Instance->CreateEntity<StaticPhysicalEntity>() };
	bigStone->Initialize(stoneModel, Vector3(0.0f, 10'000.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 100.0f));

	//Create the tree stomp model.
	PhysicalModel treeStompModel{ ResourceLoader::GetPhysicalModel(WorldAchitectConstants::TREE_STOMP_MODEL) };

	//Create the tree stomp material.
	PhysicalMaterial treeStompMaterial{ ResourceLoader::GetPhysicalMaterial(WorldAchitectConstants::TREE_STOMP_MATERIAL) };

	treeStompModel.SetMaterial(treeStompMaterial);

	DynamicArray<Matrix4> treeStompTransformations;
	treeStompTransformations.Reserve(1'000);

	//Create the tree stomps.
	for (uint64 i = 0; i < 1'000; ++i)
	{
		Vector3 position{ Vector3(CatalystMath::RandomFloatInRange(-WorldAchitectConstants::TERRAIN_SIZE * 0.5f, WorldAchitectConstants::TERRAIN_SIZE * 0.5f), 0.0f, CatalystMath::RandomFloatInRange(-WorldAchitectConstants::TERRAIN_SIZE * 0.5f, WorldAchitectConstants::TERRAIN_SIZE * 0.5f)) };

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

		const Vector3 rotation{ -90.0f, 0.0f, CatalystMath::RandomFloatInRange(0.0f, 360.0f) };
		const float uniformScale = CatalystMath::RandomFloatInRange(0.75f, 1.0f);
		const Vector3 scale{ uniformScale, uniformScale, uniformScale };

		treeStompTransformations.EmplaceFast(position, rotation, scale);
	}

	InstancedPhysicalEntity *RESTRICT treeStomps = EntitySystem::Instance->CreateEntity<InstancedPhysicalEntity>();
	treeStomps->Initialize(treeStompModel, treeStompTransformations);

	//Spawn some fog. (:
	ParticleSystemEntity *const RESTRICT fogParticles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };
	fogParticles->Initialize(ResourceLoader::GetParticleMaterial(WorldAchitectConstants::FOG_1_MATERIAL), ParticleSystemProperties(2.5f, 30.0f, 0.0f, Vector2(100.0f, 100.0f), Vector2(500.0f, 500.0f), Vector3(-WorldAchitectConstants::TERRAIN_SIZE * 0.5f, 0.0f, -WorldAchitectConstants::TERRAIN_SIZE * 0.5f), Vector3(WorldAchitectConstants::TERRAIN_SIZE * 0.5f, 250.0f, WorldAchitectConstants::TERRAIN_SIZE * 0.5f), PhysicsSystem::Instance->GetWindDirection() * 1.0f, PhysicsSystem::Instance->GetWindDirection() * 2.5f, Vector3(0.0f, 0.0f, 0.0f)));

	//Spawn some clouds. (:
	constexpr float cloudMinimumScale{ 2'500.0f };
	constexpr float cloudMaximumScale{ 7'500.0f };

	ParticleSystemEntity *const RESTRICT cloud1Particles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };
	cloud1Particles->Initialize(ResourceLoader::GetParticleMaterial(WorldAchitectConstants::CLOUD_1_MATERIAL), ParticleSystemProperties(10.0f, 120.0f, 10.0f, Vector2(cloudMinimumScale, cloudMinimumScale), Vector2(cloudMaximumScale, cloudMaximumScale), Vector3(-WorldAchitectConstants::TERRAIN_SIZE * 2.0f, 2'000.0f, -WorldAchitectConstants::TERRAIN_SIZE * 2.0f), Vector3(WorldAchitectConstants::TERRAIN_SIZE * 2.0f, 20'000.0f, WorldAchitectConstants::TERRAIN_SIZE * 2.0f), PhysicsSystem::Instance->GetWindDirection() * PhysicsSystem::Instance->GetWindStrength(), PhysicsSystem::Instance->GetWindDirection() * PhysicsSystem::Instance->GetWindStrength() * 10.0f, Vector3(0.0f, 0.0f, 0.0f)));

	ParticleSystemEntity *const RESTRICT cloud2Particles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };
	cloud2Particles->Initialize(ResourceLoader::GetParticleMaterial(WorldAchitectConstants::CLOUD_2_MATERIAL), ParticleSystemProperties(10.0f, 120.0f, 10.0f, Vector2(cloudMinimumScale, cloudMinimumScale), Vector2(cloudMaximumScale, cloudMaximumScale), Vector3(-WorldAchitectConstants::TERRAIN_SIZE * 2.0f, 2'000.0f, -WorldAchitectConstants::TERRAIN_SIZE * 2.0f), Vector3(WorldAchitectConstants::TERRAIN_SIZE * 2.0f, 20'000.0f, WorldAchitectConstants::TERRAIN_SIZE * 2.0f), PhysicsSystem::Instance->GetWindDirection() * PhysicsSystem::Instance->GetWindStrength(), PhysicsSystem::Instance->GetWindDirection() * PhysicsSystem::Instance->GetWindStrength() * 10.0f, Vector3(0.0f, 0.0f, 0.0f)));

	ParticleSystemEntity *const RESTRICT cloud3Particles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };
	cloud3Particles->Initialize(ResourceLoader::GetParticleMaterial(WorldAchitectConstants::CLOUD_3_MATERIAL), ParticleSystemProperties(10.0f, 120.0f, 10.0f, Vector2(cloudMinimumScale, cloudMinimumScale), Vector2(cloudMaximumScale, cloudMaximumScale), Vector3(-WorldAchitectConstants::TERRAIN_SIZE * 2.0f, 2'000.0f, -WorldAchitectConstants::TERRAIN_SIZE * 2.0f), Vector3(WorldAchitectConstants::TERRAIN_SIZE * 2.0f, 20'000.0f, WorldAchitectConstants::TERRAIN_SIZE * 2.0f), PhysicsSystem::Instance->GetWindDirection() * PhysicsSystem::Instance->GetWindStrength(), PhysicsSystem::Instance->GetWindDirection() * PhysicsSystem::Instance->GetWindStrength() * 10.0f, Vector3(0.0f, 0.0f, 0.0f)));

	ParticleSystemEntity *const RESTRICT cloud4Particles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };
	cloud4Particles->Initialize(ResourceLoader::GetParticleMaterial(WorldAchitectConstants::CLOUD_4_MATERIAL), ParticleSystemProperties(10.0f, 120.0f, 10.0f, Vector2(cloudMinimumScale, cloudMinimumScale), Vector2(cloudMaximumScale, cloudMaximumScale), Vector3(-WorldAchitectConstants::TERRAIN_SIZE * 2.0f, 2'000.0f, -WorldAchitectConstants::TERRAIN_SIZE * 2.0f), Vector3(WorldAchitectConstants::TERRAIN_SIZE * 2.0f, 20'000.0f, WorldAchitectConstants::TERRAIN_SIZE * 2.0f), PhysicsSystem::Instance->GetWindDirection() * PhysicsSystem::Instance->GetWindStrength(), PhysicsSystem::Instance->GetWindDirection() * PhysicsSystem::Instance->GetWindStrength() * 10.0f, Vector3(0.0f, 0.0f, 0.0f)));
	*/
}

/*
*	Updates the world architects.
*/
void WorldArchitect::Update(const float deltaTime) NOEXCEPT
{

}

/*
*	Given an X and Y coordinate in the 0.0f-1.0f range, calculate and return the island falloff multiplier.
*/
float WorldArchitect::CalculateIslandFalloffMultiplier(const float xCoordinate, const float yCoordinate) NOEXCEPT
{
	constexpr Vector2 centerCoordinate{ 0.5f, 0.5f };

	const float distanceSquared{ Vector2::LengthSquared(Vector2(xCoordinate, yCoordinate) - centerCoordinate) };
	const float distance{ distanceSquared > 0.0f ? CatalystMath::SquareRoot(distanceSquared) * 2.0f : 0.0f };
	const float inverseDistance{ 1.0f - distance };

	return inverseDistance > 0.0f ? CatalystMath::SmoothStep<1>(inverseDistance) : inverseDistance;
}

/*
*	Generates a new island.
*/
void WorldArchitect::GenerateIsland(const Vector3 &worldPosition) NOEXCEPT
{
	//Generate the terrain.
	GenerateTerrain(worldPosition);

	//Generate the vegetation.
	//GenerateVegetation(worldPosition);
}

/*
*	Generates the terrain.
*/
void WorldArchitect::GenerateTerrain(const Vector3 &worldPosition) NOEXCEPT
{
	//Create the terrain properties!
	CPUTexture2D terrainProperties{ WorldAchitectConstants::HEIGHT_MAP_RESOLUTION };

	const float randomOffset{ CatalystMath::RandomFloatInRange(0.0f, 1.0f) };
	const float randomHeight{ CatalystMath::RandomFloatInRange(500.0f, 1'500.0f) };

	for (uint32 i = 0; i < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++j)
		{
			//Calculate the X and Y coordinate.
			const float xCoordinate{ static_cast<float>(i) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) };
			const float yCoordinate{ static_cast<float>(j) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) };

			//Calculate the island falloff multiplier.
			const float islandFalloffMultiplier{ CalculateIslandFalloffMultiplier(xCoordinate, yCoordinate) };

			//Set the height.
			Vector4 &terrainPropery{ terrainProperties.At(i, j) };

			float frequency{ 2.5f };
			float contribution{ 1.0f };
			terrainPropery.W = ((PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) + 1.0f) * 0.5f) * contribution * islandFalloffMultiplier;

			frequency = 10.0f;
			contribution = 0.5f;
			terrainPropery.W += ((PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) + 1.0f) * 0.5f) * contribution * islandFalloffMultiplier;

			frequency = 25.0f;
			contribution = 0.25f;
			terrainPropery.W += ((PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) + 1.0f) * 0.5f) * contribution * islandFalloffMultiplier;

			frequency = 100.0f;
			contribution = 0.125f;
			terrainPropery.W += ((PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) + 1.0f) * 0.5f) * contribution * islandFalloffMultiplier;

			frequency = 250.0f;
			contribution = 0.0625f;
			terrainPropery.W += ((PerlinNoiseGenerator::GenerateNoise(xCoordinate * frequency, yCoordinate * frequency, 0.0f, randomOffset) + 1.0f) * 0.5f) * contribution * islandFalloffMultiplier;
		}
	}

	static constexpr float heightMapPositionoffset{ WorldAchitectConstants::TERRAIN_SIZE / WorldAchitectConstants::HEIGHT_MAP_RESOLUTION };

	for (uint32 i = 0; i < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++j)
		{
			const Vector3 left{ -heightMapPositionoffset, terrainProperties.At(i > 0 ? i - 1 : i, j).W * randomHeight, 0.0f };
			const Vector3 right{ heightMapPositionoffset, terrainProperties.At(i < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION - 1 ? i + 1 : i, j).W * randomHeight, 0.0f };
			const Vector3 up{ 0.0f, terrainProperties.At(i, j > 0 ? j - 1 : j).W * randomHeight, -heightMapPositionoffset };
			const Vector3 down{ 0.0f, terrainProperties.At(i, j < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION - 1 ? j + 1 : j).W * randomHeight, heightMapPositionoffset };
			const Vector3 center{ 0.0f, terrainProperties.At(i, j).W * randomHeight, 0.0f };

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
			const float height{ terrainProperties.At(i, j).W };
			const float worldHeight{ height * randomHeight - randomHeight * 0.1f };

			//Set the weight of the first grass layer.
			Vector4 &layerWeight{ layerWeights.At(i, j) };

			layerWeight.X = 0.0f;

			//Set the weight of the second grass layer.
			if (worldHeight < PhysicsSystem::Instance->GetWaterHeight())
			{
				layerWeight.Y = 0.0f;
			}

			else if (worldHeight > PhysicsSystem::Instance->GetWaterHeight() + 100.0f)
			{
				layerWeight.Y = 1.0f;
			}

			else
			{
				layerWeight.Y = (worldHeight - PhysicsSystem::Instance->GetWaterHeight()) * 0.01f;
			}

			//Set the weight of the rock layer.
			const Vector4 &terrainPropertiesValue{ terrainProperties.At(i, j) };
			layerWeight.Z = 1.0f - CatalystMath::SmoothStep<5>(CatalystMath::Clamp<float>(Vector3::DotProduct(Vector3(terrainPropertiesValue.X, terrainPropertiesValue.Y, terrainPropertiesValue.Z), Vector3(0.0f, 1.0f, 0.0f)) - 0.2f, 0.0f, 1.0f));

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
	TerrainMaterial terrainMaterial{ ResourceLoader::GetTerrainMaterial(WorldAchitectConstants::DEFAULT_TERRAIN_MATERIAL) };

	Texture2DHandle layerWeightsTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(layerWeights), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	//Create the terrain entity!
	TerrainEntity *RESTRICT terrain{ EntitySystem::Instance->CreateEntity<TerrainEntity>() };
	terrain->Initialize(256, terrainProperties, TerrainUniformData(3.0f, 0.5f, 1.0f, 10.0f, 2.0f, randomHeight, WorldAchitectConstants::TERRAIN_SIZE, WorldAchitectConstants::TERRAIN_SIZE * 0.05f, worldPosition + Vector3(0.0f, -(randomHeight * 0.1f), 0.0f)), layerWeightsTexture, terrainMaterial);
}

/*
*	Generates the vegetation.
*/
void WorldArchitect::GenerateVegetation(const Vector3 &worldPosition) NOEXCEPT
{
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
				Vector3 position{ Vector3(CatalystMath::RandomFloatInRange(-WorldAchitectConstants::TERRAIN_SIZE * 0.5f, WorldAchitectConstants::TERRAIN_SIZE * 0.5f), 0.0f, CatalystMath::RandomFloatInRange(-WorldAchitectConstants::TERRAIN_SIZE * 0.5f, WorldAchitectConstants::TERRAIN_SIZE * 0.5f)) };

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
}