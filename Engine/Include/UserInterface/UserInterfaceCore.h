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

//Enumeration cover all text horizontal alignments.
enum class TextHorizontalAlignment : uint8
{
	LEFT,
	CENTER,
	RIGHT
};

//Enumeration cover all text vertical alignments.
enum class TextVerticalAlignment : uint8
{
	TOP,
	CENTER,
	BOTTOM
};

//Type aliases.
using ButtonUserInterfaceElementCallback = void(*)(UserInterfaceElement *const RESTRICT);