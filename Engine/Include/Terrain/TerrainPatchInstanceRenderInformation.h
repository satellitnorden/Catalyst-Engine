#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/Padding.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TerrainPatchInstanceRenderInformation final
{

public:

	//The world position.
	Vector2 _WorldPosition;

	//The patch size.
	float _PatchSize;

	//The borders.
	int32 _Borders;

	//The height texture index.
	int32 _HeightTextureIndex;

	//The normal texture index.
	int32 _NormalTextureIndex;

	//The layer weights texture index.
	int32 _LayerWeightsTextureIndex;

	//Some padding.
	Padding<4> _Padding;

};