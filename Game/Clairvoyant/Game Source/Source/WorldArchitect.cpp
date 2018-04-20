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
#include <Entities/TerrainEntity.h>
#include <Entities/VegetationEntity.h>
#include <Entities/WaterEntity.h>

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
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/TerrainUniformData.h>
#include <Rendering/Engine Layer/TextureData.h>
#include <Rendering/Engine Layer/WaterMaterial.h>
#include <Rendering/Engine Layer/WaterUniformData.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

namespace WorldAchitectConstants
{
#if defined(CATALYST_DEBUG)
	static constexpr uint32 HEIGHT_MAP_RESOLUTION{ 1'024 };
#else
	static constexpr uint32 HEIGHT_MAP_RESOLUTION{ 4'096 };
#endif
	static constexpr float TERRAIN_HEIGHT{ 1'000.0f };
	static constexpr float TERRAIN_SIZE{ 10'000.0f };
	static constexpr float WATER_HEIGHT{ -1.0f };
	static constexpr uint64 VEGETATION_DENSITY{ 10'000'000 };

	//Resource ID's.
	static constexpr HashString DAY_ENVIRONMENT_MATERIAL{ "DayEnvironmentMaterial" };
	static constexpr HashString NIGHT_ENVIRONMENT_MATERIAL{ "NightEnvironmentMaterial" };
	static constexpr HashString STONE_MATERIAL{ "StoneMaterial" };
	static constexpr HashString TREE_STOMP_MATERIAL{ "TreeStompMaterial" };
	static constexpr HashString STONE_MODEL{ "StoneModel" };
	static constexpr HashString TREE_STOMP_MODEL{ "TreeStompModel" };
	static constexpr HashString DEFAULT_TERRAIN_MATERIAL{ "DefaultTerrainMaterial" };
	static constexpr HashString DEFAULT_VEGETATION_MATERIAL{ "DefaultVegetationMaterial" };
	static constexpr HashString DEFAULT_WATER_MATERIAL{ "DefaultWaterMaterial" };
	static constexpr HashString FOG_1_MATERIAL{ "Fog1Material" };
	static constexpr HashString CLOUD_1_MATERIAL{ "Cloud1Material" };
	static constexpr HashString CLOUD_2_MATERIAL{ "Cloud2Material" };
	static constexpr HashString CLOUD_3_MATERIAL{ "Cloud3Material" };
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
	//Load the resource collection file.
	Semaphore resourceSemaphore;
	ResourceLoader::LoadResourceCollection(CLAIRVOYANT_RESOURCES_FOLDER "ClairvoyantResourceCollection.crc", &resourceSemaphore);

	//Create the sun!
	sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	sun->SetIntensity(10.0f);

	//Create the terrain properties!
	CPUTexture2D terrainProperties{ WorldAchitectConstants::HEIGHT_MAP_RESOLUTION };

	const float randomOffset{ CatalystMath::RandomFloatInRange(0.0f, 1.0f) };

	for (uint32 i = 0; i < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++j)
		{
			float frequency = 5.0f;
			float multiplier = 1.0f;
			terrainProperties.At(i, j).W = PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			frequency *= 2.0f;
			multiplier *= 0.5f;
			terrainProperties.At(i, j).W += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			frequency *= 2.0f;
			multiplier *= 0.5f;
			terrainProperties.At(i, j).W += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;
			
			frequency *= 2.0f;
			multiplier *= 0.5f;
			terrainProperties.At(i, j).W += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			frequency *= 2.0f;
			multiplier *= 0.5f;
			terrainProperties.At(i, j).W += PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, static_cast<float>(j) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * frequency, 0.0f, randomOffset) * multiplier;

			terrainProperties.At(i, j).W = CatalystMath::LinearlyInterpolate(terrainProperties.At(i, j).W, CatalystMath::SmoothStep<1>(terrainProperties.At(i, j).W), 0.25f);
		}
	}

	static constexpr float heightMapPositionoffset{ WorldAchitectConstants::TERRAIN_SIZE / WorldAchitectConstants::HEIGHT_MAP_RESOLUTION };

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

			terrainProperties.At(i, j).X = finalNormal.X;
			terrainProperties.At(i, j).Y = finalNormal.Y;
			terrainProperties.At(i, j).Z = finalNormal.Z;
		}
	}

	//Calculate the layer weights.
	CPUTexture2D layerWeights{ WorldAchitectConstants::HEIGHT_MAP_RESOLUTION };

