#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Forward declarations.
class UserInterfaceElement;

//Enumeration covering all user interface element types.
enum class UserInterfaceElementType : uint8
{
	BUTTON,
	IMAGE,
	TEXT,

	NUMBER_OF_USER_INTERFACE_ELEMENT_TYPES
};

//Type aliases.
using ButtonUserInterfaceElementCallback = void(*)(const UserInterfaceElement *const RESTRICT);