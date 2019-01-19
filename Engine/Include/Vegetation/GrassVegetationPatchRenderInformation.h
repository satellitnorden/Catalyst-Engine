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
	StaticArray<VisibilityFlag, UNDERLYING(VegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> _Visibilities;

	//The transformations buffers.
	StaticArray<ConstantBufferHandle, UNDERLYING(VegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> _TransformationsBuffers;

	//The number of transformations.
	StaticArray<uint32, UNDERLYING(VegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> _NumberOfTransformations;

};