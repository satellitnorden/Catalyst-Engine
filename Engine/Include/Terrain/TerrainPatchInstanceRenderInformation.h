#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TerrainPatchInstanceRenderInformation final
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

	//Padding.
	Padding<8> _Padding;

};