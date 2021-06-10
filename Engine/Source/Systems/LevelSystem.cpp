//Header file.
#include <Systems/LevelSystem.h>

//Entities.
#include <Entities/Creation/DynamicModelInitializationData.h>
#include <Entities/Creation/LightInitializationData.h>
#include <Entities/Creation/StaticModelInitializationData.h>
#include <Entities/Types/DynamicModelEntity.h>
#include <Entities/Types/LightEntity.h>
#include <Entities/Types/StaticModelEntity.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/ResourceSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LevelSystem);

/*
*	Spawns a level.
*/
void LevelSystem::SpawnLevel(const ResourcePointer<LevelResource> resource) NOEXCEPT
{
	//Add the spawned level.
	_SpawnedLevels.Emplace();
	SpawnedLevel &spawned_level{ _SpawnedLevels.Back() };

	//Set the level resource.
	spawned_level._LevelResource = resource;

	//Process all level entries.
	for (const LevelEntry &level_entry : resource->_LevelEntries)
	{
		switch (level_entry._Type)
		{
			case LevelEntry::Type::DYNAMIC_MODEL:
			{
				DynamicModelEntity *const RESTRICT entity{ EntitySystem::Instance->CreateEntity<DynamicModelEntity>() };
				DynamicModelInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicModelInitializationData>() };

				data->_Properties = EntityInitializationData::Property::NONE;
				data->_InitialWorldTransform = level_entry._DynamicModelData._WorldTransform;
				data->_ModelResource = ResourceSystem::Instance->GetModelResource(level_entry._DynamicModelData._ModelResourceIdentifier);

				for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
				{
					data->_MaterialResources[i] = ResourceSystem::Instance->GetMaterialResource(level_entry._DynamicModelData._MaterialResourceIdentifiers[i]);
				}

				data->_ModelCollisionConfiguration = level_entry._DynamicModelData._ModelCollisionConfiguration;
				data->_ModelSimulationConfiguration = level_entry._DynamicModelData._ModelSimulationConfiguration;

				EntitySystem::Instance->RequestInitialization(entity, data, false);

				spawned_level._Entities.Emplace(entity);

				break;
			}

			case LevelEntry::Type::LIGHT:
			{
				LightEntity *const RESTRICT entity{ EntitySystem::Instance->CreateEntity<LightEntity>() };
				LightInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<LightInitializationData>() };

				data->_Properties = EntityInitializationData::Property::NONE;
				
				switch (level_entry._LightData._LightType)
				{
					case LightType::DIRECTIONAL:
					{
						data->_Rotation = level_entry._LightData._Rotation;

						break;
					}

					case LightType::POINT:
					{
						data->_WorldPosition = level_entry._LightData._WorldPosition;

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				data->_Color = level_entry._LightData._Color;
				data->_LightType = level_entry._LightData._LightType;
				data->_LightProperties = level_entry._LightData._LightProperties;
				data->_Intensity = level_entry._LightData._Intensity;
				data->_Radius = level_entry._LightData._Radius;
				data->_Size = level_entry._LightData._Size;

				EntitySystem::Instance->RequestInitialization(entity, data, false);

				spawned_level._Entities.Emplace(entity);

				break;
			}

			case LevelEntry::Type::STATIC_MODEL:
			{
				StaticModelEntity *const RESTRICT entity{ EntitySystem::Instance->CreateEntity<StaticModelEntity>() };
				StaticModelInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<StaticModelInitializationData>() };

				data->_Properties = EntityInitializationData::Property::NONE;
				data->_WorldTransform = level_entry._StaticModelData._WorldTransform;
				data->_ModelResource = ResourceSystem::Instance->GetModelResource(level_entry._StaticModelData._ModelResourceIdentifier);

				for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
				{
					data->_MaterialResources[i] = ResourceSystem::Instance->GetMaterialResource(level_entry._StaticModelData._MaterialResourceIdentifiers[i]);
				}

				data->_ModelCollisionConfiguration._Type = level_entry._StaticModelData._ModelCollisionConfiguration._Type;

				EntitySystem::Instance->RequestInitialization(entity, data, false);

				spawned_level._Entities.Emplace(entity);

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

/*
*	Despawns all levels.
*/
void LevelSystem::DespawnAllLevels() NOEXCEPT
{
	//Terminate and destroy all entities.
	for (const SpawnedLevel &spawned_level : _SpawnedLevels)
	{
		for (Entity *const RESTRICT entity : spawned_level._Entities)
		{
			EntitySystem::Instance->RequestTermination(entity);
			EntitySystem::Instance->RequestDestruction(entity);
		}
	}

	//Clear the spawned levels.
	_SpawnedLevels.Clear();
}