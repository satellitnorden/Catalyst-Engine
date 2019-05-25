#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Rendering.
#include <Rendering/Native/Font.h>

//User interface.
#include <UserInterface/UserInterfaceElementDescription.h>

class TextUserInterfaceElementDescription final : public UserInterfaceElementDescription
{

public:

	//The font.
	const Font *RESTRICT _Font;

	//The text.
	DynamicString _Text;

};