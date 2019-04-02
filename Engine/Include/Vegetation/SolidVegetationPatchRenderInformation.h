#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class SolidVegetationPatchRenderInformation final
{

public:

	//The visibility.
	StaticArray<VisibilityFlag, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Visibilities;

	//The transformations buffers.
	StaticArray<ConstantBufferHandle, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _TransformationsBuffers;

	//The number of transformations.
	StaticArray<uint32, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _NumberOfTransformations;

};