#pragma once

//Core.
#include <Core/EngineCore.h>

class CatalystProjectGeneralInformation final
{

public:

	//The project name.
	DynamicString projectName;

	/*
	*	Default constructor.
	*/
	CatalystProjectGeneralInformation() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	CatalystProjectGeneralInformation(const char *const RESTRICT initialProjectName) NOEXCEPT
		:
		projectName(initialProjectName)
	{

	}

};

class CatalystProjectMultithreadingInformation final
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
	CatalystProjectMultithreadingInformation() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	CatalystProjectMultithreadingInformation(const uint32 initialNumberOfGameTasks) NOEXCEPT
		:
	numberOfGameTasks(initialNumberOfGameTasks)
	{

	}

};

class CatalystProjectRenderingInformation final
{

public:

	/*
	*	Default constructor.
	*/
	CatalystProjectRenderingInformation() NOEXCEPT
	{

	}

};

class CatalystProjectSoundInformation final
{

public:

	//The maximum number of channels.
	uint32 maximumNumberOfChannels;

	/*
	*	Default constructor.
	*/
	CatalystProjectSoundInformation() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	CatalystProjectSoundInformation(const uint32 initialMaximumNumberOfChannels) NOEXCEPT
		:
		maximumNumberOfChannels(initialMaximumNumberOfChannels)
	{

	}

};

class CatalystProjectInformation final
{

public:

	//The project general information.
	CatalystProjectGeneralInformation generalInformation;

	//The project multithreading information.
	CatalystProjectMultithreadingInformation multithreadingInformation;

	//The project rendering information.
	CatalystProjectRenderingInformation renderingInformation;

	//The project sound information.
	CatalystProjectSoundInformation soundInformation;

	/*
	*	Default constructor.
	*/
	CatalystProjectInformation() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all parameters as arguments.
	*/
	CatalystProjectInformation(const CatalystProjectGeneralInformation &initialGeneralInformation, const CatalystProjectMultithreadingInformation &initialMultithreadingInformation, const CatalystProjectRenderingInformation &initialRenderingInformation, const CatalystProjectSoundInformation &initialSoundInformation) NOEXCEPT
		:
		generalInformation(initialGeneralInformation),
		multithreadingInformation(initialMultithreadingInformation),
		renderingInformation(initialRenderingInformation),
		soundInformation(initialSoundInformation)
	{

	}

};