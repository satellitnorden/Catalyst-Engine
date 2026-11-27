#pragma once

//Core.
#include <Core/Core.h>

//Generate projects.
#include <GenerateProjects/GenerateProjectParameters.h>

//Project settings.
#include <ProjectSettings/ProjectSettings.h>

class GenerateCopyDynamicLibraries
{

public:

	/*
	*	Generates!
	*/
	static void Generate(const ProjectSettings &project_settings, const GenerateProjectParameters &parameters, const bool allow_spaces, const char *const build_directory);

};