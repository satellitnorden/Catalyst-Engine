#pragma once

#include "ComponentGenerator.h"
#include "EnumerationGenerator.h"
#include "ScriptGenerator.h"

class CatalystCodeGenerator final
{

public:

	/*
	*	Runs the Catalyst code generator.
	*/
	void Run(int32 command_line_argument_count, char *command_line_arguments[]);

private:

	//The component generators.
	ComponentGenerator _ComponentGenerator;

	//The enumeration generators.
	EnumerationGenerator _EnumerationGenerator;

	//The script generator.
	ScriptGenerator _ScriptGenerator;

};