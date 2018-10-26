#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Terrain.
#include <Terrain/TerrainDisplacementInformation.h>

class TerrainPatchRenderInformation final
{

public:

	//The visibility.
	VisibilityFlag _Visibility;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//The displacement information.
	TerrainDisplacementInformation _DisplacementInformation;

};