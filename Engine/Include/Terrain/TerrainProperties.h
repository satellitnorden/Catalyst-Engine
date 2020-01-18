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

	//The patch size of the root nodes.
	float _PatchSize;

	//The patch resolution.
	uint32 _PatchResolution;

	//The maximum quad tree depth.
	uint8 _MaximumQuadTreeDepth;

	//The buffer.
	BufferHandle _Buffer;

	//The index offset.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//Denotes whether or not a height map has been set.
	bool _HasHeightMap{ false };

	//The height map.
	Texture2D<float> _HeightMap;

	//The height map texture.
	Texture2DHandle _HeightMapTexture;

	//The height map texture index.
	uint32 _HeightMapTextureIndex;

};