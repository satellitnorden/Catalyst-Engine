#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Building/ModelBuildParameters.h>
#include <Resources/Building/ResourceCollectionBuildParameters.h>

class ResourceBuilder final
{

public:

	/*
	*	Builds a resource collection.
	*/
	static void BuildResourceCollection(const ResourceCollectionBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a model.
	*/
	static void BuildModel(const ModelBuildParameters &parameters) NOEXCEPT;

};