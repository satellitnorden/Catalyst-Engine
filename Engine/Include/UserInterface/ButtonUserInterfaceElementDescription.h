#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Forward declarations.
class ButtonUserInterfaceElement;

//Type aliases.
using ButtonUserInterfaceElementCallbackFunction = void(*)(ButtonUserInterfaceElement *const RESTRICT element);

class ButtonUserInterfaceElementDescription final
{

public:

	//The minimum dimensions of the bounding box denoted in screen coordinates ([0.0f, 1.0f]).
	Vector2<float> _MinimumDimensions;

	//The maximum dimensions of the bounding box denoted in screen coordinates ([0.0f, 1.0f]).
	Vector2<float> _MaximumDimensions;

	//The idle texture.
	Texture2DHandle _IdleTexture;

	//The hovered texture.
	Texture2DHandle _HoveredTexture;

	//The clicked texture.
	Texture2DHandle _ClickedTexture;

	//The start hover callback.
	ButtonUserInterfaceElementCallbackFunction _StartHoverCallbackFunction;

	//The end hover callback function.
	ButtonUserInterfaceElementCallbackFunction _EndHoverCallbackFunction;

	//The clicked callback function.
	ButtonUserInterfaceElementCallbackFunction _ClickedCallbackFunction;

};