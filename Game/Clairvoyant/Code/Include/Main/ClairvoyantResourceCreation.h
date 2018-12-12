#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class ClairvoyantResourceCreation
{

public:

	/*
	*	Creates resources for the Clairvoyant project.
	*/
	static void CreateResources() NOEXCEPT;

private:

	/*
	*	Creates the grass vegetation intermediate resources.
	*/
	static void CreateGrassVegetationIntermediateResources() NOEXCEPT;

	/*
	*	Creates the grass vegetation resource collection.
	*/
	static void CreateGrassVegetationResourceCollection() NOEXCEPT;

	/*
	*	Creates the location intermediate resources.
	*/
	static void CreateLocationIntermediateResources() NOEXCEPT;

	/*
	*	Creates the location resource collection.
	*/
	static void CreateLocationResourceCollection() NOEXCEPT;

	/*
	*	Creates the terrain intermediate resources.
	*/
	static void CreateTerrainIntermediateResources() NOEXCEPT;

	/*
	*	Creates the terrain resource collection.
	*/
	static void CreateTerrainResourceCollection() NOEXCEPT;

};