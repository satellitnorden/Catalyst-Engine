#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all Window anchors.
enum class WindowAnchor : uint8
{
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	TOP_LEFT,
	TOP_RIGHT,
	CENTER
};

//Enumeration covering all user interface primitive types.
enum class UserInterfacePrimitiveType : uint8
{
	IMAGE,
	TEXT,

	NUMBER_OF_USER_INTERFACE_PRIMITIVE_TYPES
};

//Enumeration covering all user interface element types.
enum class UserInterfaceElementType : uint8
{
	BUTTON,
	CHECKBOX,
	IMAGE,
	PROGRESS_BAR,
	TEXT,
	TEXT_INPUT,

	NUMBER_OF_USER_INTERFACE_ELEMENT_TYPES
};

//Enumeration covering all user interface button states.
enum class UserInterfaceButtonState : uint8
{
	IDLE,
	HOVERED,
	PRESSED
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