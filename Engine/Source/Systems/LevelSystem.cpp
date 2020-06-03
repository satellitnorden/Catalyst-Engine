//Header file.
#include <Systems/LevelSystem.h>

//Entities.
#include <Entities/Creation/DynamicModelInitializationData.h>
#include <Entities/Types/DynamicModelEntity.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/ResourceSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LevelSystem);

/*
*	Loads a level.
*/
void LevelSystem::LoadLevel(const ResourcePointer<LevelResource> resource) NOEXCEPT
{
	for (const LevelEntry &level_entry : resource->_LevelEntries)
	{
		switch (level_entry._Type)
		{
			case LevelEntry::Type::DYNAMIC_MODEL:
			{
				DynamicModelEntity *const RESTRICT entity{ EntitySystem::Instance->CreateEntity<DynamicModelEntity>() };
				DynamicModelInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicModelInitializationData>() };

				data->_Properties = EntityInitializationData::Property::NONE;
				data->_InitialWorldTransform = level_entry._DynamicModelData._InitialWorldTransform;
				data->_ModelResource = ResourceSystem::Instance->GetModelResource(level_entry._DynamicModelData._ModelResourceIdentifier);
				data->_MaterialResources[0] = ResourceSystem::Instance->GetMaterialResource(level_entry._DynamicModelData._MaterialResourceIdentifiers[0]);
				data->_ModelCollisionConfiguration._Type = level_entry._DynamicModelData._ModelCollisionConfiguration._Type;
				data->_SimulatePhysics = level_entry._DynamicModelData._SimulatePhysics;
				data->_ModelPhysicsSimulationData = level_entry._DynamicModelData._ModelPhysicsSimulationData;

				EntitySystem::Instance->RequestInitialization(entity, data, false);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}
}