#pragma once

//Core.
#include <Core/EngineCore.h>

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

class CatalystProjectMultithreadingConfiguration final
{

public:

	/*
	*	Denotes the maximum number of tasks that the game can fire off simultaneously at any given time.
	*	There is no checking if this number is exceeded, the game can appear to work fine even if this is exceeded.
	*	Possible problems that can arise if this is not set up properly is deadlocks, tasks not executing as expected and even crashes.
	*/
	uint32 numberOfGameTasks;

	/*
	*	Default constructor.
	*/
	CatalystProjectMultithreadingConfiguration() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	CatalystProjectMultithreadingConfiguration(const uint32 initialNumberOfGameTasks) NOEXCEPT
		:
		numberOfGameTasks(initialNumberOfGameTasks)
	{

	}

};

class CatalystProjectRenderingConfiguration final
{

public:

	//The shadow map resolution.
	uint32 shadowMapResolution;

	/*
	*	Default constructor.
	*/
	CatalystProjectRenderingConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectSoundConfiguration final
{

public:

	//The maximum number of channels.
	uint32 maximumNumberOfChannels;

	/*
	*	Default constructor.
	*/
	CatalystProjectSoundConfiguration() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	CatalystProjectSoundConfiguration(const uint32 initialMaximumNumberOfChannels) NOEXCEPT
		:
		maximumNumberOfChannels(initialMaximumNumberOfChannels)
	{

	}

};

class CatalystProjectConfiguration final
{

public:

	//The project general configuration.
	CatalystProjectGeneralConfiguration generalConfiguration;

	//The project multithreading configuration.
	CatalystProjectMultithreadingConfiguration multithreadingConfiguration;

	//The project rendering configuration.
	CatalystProjectRenderingConfiguration renderingConfiguration;

	//The project sound configuration.
	CatalystProjectSoundConfiguration soundConfiguration;

	/*
	*	Default constructor.
	*/
	CatalystProjectConfiguration() NOEXCEPT
	{

	}

};