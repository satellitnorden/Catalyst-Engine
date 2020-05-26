#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>
#include <Core/General/StaticString.h>

class ResourceHeader final
{

public:

	//The type identifier.
	HashString _TypeIdentifier;

	//The resource identifier.
	HashString _ResourceIdentifier;

	//The resource name.
	StaticString<64> _ResourceName;

	/*
	*	Default constuctor.
	*/
	FORCE_INLINE constexpr ResourceHeader() NOEXCEPT
		:
		_TypeIdentifier(),
		_ResourceIdentifier()
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr ResourceHeader(const HashString initial_type_identifier, const HashString initial_resource_identifier, const StaticString<64> initial_resource_name) NOEXCEPT
		:
		_TypeIdentifier(initial_type_identifier),
		_ResourceIdentifier(initial_resource_identifier),
		_ResourceName(initial_resource_name)
	{

	}

};