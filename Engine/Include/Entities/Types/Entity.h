#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Core/EntityCore.h>

class Entity
{

public:

	//The entity identifier.
	EntityIdentifier _EntityIdentifier;

	//Denotes whether or not this entity is initialized.
	bool _Initialized{ false };

};