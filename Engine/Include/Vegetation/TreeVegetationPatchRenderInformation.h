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
	StaticArray<VisibilityFlag, UNDERLYING(TreeVegetationLevelOfDetail::NumberOfTreeVegetationLevelOfDetails)> _Visibilities;

	//The transformations buffers.
	StaticArray<ConstantBufferHandle, UNDERLYING(TreeVegetationLevelOfDetail::NumberOfTreeVegetationLevelOfDetails)> _TransformationsBuffers;

	//The number of transformations.
	StaticArray<uint32, UNDERLYING(TreeVegetationLevelOfDetail::NumberOfTreeVegetationLevelOfDetails)> _NumberOfTransformations;

};