//Header file.
#include <World/ClairvoyantLocationArchitect.h>

//Entities.
#include <Entities/DynamicPhysicalEntity.h>
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>

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
	/*
	//Cuuubes...! Yaaay...!!!
	EntityPlacementSystem::Instance->RegisterTwoDimensionalPlacementFunction([](const AxisAlignedBoundingBox &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities)
	{
		//Spawn one big cube in the middle, alright.
		DynamicPhysicalEntity *const RESTRICT cube{ EntityCreationSystem::Instance->CreateEntity<DynamicPhysicalEntity>() };

		DynamicPhysicalInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

		data->_Properties = EntityInitializationData::EntityProperty::None;
		data->_PhysicalFlags = PhysicalFlag::Outline | PhysicalFlag::Physical;
		data->_Model = RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube);
		data->_Material = RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red);
		data->_Position = AxisAlignedBoundingBox::CalculateCenter(box);
		TerrainSystem::Instance->GetTerrainHeightAtPosition(data->_Position, &data->_Position._Y);
		data->_Rotation = Vector3(0.0f, 0.0f, 0.0f);
		data->_Scale = Vector3(1'000.0f, 1'000.0f, 1'000.0f);
		data->_OutlineColor = Vector3(CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f), CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f), CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f));

		EntityCreationSystem::Instance->RequestInitialization(cube, data, false);

		entities->EmplaceSlow(cube);
	}, 10'000.0f);
	*/
}