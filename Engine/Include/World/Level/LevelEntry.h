#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/StaticString.h>

//Entities.
#include <Entities/Types/Entity.h>

class LevelEntry final
{

public:

	//The name.
	StaticString<128> _Name;

	//The entity.
	Entity *RESTRICT _Entity;

};