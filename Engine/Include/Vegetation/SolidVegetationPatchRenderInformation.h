#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class SolidVegetationPatchRenderInformation final
{

public:

	//The visibility.
	StaticArray<VisibilityFlag, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> _Visibilities;

	//The transformations buffers.
	StaticArray<ConstantBufferHandle, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> _TransformationsBuffers;

	//The number of transformations.
	StaticArray<uint32, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> _NumberOfTransformations;

};