	for (uint32 i = 0; i < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++i)
	{
		for (uint32 j = 0; j < WorldAchitectConstants::HEIGHT_MAP_RESOLUTION; ++j)
		{
			//Determine the weight of the grass layer.
			Vector4 terrainPropertiesValue{ terrainProperties.At(i, j) * 2.0f - 1.0f };
			
			const float heightValue{ terrainPropertiesValue.W };

			if (heightValue > (WorldAchitectConstants::WATER_HEIGHT + 0.1f))
			{
				layerWeights.At(i, j).X = 1.0f;
			}

			else if (heightValue < WorldAchitectConstants::WATER_HEIGHT)
			{
				layerWeights.At(i, j).X = 0.0f;
			}

			else
			{
				layerWeights.At(i, j).X = (heightValue + -WorldAchitectConstants::WATER_HEIGHT) * 10.0f;
			}

			layerWeights.At(i, j).X = CatalystMath::SmoothStep<1>(layerWeights.At(i, j).X);

			//Determine the weight of the dirt layer.
			constexpr float dirtLayerFrequency{ 1.0f };
			layerWeights.At(i, j).Y = (PerlinNoiseGenerator::GenerateNoise(static_cast<float>(i) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * dirtLayerFrequency, static_cast<float>(j) / static_cast<float>(WorldAchitectConstants::HEIGHT_MAP_RESOLUTION) * dirtLayerFrequency, 0.0f, randomOffset) + 1.0f) * 0.5f;
			layerWeights.At(i, j).Y = CatalystMath::SmoothStep<5>(layerWeights.At(i, j).Y);

			//Determine the weight of the rock layer.
			layerWeights.At(i, j).Z = 1.0f - CatalystMath::SmoothStep<3>(CatalystMath::Clamp(Vector3::DotProduct(Vector3(terrainPropertiesValue.X, terrainPropertiesValue.Y, terrainPropertiesValue.Z), Vector3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f));

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

	//Wait for the resources to finish loading.
	resourceSemaphore.WaitFor();

	//Set the environment material.
	RenderingSystem::Instance->SetEnvironmentMaterial(ResourceLoader::GetEnvironmentMaterial(WorldAchitectConstants::DAY_ENVIRONMENT_MATERIAL));

	//Get the terrain material data.
	TerrainMaterial terrainMaterial{ ResourceLoader::GetTerrainMaterial(WorldAchitectConstants::DEFAULT_TERRAIN_MATERIAL) };

	Texture2DHandle layerWeightsTexture = RenderingSystem::Instance->Create2DTexture(TextureData(TextureDataContainer(layerWeights), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R32G32B32A32_Float));

	//Create the terrain entity!
	TerrainEntity *RESTRICT terrain{ EntitySystem::Instance->CreateEntity<TerrainEntity>() };
	terrain->Initialize(256, terrainProperties, TerrainUniformData(3.0f, 0.5f, 1.0f, 10.0f, 2.0f, WorldAchitectConstants::TERRAIN_HEIGHT, WorldAchitectConstants::TERRAIN_SIZE, WorldAchitectConstants::TERRAIN_SIZE * 0.05f, Vector3(0.0f, 0.0f, 0.0f)), layerWeightsTexture, terrainMaterial);

	//Load the water material.
	WaterMaterial waterMaterial{ ResourceLoader::GetWaterMaterial(WorldAchitectConstants::DEFAULT_WATER_MATERIAL) };

	//Create the water.
	WaterEntity *RESTRICT water = EntitySystem::Instance->CreateEntity<WaterEntity>();
	water->Initialize(waterMaterial, WaterUniformData(WorldAchitectConstants::TERRAIN_SIZE, 250.0f, Vector3(0.0f, 0.0f, 0.0f)));

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

	/*
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
	*/

	//Create some grass.
	constexpr uint8 numberOfVegetationLayers{ 8 };

	StaticArray<float, numberOfVegetationLayers> cutoffDistances
	{
		512.0f,
		256.0f,
		128.0f,
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

			//Create the vegetation.
			for (uint64 i = 0; i < WorldAchitectConstants::VEGETATION_DENSITY; ++i)
			{
				Vector3 position{ Vector3(CatalystMath::RandomFloatInRange(-WorldAchitectConstants::TERRAIN_SIZE * 0.5f, WorldAchitectConstants::TERRAIN_SIZE * 0.5f), 0.0f, CatalystMath::RandomFloatInRange(-WorldAchitectConstants::TERRAIN_SIZE * 0.5f, WorldAchitectConstants::TERRAIN_SIZE * 0.5f)) };

				const Vector3 terrainNormal{ PhysicsSystem::Instance->GetTerrainNormalAtPosition(position) };

				const float dotProduct{ Vector3::DotProduct(terrainNormal, Vector3(0.0f, 1.0f, 0.0f)) };

				if (dotProduct < 0.8f)
				{
					continue;
				}

				else if (!CatalystMath::RandomChance((dotProduct - 0.8f) * 5.0f))
				{
					continue;
				}

				position.Y = PhysicsSystem::Instance->GetTerrainHeightAtPosition(position);

				if (position.Y <= PhysicsSystem::Instance->GetWaterHeight())
				{
					continue;
				}

				if (position.Y > 725.0f)
				{
					continue;
				}

				position.Y -= 0.1f;

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
}

/*
*	Updates the world architects.
*/
void WorldArchitect::Update(const float deltaTime) NOEXCEPT
{
	//Constantly rotate the sun.
	sun->Rotate(Vector3(-10.0f * deltaTime, 0.0f, 0.0f));
}