#pragma once

//Core.
#include <Core/Core.h>

#if defined(PLATFORM_WIN64)
	#define VISUAL_STUDIO_SOLUTION_SUPPORTED
#endif

#if defined(VISUAL_STUDIO_SOLUTION_SUPPORTED)
//Generate projects.
#include <GenerateProjects/GenerateProjectParameters.h>

class GenerateVisualStudioSolution
{

public:

	/*
	*	Generates!
	*/
	static void Generate(GenerateProjectParameters parameters);

private:

	/*
	*	Generates the resource file.
	*/
	static void GenerateResourceFile(const GenerateProjectParameters &parameters);

	/*
	*	Generates the project file.
	*/
	static void GenerateProjectFile(const GenerateProjectParameters &parameters, const std::string &project_guid);

	/*
	*	Generates the filters file.
	*/
	static void GenerateFiltersFile(const GenerateProjectParameters &parameters);

	/*
	*	Generates the solution file.
	*/
	static void GenerateSolutionFile(const GenerateProjectParameters &parameters, const std::string &solution_guid, const std::string &project_guid);

	/*
	*   Adds compile files for the given directory.
	*/
	static void AddCompileFiles(const GenerateProjectParameters &parameters, std::ofstream &file, const GenerateProjectParameters::Directory &directory);

	/*
	*	Adds filter files.
	*/
	static void AddFilterFiles(std::ofstream &file, const GenerateProjectParameters::Directory &directory, const std::string &filter_string);

	/*
	*	Adds filter identifiers.
	*/
	static void AddFilterIdentifiers(std::ofstream &file, const GenerateProjectParameters::Directory &directory, const std::string &filter_string);

};
#endif