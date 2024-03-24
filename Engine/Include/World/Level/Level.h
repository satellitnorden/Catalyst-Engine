#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Core/Entity.h>

class Level final
{

public:

	//The entities.
	DynamicArray<Entity *RESTRICT> _Entities;

};