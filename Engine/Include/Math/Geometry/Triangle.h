#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

class Triangle final
{

public:

	/*
	*	Calculates the center of a triangle.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> CalculateCenter(const Triangle &triangle) NOEXCEPT
	{
		//Define constants.
		constexpr float RECIPROCAL{ 1.0f / 3.0f };

		return (triangle._Vertices[0] + triangle._Vertices[1] + triangle._Vertices[2]) * RECIPROCAL;
	}

	//The vertices.
	StaticArray<Vector3<float>, 3> _Vertices;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Triangle() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE Triangle(const StaticArray<Vector3<float>, 3> &initial_vertices) NOEXCEPT
		:
		_Vertices(initial_vertices)
	{

	}

};