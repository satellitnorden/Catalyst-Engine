#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Terrain.
#include <Terrain/TerrainQuadTree.h>

//World.
#include <World/Core/WorldPosition.h>
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class TerrainComponent final
{

public:

	//The world position.
	WorldPosition _WorldPosition;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The patch size.
	uint32 _PatchSize;

	//The height map.
	Texture2D<float32> _HeightMap;

	//The normal map.
	Texture2D<Vector3<float32>> _NormalMap;

	//The index map.
	Texture2D<Vector4<uint8>> _IndexMap;

	//The blend map.
	Texture2D<Vector4<float32>> _BlendMap;

	//The base resolution.
	uint32 _BaseResolution;

	//The maximum subdivision steps.
	uint8 _MaximumSubdivisionSteps;

	//The buffer.
	BufferHandle _Buffer;

	//The index offset.
	uint32 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//The height map texture.
	Texture2DHandle _HeightMapTexture;

	//The Height map texture index.
	uint32 _HeightMapTextureIndex;

	//The normal map texture.
	Texture2DHandle _NormalMapTexture;

	//The normal map texture index.
	uint32 _NormalMapTextureIndex;

	//The index map texture.
	Texture2DHandle _IndexMapTexture;

	//The index map texture index.
	uint32 _IndexMapTextureIndex;

	//The blend map texture.
	Texture2DHandle _BlendMapTexture;

	//The blend map texture index.
	uint32 _BlendMapTextureIndex;

	//Denotes whether or not this terrain is visible.
	bool _Visibility;

	//The quad tree.
	TerrainQuadTree _QuadTree;

};