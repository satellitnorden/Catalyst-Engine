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
	*	Creates the terrain intermediate resources.
	*/
	static void CreateTerrainIntermediateResources() NOEXCEPT;

	/*
	*	Creates the terrain resource collection.
	*/
	static void CreateTerrainResourceCollection() NOEXCEPT;

};