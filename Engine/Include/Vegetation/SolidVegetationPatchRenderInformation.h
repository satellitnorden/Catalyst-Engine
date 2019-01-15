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
	VisibilityFlag _Visibility;

	//The transformations buffer.
	ConstantBufferHandle _TransformationsBuffer;

	//The number of transformations.
	uint32 _NumberOfTransformations;

};