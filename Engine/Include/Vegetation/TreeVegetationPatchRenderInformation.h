#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class TreeVegetationPatchRenderInformation final
{

public:

	//The visibility.
	StaticArray<VisibilityFlag, UNDERLYING(TreeVegetationLevelOfDetail::NumberOfTreeVegetationLevelOfDetails)> _Visibilities;

	//The transformations buffer.
	ConstantBufferHandle _TransformationsBuffer;

	//The number of transformations.
	StaticArray<uint32, UNDERLYING(TreeVegetationLevelOfDetail::NumberOfTreeVegetationLevelOfDetails)> _NumberOfTransformations;

	//The transformations offsets.
	StaticArray<uint64, UNDERLYING(TreeVegetationLevelOfDetail::NumberOfTreeVegetationLevelOfDetails)> _TransformationsOffsets;

};