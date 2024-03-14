#pragma once

/*
*	This file contains classes for level entries,
*	where one level entry is one entity.
*	Contains some functionality for serializing/deserializing level entries as well.
*/

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/StaticString.h>

//Entities.
#include <Entities/Core/Entity.h>

class LevelEntry final
{

public:

	//The name.
	StaticString<64> _Name;

	//The entity.
	Entity *RESTRICT _Entity;

};