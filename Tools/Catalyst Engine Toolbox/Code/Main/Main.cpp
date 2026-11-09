//Core.
#include <Core/Core.h>

//Generate projects.
#include <GenerateProjects/GenerateProjects.h>

//Project settings.
#include <ProjectSettings/ProjectSettings.h>

//STD.
#include <cstring>

int32 main(int32 argument_count, char *arguments[])
{
	//Read the project settings.
	ProjectSettings project_settings;
	project_settings.Read("../Project.json");

	//Decide what to do.
	if (argument_count > 1 && strcmp(arguments[1], "generate_projects") == 0)
	{
		GenerateProjects::Run(project_settings);
	}

	return 0;
}