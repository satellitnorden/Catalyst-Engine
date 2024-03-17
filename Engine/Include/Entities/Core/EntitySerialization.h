#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StreamArchive.h>

//Entities.
#include <Entities/Core/Entity.h>

//World.
#include <World/Core/WorldTransform.h>

//Third party.
#include <ThirdParty/json/json.hpp>

namespace EntitySerialization
{

	/*
	*	Serializes the given entity to the given JSON object with the given name.
	*/
	void SerializeToJSON
	(
		nlohmann::json &JSON,
		Entity *const RESTRICT entity
	) NOEXCEPT;

	/*
	*	Serializes an entity from the given JSON object to the give stream archive.
	*/
	void SerializeToStreamArchive
	(
		const nlohmann::json &JSON,
		const char *const RESTRICT name,
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
		const WorldTransform *const RESTRICT world_transform
	) NOEXCEPT;

}