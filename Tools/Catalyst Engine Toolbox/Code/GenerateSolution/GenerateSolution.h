#pragma once

//Core.
#include <Core/Core.h>

//Project settings.
#include <ProjectSettings/ProjectSettings.h>

class GenerateSolution
{

public:

	/*
	*	Generates a solution with the given project settings.
	*/
	static void Run(const ProjectSettings &project_settings);

private:

	/*
	*	Cleans.
	*/
	static void Clean(const ProjectSettings &project_settings);

};