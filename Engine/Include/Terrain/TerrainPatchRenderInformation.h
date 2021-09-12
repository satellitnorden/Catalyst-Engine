#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class TerrainPatchRenderInformation final
{

public:

	//The world position.
	Vector2<float32> _WorldPosition;

	//The patch size.
	float32 _PatchSize;

	//The borders.
	int32 _Borders;

	//The height map texture index.
	uint32 _HeightMapTextureIndex;

	//The height map resolution.
	uint32 _HeightMapResolution;

	//The material maps resolution.
	uint32 _MaterialMapsResolution;

	//The normal map texture index.
	uint32 _NormalMapTextureIndex;

	//The index map texture index.
	uint32 _IndexMapTextureIndex;

	//The blend map texture index.
	uint32 _BlendMapTextureIndex;

	//Denotes the visibilty of this patch.
	bool _Visibility{ false };

	/*
	*	Equality operator overload.
	*/
	FORCE_INLINE NO_DISCARD bool operator==(const TerrainPatchRenderInformation &other) NOEXCEPT
	{
		return	_WorldPosition == other._WorldPosition
				&& _PatchSize == other._PatchSize
				&& _HeightMapTextureIndex == other._HeightMapTextureIndex
				&& _HeightMapResolution == other._HeightMapResolution
				&& _MaterialMapsResolution == other._MaterialMapsResolution
				&& _NormalMapTextureIndex == other._NormalMapTextureIndex
				&& _IndexMapTextureIndex == other._IndexMapTextureIndex
				&& _BlendMapTextureIndex == other._BlendMapTextureIndex;
	}

};