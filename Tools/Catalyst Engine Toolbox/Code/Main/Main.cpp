//Core.
#include <Core/Core.h>

//Generate code.
#include <GenerateCode/GenerateCode.h>

//Generate projects.
#include <GenerateProjects/GenerateProjects.h>

//Project settings.
#include <ProjectSettings/ProjectSettings.h>

//STD.
#include <cstring>
#include <filesystem>

int32 main(int32 argument_count, char *arguments[])
{
	//Set up the project settings.
	ProjectSettings project_settings;

	//Search for the 'Project.json' file - Depending on the current working directory, it may exist in different places.
	if (std::filesystem::exists("../Project.json"))
	{
		project_settings.Read("../Project.json");
	}

	else if (std::filesystem::exists("../../../Project.json"))
	{
		project_settings.Read("../../../Project.json");
	}

	//Decide what to do.
	if (argument_count > 1 && strcmp(arguments[1], "generate_projects") == 0)
	{
		GenerateProjects::Run(project_settings);
	}

	else if (argument_count > 1 && strcmp(arguments[1], "generate_code") == 0)
	{
		GenerateCode::Run(project_settings);
	}

	return 0;
}