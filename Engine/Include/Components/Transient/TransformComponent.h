#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

class TransformComponent final
{

public:

	//The world transform.
	Matrix4 _WorldTransform{ };
	
};