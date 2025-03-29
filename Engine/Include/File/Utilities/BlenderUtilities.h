#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>
#include <Math/General/Quaternion.h>

namespace BlenderUtilities
{

	/*
	*	Applies Blender transform on a position/scale.
	*/
	FORCE_INLINE void Transform(Vector3<float32> *const RESTRICT value) NOEXCEPT
	{
		Swap(&value->_Y, &value->_Z);
	}

	/*
	*	Applies Blender transform on a rotation.
	*/
	FORCE_INLINE void Transform(Quaternion *const RESTRICT value) NOEXCEPT
	{
		Swap(&value->_Y, &value->_Z);
	}

	/*
	*	Applies Blender transform on a texture coordinate.
	*/
	FORCE_INLINE void Transform(Vector2<float32> *const RESTRICT value) NOEXCEPT
	{
		value->_Y = 1.0f - value->_Y;
	}

}