#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TerrainRenderInformation final
{

public:

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The displacement information.
	TerrainDisplacementInformation _DisplacementInformation;

};