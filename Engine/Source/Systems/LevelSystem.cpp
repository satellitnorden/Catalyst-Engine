//Header file.
#include <Systems/LevelSystem.h>

//Entities.
#include <Entities/Core/EntitySerialization.h>

//Singleton definition.
DEFINE_SINGLETON(LevelSystem);

/*
*	Spawns a level.
*/
void LevelSystem::SpawnLevel
(
	const WorldTransform &world_transform,
	const AssetPointer<LevelAsset> level_asset,
	DynamicArray<LevelEntry> *const RESTRICT level_entries
) NOEXCEPT
{
	//Set up the stream archive position.
	uint64 stream_archive_position{ 0 };

	//Read the number of entities.
	uint64 number_of_entities;
	level_asset->_StreamArchive.Read(&number_of_entities, sizeof(uint64), &stream_archive_position);

	//Add all level entries.
	level_entries->Reserve(number_of_entities);

	//Add all the entities.
	for (uint64 entity_index{ 0 }; entity_index < number_of_entities; ++entity_index)
	{
		//Add a new level entry.
		level_entries->Emplace();
		LevelEntry &new_level_entry{ level_entries->Back() };

		//Read the name.
		level_asset->_StreamArchive.Read(new_level_entry._Name.Data(), new_level_entry._Name.Size(), &stream_archive_position);

		//Deserialize the entitiy.
		new_level_entry._Entity = EntitySerialization::DeserializeFromStreamArchive(level_asset->_StreamArchive, &stream_archive_position);
	}
}