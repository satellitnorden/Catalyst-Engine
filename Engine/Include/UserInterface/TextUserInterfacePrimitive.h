#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/FontAsset.h>

//User interface.
#include <UserInterface/UserInterfacePrimitive.h>

class TextUserInterfacePrimitive final : public UserInterfacePrimitive
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