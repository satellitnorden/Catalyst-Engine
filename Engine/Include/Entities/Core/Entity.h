#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Type aliases.
using EntityIdentifier = uint64;

class Entity
{

public:

	//The entity identifier.
	EntityIdentifier _EntityIdentifier;

	//Denotes whether or not this entity is initialized.
	bool _Initialized{ false };

};