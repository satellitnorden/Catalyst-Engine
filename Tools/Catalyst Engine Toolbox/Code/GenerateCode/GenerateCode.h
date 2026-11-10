#pragma once

//Core.
#include <Core/Core.h>

//Project settings.
#include <ProjectSettings/ProjectSettings.h>

class GenerateCode
{

public:

	/*
	*	Generates the code with the given project settings.
	*/
	static void Run(const ProjectSettings &project_settings);

};