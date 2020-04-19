#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Animation.
#include <Animation/Animation.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Material.h>

//Resources.
#include <Resources/Core/AnimatedModelResource.h>
#include <Resources/Core/ResourcePointer.h>

class AnimatedModelInitializationData final : public EntityInitializationData
{

public:

	//The animated model resource.
	ResourcePointer<AnimatedModelResource> _AnimatedModelResource;

	//The transform.
	Matrix4x4 _Transform{ MatrixConstants::IDENTITY };

	//The material.
	Material _Material;

	//The initial animation.
	const Animation* RESTRICT _InitialAnimation{ nullptr };

};