#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class CatalystProjectInformation final
{

public:

	//The project name.
	DynamicString projectName;

	//The shaders path.
	DynamicString shadersPath;

	/*
	*	Default constructor.
	*/
	CatalystProjectInformation() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all parameters as arguments.
	*/
	CatalystProjectInformation(const char *const RESTRICT initialProjectName, const char *const RESTRICT initialShadersPath) NOEXCEPT
		:
		projectName(initialProjectName),
		shadersPath(initialShadersPath)
	{

	}

};