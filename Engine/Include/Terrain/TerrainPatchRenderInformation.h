#pragma once

//Terrain.
#include <Terrain/TerrainCore.h>

#if NEW_TERRAIN_SYSTEM

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class TerrainPatchRenderInformation final
{

public:

	//The world position.
	Vector2<float> _WorldPosition;

	//The patch size.
	float _PatchSize;

	//The borders.
	int32 _Borders;

	//The height texture index.
	int32 _HeightTextureIndex;

	//The normal and material texture index.
	int32 _NormalAndMaterialTextureIndex;

	//The material texture resolution.
	float _MaterialTextureResolution;

	//The material texture resolution squared.
	float _MaterialTextureResolutionSquared;

	//The inverse material texture resolution.
	float _InverseMaterialTextureResolution;

	//Denotes the visibilty of this patch.
	bool _Visibility{ false };

	//The buffer.
	BufferHandle _Buffer;

	//The index offset.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

};

#else

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class TerrainPatchRenderInformation final
{

public:

	//The world position.
	Vector2<float> _WorldPosition;

	//The patch size.
	float _PatchSize;

	//The borders.
	int32 _Borders;

	//The height texture index.
	int32 _HeightTextureIndex;

	//The normal and material texture index.
	int32 _NormalAndMaterialTextureIndex;

	//The material texture resolution.
	float _MaterialTextureResolution;

	//The material texture resolution squared.
	float _MaterialTextureResolutionSquared;

	//The inverse material texture resolution.
	float _InverseMaterialTextureResolution;

	//Denotes the visibilty of this patch.
	bool _Visibility{ false };

};

#endif