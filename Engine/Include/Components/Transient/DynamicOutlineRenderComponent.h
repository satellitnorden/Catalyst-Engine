#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class DynamicOutlineRenderComponent final
{

public:

	//The physical flags.
	PhysicalFlag _PhysicalFlags;

	//The outline color.
	Vector3<float> _Color;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The offset for the indices in the buffer.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//Denotes whether or not this static physical model is in the view frustum.
	bool _IsInViewFrustum;

};