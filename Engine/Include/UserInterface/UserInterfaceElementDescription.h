#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/UserInterfaceCore.h>

class UserInterfaceElementDescription
{

public:

	//The type.
	UserInterfaceElementType _Type;

	//The minimum of the screen space axis aligned box.
	Vector2<float32> _Minimum;

	//The maximum of the screen space axis aligned box.
	Vector2<float32> _Maximum;

	//The opacity.
	float32 _Opacity;

};