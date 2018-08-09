#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TerrainRenderComponent final
{

public:

	//Denotes whether or not the terrain is in the view frustum.
	bool isInViewFrustum;

	//The render data table.
	RenderDataTableHandle renderDataTable{ };

	//The vertex buffer.
	ConstantBufferHandle vertexAndIndexBuffer{ };

	//The index buffer offset.
	uint64 indexBufferOffset{ 0 };

	//The index count.
	uint32 indexCount{ 0 };

};