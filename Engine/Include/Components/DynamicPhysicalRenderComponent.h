#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class DynamicPhysicalRenderComponent final
{

public:

	//Denotes whether or not this static physical model is in the view frustum.
	bool isInViewFrustum;

	//The render data table.
	RenderDataTableHandle renderDataTable;

	//The buffer.
	ConstantBufferHandle buffer;

	//The offset for the indices in the buffer.
	uint64 indexOffset;

	//The index count.
	uint32 indexCount;

};