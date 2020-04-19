#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class EntityInitializationData
{

public:

	//Enumeration covering all entity properties.
	enum class Property : uint8
	{
		/*
		*	Entities initialized with the none property
		*	surprisingly has no properties.
		*/
		NONE = BIT(0),

		/*
		*	Entities initialized with the automatic termination property will be automatically terminated
		*	at a point defined by it's entity type. Not all entity types support automatic termination.
		*/
		AUTOMATIC_TERMINATION = BIT(1),

		/*
		*	Entities initialized with the automatic destruction property
		*	will be destroyed as soon as the entity has been terminated.
		*/
		AUTOMATIC_DESTRUCTION = BIT(2)
	};

	//The entity properties.
	Property _Properties;

};

ENUMERATION_BIT_OPERATIONS(EntityInitializationData::Property);