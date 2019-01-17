#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class GrassVegetationPatchRenderInformation final
{

public:

	//The visibility.
	StaticArray<VisibilityFlag, UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> _Visibilities;

	//The transformations buffers.
	StaticArray<ConstantBufferHandle, UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> _TransformationsBuffers;

	//The number of transformations.
	StaticArray<uint32, UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> _NumberOfTransformations;

};