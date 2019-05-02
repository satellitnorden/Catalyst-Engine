#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//User interface.
#include <UserInterface/ButtonUserInterfaceElement.h>

class UserInterfaceComponent final
{

public:

	//Container for all button user interface elements.
	DynamicArray<ButtonUserInterfaceElement *RESTRICT> _ButtonUserInterfaceElements;

};

/*
*	Function for specifying during which update phase(s) read access is allowed to a specific type.
*/
template <>
static NO_DISCARD UpdatePhase AllowedReadAccess<UserInterfaceComponent>() NOEXCEPT
{
	return UpdatePhase::Pre | UpdatePhase::Logic | UpdatePhase::Physics | UpdatePhase::Render | UpdatePhase::Post;
}

/*
*	Function for specifying during which update phase(s) write access is allowed to a specific type.
*/
template <>
static NO_DISCARD UpdatePhase AllowedWriteAccess<UserInterfaceComponent>() NOEXCEPT
{
	return UpdatePhase::Pre | UpdatePhase::Logic | UpdatePhase::Physics | UpdatePhase::Render | UpdatePhase::Post;
}