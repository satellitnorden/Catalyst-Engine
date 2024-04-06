#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

/*
*	Component editable field class definition.
*/
class ComponentEditableField final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		FLOAT,
		ENUMERATION,
		EULER_ANGLES,
		HASH_STRING,
		MATERIAL_ASSET,
		MODEL_ASSET,
		UINT32,
		VECTOR2,
		VECTOR3,
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