#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class DynamicPhysicalRenderComponent final
{

public:

	//The physical flags.
	uint8 _PhysicalFlags;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The offset for the indices in the buffer.
	uint64 _IndexOffset;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The index count.
	uint32 _IndexCount;

	//Denotes whether or not this static physical model is in the view frustum.
	bool _IsInViewFrustum;

};