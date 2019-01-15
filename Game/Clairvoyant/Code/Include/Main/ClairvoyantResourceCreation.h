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
	*	Creates the debris vegetation intermediate resources.
	*/
	static void CreateDebrisVegetationIntermediateResources() NOEXCEPT;

	/*
	*	Creates the debris vegetation resource collection.
	*/
	static void CreateDebrisVegetationResourceCollection() NOEXCEPT;

	/*
	*	Creates the grass vegetation intermediate resources.
	*/
	static void CreateGrassVegetationIntermediateResources() NOEXCEPT;

	/*
	*	Creates the grass vegetation resource collection.
	*/
	static void CreateGrassVegetationResourceCollection() NOEXCEPT;

	/*
	*	Creates the solid vegetation intermediate resources.
	*/
	static void CreateSolidVegetationIntermediateResources() NOEXCEPT;

	/*
	*	Creates the solid vegetation resource collection.
	*/
	static void CreateSolidVegetationResourceCollection() NOEXCEPT;

	/*
	*	Creates the terrain intermediate resources.
	*/
	static void CreateTerrainIntermediateResources() NOEXCEPT;

	/*
	*	Creates the terrain resource collection.
	*/
	static void CreateTerrainResourceCollection() NOEXCEPT;

	/*
	*	Creates the world intermediate resources.
	*/
	static void CreateWorldIntermediateResources() NOEXCEPT;

	/*
	*	Creates the world resource collection.
	*/
	static void CreateWorldResourceCollection() NOEXCEPT;

};