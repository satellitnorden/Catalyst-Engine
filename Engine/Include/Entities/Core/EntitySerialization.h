#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StreamArchive.h>

#if defined(CATALYST_EDITOR)
//Editor.
#include <Editor/EditorEntityData.h>
#endif

//Entities.
#include <Entities/Core/Entity.h>

//World.
#include <World/Core/WorldTransform.h>

//Third party.
#include <ThirdParty/json/json.hpp>

//Forward declarations.
class ComponentInitializationData;

namespace EntitySerialization
{
	//Type aliases.
	using DeserializeFunction = void(*)
	(
		ComponentInitializationData *const RESTRICT initialization_data,
		void *const RESTRICT user_data
	);

#if defined(CATALYST_EDITOR)
	/*
	*	Serializes the given entity to the given JSON object with the given name.
	*/
	void SerializeToJSON
	(
		const EditorEntityData &editor_entity_data,
		nlohmann::json &JSON,
		Entity *const RESTRICT entity
	) NOEXCEPT;
#endif

	/*
	*	Serializes an entity from the given JSON object to the give stream archive.
	*/
	void SerializeToStreamArchive
	(
		const nlohmann::json &JSON,
		StreamArchive *const RESTRICT stream_archive
	) NOEXCEPT;

	/*
	*	Deserializes an entity from the given stream archive.
	*	Takes an optional world transform to apply to spawned entities.
	*/
	NO_DISCARD Entity *const RESTRICT DeserializeFromStreamArchive
	(
		const StreamArchive &stream_archive,
		uint64 *const RESTRICT stream_archive_position,
		const WorldTransform *const RESTRICT world_transform,
		const DeserializeFunction deserialize_function = nullptr,
		void *const RESTRICT deserialize_user_data = nullptr
	) NOEXCEPT;

}