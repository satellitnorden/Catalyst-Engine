#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class TreeVegetationPatchRenderInformation final
{

public:

	//The visibility.
	StaticArray<VisibilityFlag, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Visibilities;

	//The transformations buffers.
	StaticArray<ConstantBufferHandle, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _TransformationsBuffers;

	//The number of transformations.
	StaticArray<uint32, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _NumberOfTransformations;

};