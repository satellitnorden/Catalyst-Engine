//Header file.
#include "CatalystCodeGenerator.h"

/*
*	Runs the Catalyst code generator.
*/
void CatalystCodeGenerator::Run(int32 command_line_argument_count, char *command_line_arguments[])
{
	//Runs the script generator first, since it generates code that might be basis for further code generation.
	_ScriptGenerator.Run(command_line_argument_count, command_line_arguments);

	//Run the enumeration generator.
	_EnumerationGenerator.Run();
}