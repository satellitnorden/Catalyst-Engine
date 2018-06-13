#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

class TerrainRenderComponent final
{

public:

	//Denotes whether or not the terrain is in the view frustum.
	bool isInViewFrustum;

	//The descriptor set.
	RenderDataTableHandle descriptorSet{ };

	//The vertex buffer.
	ConstantBufferHandle vertexAndIndexBuffer{ };

	//The index buffer offset.
	uint64 indexBufferOffset{ 0 };

	//The index count.
	uint32 indexCount{ 0 };

};