#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/DynamicString.h>

//Rendering.
#include <Rendering/Engine/Resolution.h>

class CatalystProjectGeneralConfiguration final
{

public:

	//The project name.
	DynamicString _ProjectName;

	/*
	*	Default constructor.
	*/
	CatalystProjectGeneralConfiguration() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	CatalystProjectGeneralConfiguration(const char *const RESTRICT initialProjectName) NOEXCEPT
		:
		_ProjectName(initialProjectName)
	{

	}

};

class CatalystProjectRenderingConfiguration final
{

public:

	//The resolution.
	Resolution _Resolution;

	//The resolution scale.
	float _ResolutionScale;

	//The shadow map resolution.
	uint32 _ShadowMapResolution;

	/*
	*	Default constructor.
	*/
	CatalystProjectRenderingConfiguration() NOEXCEPT
	{

	}

};

//Terrain.
#include <Terrain/TerrainCore.h>

class CatalystProjectTerrainConfiguration final
{

public:

	//The height generation function.
	HeightGenerationFunction _HeightGenerationFunction;

	//The layer weights generation function.
	LayerWeightsGenerationFunction _LayerWeightsGenerationFunction;

	//The patch properties generation function.
	PatchPropertiesGenerationFunction _PatchPropertiesGenerationFunction;

	/*
	*	Default constructor.
	*/
	CatalystProjectTerrainConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectConfiguration final
{

public:

	//The project general configuration.
	CatalystProjectGeneralConfiguration _GeneralConfiguration;

	//The project rendering configuration.
	CatalystProjectRenderingConfiguration _RenderingConfiguration;

	//The project terrain configuration.
	CatalystProjectTerrainConfiguration _TerrainConfiguration;

};