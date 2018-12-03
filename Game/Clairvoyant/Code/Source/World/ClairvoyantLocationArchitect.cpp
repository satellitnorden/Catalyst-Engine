//Header file.
#include <World/ClairvoyantLocationArchitect.h>

//Entities.
#include <Entities/Creation/DynamicPhysicalInitializationData.h>
#include <Entities/Creation/ParticleSystemInitializationData.h>
#include <Entities/Creation/PointLightInitializationData.h>
#include <Entities/Types/DynamicPhysicalEntity.h>
#include <Entities/Types/ParticleSystemEntity.h>
#include <Entities/Types/PointLightEntity.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/EntityPlacementSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>

//Static variable definitions.
DynamicArray<Vector3> ClairvoyantLocationArchitect::_Offsets;

/*
*	Initializes the Clairvoyant location architect.
*/
void ClairvoyantLocationArchitect::Initialize() NOEXCEPT
{
	//Initialize the offsets.
	InitializeOffsets();

	EntityPlacementSystem::Instance->RegisterTwoDimensionalPlacementFunction([](const AxisAlignedBoundingBox &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities)
	{
		//Find the most appropriate position.
		Vector3 position{ FindMostAppropriatePosition(AxisAlignedBoundingBox(box._Minimum + Vector3(500.0f, 0.0f, 500.0f), box._Maximum - Vector3(500.0f, 0.0f, 500.0f)), 1'000.0f) };
		position._Y += 500.0f;

		{
			//Create the tower!
			DynamicPhysicalEntity *const RESTRICT cube{ EntityCreationSystem::Instance->CreateEntity<DynamicPhysicalEntity>() };

			DynamicPhysicalInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

			data->_Properties = EntityInitializationData::EntityProperty::None;
			data->_PhysicalFlags = PhysicalFlag::Physical;
			data->_Model = RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube);
			data->_Material = ResourceLoader::GetPhysicalMaterial(HashString("TowerMaterial"));
			data->_Position = position;
			data->_Rotation = Vector3(0.0f, 0.0f, 0.0f);
			data->_Scale = Vector3(50.0f, 1'000.0f, 50.0f);
			data->_OutlineColor = Vector3(0.0f, 0.0f, 0.0f);

			EntityCreationSystem::Instance->RequestInitialization(cube, data, false);

			entities->EmplaceSlow(cube);
		}

		{
			constexpr uint8 NUMBER_OF_BUILDINGS{ 20 };

			//Create some more "buildings"!
			DynamicArray<Vector3> previousPositions;

			previousPositions.EmplaceSlow(position);

			for (uint8 i{ 0 }; i < NUMBER_OF_BUILDINGS; ++i)
			{
				for (uint8 j{ 0 }; j < 10; ++j)
				{
					//Generate a random position.
					const Vector3 testPosition{ CatalystBaseMath::RandomFloatInRange(position._X - 500.0f, position._X + 500.0f), 0.0f, CatalystBaseMath::RandomFloatInRange(position._Z - 500.0f, position._Z + 500.0f) };

					//If this position is too close to any other position, discard it.
					bool tooClose{ false };

					for (const Vector3 &previousPosition : previousPositions)
					{
						if (Vector3::LengthSquaredXZ(previousPosition - testPosition) < 52.5f * 52.5f)
						{
							tooClose = true;

							break;
						}
					}

					if (!tooClose)
					{
						//Create a "building" at this position.
						DynamicPhysicalEntity *const RESTRICT building{ EntityCreationSystem::Instance->CreateEntity<DynamicPhysicalEntity>() };

						DynamicPhysicalInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

						data->_Properties = EntityInitializationData::EntityProperty::None;
						data->_PhysicalFlags = PhysicalFlag::Physical;
						data->_Model = RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube);
						data->_Material = ResourceLoader::GetPhysicalMaterial(HashString("TowerMaterial"));
						data->_Position = testPosition;
						TerrainSystem::Instance->GetTerrainHeightAtPosition(data->_Position, &data->_Position._Y);
						data->_Position._Y += 25.0f;
						data->_Rotation = Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f);
						data->_Scale = Vector3(50.0f, 50.0f, 50.0f);
						data->_OutlineColor = Vector3(0.0f, 0.0f, 0.0f);

						EntityCreationSystem::Instance->RequestInitialization(building, data, false);

						entities->EmplaceSlow(building);
						previousPositions.EmplaceSlow(data->_Position);

						break;
					}
				}
			}
		}

		{
			//Create the light!
			constexpr StaticArray<Vector3, 4> colors
			{
				Vector3(1.0f, 0.0f, 0.0f),
				Vector3(0.0f, 1.0f, 1.0f),
				Vector3(1.0f, 0.1f, 0.0f),
				Vector3(0.1f, 0.0f, 1.0f)
			};

			PointLightEntity *const RESTRICT light{ EntityCreationSystem::Instance->CreateEntity<PointLightEntity>() };

			PointLightInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<PointLightInitializationData>() };

			data->_Enabled = true;
			data->_Properties = EntityInitializationData::EntityProperty::None;
			data->_Color = colors[CatalystBaseMath::RandomIntegerInRange<uint64>(0, 3)];
			data->_Position = position + Vector3(0.0f, 600.0f, 0.0f);
			data->_Intensity = 1.0f;
			data->_AttenuationDistance = 2'500.0f;

			EntityCreationSystem::Instance->RequestInitialization(light, data, false);

			entities->EmplaceSlow(light);
		}

		{
			constexpr StaticArray<Vector3, 4> colors
			{
				Vector3(1.0f, 0.0f, 0.0f),
				Vector3(0.0f, 1.0f, 1.0f),
				Vector3(1.0f, 0.1f, 0.0f),
				Vector3(0.1f, 0.0f, 1.0f)
			};

			//Create a particle system!
			ParticleSystemEntity *const RESTRICT particles{ EntityCreationSystem::Instance->CreateEntity<ParticleSystemEntity>() };

			ParticleSystemInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<ParticleSystemInitializationData>() };

			data->_Properties = EntityInitializationData::EntityProperty::None;
			data->_Material = RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle);
			data->_ParticleSystemProperties._Properties = ParticleSystemProperty::AffectedByWind | ParticleSystemProperty::Looping;
			data->_ParticleSystemProperties._FadeTime = 0.1f;
			data->_ParticleSystemProperties._Lifetime = 1.0f;
			data->_ParticleSystemProperties._SpawnFrequency = 0.0001f;
			data->_ParticleSystemProperties._MinimumScale = Vector2<float>(1.25f, 1.25f);
			data->_ParticleSystemProperties._MaximumScale = Vector2<float>(2.5f, 2.5f);
			data->_ParticleSystemProperties._MinimumPosition = Vector3(-25.0f, 0.0f, -25.0f);
			data->_ParticleSystemProperties._MaximumPosition = Vector3(25.0f, 0.0f, 25.0f);
			data->_ParticleSystemProperties._MinimumVelocity = Vector3(0.0f, 0.0f, 0.0f);
			data->_ParticleSystemProperties._MaximumVelocity = Vector3(0.0f, 500.0f, 0.0f);
			data->_Position = position + Vector3(0.0f, 500.0f, 0.0f);

			EntityCreationSystem::Instance->RequestInitialization(particles, data, false);

			entities->EmplaceSlow(particles);
		}

	}, 10'000.0f);
}

