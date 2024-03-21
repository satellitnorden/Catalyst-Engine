#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//User interface.
#include <UserInterface/UserInterfacePrimitiveDescription.h>

class TextUserInterfacePrimitiveDescription final : public UserInterfacePrimitiveDescription
{

public:

	//The font.
	AssetPointer<FontAsset> _Font;

	//The scale.
	float32 _Scale;

	//The horizontal alignment.
	TextHorizontalAlignment _HorizontalAlignment;

	//The vertical alignment.
	TextVerticalAlignment _VerticalAlignment;

	//The text.
	DynamicString _Text;

};