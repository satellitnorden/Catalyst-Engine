#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>
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
#if 1
		(*value) = Quaternion(-value->_X, -value->_Z, -value->_Y, value->_W);
#else
		Swap(&value->_Y, &value->_Z);
#endif
	}

	/*
	*	Applies Blender transform on a texture coordinate.
	*/
	FORCE_INLINE void Transform(Vector2<float32> *const RESTRICT value) NOEXCEPT
	{
		value->_Y = 1.0f - value->_Y;
	}

	/*
	*	Applies Blender transform on a matrix.
	*/
	FORCE_INLINE void Transform(Matrix4x4 *const RESTRICT value) NOEXCEPT
	{
		//Swap Y and Z axis on the translation.
		Swap(&value->_Matrix[3]._Y, &value->_Matrix[3]._Z);
	}

}