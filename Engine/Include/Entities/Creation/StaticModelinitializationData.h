#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Model.h>

class StaticModelInitializationData final : public EntityInitializationData
{

public:

	//The transform.
	Matrix4 _Transform{ MatrixConstants::IDENTITY };

	//The model.
	const Model *RESTRICT _Model{ nullptr };

};