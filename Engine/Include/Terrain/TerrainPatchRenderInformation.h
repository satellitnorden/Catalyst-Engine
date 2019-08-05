#pragma once

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

	//The normal texture index.
	int32 _NormalTextureIndex;

	//The material texture index.
	int32 _MaterialTextureIndex;

	//The material texture resolution.
	float _MaterialTextureResolution;

	//The material texture resolution squared.
	float _MaterialTextureResolutionSquared;

	//The inverse material texture resolution.
	float _InverseMaterialTextureResolution;

	//Denotes the visibilty of this patch.
	bool _Visibility{ false };

};