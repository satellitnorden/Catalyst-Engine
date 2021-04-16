#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Definitions for the Catalyst screen coordinate space.
namespace CatalystScreenCoordinateSpace
{

	//The right vector.
	constexpr Vector2<float32> RIGHT{ 1.0f, 0.0f };

	//The left vector.
	constexpr Vector2<float32> LEFT{ -1.0f, 0.0f };

	//The up vector.
	constexpr Vector2<float32> UP{ 0.0f, 1.0f };

	//The down vector.
	constexpr Vector2<float32> DOWN{ 0.0f, -1.0f };

}

/*
*	Definitions for the Catalyst world coordinate space.
*	The Catalyst world coordinate space is left handed with positive X being right, positive Y being up and positive Z being forward.
*/
namespace CatalystWorldCoordinateSpace
{

	//The right vector.
	constexpr Vector3<float32> RIGHT{ 1.0f, 0.0f, 0.0f };

	//The left vector.
	constexpr Vector3<float32> LEFT{ -1.0f, 0.0f, 0.0f };

	//The up vector.
	constexpr Vector3<float32> UP{ 0.0f, 1.0f, 0.0f };

	//The down vector.
	constexpr Vector3<float32> DOWN{ 0.0f, -1.0f, 0.0f };

	//The forward vector.
	constexpr Vector3<float32> FORWARD{ 0.0f, 0.0f, 1.0f };

	//The backward vector.
	constexpr Vector3<float32> BACKWARD{ 0.0f, 0.0f, -1.0f };

}

//Catalyst coordinate spaces utilities.
namespace CatalystCoordinateSpacesUtilities
{

	/*
	*	Returns a world space right vector with the given rotation.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> RotatedWorldRightVector(const EulerAngles &rotation) NOEXCEPT
	{
		return CatalystWorldCoordinateSpace::RIGHT.Rotated(rotation);
	}

	/*
	*	Returns a world space up vector with the given rotation.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> RotatedWorldUpVector(const EulerAngles &rotation) NOEXCEPT
	{
		return CatalystWorldCoordinateSpace::UP.Rotated(rotation);
	}

	/*
	*	Returns a world space forward vector with the given rotation.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> RotatedWorldForwardVector(const EulerAngles &rotation) NOEXCEPT
	{
		return CatalystWorldCoordinateSpace::FORWARD.Rotated(rotation);
	}

}