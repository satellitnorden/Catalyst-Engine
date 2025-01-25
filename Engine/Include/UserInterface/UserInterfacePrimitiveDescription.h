#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/UserInterfaceCore.h>

class UserInterfacePrimitiveDescription
{

public:

	//The type.
	UserInterfacePrimitiveType _Type;

	//The minimum of the screen space axis aligned box.
	Vector2<float32> _Minimum;

	//The maximum of the screen space axis aligned box.
	Vector2<float32> _Maximum;

	//The color.
	Vector3<float32> _Color{ 1.0f, 1.0f, 1.0f };

	//The opacity.
	float32 _Opacity{ 1.0f };

};