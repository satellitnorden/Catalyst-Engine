#pragma once

//Core.
#include <Core/Core.h>

//Generate solution.
#include <GenerateSolution/GenerateSolutionParameters.h>

class GenerateVisualStudioSolution
{

public:

	/*
	*	Generates!
	*/
	static void Generate(GenerateSolutionParameters parameters);

private:

	/*
	*	Generates the resource file.
	*/
	static void GenerateResourceFile(const GenerateSolutionParameters &parameters);

	/*
	*	Generates the project file.
	*/
	static void GenerateProjectFile(const GenerateSolutionParameters &parameters, const std::string &project_guid);

	/*
	*	Generates the filters file.
	*/
	static void GenerateFiltersFile(const GenerateSolutionParameters &parameters);

	/*
	*	Generates the solution file.
	*/
	static void GenerateSolutionFile(const GenerateSolutionParameters &parameters, const std::string &solution_guid, const std::string &project_guid);

	/*
	*   Adds compile files for the given directory.
	*/
	static void AddCompileFiles(const GenerateSolutionParameters &parameters, std::ofstream &file, const GenerateSolutionParameters::Directory &directory);

	/*
	*	Adds filter files.
	*/
	static void AddFilterFiles(std::ofstream &file, const GenerateSolutionParameters::Directory &directory, const std::string &filter_string);

	/*
	*	Adds filter identifiers.
	*/
	static void AddFilterIdentifiers(std::ofstream &file, const GenerateSolutionParameters::Directory &directory, const std::string &filter_string);

};