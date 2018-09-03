#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TerrainRenderComponent final
{

public:

	//Denotes whether or not the terrain is in the view frustum.
	bool _IsInViewFrustum;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The vertex buffer.
	ConstantBufferHandle _Buffer;

	//The index buffer offset.
	uint64 _IndexOffset{ 0 };

	//The index count.
	uint32 _IndexCount{ 0 };

};