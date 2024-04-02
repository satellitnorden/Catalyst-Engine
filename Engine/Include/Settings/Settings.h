#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StreamArchive.h>

//Third party.
#include <ThirdParty/json/json.hpp>

class Settings final
{

public:

	/*
	*	Serializes settings from JSON into a stream archive.
	*/
	static void SerializeToStreamArchive(const nlohmann::json &JSON, StreamArchive *const RESTRICT stream_archive) NOEXCEPT;

	/*
	*	Deserializes a settings object from the given stream archive.
	*/
	template <typename TYPE>
	static void DeserializeFromStreamArchive(const StreamArchive &stream_archive, TYPE *const RESTRICT object) NOEXCEPT
	{
		DeserializeFromStreamArchiveInternal(TYPE::CLASS_IDENTIFIER, stream_archive, object);
	}

private:

	/*
	*	Deserializes a settings object from the given stream archive internally.
	*/
	static void DeserializeFromStreamArchiveInternal(const uint64 class_identifier, const StreamArchive &stream_archive, void *const RESTRICT object) NOEXCEPT;

};