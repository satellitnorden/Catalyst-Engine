#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Native/Material.h>
#include <Rendering/Native/Model.h>

class VegetationComponent final
{

public:

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox _WorldSpaceAxisAlignedBoundingBox;

	//Denotes the visibility of this vegetation.
	bool _Visibility;

	//The model.
	const Model *RESTRICT _Model;

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

	//The transformations buffer.
	BufferHandle _TransformationsBuffer;

	//The number of transformations.
	uint32 _NumberOfTransformations;

};