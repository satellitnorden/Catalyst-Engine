#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all user interface primitive types.
enum class UserInterfacePrimitiveType : uint8
{
	IMAGE,
	TEXT,

	NUMBER_OF_USER_INTERFACE_PRIMITIVE_TYPES
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