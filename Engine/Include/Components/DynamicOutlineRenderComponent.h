#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class DynamicOutlineRenderComponent final
{

public:

	//The physical flags.
	PhysicalFlag _PhysicalFlags;

	//The outline color.
	Vector3 _Color;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The offset for the indices in the buffer.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//Denotes whether or not this static physical model is in the view frustum.
	bool _IsInViewFrustum;

};