#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

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

class CatalystProjectRenderingInformation final
{

public:

	//The shaders path.
	DynamicString shadersPath;

	/*
	*	Default constructor.
	*/
	CatalystProjectRenderingInformation() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	CatalystProjectRenderingInformation(const char *const RESTRICT initialShadersPath) NOEXCEPT
		:
		shadersPath(initialShadersPath)
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
	CatalystProjectInformation(const CatalystProjectGeneralInformation &initialGeneralInformation, const CatalystProjectRenderingInformation &initialRenderingInformation, const CatalystProjectSoundInformation &initialSoundInformation) NOEXCEPT
		:
		generalInformation(initialGeneralInformation),
		renderingInformation(initialRenderingInformation),
		soundInformation(initialSoundInformation)
	{

	}

};