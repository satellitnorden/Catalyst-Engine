#include "CatalystCodeGeneratorCore.h"
#include "CatalystCodeGenerator.h"

int32 main(int32 command_line_argument_count, char *command_line_arguments[])
{
	//Set up the Catalyst code generator.
	CatalystCodeGenerator code_generator;

	//Run it.
	code_generator.Run(command_line_argument_count, command_line_arguments);

	//Return success!
	return 0;
}