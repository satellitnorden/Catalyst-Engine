#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>

class TerrainQuadTreeNode final
{

public:

	//The depth.
	uint8 _Depth{ 0 };

	//The borders.
	int32 _Borders;

	//The child nodes.
	DynamicArray<TerrainQuadTreeNode> _ChildNodes;

	//The minimum world position of this node.
	Vector2<float32> _Minimum;

	//The maximum world position of this node.
	Vector2<float32> _Maximum;

	//The height map resolution.
	uint32 _HeightMapResolution;

	//The height map texture.
	Texture2DHandle _HeightMapTexture;

	//The height map texture index.
	uint32 _HeightMapTextureIndex;

	//The material maps resolution.
	uint32 _MaterialMapsResolution;

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

	//The terrain patch information.
	TerrainPatchInformation _TerrainPatchInformation;

	//The terrain patch render information.
	TerrainPatchRenderInformation _TerrainPatchRenderInformation;

	/*
	*	Returns whether or not this node is subdivided.
	*/
	FORCE_INLINE NO_DISCARD bool IsSubdivided() const NOEXCEPT
	{
		return !_ChildNodes.Empty();
	}

	/*
	*	Returns whether or not a world position is within this node.
	*/
	FORCE_INLINE NO_DISCARD bool IsWithin(const Vector3<float32> &world_position) const NOEXCEPT
	{
		return	world_position._X >= _Minimum._X
				&& world_position._X < _Maximum._X
				&& world_position._Z >= _Minimum._Y
				&& world_position._Z < _Maximum._Y;
	}

};