/*
*	Initializes the offsets.
*/
void ClairvoyantLocationArchitect::InitializeOffsets() NOEXCEPT
{
	//Reserve the appropriate amount of memory.
	_Offsets.UpsizeFast(OFFSETS_RESOLUTION * OFFSETS_RESOLUTION);

	for (uint8 i{ 0 }; i < OFFSETS_RESOLUTION; ++i)
	{
		for (uint8 j{ 0 }; j < OFFSETS_RESOLUTION; ++j)
		{
			//Calculate the offset.
			_Offsets[((j * OFFSETS_RESOLUTION) + i)] = Vector3(static_cast<float>(i) / static_cast<float>(OFFSETS_RESOLUTION) - 0.5f, 0.0f, static_cast<float>(j) / static_cast<float>(OFFSETS_RESOLUTION) - 0.5f);
		}
	}
}

/*
*	Given an axis-aligned bounding box and an extent, find the most appropiate position.
*/
Vector3 ClairvoyantLocationArchitect::FindMostAppropriatePosition(const AxisAlignedBoundingBox &box, const float extent) NOEXCEPT
{
	//Calculate the box center.
	const Vector3 boxCenter{ AxisAlignedBoundingBox::CalculateCenter(box) };

	//Calculate the box extents.
	const Vector3 boxExtents{ box._Maximum._X - box._Minimum._X, 0.0f, box._Maximum._Z - box._Minimum._Z };

	//Find the best position with the best (least) height difference.
	Vector3 bestPosition{ AxisAlignedBoundingBox::CalculateCenter(box) };
	float bestHeightDifference{ FLOAT_MAXIMUM };

	for (uint8 i{ 0 }; i < OFFSETS_RESOLUTION; ++i)
	{
		for (uint8 j{ 0 }; j < OFFSETS_RESOLUTION; ++j)
		{
			//Calculate the test position.
			const Vector3 testPosition{ boxCenter + boxExtents * _Offsets[((j * OFFSETS_RESOLUTION) + i)] };

			//Calculate the height difference.
			float lowestTerrainHeight{ FLOAT_MAXIMUM };
			float highestTerrainHeight{ -FLOAT_MAXIMUM };
			float terrainHeight;

			for (uint8 i{ 0 }; i < OFFSETS_RESOLUTION; ++i)
			{
				for (uint8 j{ 0 }; j < OFFSETS_RESOLUTION; ++j)
				{
					TerrainSystem::Instance->GetTerrainHeightAtPosition(testPosition + _Offsets[((j * OFFSETS_RESOLUTION) + i)] * extent, &terrainHeight);

					lowestTerrainHeight = CatalystBaseMath::Minimum<float>(lowestTerrainHeight, terrainHeight);
					highestTerrainHeight = CatalystBaseMath::Maximum<float>(highestTerrainHeight, terrainHeight);
				}
			}

			const float heightDifference{ highestTerrainHeight - lowestTerrainHeight };

			//Pick this position if the height difference is lower.
			if (bestHeightDifference > heightDifference && lowestTerrainHeight > 0.0f)
			{
				bestHeightDifference = heightDifference;
				bestPosition = testPosition;
				bestPosition._Y = lowestTerrainHeight;
			}
		}
	}

	return bestPosition;
}