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

	//Add all level entries.
	level->_LevelEntries.Reserve(number_of_entities);

	//Add all the entities.
	for (uint64 entity_index{ 0 }; entity_index < number_of_entities; ++entity_index)
	{
		//Add a new level entry.
		level->_LevelEntries.Emplace();
		LevelEntry &new_level_entry{ level->_LevelEntries.Back() };

		//Deserialize the entitiy.
		new_level_entry._Entity = EntitySerialization::DeserializeFromStreamArchive(level_asset->_StreamArchive, &stream_archive_position, &world_transform);
	}
}

/*
*	Despawns the given level.
*/
void LevelSystem::DespawnLevel(Level *const RESTRICT level) NOEXCEPT
{
	for (LevelEntry &level_entry : level->_LevelEntries)
	{
		EntitySystem::Instance->DestroyEntity(level_entry._Entity);
	}

	level->_LevelEntries.Clear();
}