#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Matrix.h>

//Resources.
#include <Resources/Core/AnimatedModelResource.h>
#include <Resources/Core/AnimationResource.h>
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ResourcePointer.h>

class AnimatedModelInitializationData final : public EntityInitializationData
{

public:

	//The animated model resource.
	ResourcePointer<AnimatedModelResource> _AnimatedModelResource;

	//The transform.
	Matrix4x4 _Transform{ MatrixConstants::IDENTITY };

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

	//The initial animation resource.
	ResourcePointer<AnimationResource> _InitialAnimationResource;

};