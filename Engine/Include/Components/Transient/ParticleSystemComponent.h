#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>
#include <Math/General/Vector.h>

class ParticleSystemComponent final
{

public:

	//The world transform.
	Matrix4 _WorldTransform;

	//The global material index.
	uint32 _GlobalMaterialIndex;

	//The position extent.
	Vector3<float> _PositionExtent;

	//The velocity extent.
	Vector3<float> _VelocityExtent;

	//The size extent.
	Vector2<float> _SizeExtent;

	//The fade time.
	float _FadeTime;

	//The lifetime.
	float _Lifetime;

};