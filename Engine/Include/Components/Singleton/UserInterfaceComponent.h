#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>

class UserInterfaceComponent final
{

public:

	//Container for all user interface elements.
	DynamicArray<UserInterfaceElement *RESTRICT> _UserInterfaceElements;

};