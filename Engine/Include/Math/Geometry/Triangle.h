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
	*	Constructor taking all values as arguments.
	*/
	constexpr Triangle(	const Vector3<float> &initialVertex1,
						const Vector3<float> &initialVertex2,
						const Vector3<float> &initialVertex3) NOEXCEPT
		:
		_Vertex1(initialVertex1),
		_Vertex2(initialVertex2),
		_Vertex3(initialVertex3)
	{

	}

};