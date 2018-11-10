//Header file.
#include <World/ClairvoyantLocationArchitect.h>

//Entities.
#include <Entities/DynamicPhysicalEntity.h>
#include <Entities/ParticleSystemEntity.h>
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>

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
	}, 10'000.0f);
}