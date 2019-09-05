#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Material.h>
#include <Rendering/Native/Model.h>

class VegetationInitializationData final : public EntityInitializationData
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

	//The large scale wind displacement factor.
	float _LargeScaleWindDisplacementFactor;

	//The medium scale wind displacement factor.
	float _MediumScaleWindDisplacementFactor;

	//The small scale wind displacement factor.
	float _SmallScaleWindDisplacementFactor;

	//The transformations.
	DynamicArray<Matrix4> _Transformations;

};