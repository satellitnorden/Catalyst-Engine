#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TerrainRenderComponent final
{

public:

	//The vertex buffer.
	ConstantBufferHandle _Buffer;

	//The index buffer offset.
	uint64 _IndexOffset{ 0 };

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The material data table.
	RenderDataTableHandle _MaterialRenderDataTable;

	//The index count.
	uint32 _IndexCount{ 0 };

	//Denotes whether or not the terrain is in the view frustum.
	bool _IsInViewFrustum;

};