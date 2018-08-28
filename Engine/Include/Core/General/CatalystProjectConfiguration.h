#pragma once

//Core.
#include <Core/EngineCore.h>
#include <Core/General/DynamicString.h>

//Rendering.
#include <Rendering/Engine/Resolution.h>

class CatalystProjectGeneralConfiguration final
{

public:

	//The project name.
	DynamicString projectName;

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
		projectName(initialProjectName)
	{

	}

};

class CatalystProjectRenderingConfiguration final
{

public:

	//The resolution.
	Resolution resolution;

	//The resolution scale.
	float resolutionScale;

	//The shadow map resolution.
	uint32 shadowMapResolution;

	/*
	*	Default constructor.
	*/
	CatalystProjectRenderingConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectConfiguration final
{

public:

	//The project general configuration.
	CatalystProjectGeneralConfiguration generalConfiguration;

	//The project rendering configuration.
	CatalystProjectRenderingConfiguration renderingConfiguration;

};