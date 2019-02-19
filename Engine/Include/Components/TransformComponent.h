#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Matrix4.h>

class TransformComponent final
{

public:

	//The position.
	Vector3<float> _Position{ Vector3<float>::ZERO };

	//The rotation.
	Vector3<float> _Rotation{ Vector3<float>::ZERO };

	//The scale.
	Vector3<float> _Scale{ 1.0f, 1.0f, 1.0f };

	//The local transform.
	Matrix4 _LocalTransform;

	//The world transform.
	Matrix4 _WorldTransform;
	
};