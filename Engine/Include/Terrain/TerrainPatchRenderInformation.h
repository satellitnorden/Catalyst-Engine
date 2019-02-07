#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Terrain.
#include <Terrain/TerrainPatchInstanceRenderInformation.h>

class TerrainPatchRenderInformation final
{

public:

	//The visibility.
	VisibilityFlag _Visibility;

	//The instance information.
	TerrainPatchInstanceRenderInformation _InstanceInformation;

};