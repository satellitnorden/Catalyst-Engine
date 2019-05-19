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

/*
*	Function for specifying during which update phase(s) read access is allowed to a specific type.
*/
template <>
static NO_DISCARD UpdatePhase AllowedReadAccess<UserInterfaceComponent>() NOEXCEPT
{
	return UpdatePhase::Pre | UpdatePhase::Main | UpdatePhase::Post;
}

/*
*	Function for specifying during which update phase(s) write access is allowed to a specific type.
*/
template <>
static NO_DISCARD UpdatePhase AllowedWriteAccess<UserInterfaceComponent>() NOEXCEPT
{
	return UpdatePhase::Pre | UpdatePhase::Main | UpdatePhase::Post;
}