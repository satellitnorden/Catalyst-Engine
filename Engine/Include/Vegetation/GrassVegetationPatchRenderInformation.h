#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class GrassVegetationPatchRenderInformation final
{

public:

	//The visibility.
	StaticArray<VisibilityFlag, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Visibilities;

	//The transformations buffers.
	StaticArray<BufferHandle, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _TransformationsBuffers;

	//The number of transformations.
	StaticArray<uint32, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _NumberOfTransformations;

};