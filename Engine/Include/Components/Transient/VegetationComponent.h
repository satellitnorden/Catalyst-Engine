#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Material.h>
#include <Rendering/Native/Model.h>

class VegetationComponent final
{

public:

	//The model.
	const Model *RESTRICT _Model{ nullptr };

	//The material.
	Material _Material;

	//The mask texture index.
	uint32 _MaskTextureIndex;

	//The cutoff distance.
	float _CutoffDistance;

	//The transformations buffer.
	BufferHandle _TransformationsBuffer;

	//The number of transformations.
	uint32 _NumberOfTransformations;

};