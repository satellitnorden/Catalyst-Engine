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
	*	Serializes settings from an object into a stream archive.
	*/
	template <typename TYPE>
	static void SerializeToStreamArchive(const TYPE *const RESTRICT object, StreamArchive *const RESTRICT stream_archive) NOEXCEPT
	{
		SerializeToStreamArchiveInternal(TYPE::CLASS_IDENTIFIER, object, stream_archive);
	}

	/*
	*	Deserializes a settings object from the given stream archive.
	*/
	template <typename TYPE>
	static void DeserializeFromStreamArchive(const StreamArchive &stream_archive, TYPE *const RESTRICT object) NOEXCEPT
	{
		DeserializeFromStreamArchiveInternal(TYPE::CLASS_IDENTIFIER, stream_archive, object);
	}

	/*
	*	Calculates the size, in bytes, of the given object.
	*/
	template <typename TYPE>
	static NO_DISCARD uint64 Size(const TYPE *const RESTRICT object) NOEXCEPT
	{
		return SizeInternal(TYPE::CLASS_IDENTIFIER, object);
	}

private:

	/*
	*	Serializes settings from an object into a stream archive.
	*/
	static void SerializeToStreamArchiveInternal(const uint64 class_identifier, const void *const RESTRICT object, StreamArchive *const RESTRICT stream_archive) NOEXCEPT;

	/*
	*	Deserializes a settings object from the given stream archive internally.
	*/
	static void DeserializeFromStreamArchiveInternal(const uint64 class_identifier, const StreamArchive &stream_archive, void *const RESTRICT object) NOEXCEPT;

	/*
	*	Calculates the size, in bytes, of the given object internally.
	*/
	static NO_DISCARD uint64 SizeInternal(const uint64 class_identifier, const void *const RESTRICT object) NOEXCEPT;

};