#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class CatalystProjectInformation final
{

public:

	//The project name.
	DynamicString projectName;

	/*
	*	Default constructor.
	*/
	CatalystProjectInformation() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all parameters as arguments.
	*/
	CatalystProjectInformation(DynamicString &&initialProjectName) NOEXCEPT
		:
		projectName(initialProjectName)
	{

	}

};