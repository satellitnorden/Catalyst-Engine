#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Systems.
#include <Systems/RenderingSystem.h>

//User interface.
#include <UserInterface/UserInterfaceCore.h>

class UserInterfacePrimitive
{

public:

	//The type.
	UserInterfacePrimitiveType _Type;

	//The minimum of the screen space axis aligned box.
	Vector2<float32> _Minimum;

	//The maximum of the screen space axis aligned box.
	Vector2<float32> _Maximum;

	//The color.
	Vector3<float32> _Color;

	//The opacity.
	float32 _Opacity;

	/*
	*	Corrects for aspect ratio.
	*/
	FORCE_INLINE void CorrectForAspectRatio() NOEXCEPT
	{
		const Vector2<float32> center{ BaseMath::LinearlyInterpolate(_Minimum, _Maximum, 0.5f) };
		Vector2<float32> half_extents{ (_Maximum - _Minimum) * 0.5f };

		half_extents._X /= RenderingSystem::Instance->GetScaledAspectRatio();

		_Minimum = center - half_extents;
		_Maximum = center + half_extents;
	}

};