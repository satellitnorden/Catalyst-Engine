#define VISUAL_STUDIO_CODE_WORKSPACE_SUPPORTED (1)

#if VISUAL_STUDIO_CODE_WORKSPACE_SUPPORTED
#pragma once

//Core.
#include <Core/Core.h>

//Generate projects.
#include <GenerateProjects/GenerateProjectParameters.h>

class GenerateVisualStudioCodeWorkspace
{

public:

	/*
	*	Generates!
	*/
	static void Generate(const GenerateProjectParameters &parameters);

private:

	/*
	*	Generates the '.code-workspace' file.
	*/
	static void GenerateCodeWorkspaceFile(const GenerateProjectParameters &parameters);

	/*
	*	Generates the 'CMakePresets.json' file.
	*/
	static void GenerateCMakePresetsFile(const GenerateProjectParameters &parameters);

	/*
	*	Generates the 'settings.json' file.
	*/
	static void GenerateSettingsFile(const GenerateProjectParameters &parameters);

	/*
	*	Generates the 'extensions.json' file.
	*/
	static void GenerateExtensionsFile(const GenerateProjectParameters &parameters);

	/*
	*	Generates the 'tasks.json' file.
	*/
	static void GenerateTasksFile(const GenerateProjectParameters &parameters);

	/*
	*	Generates the 'launch.json' file.
	*/
	static void GenerateLaunchFile(const GenerateProjectParameters &parameters);

};
#endif