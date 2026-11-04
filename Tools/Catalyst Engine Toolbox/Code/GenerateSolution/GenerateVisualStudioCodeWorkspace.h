#pragma once

//Core.
#include <Core/Core.h>

//Generate solution.
#include <GenerateSolution/GenerateSolutionParameters.h>

class GenerateVisualStudioCodeWorkspace
{

public:

	/*
	*	Generates!
	*/
	static void Generate(const GenerateSolutionParameters &parameters);

private:

	/*
	*	Generates the '.code-workspace' file.
	*/
	static void GenerateCodeWorkspaceFile(const GenerateSolutionParameters &parameters);

	/*
	*	Generates the '.vscode\c_cpp_properties.json' file.
	*/
	static void GenerateCCPPPropertiesFile(const GenerateSolutionParameters &parameters);

};