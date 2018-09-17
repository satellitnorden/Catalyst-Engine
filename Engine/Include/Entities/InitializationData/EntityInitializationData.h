#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class EntityInitializationData
{

public:

	//Enumeration covering all entity properties.
	enum class EntityProperty : uint8
	{
		/*
		*	Entities initialized with the automatic destruction property
		*	will be destroyed as soon as the entity has been terminated.
		*/
		AutomaticDestruction = BIT(0),
		/*
		*	Entities initialized with the automatic termination property will be automatically terminated
		*	at a point defined by it's entity type. Not all entity types support automatic termination.
		*/
		AutomaticTermination = BIT(1)
	};

	//The entity properties.
	uint8 _Properties;

};