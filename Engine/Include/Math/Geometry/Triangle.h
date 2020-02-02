#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class Triangle final
{

public:

	//The first vertex.
	Vector3<float> _Vertex1;

	//The second vertex.
	Vector3<float> _Vertex2;

	//The third vertex.
	Vector3<float> _Vertex3;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Triangle() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Triangle(const Vector3<float> &initial_vertex_1,
									const Vector3<float> &initial_vertex_2,
									const Vector3<float> &initial_vertex_3) NOEXCEPT
		:
		_Vertex1(initial_vertex_1),
		_Vertex2(initial_vertex_2),
		_Vertex3(initial_vertex_3)
	{

	}

};