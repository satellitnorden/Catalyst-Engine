//Header file.
#include <GenerateCode/GenerateCode.h>

//Generate code.
#include <GenerateCode/ComponentGenerator.h>
#include <GenerateCode/EnumerationGenerator.h>
#include <GenerateCode/EventGenerator.h>
#include <GenerateCode/ScriptGenerator.h>
#include <GenerateCode/SettingsGenerator.h>
#include <GenerateCode/SystemGenerator.h>

//Macros.
#define CHECK_ERROR_CODE() if (error_code) { std::cerr << "Error at " << __LINE__ << ": " << error_code.message() << std::endl; }

/*
*	Generates the code with the given project settings.
*/
void GenerateCode::Run(const ProjectSettings &project_settings)
{
	//Run the script generator.
	ScriptGenerator script_generator;
	script_generator.Run(project_settings);

	//Run the settings generator.
	SettingsGenerator settings_generator;
	settings_generator.Run();

	//Run the component generator.
	ComponentGenerator component_generator;
	component_generator.Run();

	//Run the enumeration generator.
	EnumerationGenerator enumeration_generator;
	enumeration_generator.Run();

	//Run the event generator.
	EventGenerator event_generator;
	event_generator.Run();

	//Run the system generator.
	SystemGenerator system_generator;
	system_generator.Run();
}