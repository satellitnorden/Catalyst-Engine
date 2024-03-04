#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Forward declarations.
class Entity;

class ScriptContext final
{

public:

	//The entity.
	Entity *RESTRICT _Entity;

	//The data.
	void *RESTRICT _Data;

};