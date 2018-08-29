#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/TerrainMaterial.h>
#include <Rendering/Engine/TerrainUniformData.h>

class TerrainInitializationData final
{

public:

	//The axis aligned bounding box.
	AxisAlignedBoundingBox axisAlignedBoundingBox;

	//The terrain properties.
	CPUTexture2D terrainProperties;

	//The terrain uniform data.
	TerrainUniformData terrainUniformData;

	//The layer weights texture.
	Texture2DHandle layerWeightsTexture;

	//The terrain material.
	TerrainMaterial terrainMaterial;

};