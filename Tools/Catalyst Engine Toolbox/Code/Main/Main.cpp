//Core.
#include <Core/Core.h>

//Generate solution.
#include <GenerateSolution/GenerateSolution.h>

//Project settings.
#include <ProjectSettings/ProjectSettings.h>

int32 main(int32 argument_count, char *arguments[])
{
	//Read the project settings.
	ProjectSettings project_settings;
	project_settings.Read("..\\Project.json");

	//Decide what to do.
	if (argument_count > 1 && strcmp(arguments[1], "generate_solution") == 0)
	{
		GenerateSolution::Run(project_settings);
	}

	return 0;
}