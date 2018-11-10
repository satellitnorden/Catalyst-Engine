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

/*
*	Initializes the Clairvoyant location architect.
*/
void ClairvoyantLocationArchitect::Initialize() NOEXCEPT
{
	//Cuuubes...! Yaaay...!!!
	EntityPlacementSystem::Instance->RegisterTwoDimensionalPlacementFunction([](const AxisAlignedBoundingBox &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities)
	{
		//Find a suitable location. (:
		constexpr uint8 NUMBER_OF_TESTS{ 100 };

		Vector3 bestPosition{ 0.0f, 0.0f, 0.0f };
		float bestHeightDifference{ FLOAT_MAXIMUM };

		for (uint8 i{ 0 }; i < NUMBER_OF_TESTS; ++i)
		{
			//Generate a random position within the box.
			const Vector3 testPosition{ CatalystBaseMath::RandomFloatInRange(box._Minimum._X, box._Maximum._X) , 0.0f, CatalystBaseMath::RandomFloatInRange(box._Minimum._Z, box._Maximum._Z) };

			//Calculate the height difference at the test position.
			float lowestTerrainHeight{ FLOAT_MAXIMUM };
			float highestTerrainHeight{ -FLOAT_MAXIMUM };

			float terrainHeight;

			TerrainSystem::Instance->GetTerrainHeightAtPosition(testPosition, &terrainHeight);
			lowestTerrainHeight = CatalystBaseMath::Minimum<float>(lowestTerrainHeight, terrainHeight);
			highestTerrainHeight = CatalystBaseMath::Maximum<float>(highestTerrainHeight, terrainHeight);

			TerrainSystem::Instance->GetTerrainHeightAtPosition(testPosition + Vector3(-50.0f, 0.0f, -50.0f), &terrainHeight);
			lowestTerrainHeight = CatalystBaseMath::Minimum<float>(lowestTerrainHeight, terrainHeight);
			highestTerrainHeight = CatalystBaseMath::Maximum<float>(highestTerrainHeight, terrainHeight);

			TerrainSystem::Instance->GetTerrainHeightAtPosition(testPosition + Vector3(-50.0f, 0.0f, 50.0f), &terrainHeight);
			lowestTerrainHeight = CatalystBaseMath::Minimum<float>(lowestTerrainHeight, terrainHeight);
			highestTerrainHeight = CatalystBaseMath::Maximum<float>(highestTerrainHeight, terrainHeight);

			TerrainSystem::Instance->GetTerrainHeightAtPosition(testPosition + Vector3(50.0f, 0.0f, 50.0f), &terrainHeight);
			lowestTerrainHeight = CatalystBaseMath::Minimum<float>(lowestTerrainHeight, terrainHeight);
			highestTerrainHeight = CatalystBaseMath::Maximum<float>(highestTerrainHeight, terrainHeight);

			TerrainSystem::Instance->GetTerrainHeightAtPosition(testPosition + Vector3(50.0f, 0.0f, -50.0f), &terrainHeight);
			lowestTerrainHeight = CatalystBaseMath::Minimum<float>(lowestTerrainHeight, terrainHeight);
			highestTerrainHeight = CatalystBaseMath::Maximum<float>(highestTerrainHeight, terrainHeight);

			const float heightDifference{ highestTerrainHeight - lowestTerrainHeight };

			if (bestHeightDifference > heightDifference)
			{
				bestHeightDifference = heightDifference;
				bestPosition = testPosition;
				bestPosition._Y = lowestTerrainHeight + 500.0f;
			}
		}

		{
			//Create the tower!
			DynamicPhysicalEntity *const RESTRICT cube{ EntityCreationSystem::Instance->CreateEntity<DynamicPhysicalEntity>() };

			DynamicPhysicalInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

			data->_Properties = EntityInitializationData::EntityProperty::None;
			data->_PhysicalFlags = PhysicalFlag::Physical;
			data->_Model = RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube);
			data->_Material = ResourceLoader::GetPhysicalMaterial(HashString("TowerMaterial"));
			data->_Position = bestPosition;
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
			data->_Position = bestPosition + Vector3(0.0f, 600.0f, 0.0f);
			data->_Intensity = 1.0f;
			data->_AttenuationDistance = 2'500.0f;

			EntityCreationSystem::Instance->RequestInitialization(light, data, false);

			entities->EmplaceSlow(light);
		}

	}, 10'000.0f);
}