//Header file.
#include <Systems/LevelSystem.h>

//Entities.
#include <Entities/Core/EntitySerialization.h>

//Systems.
#include <Systems/EntitySystem.h>

//Singleton definition.
DEFINE_SINGLETON(LevelSystem);

/*
*	Spawns a level.
*/
void LevelSystem::SpawnLevel
(
	const WorldTransform &world_transform,
	const AssetPointer<LevelAsset> level_asset,
	Level *const RESTRICT level
) NOEXCEPT
{
	//Set up the stream archive position.
	uint64 stream_archive_position{ 0 };

	//Read the number of entities.
	uint64 number_of_entities;
	level_asset->_StreamArchive.Read(&number_of_entities, sizeof(uint64), &stream_archive_position);

	//Add all entities.
	level->_Entities.Reserve(number_of_entities);

	//Keep track of the entity identifiers, so we can set up entity links.
	DynamicArray<uint64> entity_identifiers;
	entity_identifiers.Reserve(number_of_entities);

	//Add all the entities.
	for (uint64 entity_index{ 0 }; entity_index < number_of_entities; ++entity_index)
	{
		//Add a new entity.
		level->_Entities.Emplace();
		Entity *RESTRICT &new_entity{ level->_Entities.Back() };

		//Read the entity identifier.
		entity_identifiers.Emplace();
		level_asset->_StreamArchive.Read(&entity_identifiers.Back(), sizeof(uint64), &stream_archive_position);

		//Deserialize the entitiy.
		new_entity = EntitySerialization::DeserializeFromStreamArchive(level_asset->_StreamArchive, &stream_archive_position, &world_transform);
	}
}

/*
*	Despawns the given level.
*/
void LevelSystem::DespawnLevel(Level *const RESTRICT level) NOEXCEPT
{
	for (Entity *const RESTRICT entity : level->_Entities)
	{
		EntitySystem::Instance->DestroyEntity(entity);
	}

	level->_Entities.Clear();
}