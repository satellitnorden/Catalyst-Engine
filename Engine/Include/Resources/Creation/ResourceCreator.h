#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Parameters/ModelCreationParameters.h>

class ResourceCreator final
{

public:

	/*
	*	Creates a model.
	*/
	static void CreateModel(const ModelCreationParameters &parameters) NOEXCEPT;

};