#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Animation.
#include <Animation/Animation.h>
#include <Animation/AnimatedModel.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Material.h>

class AnimatedModelInitializationData final : public EntityInitializationData
{

public:

	//The transform.
	Matrix4 _Transform{ MatrixConstants::IDENTITY };

	//The model.
	const AnimatedModel *RESTRICT _Model{ nullptr };

	//The material.
	Material _Material;

	//The initial animation.
	const Animation* RESTRICT _InitialAnimation{ nullptr };

};