#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Resources.
#include <Resources/Core/FontResource.h>
#include <Resources/Core/ResourcePointer.h>

//User interface.
#include <UserInterface/UserInterfaceElementDescription.h>

class TextUserInterfaceElementDescription final : public UserInterfaceElementDescription
{

public:

	//The font resource.
	ResourcePointer<FontResource> _FontResource;

	//The scale.
	float32 _Scale;

	//The horizontal alignment.
	TextHorizontalAlignment _HorizontalAlignment;

	//The vertical alignment.
	TextVerticalAlignment _VerticalAlignment;

	//The text.
	DynamicString _Text;

};