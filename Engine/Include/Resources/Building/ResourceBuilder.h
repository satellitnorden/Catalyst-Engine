#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Building/ModelBuildParameters.h>
#include <Resources/Building/ResourceCollectionBuildParameters.h>
#include <Resources/Building/Texture2DBuildParameters.h>

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

	/*
	*	Builds a texture 2D.
	*/
	static void BuildTexture2D(const Texture2DBuildParameters &parameters) NOEXCEPT;

};