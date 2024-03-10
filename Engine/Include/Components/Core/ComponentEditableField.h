#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/StreamArchive.h>
#include <Core/Utilities/StringUtilities.h>

//Entities.
#include <Entities/Types/Entity.h>

//File.
#include <File/Core/BinaryFile.h>

//Forward declarations.
class Component;
class ComponentInitializationData;

/*
*	Component editable field class definition.
*/
class ComponentEditableField final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		MATERIAL_ASSET,
		MODEL_ASSET,
		WORLD_TRANSFORM
	};

	//The name.
	const char *RESTRICT _Name;

	//The identifier.
	uint64 _Identifier;

	//The type.
	Type _Type;

	//The offset, in bytes, into the initialization data object.
	uint64 _InitializationDataOffset;

	//The offset, in bytes, into the instance data object.
	uint64 _InstanceDataOffset;

	/*
	*	Constructor.
	*/
	FORCE_INLINE ComponentEditableField() NOEXCEPT
	{

	}

};

/*
*	Applies an editable field to initialization data.
*/
void ApplyEditableFieldToInitializationData
(
	const ComponentEditableField &editable_field,
	const void *const RESTRICT data,
	ComponentInitializationData *const RESTRICT initialization_data
) NOEXCEPT;

/*
*	Serializes a component editable field to the given stream archive.
*/
void SerializeEditableField
(
	const ComponentEditableField &editable_field,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	StreamArchive *const RESTRICT stream_archive
) NOEXCEPT;

/*
*	Deserializes a component editable field to the given initialization data.
*/
NO_DISCARD uint64 DeserializeEditableField
(
	const ComponentEditableField &editable_field,
	const void *const RESTRICT data,
	ComponentInitializationData *const RESTRICT initialization_data
) NOEXCEPT;