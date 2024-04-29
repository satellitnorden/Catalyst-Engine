//Header file.
#include <Systems/LevelSystem.h>

//Entities.
#include <Entities/Core/EntitySerialization.h>

//Systems.
#include <Systems/EntitySystem.h>

/*
*	Spawns a level.
*/
void LevelSystem::SpawnLevel
(
	const WorldTransform &world_transform,
	const AssetPointer<LevelAsset> level_asset,
	Level *const RESTRICT level,
	SpawnFunction spawn_function,
	void *const RESTRICT spawn_function_user_data
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
		struct DeserializeFunctionUserData
		{
			SpawnFunction _SpawnFunction;
			uint64 _EntityIdentifier;
			void *const RESTRICT _UserData;
		} deserialize_function_user_data
		{
			spawn_function,
			entity_identifiers.Back(),
			spawn_function_user_data
		};

		new_entity = EntitySerialization::DeserializeFromStreamArchive
		(
			level_asset->_StreamArchive,
			&stream_archive_position,
			&world_transform,
			[]
			(
				ComponentInitializationData *const RESTRICT initialization_data,
				void *const RESTRICT user_data
			)
			{
				DeserializeFunctionUserData *const RESTRICT _user_data{ static_cast<DeserializeFunctionUserData *const RESTRICT>(user_data) };

				if (_user_data->_SpawnFunction)
				{
					_user_data->_SpawnFunction(initialization_data, _user_data->_EntityIdentifier, _user_data->_UserData);
				}
			},
			&deserialize_function_user_data
		);
	}

	//Read the number of entity linkts.
	uint64 number_of_entity_links;
	level_asset->_StreamArchive.Read(&number_of_entity_links, sizeof(uint64), &stream_archive_position);

	for (uint64 i{ 0 }; i < number_of_entity_links; ++i)
	{
		uint64 from_identifier;
		level_asset->_StreamArchive.Read(&from_identifier, sizeof(uint64), &stream_archive_position);

		uint64 to_identifier;
		level_asset->_StreamArchive.Read(&to_identifier, sizeof(uint64), &stream_archive_position);

		Entity *RESTRICT from_entity{ nullptr };
		Entity *RESTRICT to_entity{ nullptr };

		for (uint64 entity_index{ 0 }; entity_index < entity_identifiers.Size(); ++entity_index)
		{
			if (entity_identifiers[entity_index] == from_identifier)
			{
				from_entity = level->_Entities[entity_index];
			}

			if (entity_identifiers[entity_index] == to_identifier)
			{
				to_entity = level->_Entities[entity_index];
			}
		}

		ASSERT(from_entity && to_entity, "Broken links!");

		EntitySystem::Instance->GetEntityLinks()->AddLink(from_entity, to_entity);
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