#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class LineSegment3D final
{

public:

	//The start.
	Vector3<float32> _Start;

	//The end.
	Vector3<float32> _End;

	//The thickness.
	float32 _Thickness;

};