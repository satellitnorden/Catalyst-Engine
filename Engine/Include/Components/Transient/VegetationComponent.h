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

	//Enumeration covering all level of details.
	enum class LevelOfDetail : uint8
	{
		Full,
		Impostor
	};

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox _WorldSpaceAxisAlignedBoundingBox;

	//Denotes the visibility of this vegetation.
	bool _Visibility;

	//The current level of detail.
	LevelOfDetail _LevelOfDetail{ LevelOfDetail::Full };

	//The model.
	const Model *RESTRICT _Model;

	//The material.
	Material _Material;

	//The cutoff distance.
	float _CutoffDistance;

	//The impostor albedo texture index.
	uint32 _ImpostorAlbedoTextureIndex;

	//The impostor mask texture index.
	uint32 _ImpostorMaskTextureIndex;

	//The impostor distance, squared.
	float _ImpostorDistanceSquared;

	//The impostor half width.
	float _ImpostorHalfWidth;

	//The impostor height.
	float _ImpostorHeight;

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