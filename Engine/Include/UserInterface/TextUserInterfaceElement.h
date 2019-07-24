#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Rendering.
#include <Rendering/Native/Font.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>

class TextUserInterfaceElement final : public UserInterfaceElement
{

public:

	//The font.
	const Font *RESTRICT _Font;

	//The scale.
	float _Scale;

	//The text.
	DynamicString _Text;

};