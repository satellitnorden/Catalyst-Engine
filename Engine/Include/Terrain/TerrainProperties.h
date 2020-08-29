#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <World/Core/WorldPosition.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Terrain.
#include <Terrain/TerrainCore.h>

class TerrainProperties final
{

public:

	//The patch size.
	float32 _PatchSize;

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

	//Denotes whether or note a terrain world center has been set.
	bool _HasTerrainWorldCenter{ false };

	//The terrain world center.
	WorldPosition _TerrainWorldCenter;

	//Denotes whether or not a height map has been set.
	bool _HasHeightMap{ false };

	//The height map.
	Texture2D<float32> _HeightMap;

	//The height map texture.
	Texture2DHandle _HeightMapTexture;

	//The height map texture index.
	uint32 _HeightMapTextureIndex;

	//Denotes whether or not an index map has been set.
	bool _HasIndexMap{ false };

	//The index map.
	Texture2D<Vector4<uint8>> _IndexMap;

	//The index map texture.
	Texture2DHandle _IndexMapTexture;

	//The index map texture index.
	uint32 _IndexMapTextureIndex;

	//Denotes whether or not a blend map has been set.
	bool _HasBlendMap{ false };

	//The blend map.
	Texture2D<Vector4<uint8>> _BlendMap;

	//The blend map texture.
	Texture2DHandle _BlendMapTexture;

	//The blend map texture index.
	uint32 _BlendMapTextureIndex;

};