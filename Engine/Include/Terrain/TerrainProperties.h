#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Terrain.
#include <Terrain/TerrainCore.h>

class TerrainProperties final
{

public:

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//The height function.
	HeightFunction _HeightFunction;

	//The material function.
	MaterialFunction _MaterialFunction;

};