#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/InitializationData/EntityInitializationData.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/TerrainMaterial.h>
#include <Rendering/Engine/TerrainUniformData.h>

class TerrainInitializationData final : public EntityInitializationData
{

public:

	//The axis aligned bounding box.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The terrain properties.
	CPUTexture2D _TerrainProperties;

	//The terrain uniform data.
	TerrainUniformData _TerrainUniformData;

	//The layer weights texture.
	Texture2DHandle _LayerWeightsTexture;

	//The terrain material.
	TerrainMaterial _TerrainMaterial;

};