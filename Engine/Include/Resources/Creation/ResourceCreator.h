#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/Model.h>

//Resources.
#include <Resources/Data/ModelData.h>

class ResourceCreator final
{

public:

	/*
	*	Creates a model.
	*/
	static void CreateModel(const ModelData &data, Model *const RESTRICT model) NOEXCEPT;

};