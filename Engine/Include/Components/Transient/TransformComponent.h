#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Matrix4.h>

class TransformComponent final
{

public:

	//The local transform.
	Matrix4 _LocalTransform{ };

	//The world transform.
	Matrix4 _WorldTransform{ };
	
};