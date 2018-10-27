#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TerrainPatchRenderInformation final
{

public:

	//The visibility.
	VisibilityFlag _Visibility;

	//The normal render data table.
	RenderDataTableHandle _NormalRenderDataTable;

	//The world position.
	Vector2 _WorldPosition;

	//The patch size.
	float _PatchSize;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

};