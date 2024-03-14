#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/StreamArchive.h>
#include <Core/General/HashString.h>
#include <Core/Utilities/StringUtilities.h>

//Entities.
#include <Entities/Core/Entity.h>

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
		EULER_ANGLES,
		MATERIAL_ASSET,
		MODEL_ASSET,
		WORLD_TRANSFORM
	};

	//The name.
	const char *RESTRICT _Name;

	//The identifier.
	HashString _Identifier;

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