#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/AxisAlignedBoundingBox.h>
#include <Rendering/Engine Layer/CPUTexture2D.h>
#include <Rendering/Engine Layer/RenderingCore.h>
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/TerrainUniformData.h>

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