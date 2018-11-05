#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TerrainPatchRenderInformation final
{

public:

	//The visibility.
	VisibilityFlag _Visibility;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The world position.
	Vector2 _WorldPosition;

	//The patch size.
	float _PatchSize;

};