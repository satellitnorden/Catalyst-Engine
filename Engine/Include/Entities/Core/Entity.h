#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Type aliases.
using EntityIdentifier = uint64;

class Entity
{

public:

	//Enumeration covering all flags.
	enum class Flags : uint8
	{
		NONE = 0,
		
		/*
		*	Denotes whether or not this entity is initialized.
		*/
		INITIALIZED = BIT(0),

#if defined(CATALYST_EDITOR)
		/*
		*	Denotes whether or not this entity is selected in the editor.
		*/
		EDITOR_SELECTED = BIT(1)
#endif
	};

	//The entity identifier.
	EntityIdentifier _EntityIdentifier;

	//The flags.
	Flags _Flags{ Flags::NONE };

};

ENUMERATION_BIT_OPERATIONS(Entity::Flags);