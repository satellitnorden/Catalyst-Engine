#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Algorithms/HashAlgorithms.h>
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
		MODEL_RESOURCE,
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
*	Applies an editable field to an entity.
*/
void ApplyEditableFieldToEntity
(
	const ComponentEditableField &editable_field,
	const Component *const RESTRICT component,
	const Entity *const RESTRICT entity,
	const void *const RESTRICT data
) NOEXCEPT;

/*
*	Serializes a component editable field to the given file.
*/
void SerializeEditableField
(
	const ComponentEditableField &editable_field,
	const Component *const RESTRICT component,
	const Entity *const RESTRICT entity,
	BinaryFile<BinaryFileMode::OUT> *const RESTRICT file
) NOEXCEPT;

/*
*	Deserializes a component editable field to the given initialization data.
*/
void DeserializeEditableField
(
	const ComponentEditableField &editable_field,
	const void *const RESTRICT data,
	ComponentInitializationData *const RESTRICT initialization_data
) NOEXCEPT;