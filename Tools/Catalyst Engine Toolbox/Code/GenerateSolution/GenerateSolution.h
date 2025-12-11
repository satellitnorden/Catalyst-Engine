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
	static void Clean();

	/*
	*	Generates the "RunCodeGeneration.bat" file.
	*/
	static void GenerateRunCodeGenerationBatchFile(const ProjectSettings &project_settings, const bool for_pre_generation);

};