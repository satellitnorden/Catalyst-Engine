#pragma once

//Core.
#include <Core/Core.h>

//Generate projects.
#include <GenerateProjects/GenerateProjectParameters.h>

//Project settings.
#include <ProjectSettings/ProjectSettings.h>

//Constants.
#define CLION_GENERATE_IDEA (0)

class GenerateCLion
{

public:

	/*
	*	Generates!
	*/
	static void Generate(const ProjectSettings &project_settings, const GenerateProjectParameters &parameters);

private:

#if CLION_GENERATE_IDEA
	/*
	*	Generates the run configuration files. file.
	*/
	static void GenerateRunConfigurationFiles(const GenerateProjectParameters &parameters);

	/*
	*	Generates the 'workspace.xml' file.
	*/
	static void GenerateWorkspaceFile(const GenerateProjectParameters &parameters);
#endif

};