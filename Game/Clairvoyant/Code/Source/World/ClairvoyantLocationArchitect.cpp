//Header file.
#include <World/ClairvoyantLocationArchitect.h>

//Entities.
#include <Entities/Creation/DynamicPhysicalInitializationData.h>
#include <Entities/Creation/PointLightInitializationData.h>
#include <Entities/Types/DynamicPhysicalEntity.h>
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

	//Cuuubes...! Yaaay...!!!
	EntityPlacementSystem::Instance->RegisterTwoDimensionalPlacementFunction([](const AxisAlignedBoundingBox &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities)
	{
		//Find the most appropriate position.
		Vector3 position{ FindMostAppropriatePosition(box, 100.0f) };
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
			data->_Scale = Vector3(100.0f, 1'000.0f, 100.0f);
			data->_OutlineColor = Vector3(0.0f, 0.0f, 0.0f);

			EntityCreationSystem::Instance->RequestInitialization(cube, data, false);

			entities->EmplaceSlow(cube);
		}

		{
			constexpr StaticArray<Vector3, 4> colors
			{
				Vector3(1.0f, 0.0f, 0.0f),
				Vector3(0.0f, 1.0f, 1.0f),
				Vector3(1.0f, 0.1f, 0.0f),
				Vector3(0.1f, 0.0f, 1.0f)
			};

			//Create the light!
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
			_Offsets[((j * OFFSETS_RESOLUTION) + i)] = Vector3(static_cast<float>(i) / static_cast<float>(OFFSETS_RESOLUTION - 1) - 0.5f, 0.0f, static_cast<float>(j) / static_cast<float>(OFFSETS_RESOLUTION - 1) - 0.5f);
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
			if (bestHeightDifference > heightDifference)
			{
				bestHeightDifference = heightDifference;
				bestPosition = testPosition;
				bestPosition._Y = lowestTerrainHeight;
			}
		}
	}

	return bestPosition;
}