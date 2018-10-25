#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class VegetationPatchRenderInformation final
{

public:

	//The visibility.
	VisibilityFlag _Visibility;

	//The level of detail.
	VegetationLevelOfDetail _LevelOfDetail{ VegetationLevelOfDetail::Low };

	//The transformations buffer.
	ConstantBufferHandle _TransformationsBuffer;

	//The number of transformations.
	uint32 _NumberOfTransformations;

};