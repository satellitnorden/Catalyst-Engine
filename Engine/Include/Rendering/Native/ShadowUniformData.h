#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Matrix.h>

class ShadowUniformData final
{

public:

	//The world to light matrices.
	StaticArray<Matrix4x4, 4> _WorldToLightMatrices;

	//The shadow map render target indices.
	StaticArray<Vector4<uint32>, 4> _ShadowMapRenderTargetIndices;

	//The shadow map cascade distances squared.
	StaticArray<Vector4<float32>, 4> _ShadowMapCascadeDistancesSquared;

};