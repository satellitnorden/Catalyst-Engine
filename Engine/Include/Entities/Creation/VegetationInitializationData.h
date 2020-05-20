#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Matrix.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

class VegetationInitializationData final : public EntityInitializationData
{

public:

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

	//The cutoff distance.
	float _CutoffDistance;

	//The impostor material index.
	uint32 _ImpostorMaterialIndex;

	//The impostor distance.
	float _ImpostorDistance;

	//The impostor width.
	float _ImpostorWidth;

	//The impostor height.
	float _ImpostorHeight;

	//The large scale wind displacement factor.
	float _LargeScaleWindDisplacementFactor;

	//The medium scale wind displacement factor.
	float _MediumScaleWindDisplacementFactor;

	//The small scale wind displacement factor.
	float _SmallScaleWindDisplacementFactor;

	//The transformations.
	DynamicArray<Matrix4x4> _Transformations;

};