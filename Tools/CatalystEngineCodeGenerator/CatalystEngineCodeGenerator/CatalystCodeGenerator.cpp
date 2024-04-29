//Header file.
#include "CatalystCodeGenerator.h"

/*
*	Runs the Catalyst code generator.
*/
void CatalystCodeGenerator::Run(int32 command_line_argument_count, char *command_line_arguments[])
{
	//Run the script generator first, since it generates code that might be basis for further code generation.
	_ScriptGenerator.Run(command_line_argument_count, command_line_arguments);

	//Likewise, the settings generator might generate code that might be basis for further code generation, so run that before as well.
	_SettingsGenerator.Run();

	//Run the component generator.
	_ComponentGenerator.Run();

	//Run the enumeration generator.
	_EnumerationGenerator.Run();

	//Run the system generator.
	_SystemGenerator.Run();
}