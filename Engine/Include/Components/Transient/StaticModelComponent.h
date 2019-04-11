#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Model.h>

class StaticModelComponent final
{

public:

	//The model.
	const Model *RESTRICT _Model{ nullptr };

};