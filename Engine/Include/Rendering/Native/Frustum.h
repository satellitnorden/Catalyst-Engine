#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

class Frustum final
{

public:

	//The planes.
	StaticArray<Vector4<float32>, 6> _Planes;

};