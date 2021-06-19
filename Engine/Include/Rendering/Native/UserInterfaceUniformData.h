#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Matrix.h>
#include <Math/General/Vector.h>

class UserInterfaceUniformData final
{

public:

	//The to world matrix.
	Matrix4x4 _ToWorldMatrix;

	//The normal.
	Vector3<float32> _Normal;

	//Some padding.
	Padding<4> _Padding;

	//The scale.
	Vector2<float32> _Scale;

	//The roughness.
	float32 _Roughness;

	//The metallic.
	float32 _Metallic;

	//The ambient occlusion.
	float32 _AmbientOcclusion;

	//The emissive multiplier.
	float32 _EmissiveMultiplier;

};