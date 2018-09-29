#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class VegetationPatchRenderInformation final
{

public:

	//Denotes whether or not to draw this patch.
	bool _Draw;

	//The transformations buffer.
	ConstantBufferHandle _TransformationsBuffer;

	//The number of transformations.
	uint32 _NumberOfTransformations;

};