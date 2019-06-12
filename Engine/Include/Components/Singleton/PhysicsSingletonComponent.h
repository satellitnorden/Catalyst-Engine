#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Physics.
#include <Physics/CharacterMovement.h>

class PhysicsSingletonComponent final
{

public:

	//Container for all character movements.
	DynamicArray<CharacterMovement *RESTRICT> _CharacterMovements;

};