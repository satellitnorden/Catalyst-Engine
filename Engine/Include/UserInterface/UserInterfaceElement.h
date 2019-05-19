#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/UserInterfaceCore.h>

class UserInterfaceElement
{

public:

	//The type.
	UserInterfaceElementType _Type;

	//The minimum of the screen space axis aligned box.
	Vector2<float> _Minimum;

	//The maximum of the screen space axis aligned box.
	Vector2<float> _Maximum;

	//The active texture index.
	uint32 _ActiveTextureIndex;

};