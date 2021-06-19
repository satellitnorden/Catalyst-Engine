#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//User interface.
#include <UserInterface/UserInterfacePrimitive.h>

class UserInterfaceElement
{

public:

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~UserInterfaceElement() NOEXCEPT
	{

	}

	/*
	*	Returns the user interface primitives.
	*/
	FORCE_INLINE virtual void RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT = 0;

};